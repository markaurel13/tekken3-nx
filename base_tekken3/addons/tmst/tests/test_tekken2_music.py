"""tools/tekken2_music.py without a disc: its pieces against the title lists,
the tables, the sound groups and the .bgm it writes."""
import hashlib, struct, sys, tempfile, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))           # by name: its worker processes import it again
import tekken2_music  # noqa: E402


def names(file: str) -> list[str]:
    lines = (ADDON / 'data/jukebox' / file).read_text(encoding='utf-8').splitlines()
    return [l.split('=', 1)[0].strip() for l in lines if l.strip() and not l.startswith('#')]


class FakeDisc:
    def __init__(self, exe: bytes, sectors: dict):
        self.exe, self.sectors = exe, sectors

    def read_file(self, name):
        return self.exe if name == 'SLUS_002.13' else None

    def raw(self, lba):
        return self.sectors.get(lba, bytes(2352))


class Tekken2MusicTest(unittest.TestCase):
    def test_every_listed_file_is_made(self):
        made = {(folder, name) for folder, name, _ in tekken2_music.pieces()}
        for folder in ('console', 'arcade'):
            for game in ('tekken1', 'tekken2'):
                for name in names(f'{game}-{folder}.txt'):
                    self.assertIn((folder, name), made, f'{game}-{folder}.txt')
        self.assertEqual(len(made), len(tekken2_music.pieces()))

    def test_tables_are_checked(self):
        exe = bytearray(0x2400)
        struct.pack_into('<3I', exe, tekken2_music.XA_TABLE + 12 * 3, 3, 99, 3)
        struct.pack_into('<BBhBBh', exe, tekken2_music.MUSIC_TABLE + 8 * 5, 0, 70, 3, 1, 120, 0)
        with self.assertRaisesRegex(Exception, 'not the ones known'):
            tekken2_music.tables(FakeDisc(bytes(exe), {}))
        known = tekken2_music.TABLES_SHA256
        tekken2_music.TABLES_SHA256 = (
            hashlib.sha256(exe[tekken2_music.XA_TABLE:tekken2_music.XA_TABLE + 12 * tekken2_music.XA_TRACKS]).hexdigest(),
            hashlib.sha256(exe[tekken2_music.MUSIC_TABLE:tekken2_music.MUSIC_TABLE + 8 * tekken2_music.MUSIC_IDS]).hexdigest())
        try:
            tracks, music = tekken2_music.tables(FakeDisc(bytes(exe), {}))
        finally:
            tekken2_music.TABLES_SHA256 = known
        self.assertEqual(tracks[3], (3, 99, 3))
        self.assertEqual((music[5, 0], music[5, 1]), ((0, 70, 3), (1, 120, 0)))

    def test_groups_of_the_channel(self):
        def sector(channel, fill, audio=True, coding=1):
            raw = bytearray(2352)
            raw[17], raw[18], raw[19] = channel, 0x64 if audio else 0x08, coding
            raw[24:24 + 2304] = bytes([fill]) * 2304
            return bytes(raw)
        disc = FakeDisc(b'', {100 + 2: sector(2, 1), 100 + 10: sector(2, 2), 100 + 18: sector(5, 3),
                              100 + 26: sector(2, 4, audio=False)})
        self.assertEqual(tekken2_music.groups(disc, 100, (2, 26, 2)), (bytes([1]) * 2304 + bytes([2]) * 2304, True))
        disc.sectors[100 + 10] = sector(2, 2, coding=5)                     # 18 900 Hz: not kept as it is
        self.assertFalse(tekken2_music.groups(disc, 100, (2, 26, 2))[1])

    def test_decode_and_write(self):
        group = bytearray(128)
        group[4:12] = bytes([12, 12, 12, 12, 12, 12, 12, 12])               # filter 0, shift 12
        group[16:] = bytes([0xF1]) * 112                                    # left nibble 1, right 15 (-1)
        pcm = tekken2_music.decode_groups(bytes(group))
        self.assertEqual(pcm.shape, (112, 2))
        self.assertEqual((pcm[0, 0], pcm[0, 1]), (1.0, -1.0))
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 't2-paul.bgm'
            tekken2_music.write(path, tekken2_music.resample(pcm * 1000))
            data = path.read_bytes()
        frames = 112 * 44100 // 37800
        self.assertEqual(data[:20], b'T3BGM001' + struct.pack('<3I', 44100, frames, 0))
        self.assertEqual(len(data), 20 + 4 * frames)


if __name__ == '__main__':
    unittest.main()
