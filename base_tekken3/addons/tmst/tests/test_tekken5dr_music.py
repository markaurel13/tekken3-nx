"""tools/tekken5dr_music.py without the ISO: its pieces against the title
list, the ISO 9660 lookup, the NPSF channels and the .bgm it writes."""
import hashlib, re, struct, sys, tempfile, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))           # by name: its worker processes import it again
import tekken5dr_music as M  # noqa: E402
import adpcm_pack  # noqa: E402

SECTOR = 2048


def listed() -> list[str]:
    lines = (ADDON / 'data/jukebox/tekken5-dr.txt').read_text(encoding='utf-8').splitlines()
    return [l.split('=', 1)[0].strip() for l in lines if l.strip() and not l.startswith('#')]


def record(name: bytes, lba: int, size: int, folder: bool) -> bytes:
    r = bytearray(33 + len(name) + (len(name) + 1) % 2)
    r[0] = len(r)
    struct.pack_into('<I', r, 2, lba)
    struct.pack_into('<I', r, 10, size)
    r[25] = 2 if folder else 0
    r[32] = len(name)
    r[33:33 + len(name)] = name
    return bytes(r)


def iso_with(path_parts: list[str], data: bytes) -> bytes:
    """An ISO 9660 image with the file at /a/b/c (each part a folder of one entry), data from sector 30."""
    image = bytearray(SECTOR * 30)
    image[16 * SECTOR + 1:16 * SECTOR + 6] = b'CD001'
    lba = 20
    image[16 * SECTOR + 156:16 * SECTOR + 190] = record(b'\0', lba, SECTOR, True)
    for i, part in enumerate(path_parts):
        last = i == len(path_parts) - 1
        entries = record(b'\0', lba, SECTOR, True) + record(b'\1', lba, SECTOR, True)
        entries += record(b'OTHER.BIN;1', 29, 4, False)
        entries += record((part + (';1' if last else '')).encode(), 30 if last else lba + 1,
                          len(data) if last else SECTOR, not last)
        image[lba * SECTOR:lba * SECTOR + len(entries)] = entries
        lba += 1
    return bytes(image) + data + bytes(-len(data) % SECTOR)


def adpcm_block(shift: int, filt: int, nibbles: list[int]) -> bytes:
    return bytes([filt << 4 | shift, 0]) + bytes(nibbles[i] | nibbles[i + 1] << 4 for i in range(0, 28, 2))


def npsf(left: bytes, right: bytes, loop: int, volume: int, interleave: int = 0x40) -> bytes:
    """An NPSF as the game's: the header's block is both channels' (each its half), the last one shorter."""
    head = bytearray(0x800)
    head[:4] = b'NPSF'
    struct.pack_into('<IIIIiII', head, 4, 2 * interleave, len(left), 2, 0x800, loop, 44100, volume)
    head[0x34:0x3C] = b'test.wav'
    body = bytearray()
    for at in range(0, len(left), interleave):
        body += left[at:at + interleave] + right[at:at + interleave]
    return bytes(head + body)


class Tekken5DrMusicTest(unittest.TestCase):
    def test_every_listed_file_is_made(self):
        made = [p[0] for p in M.PIECES]
        self.assertEqual(len(made), len(set(made)))
        self.assertEqual(sorted(listed()), sorted(made))
        for name, number, sector, size, sha in M.PIECES:
            self.assertRegex(name, r'^t5dr-[a-z-]+\.bgm$')
            self.assertTrue(re.fullmatch('[0-9a-f]{64}', sha), name)

    def test_disabled_not_made(self):
        lines = (ADDON / 'data/jukebox/tekken5-dr.txt').read_text(encoding='utf-8').splitlines()
        off = sorted(l.split('=', 1)[0].strip() for l in lines if not l.startswith('#') and '--disable' in l)
        self.assertEqual(M.skipped(), off)
        image = iso_with(['PSP_GAME', 'USRDIR', 'TKPDATA1.BIN'], b'')
        with tempfile.TemporaryDirectory() as tmp:
            iso, out = Path(tmp) / 'dr.iso', Path(tmp) / 'out'
            iso.write_bytes(image)
            out.mkdir()
            (out / off[0]).write_bytes(b'T3BGM001')                    # made before: removed
            self.assertEqual(M.convert(iso, out, say=lambda _: None, skip=[p[0] for p in M.PIECES]), [])
            self.assertEqual(list(out.iterdir()), [])

    def test_iso_lookup(self):
        image = iso_with(['PSP_GAME', 'USRDIR', 'TKPDATA1.BIN'], b'hello')
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'dr.iso'
            path.write_bytes(image)
            with path.open('rb') as f:
                self.assertEqual(M.locate(f, M.ARCHIVE), (30, 5))
                self.assertIsNone(M.locate(f, '/PSP_GAME/USRDIR/TKPDATA0.BIN'))
            path.write_bytes(bytes(SECTOR * 20))
            with path.open('rb') as f, self.assertRaises(M.IsoError):
                M.locate(f, M.ARCHIVE)

    def test_channels_with_a_short_last_block(self):
        left, right = bytes(range(160)), bytes((x + 7) % 256 for x in range(160))   # blocks of 0x80 (0x40 a channel), then 0x40
        l, r, loop, volume = M.channels(npsf(left, right, 56, 900))
        self.assertEqual((l, r, loop, volume), (left, right, 56, 900))

    def test_make_writes_a_bgm(self):
        left = adpcm_block(12, 0, [3] * 28) * 4
        right = adpcm_block(12, 0, [5] * 28) * 4
        stream = npsf(left, right, 28, 500)
        image = iso_with(['PSP_GAME', 'USRDIR', 'TKPDATA1.BIN'], stream)
        piece = ('t5dr-test.bgm', 0, 0, len(stream), hashlib.sha256(stream).hexdigest())
        with tempfile.TemporaryDirectory() as tmp:
            iso, out = Path(tmp) / 'dr.iso', Path(tmp) / 't5dr-test.bgm'
            iso.write_bytes(image)
            self.assertEqual(M.make(str(iso), 30, piece, str(out)), out.name)
            data = out.read_bytes()
            frames, loop, gain = adpcm_pack.header(data)              # the NPSF's ADPCM, kept as it is
            self.assertEqual((frames, loop, gain), (4 * 28, 28, adpcm_pack.gain_of(M.LEVEL * 0.5)))
            self.assertEqual(data[adpcm_pack.HEAD:adpcm_pack.HEAD + 32], left[:16] + right[:16])
            self.assertEqual(list(adpcm_pack.decode(data)[:2]), [3 * gain >> 16, 5 * gain >> 16])
            bad = piece[:4] + ('0' * 64,)
            self.assertIsNone(M.make(str(iso), 30, bad, str(out)))

    def test_no_loop_plays_whole(self):
        left = adpcm_block(12, 0, [1] * 28)
        stream = npsf(left, left, -1, 1000)
        image = iso_with(['PSP_GAME', 'USRDIR', 'TKPDATA1.BIN'], stream)
        piece = ('t5dr-test.bgm', 0, 0, len(stream), hashlib.sha256(stream).hexdigest())
        with tempfile.TemporaryDirectory() as tmp:
            iso, out = Path(tmp) / 'dr.iso', Path(tmp) / 't5dr-test.bgm'
            iso.write_bytes(image)
            M.make(str(iso), 30, piece, str(out))
            self.assertEqual(struct.unpack_from('<I', out.read_bytes(), 16)[0], 0)


if __name__ == '__main__':
    unittest.main()
