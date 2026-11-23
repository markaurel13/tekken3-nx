"""tools/ttt_arcade_music.py and tools/ttm_render.c, on a small music.ttm of
our own (skipped without a C compiler): one piece, one voice playing a bank
of constant samples, looping."""
import struct, sys, tempfile, unittest, zlib
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))
import ttt_arcade_music  # noqa: E402

LOOP, SEAM, FADE = 4000, 12000, 100


def body() -> bytes:
    out = struct.pack('<4I', 0, 0, 1, 1) + bytes([0, 0, 0, 0])            # one bank, number 0
    init = [(0, 0x7F7F), (2, 0x4000), (4, 0), (5, 0), (6, 0xFFFF), (7, 0), (3, 0x4002)]   # vol, freq, start..loop, KEYON|LOOP (bit 15 never set at init)
    out += struct.pack('<6I', 0x11, LOOP, SEAM, FADE, len(init), 1)
    out += b''.join(struct.pack('<HH', r, v) for r, v in init)
    out += struct.pack('<IHHH', 1, 0x202, 0, 0xFFFF)                       # key on, one sample in
    return out + bytes([0x40]) * 65536                                     # linear 0x4000


class TttArcadeMusicTest(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        self.pack = self.root / ttt_arcade_music.PACKS[0]
        self.pack.parent.mkdir(parents=True)
        raw = body()
        self.pack.write_bytes(b'TTTM' + struct.pack('<II', 1, len(raw)) + zlib.compress(raw))

    def tearDown(self) -> None:
        self.tmp.cleanup()

    @unittest.skipUnless(ttt_arcade_music.compiler(), 'no C compiler')
    def test_pack_to_bgm(self) -> None:
        out = self.root / ttt_arcade_music.OUT
        names = ttt_arcade_music.convert(self.pack, out, ttt_arcade_music.compiler(), say=lambda _: None)
        self.assertEqual(names, ['arcade-paul.bgm'])
        data = (out / 'arcade-paul.bgm').read_bytes()
        magic, rate, frames, loop = data[:8], *struct.unpack_from('<III', data, 8)
        self.assertEqual((magic, rate), (b'T3BGM001', 44100))
        self.assertEqual(len(data), 20 + 4 * frames)
        self.assertLessEqual(abs(loop - LOOP // 2), 8)
        self.assertEqual(frames - loop, (SEAM - LOOP) // 2)
        left, right = struct.unpack_from('<2h', data, 20 + 4 * (frames - 1))
        self.assertGreater(left, 0)                                         # the voice sounds to the end
        self.assertEqual(left, right)

    def test_not_a_pack(self) -> None:
        self.pack.write_bytes(b'nope')
        with self.assertRaises(ValueError):
            ttt_arcade_music.inflate(self.pack)


if __name__ == '__main__':
    unittest.main()
