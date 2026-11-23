"""tools/adpcm_pack.py (the ADPCM packs the setup writes) and
src/jukebox/tmst_adpcm.c (the Jukebox's decoding of them), built alone with
the C compiler at hand (skipped without one): the same frames, the loop too."""
import os, random, shutil, struct, subprocess, sys, tempfile, unittest
from array import array
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))
import adpcm_pack as P  # noqa: E402


def compiler():
    for name in (os.environ.get('CC'), 'clang', 'gcc', 'cc'):
        if name and shutil.which(name):
            return shutil.which(name)
    return None


def block(shift: int, filt: int, nibbles) -> bytes:
    return bytes([filt << 4 | shift, 0]) + bytes(nibbles[i] | nibbles[i + 1] << 4 for i in range(0, 28, 2))


def noise(blocks: int, seed: int) -> bytes:
    """Random blocks: every shift (13..15 too) and filter (5..15 too)."""
    r = random.Random(seed)
    return b''.join(bytes([r.randrange(256), 0]) + bytes(r.randrange(256) for _ in range(14)) for _ in range(blocks))


class AdpcmPackTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.dir = Path(self.tmp.name)

    def tearDown(self):
        self.tmp.cleanup()

    def test_write_and_decode(self):
        left = block(12, 0, [1, 15] + [0] * 26) + block(12, 1, [0] * 28)      # 1, -1, then 60/64 of the last
        right = block(0, 0, [7] * 28) * 2 + b'\0' * 16                          # the longer channel: cut
        path = self.dir / 'a.bgm'
        self.assertEqual(P.write(path, left, right, 30, 0.5), 56)
        data = path.read_bytes()
        self.assertEqual(data[:8], P.MAGIC)
        self.assertEqual(P.header(data), (56, 30, 32768))
        self.assertEqual(len(data), P.HEAD + 2 * 32)
        self.assertEqual(data[P.HEAD:P.HEAD + 16], left[:16])                   # the game's bytes as they are
        self.assertEqual(data[P.HEAD + 16:P.HEAD + 32], right[:16])
        pcm = P.decode(data)
        self.assertEqual(list(pcm[0:6:2]), [0, -1, 0])                          # 1 >> 1, -1 >> 1 (floor), 0
        self.assertEqual(pcm[1], 7 << 12 >> 1)
        P.write(path, left, right, 56, 1.0)                                     # a loop not inside: 0
        self.assertEqual(P.header(path.read_bytes())[1], 0)

    def build(self) -> Path:
        exe = self.dir / 'adpcm_main.exe'
        libm = [] if sys.platform == 'win32' else ['-lm']
        subprocess.run([compiler(), '-O2', '-Wall', '-Werror', '-I', str(ADDON / 'src/jukebox'),
                        str(ADDON / 'tests/c/adpcm_main.c'), str(ADDON / 'src/jukebox/tmst_adpcm.c'), '-o', str(exe),
                        *libm], check=True)
        return exe

    def run_c(self, exe: Path, path: Path, frames: int, chunk: int) -> list:
        raw = self.dir / 'p.raw'
        subprocess.run([str(exe), str(path), str(raw), str(frames), str(chunk)], check=True)
        got = array('h', raw.read_bytes())
        if sys.byteorder != 'little':
            got.byteswap()
        return list(got)

    @unittest.skipUnless(compiler(), 'no C compiler')
    def test_c_plays_xa_as_the_setup_measured_it(self):
        """An XA pack: the C's 44 100 Hz frames are tools/tekken2_music.py's
        decode_groups() and resample() at the pack's gain (a sample's rounding
        apart), twice through (it loops whole)."""
        try:
            import numpy as np
            import tekken2_music as T
        except ImportError:
            self.skipTest('numpy')
        exe = self.build()
        r = random.Random(7)
        groups = bytearray()
        for _ in range(9):
            g = bytearray(r.randrange(256) for _ in range(128))
            g[4:12] = bytes(r.randrange(4) << 4 | r.randrange(16) for _ in range(8))   # every shift, filters 0..3
            groups += g
        path = self.dir / 'x.bgm'
        self.assertEqual(P.write_xa(path, bytes(groups), 0.37), 9 * 112)
        frames, gain = P.xa_header(path.read_bytes())
        self.assertEqual(path.read_bytes()[P.HEAD:], bytes(groups))             # the disc's bytes as they are
        y = T.resample(T.decode_groups(bytes(groups))) * (gain / 65536)
        want = np.clip(np.where(y >= 0, np.floor(y + 0.5), -np.floor(-y + 0.5)), -32768, 32767).astype(int)
        self.assertEqual(len(want), 9 * 112 * 7 // 6)
        want = np.concatenate([want, want]).reshape(-1)
        for chunk in (1, 333, 2048):
            got = np.array(self.run_c(exe, path, len(want) // 2, chunk))
            self.assertLessEqual(int(np.abs(got - want).max()), 1, chunk)

    @unittest.skipUnless(compiler(), 'no C compiler')
    def test_c_decodes_the_same(self):
        exe = self.build()
        for loop, gain, seed in ((0, 1.0, 1), (100, 0.6, 2), (28 * 7, 1.7, 3), (28 * 40 - 1, 0.25, 4)):
            path = self.dir / 'p.bgm'
            P.write(path, noise(40, seed), noise(41, seed + 10), loop, gain)
            data = path.read_bytes()
            frames = P.header(data)[0]
            full = P.decode(data)
            want = list(full) + list(full[2 * loop:]) * 3                       # the end, then the loop again
            for chunk in (28, 100, 2048):
                got = self.run_c(exe, path, len(want) // 2, chunk)
                self.assertEqual(got, want, (loop, gain, chunk, frames))


if __name__ == '__main__':
    unittest.main()
