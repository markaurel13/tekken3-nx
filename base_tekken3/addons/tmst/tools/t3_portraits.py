"""Tekken 3's own loading-screen (VS) portraits of the 21 stock fighters, from
the player's Tekken 3 disc (the one the Expanded runs), for Tournament Mode's
screens (issue #3): workspace/tournament/portraits/<key>.png, where the game
otherwise keeps a portrait only once a loading screen has shown it
(src/tournament/tmst_tournament_art.c art_portrait, art_set_portrait).

Where (measured 2026-11-11, docs/handoffs/handoff_issue3.md): TEKKEN3/TEKKEN3.BNS,
its entry 14 + the fighter's ID (Paul 14 .. True Ogre 34); the entries' table
in SLUS_004.02 at file offset 0x14C00, 303 (sector, size) pairs, sectors of
2048 bytes from the start of the BNS. Each entry is packed with the game's LZ
(0x80031C14): a flag byte whose highest set bit only ends it (up to 7 items), a
0 flag byte the end; a set bit a literal byte, a clear one 2 bytes big-endian v,
v & 0x7FF back (0: 2048), (v >> 11) & 31 bytes (0: 32). Unpacked: an 8-bit TIM,
one 256-colour palette, 126 x 252, drawn as four bands of 64 rows, each on its
quarter of the palette. The PNG is the cache's: RGB, 5 bits widened as
art_read_texture does (* 255 / 31), see-through texels magenta (0xff00ff).

Supported: Tekken 3 USA (SLUS-00402), as the Expanded keeps it (game.toml's
disc: a .cue with its .bin files), or any .cue, .bin or .chd of it.
"""
from __future__ import annotations
import re, struct, zlib
from pathlib import Path
from disc_image import DiscError, open_disc

VERSION = 1
OUT = 'addons/tmst/workspace/tournament/portraits'
KEYS = ('paul', 'law', 'lei', 'king', 'yoshimitsu', 'nina', 'hwoarang', 'xiaoyu', 'eddy', 'jin', 'julia', 'kuma',
        'bryan', 'heihachi', 'ogre', 'mokujin', 'gunjack', 'gon', 'anna', 'drb', 'trueogre')   # IDs 0..20
FIRST_ENTRY = 14
EXE, BNS = 'TEKKEN3/SLUS_004.02', 'TEKKEN3/TEKKEN3.BNS'
EXE_HEADER = (0x80079C70, 0x80010000, 0x00121000)             # entry, load address, text size (US)
TABLE, ENTRIES = 0x14C00, 303
W, H, BAND = 126, 252, 64
KEY_COLOUR = (255, 0, 255)


def find_disc(root: Path) -> Path | None:
    """The Tekken 3 disc the Expanded runs: game.toml's disc, beside it."""
    try:
        text = (root / 'game.toml').read_text(encoding='utf-8')
    except OSError:
        return None
    m = re.search(r'^\s*disc\s*=\s*"([^"]+)"', text, re.M)
    if not m:
        return None
    path = root / m[1]
    return path if path.is_file() else None


def unpack(data: bytes) -> bytes:
    """Tekken 3's LZ (see the module's text)."""
    out = bytearray()
    i = 1
    flags = data[0]
    while True:
        while flags >= 2:
            if flags & 1:
                out.append(data[i]); i += 1
            else:
                v = data[i] << 8 | data[i + 1]; i += 2
                back, n = v & 0x7FF or 2048, (v >> 11) & 31 or 32
                start = len(out) - back
                if start < 0:
                    raise ValueError('a reference before the start')
                for k in range(n):
                    out.append(out[start + k])
            flags >>= 1
        flags = data[i]; i += 1
        if flags == 0:
            return bytes(out)


def colour(c: int) -> tuple[int, int, int]:
    if not c:
        return KEY_COLOUR
    return ((c & 31) * 255 // 31, (c >> 5 & 31) * 255 // 31, (c >> 10 & 31) * 255 // 31)


def portrait(tim: bytes) -> bytes:
    """A VS portrait's TIM to the cache's RGB rows (W x H x 3)."""
    if len(tim) < 20 or struct.unpack_from('<2I', tim, 0) != (0x10, 9):
        raise ValueError('not an 8-bit TIM with a palette')
    clut_len, = struct.unpack_from('<I', tim, 8)
    cw, ch = struct.unpack_from('<2H', tim, 16)
    if cw * ch < 256:
        raise ValueError(f'a palette of {cw * ch} colours')
    pal = struct.unpack_from('<256H', tim, 20)
    at = 8 + clut_len
    _, _, _, iw, ih = struct.unpack_from('<I4H', tim, at)
    if (iw * 2, ih) != (W, H):
        raise ValueError(f'a picture of {iw * 2} x {ih}')
    pix = tim[at + 12:at + 12 + W * H]
    if len(pix) != W * H:
        raise ValueError('a picture cut short')
    out = bytearray()
    for y in range(H):
        base = y // BAND * BAND
        for x in range(W):
            out += bytes(colour(pal[base + pix[y * W + x]]))
    return bytes(out)


def png(path: Path, rgb: bytes) -> None:
    raw = b''.join(b'\0' + rgb[r * W * 3:(r + 1) * W * 3] for r in range(H))
    chunk = lambda t, d: struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d))
    path.write_bytes(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>2I5B', W, H, 8, 2, 0, 0, 0))
                     + chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b''))


def table(exe: bytes) -> list[tuple[int, int]]:
    if exe[:8] != b'PS-X EXE':
        raise DiscError('SLUS_004.02 is not a PlayStation program')
    head = struct.unpack_from('<I', exe, 0x10) + struct.unpack_from('<2I', exe, 0x18)
    if head != EXE_HEADER or len(exe) < TABLE + ENTRIES * 8:
        raise DiscError('this SLUS_004.02 is not Tekken 3 USA (SLUS-00402)')
    return [struct.unpack_from('<2I', exe, TABLE + 8 * i) for i in range(ENTRIES)]


def convert(disc_path, out: Path, say=print) -> list[str]:
    """The 21 portraits into out (written over); their file names."""
    with open_disc(disc_path) as disc:
        exe = disc.read_file(EXE)
        bns = disc.locate(BNS)
        if exe is None or bns is None:
            raise DiscError(f'{Path(disc_path).name} is not Tekken 3 (no {EXE} or {BNS})')
        entries = table(exe)
        out.mkdir(parents=True, exist_ok=True)
        names = []
        for fighter, key in enumerate(KEYS):
            sector, size = entries[FIRST_ENTRY + fighter]
            data = b''.join(disc.read(bns[0] + sector + k) for k in range(-(-size // 2048)))[:size]
            try:
                rgb = portrait(unpack(data))
            except (ValueError, IndexError, struct.error) as error:
                say(f'Tekken 3 portraits: {key}: {error}; left out')
                continue
            png(out / f'{key}.png', rgb)
            names.append(f'{key}.png')
    say(f'Tekken 3 portraits: {len(names)} of {len(KEYS)} written')
    return names


if __name__ == '__main__':
    import sys
    root = Path(__file__).resolve().parents[3]
    disc = Path(sys.argv[1]) if len(sys.argv) > 1 else find_disc(root)
    out = Path(sys.argv[2]) if len(sys.argv) > 2 else root / OUT
    if not disc:
        sys.exit('no Tekken 3 disc (game.toml)')
    convert(disc, out)
