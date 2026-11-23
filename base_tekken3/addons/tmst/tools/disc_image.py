"""A PlayStation disc image, read without any tool of its own: a .cue with its
.bin files (one per track or a single-file dump), a .bin alone (its .cue
beside it, when one names it) or a .chd (MAME's compressed disc, version 5,
as chdman makes it).

    with open_disc(path) as disc:
        disc.read(lba)              # 2048 bytes of user data of track 1's sector lba
        disc.read_file('SYSTEM.CNF') # a file of the ISO 9660 file system, or None
        disc.tracks                 # [Track(number, kind, frames)]
        disc.locate('A/B.XAS')      # (first sector, size) of a file, or None
        disc.audio(2)               # a CD audio track: 16-bit stereo, little-endian, from its INDEX 01

CHD codecs read: cdlz (LZMA), cdzl (zlib), cdfl (FLAC: chdman uses it for
audio, flac_decode()) and cdzs (zstd, on a Python with compression.zstd); a
parent CHD (a delta) raises DiscError.
"""
from __future__ import annotations
import lzma, re, struct, sys, zlib
from array import array
from operator import mul
from dataclasses import dataclass
from pathlib import Path

SECTOR = 2352
SYNC = b'\x00' + b'\xff' * 10 + b'\x00'
EXTENSIONS = ('.cue', '.bin', '.chd')


class DiscError(Exception):
    pass


@dataclass
class Track:
    number: int
    kind: str               # MODE2/2352, MODE1/2352, AUDIO, ...
    frames: int


def user_data(sector: bytes) -> bytes:
    """The 2048 bytes of user data of a raw 2352-byte sector (mode 1 or mode 2 form 1)."""
    if sector[:12] == SYNC and sector[15] == 1:
        return sector[16:16 + 2048]
    return sector[24:24 + 2048]


# ---- .cue / .bin ------------------------------------------------------------

def msf(text: str) -> int:
    m, s, f = (int(x) for x in text.split(':'))
    return (m * 60 + s) * 75 + f


def cue_for(path: Path) -> Path:
    """A .bin chosen in place of its .cue: the .cue beside it that names it, if any."""
    if path.suffix.lower() != '.bin':
        return path
    for cue in sorted(set(path.parent.glob('*.cue')) | set(path.parent.glob('*.CUE'))):
        try:
            text = cue.read_text(encoding='utf-8', errors='replace')
        except OSError:
            continue
        if any(n.lower() == path.name.lower() for n in re.findall(r'FILE\s+"([^"]+)"', text, re.I)):
            return cue
    return path


class CueDisc:
    def __init__(self, path: Path):
        self.path = path
        self.tracks: list[Track] = []
        if path.suffix.lower() == '.cue':
            self._parse_cue(path)
        else:                                       # a .bin with no .cue: one data track
            self.data_file, self.data_start = path, 0
            self.tracks = [Track(1, 'MODE2/2352', path.stat().st_size // SECTOR)]
        self.handle = open(self.data_file, 'rb')

    def _parse_cue(self, cue: Path) -> None:
        current = None
        entries = []                                # (file, number, kind, INDEX 01, INDEX 00 sector in its file)
        for line in cue.read_text(encoding='utf-8', errors='replace').splitlines():
            w = line.split()
            if not w:
                continue
            key = w[0].upper()
            if key == 'FILE':
                m = re.search(r'"([^"]+)"', line)
                name = m[1] if m else w[1]
                current = cue.parent / name
                if not current.is_file():
                    raise DiscError(f'{cue.name} names {name}, which is not beside it')
            elif key == 'TRACK' and current:
                entries.append([current, int(w[1]), w[2].upper(), 0, None])
            elif key == 'INDEX' and entries and int(w[1]) in (0, 1):
                entries[-1][4 - int(w[1])] = msf(w[2])
        if not entries:
            raise DiscError(f'{cue.name} names no track')
        self.spans = {}                             # track: (file, INDEX 01 sector, end sector) in its file
        for i, (file, number, kind, start, _) in enumerate(entries):
            nxt = entries[i + 1] if i + 1 < len(entries) else None
            # A track ends where the next one's pregap (INDEX 00) starts, in the same file.
            end = (nxt[3] if nxt[4] is None else nxt[4]) if nxt and nxt[0] == file else file.stat().st_size // SECTOR
            self.tracks.append(Track(number, kind, end - start))
            self.spans[number] = (file, start, end)
        self.data_file, self.data_start = entries[0][0], entries[0][3]

    def raw(self, lba: int) -> bytes:
        self.handle.seek((self.data_start + lba) * SECTOR)
        data = self.handle.read(SECTOR)
        if len(data) != SECTOR:
            raise DiscError(f'sector {lba} is past the end of {self.data_file.name}')
        return data

    def audio(self, number: int) -> bytes:
        if number not in getattr(self, 'spans', {}):
            raise DiscError(f'{self.path.name}: no track {number} (a .bin without its .cue has only track 1)')
        file, start, end = self.spans[number]
        with open(file, 'rb') as f:
            f.seek(start * SECTOR)
            return f.read(max(0, end - start) * SECTOR)

    def close(self) -> None:
        self.handle.close()


# ---- .chd (version 5) -------------------------------------------------------

FRAME = 2448                                        # a CD frame in a CHD: sector + subcode
SUBCODE = 96
# Map entry types (MAME src/lib/util/chd.h).
TYPE_0, TYPE_3, NONE, SELF, PARENT = 0, 3, 4, 5, 6
RLE_SMALL, RLE_LARGE, SELF_0, SELF_1, PARENT_SELF, PARENT_0, PARENT_1 = 7, 8, 9, 10, 11, 12, 13


class Bits:
    """MSB-first bit reader; past the end it reads zeros (as MAME's)."""
    def __init__(self, data: bytes):
        self.value, self.left = int.from_bytes(data, 'big'), len(data) * 8

    def read(self, n: int) -> int:
        if n == 0:
            return 0
        self.left -= n
        if self.left >= 0:
            return (self.value >> self.left) & ((1 << n) - 1)
        out = (self.value << -self.left) & ((1 << n) - 1) if self.left > -n else 0
        self.value, self.left = 0, 0
        return out

    def peek(self, n: int) -> int:
        value, left = self.value, self.left
        out = self.read(n)
        self.value, self.left = value, left
        return out


def huffman_map_decoder(bits: Bits, codes: int = 16, maxbits: int = 8):
    """MAME's huffman_decoder<16, 8>::import_tree_rle: the code lengths, then canonical codes."""
    lengths, numbits = [], 4
    while len(lengths) < codes:
        n = bits.read(numbits)
        if n != 1:
            lengths.append(n)
        else:
            n = bits.read(numbits)
            if n == 1:
                lengths.append(1)
            else:
                lengths += [n] * (bits.read(numbits) + 3)
    if len(lengths) != codes:
        raise DiscError('the CHD map is damaged (its code table)')
    histo = [0] * 33
    for n in lengths:
        histo[n] += 1
    start = 0
    for length in range(32, 0, -1):
        nxt = (start + histo[length]) >> 1
        if length != 1 and nxt * 2 != start + histo[length]:
            raise DiscError('the CHD map is damaged (its codes)')
        histo[length], start = start, nxt
    table = [None] * (1 << maxbits)
    for symbol, n in enumerate(lengths):
        if n:
            code = histo[n]
            histo[n] += 1
            shift = maxbits - n
            for i in range(code << shift, (code + 1) << shift):
                table[i] = (symbol, n)

    def decode() -> int:
        entry = table[bits.peek(maxbits)]
        if entry is None:
            raise DiscError('the CHD map is damaged (a code)')
        bits.read(entry[1])
        return entry[0]
    return decode


def zstd_decompress(data: bytes, size: int) -> bytes:
    try:
        from compression import zstd             # Python 3.14+
    except ImportError:
        raise DiscError('this CHD is compressed with zstd, which this Python cannot read: '
                        'convert it again with chdman createcd (default codecs)') from None
    return zstd.decompress(data)[:size]


def lzma_decompress(data: bytes, size: int) -> bytes:
    # MAME's LZMA codec: raw LZMA1, lc 3 lp 0 pb 2, the dictionary sized for the hunk.
    d = lzma.LZMADecompressor(lzma.FORMAT_RAW, filters=[
        {'id': lzma.FILTER_LZMA1, 'dict_size': 1 << 20, 'lc': 3, 'lp': 0, 'pb': 2}])
    return d.decompress(data, size)


def inflate(data: bytes, size: int) -> bytes:
    return zlib.decompressobj(-15).decompress(data, size)


CD_CODECS = {b'cdlz': lzma_decompress, b'cdzl': inflate, b'cdzs': zstd_decompress}

# FLAC (MAME's chd_cd_flac_compressor writes bare frames: 44 100 Hz, stereo, 16-bit).
FLAC_BLOCKS = {1: 192, 2: 576, 3: 1152, 4: 2304, 5: 4608, **{c: 256 << (c - 8) for c in range(8, 16)}}
FLAC_BITS = {0: 16, 1: 8, 2: 12, 4: 16, 5: 20, 6: 24}
FIXED = ([], [1], [2, -1], [3, -3, 1], [4, -6, 4, -1])


def flac_decode(data: bytes, samples: int) -> tuple[array, int]:
    """FLAC frames until samples stereo samples: (s16 frames, left and right
    interleaved; the bytes the frames took). The bits are read from a string
    of '0' and '1' (str.index finds a Rice code's end at C speed)."""
    bits = bin(int.from_bytes(data, 'big') | (1 << len(data) * 8))[3:]
    p = 0

    def read(n: int) -> int:
        nonlocal p
        p += n
        return int(bits[p - n:p], 2) if n else 0

    def signed(n: int) -> int:
        v = read(n)
        return v - (1 << n) if n and v >> (n - 1) else v

    def residual(n: int, order: int) -> list:
        nonlocal p
        method = read(2)
        width, escape = (4, 15) if method == 0 else (5, 31)
        porder = read(4)
        out = []
        for part in range(1 << porder):
            count = (n >> porder) - (order if part == 0 else 0)
            k = read(width)
            if k == escape:
                size = read(5)
                out += [signed(size) for _ in range(count)]
                continue
            append, find = out.append, bits.index
            for _ in range(count):
                one = find('1', p)
                u = (one - p) << k | (int(bits[one + 1:one + 1 + k], 2) if k else 0)
                p = one + 1 + k
                append(u >> 1 ^ -(u & 1))
        return out

    def predict(warm: list, coefs: list, shift: int, res: list) -> list:
        s, order = warm, len(coefs)
        if not order:
            return s + res
        rc = coefs[::-1]
        append = s.append
        for r in res:
            append(r + (sum(map(mul, rc, s[-order:])) >> shift))
        return s

    def subframe(n: int, bps: int) -> list:
        nonlocal p
        read(1)
        kind, wasted = read(6), read(1)
        if wasted:
            one = bits.index('1', p)
            wasted, p = one - p + 1, one + 1
            bps -= wasted
        if kind == 0:
            s = [signed(bps)] * n
        elif kind == 1:
            s = [signed(bps) for _ in range(n)]
        elif 8 <= kind <= 12:
            order = kind - 8
            warm = [signed(bps) for _ in range(order)]
            s = predict(warm, FIXED[order], 0, residual(n, order))
        elif kind >= 32:
            order = kind - 31
            warm = [signed(bps) for _ in range(order)]
            precision, shift = read(4) + 1, signed(5)
            coefs = [signed(precision) for _ in range(order)]
            s = predict(warm, coefs, shift, residual(n, order))
        else:
            raise DiscError(f'a FLAC subframe of a kind not known ({kind})')
        return [v << wasted for v in s] if wasted else s

    left, right = [], []
    while len(left) < samples:
        if read(15) != 0x7FFC:
            raise DiscError('the CHD audio is damaged (a FLAC frame)')
        read(1)
        bcode, rcode, chan, scode = read(4), read(4), read(4), read(3)
        read(1)
        ones = 8 - (read(8) ^ 0xFF).bit_length()            # the frame number, UTF-8 coded
        read(8 * (ones - 1) if ones > 1 else 0)
        n = read(8) + 1 if bcode == 6 else read(16) + 1 if bcode == 7 else FLAC_BLOCKS.get(bcode, 0)
        read(8 if rcode == 12 else 16 if rcode in (13, 14) else 0)
        read(8)                                             # CRC-8
        bps = FLAC_BITS.get(scode)
        if not n or not bps or chan > 10:
            raise DiscError('the CHD audio is damaged (a FLAC frame header)')
        a = subframe(n, bps + (chan == 9))
        b = subframe(n, bps + (chan in (8, 10)))
        if chan == 8:                                       # left, side
            b = [x - y for x, y in zip(a, b)]
        elif chan == 9:                                     # side, right
            a = [x + y for x, y in zip(a, b)]
        elif chan == 10:                                    # mid, side
            a, b = ([(m << 1 | s & 1) + s >> 1 for m, s in zip(a, b)],
                    [(m << 1 | s & 1) - s >> 1 for m, s in zip(a, b)])
        left += a
        right += b
        p = (p + 7) // 8 * 8 + 16                           # byte boundary, CRC-16
    out = array('h', bytes(4 * samples))
    out[0::2] = array('h', left[:samples])
    out[1::2] = array('h', right[:samples])
    return out, p // 8


class ChdDisc:
    def __init__(self, path: Path):
        self.path = path
        self.handle = open(path, 'rb')
        head = self.handle.read(124)
        if head[:8] != b'MComprHD':
            raise DiscError(f'{path.name} is not a CHD file')
        version = struct.unpack('>I', head[12:16])[0]
        if version != 5:
            raise DiscError(f'{path.name} is a version {version} CHD: convert it again with a recent chdman '
                            '(chdman createcd) to make a version 5 one')
        self.codecs = [head[16 + 4 * i:20 + 4 * i] for i in range(4)]
        (self.logical, self.mapoffset, self.metaoffset, self.hunkbytes,
         self.unitbytes) = struct.unpack('>QQQII', head[32:64])
        if head[104:124].strip(b'\0'):
            raise DiscError(f'{path.name} is a delta CHD (it needs its parent): use the full one')
        self.hunks = -(-self.logical // self.hunkbytes)
        self._read_map()
        self.tracks = self._read_tracks()
        if not self.tracks:
            raise DiscError(f'{path.name} is not a CD image (no track list)')
        first = self.tracks[0]
        self.data_start = first.pregap if first.pgtype.startswith('V') else 0
        # Each track's INDEX 01 frame and length: a pregap is stored only when its type is V...,
        # and every track is padded to 4 frames (chdman's CD_TRACK_PADDING).
        self.spans, at = {}, 0
        for t in self.tracks:
            pregap = t.pregap if t.pgtype.startswith('V') else 0
            self.spans[t.number] = (at + pregap, t.frames - pregap)
            at += -(-t.frames // 4) * 4
        self.cache: dict[int, bytes] = {}

    def _read_map(self) -> None:
        f = self.handle
        if self.codecs[0] == b'\0\0\0\0':            # an uncompressed CHD: a 4-byte hunk number each
            f.seek(self.mapoffset)
            raw = f.read(4 * self.hunks)
            self.map = [(NONE, self.hunkbytes, struct.unpack_from('>I', raw, 4 * i)[0] * self.hunkbytes)
                        for i in range(self.hunks)]
            return
        f.seek(self.mapoffset)
        h = f.read(16)
        mapbytes = struct.unpack('>I', h[0:4])[0]
        firstoffs = int.from_bytes(h[4:10], 'big')
        lengthbits, selfbits, parentbits = h[12], h[13], h[14]
        bits = Bits(f.read(mapbytes))
        decode = huffman_map_decoder(bits)
        kinds, last, repeat = [], 0, 0
        for _ in range(self.hunks):
            if repeat:
                kinds.append(last)
                repeat -= 1
                continue
            v = decode()
            if v == RLE_SMALL:
                kinds.append(last)
                repeat = 2 + decode()
            elif v == RLE_LARGE:
                kinds.append(last)
                repeat = 2 + 16 + (decode() << 4)
                repeat += decode()
            else:
                kinds.append(v)
                last = v
        self.map = []
        offset, last_self, last_parent = firstoffs, 0, 0
        for n, kind in enumerate(kinds):
            if TYPE_0 <= kind <= TYPE_3:
                length = bits.read(lengthbits)
                self.map.append((kind, length, offset))
                offset += length
                bits.read(16)                        # crc
            elif kind == NONE:
                self.map.append((NONE, self.hunkbytes, offset))
                offset += self.hunkbytes
                bits.read(16)
            elif kind == SELF:
                last_self = bits.read(selfbits)
                self.map.append((SELF, 0, last_self))
            elif kind == PARENT:
                last_parent = bits.read(parentbits)
                self.map.append((PARENT, 0, last_parent))
            elif kind in (SELF_0, SELF_1):
                if kind == SELF_1:
                    last_self += 1
                self.map.append((SELF, 0, last_self))
            elif kind == PARENT_SELF:
                last_parent = n * self.hunkbytes // self.unitbytes
                self.map.append((PARENT, 0, last_parent))
            elif kind in (PARENT_0, PARENT_1):
                if kind == PARENT_1:
                    last_parent += self.hunkbytes // self.unitbytes
                self.map.append((PARENT, 0, last_parent))
            else:
                raise DiscError('the CHD map is damaged (a hunk type)')

    def _read_tracks(self) -> list:
        tracks, at = [], self.metaoffset
        while at:
            self.handle.seek(at)
            h = self.handle.read(16)
            tag, length, nxt = h[:4], int.from_bytes(h[5:8], 'big'), struct.unpack('>Q', h[8:16])[0]
            data = self.handle.read(length).rstrip(b'\0').decode('ascii', 'replace')
            if tag in (b'CHT2', b'CHTR', b'CHCD'):
                fields = dict(re.findall(r'(\w+):(\S+)', data))
                t = Track(int(fields.get('TRACK', len(tracks) + 1)), fields.get('TYPE', ''), int(fields.get('FRAMES', 0)))
                t.pregap, t.pgtype = int(fields.get('PREGAP', 0)), fields.get('PGTYPE', '')
                tracks.append(t)
            at = nxt
        for t in tracks:
            t.kind = 'AUDIO' if t.kind == 'AUDIO' else t.kind.replace('_RAW', '/2352')
        return sorted(tracks, key=lambda t: t.number)

    def hunk(self, n: int) -> bytes:
        if n in self.cache:
            return self.cache[n]
        kind, length, offset = self.map[n]
        if kind == SELF:
            data = self.hunk(offset)
        elif kind == PARENT:
            raise DiscError(f'{self.path.name} needs its parent CHD')
        elif kind == NONE:
            self.handle.seek(offset)
            data = self.handle.read(self.hunkbytes)
        else:
            self.handle.seek(offset)
            data = self._cd_decompress(self.codecs[kind], self.handle.read(length))
        if len(self.cache) > 64:
            self.cache.clear()
        self.cache[n] = data
        return data

    def _cd_decompress(self, codec: bytes, src: bytes) -> bytes:
        """MAME's chd_cd_decompressor: the sectors' and the subcodes' streams, then sync headers back
        (chd_cd_flac_compressor: FLAC frames, then the subcodes)."""
        frames = self.hunkbytes // FRAME
        if codec == b'cdfl':
            pcm, used = flac_decode(src, frames * SECTOR // 4)
            if sys.byteorder == 'little':                   # CD audio is kept big-endian in a CHD
                pcm.byteswap()
            sectors, subcodes = pcm.tobytes(), inflate(src[used:], frames * SUBCODE)
            return b''.join(sectors[i * SECTOR:(i + 1) * SECTOR] + subcodes[i * SUBCODE:(i + 1) * SUBCODE]
                            for i in range(frames))
        base = CD_CODECS.get(codec)
        if base is None:
            raise DiscError(f'{self.path.name} has data compressed with {codec.decode("ascii", "replace")}, '
                            'which this setup cannot read: convert it again with chdman createcd -c cdlz,cdzl')
        ecc_bytes = (frames + 7) // 8
        size_bytes = 2 if self.hunkbytes < 65536 else 3
        base_len = int.from_bytes(src[ecc_bytes:ecc_bytes + size_bytes], 'big')
        start = ecc_bytes + size_bytes
        sectors = base(src[start:start + base_len], frames * SECTOR)
        sub_codec = zstd_decompress if codec == b'cdzs' else inflate
        subcodes = sub_codec(src[start + base_len:], frames * SUBCODE)
        out = bytearray()
        for i in range(frames):
            sector = bytearray(sectors[i * SECTOR:(i + 1) * SECTOR])
            if src[i // 8] & (1 << (i % 8)):         # sync header (and ECC) left out: the sync back
                sector[:12] = SYNC
            out += sector + subcodes[i * SUBCODE:(i + 1) * SUBCODE]
        return bytes(out)

    def raw(self, lba: int) -> bytes:
        frame = self.data_start + lba
        per_hunk = self.hunkbytes // FRAME
        h = self.hunk(frame // per_hunk)
        at = (frame % per_hunk) * FRAME
        return h[at:at + SECTOR]

    def audio(self, number: int) -> bytes:
        if number not in self.spans:
            raise DiscError(f'{self.path.name}: no track {number}')
        start, count = self.spans[number]
        per_hunk = self.hunkbytes // FRAME
        out = bytearray()
        for frame in range(start, start + count):
            h = self.hunk(frame // per_hunk)
            at = (frame % per_hunk) * FRAME
            out += h[at:at + SECTOR]
        pcm = array('h', bytes(out))
        if sys.byteorder == 'little':
            pcm.byteswap()
        return pcm.tobytes()

    def close(self) -> None:
        self.handle.close()


# ---- the disc, whichever image ---------------------------------------------

class Disc:
    def __init__(self, image):
        self.image, self.tracks = image, image.tracks

    def __enter__(self):
        return self

    def __exit__(self, *error):
        self.image.close()
        return False

    def read(self, lba: int) -> bytes:
        return user_data(self.image.raw(lba))

    def _find(self, lba: int, size: int, name: str):
        name = name.upper()
        for at in range(0, min(size, 64 * 2048), 2048):
            s = self.read(lba + at // 2048)
            i = 0
            while i < 2048 and s[i]:
                n = s[i + 32]
                entry = s[i + 33:i + 33 + n].decode('ascii', 'replace').upper().split(';')[0]
                if entry == name:
                    return struct.unpack_from('<I', s, i + 2)[0], struct.unpack_from('<I', s, i + 10)[0]
                i += s[i]
        return None

    def locate(self, path: str) -> tuple[int, int] | None:
        """(first sector, size) of a file of the ISO 9660 file system, or None."""
        pvd = self.read(16)
        if pvd[1:6] != b'CD001':
            return None
        lba, size = struct.unpack_from('<I', pvd, 156 + 2)[0], struct.unpack_from('<I', pvd, 156 + 10)[0]
        for part in path.replace('\\', '/').strip('/').split('/'):
            found = self._find(lba, size, part)
            if not found:
                return None
            lba, size = found
        return lba, size

    def read_file(self, path: str) -> bytes | None:
        found = self.locate(path)
        if not found:
            return None
        lba, size = found
        out = b''.join(self.read(lba + i) for i in range(-(-size // 2048)))
        return out[:size]

    def raw(self, lba: int) -> bytes:
        """Track 1's sector lba, all 2352 bytes (an XA audio sector's subheader and sound groups)."""
        return self.image.raw(lba)

    def audio(self, number: int) -> bytes:
        return self.image.audio(number)


def open_disc(path) -> Disc:
    path = Path(path)
    if not path.is_file():
        raise DiscError(f'{path} does not exist')
    suffix = path.suffix.lower()
    if suffix == '.chd':
        return Disc(ChdDisc(path))
    if suffix in ('.cue', '.bin'):
        return Disc(CueDisc(cue_for(path)))
    raise DiscError(f'{path.name}: a .cue, .bin or .chd file is needed')
