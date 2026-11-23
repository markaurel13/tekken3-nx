"""tools/t2_remake_patch.py and tools/t2_remake_stages.py users_picture()
without a disc or the user's pictures: made-up pictures and keys."""
import hashlib, io, sys, tempfile, unittest, zlib
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))
try:
    import numpy as np
    from PIL import Image
    import t2_remake_patch as tp
    import t2_remake_stages as rs
except ImportError as error:                     # numpy, Pillow: the setup's python_packages
    raise unittest.SkipTest(str(error))

KEY_A, KEY_B = hashlib.sha256(b'a').digest(), hashlib.sha256(b'b').digest()


def picture(w=96, h=40, mode='RGB', seed=1):
    rng = np.random.default_rng(seed)
    shape = (h, w, 3) if mode == 'RGB' else (h, w)
    return Image.fromarray(rng.integers(0, 256, shape, dtype=np.uint8), mode)


class Patch(unittest.TestCase):
    def test_round_trip_to_the_pixel(self):
        for mode in ('RGB', 'L'):
            img = picture(mode=mode)
            data = tp.encode(img, 17, 'beach-panorama.png', [KEY_A])
            back = tp.decode(data, KEY_A)
            self.assertEqual((back.mode, back.size, back.tobytes()), (img.mode, img.size, img.tobytes()))

    def test_another_key_does_not_open_it(self):
        data = tp.encode(picture(), 17, 'beach-panorama.png', [KEY_A])
        with self.assertRaisesRegex(ValueError, 'does not open'):
            tp.decode(data, KEY_B)

    def test_each_disc_version_opens_it(self):
        img = picture()
        data = tp.encode(img, 17, 'beach-panorama.png', [KEY_A, KEY_B])
        self.assertEqual(tp.decode(data, KEY_A).tobytes(), img.tobytes())
        self.assertEqual(tp.decode(data, KEY_B).tobytes(), img.tobytes())

    def test_damage_is_found(self):
        data = bytearray(tp.encode(picture(), 17, 'beach-panorama.png', [KEY_A]))
        data[-10] ^= 1
        with self.assertRaises(ValueError):
            tp.decode(bytes(data), KEY_A)

    def test_payload_is_no_picture(self):
        data = tp.encode(picture(400, 200), 17, 'beach-panorama.png', [KEY_A])
        p = tp.header(data)
        self.assertNotIn(b'RIFF', p['payload'][:64])
        self.assertNotIn(b'WEBP', p['payload'][:64])
        self.assertGreaterEqual(len(zlib.compress(p['payload'], 9)), len(p['payload']) * 0.995)
        with tempfile.TemporaryDirectory() as tmp:
            good = Path(tmp) / 'beach-panorama.t2p'
            good.write_bytes(data)
            self.assertEqual(tp.check([good]), [])
            # the same picture unmixed: its payload a WebP as it is
            buf = io.BytesIO()
            picture(400, 200).save(buf, 'WEBP', lossless=True)
            plain = data[:len(data) - len(p['payload'])]
            bad = Path(tmp) / 'bad.t2p'
            bad.write_bytes(plain[:8] + plain[8:].replace(len(p['payload']).to_bytes(4, 'little'),
                                                           len(buf.getvalue()).to_bytes(4, 'little'), 1)
                            + buf.getvalue())
            self.assertTrue(any('picture as it is' in e for e in tp.check([bad])))

    def test_unknown_picture_is_refused(self):
        with tempfile.TemporaryDirectory() as tmp:
            f = Path(tmp) / 'x.t2p'
            f.write_bytes(tp.encode(picture(), 3, 'beach-panorama.png', [KEY_A]))     # Beach is stage 17
            self.assertTrue(tp.check([f]))

    def test_the_recipes_pictures(self):
        want = tp.wanted()
        self.assertEqual(len(want), 23)                 # 22 panoramas and Hong Kong's sign
        self.assertEqual(want['eternal-darkness-panorama.png'], 21)
        self.assertEqual(want['hong-kong-neon-text.png'], 5)
        self.assertEqual(want['kyoto-sunset-panorama.png'], 15)

    def test_the_patches_in_git(self):
        files = tp.patch_files()
        if not files:
            self.skipTest('no patches')
        self.assertEqual({tp.header(f.read_bytes())['name'] for f in files}, set(tp.wanted()))
        self.assertEqual(tp.check(), [])


class UsersPicture(unittest.TestCase):
    def frame_test(self, owner, size):
        cfg = rs.gen.DATA['arcade'][owner]
        hw, hh = rs.gen.DATA['frames'][owner].get('hd', (2000, 667))
        with tempfile.TemporaryDirectory() as tmp:
            picture(hw, hh, seed=3).save(Path(tmp) / cfg['panorama'])
            one = rs.users_picture(owner, size, Path(tmp))
            two = rs.users_picture(owner, size, Path(tmp))
        self.assertIsNotNone(one)
        (img, ext), (img2, _) = one, two
        self.assertEqual(img.shape, (size[1] + ext, size[0], 3))
        self.assertTrue(np.array_equal(img, img2))          # the same every time
        return ext

    def test_sky_above(self):                               # New York: the old picture reaches higher
        self.assertEqual(self.frame_test('bryan', (1536, 512)), 64)

    def test_taller_than_the_old(self):                     # Serene Forest: its crop reaches up
        self.assertEqual(self.frame_test('mokujin', (1536, 1024)), 0)

    def test_no_picture(self):
        with tempfile.TemporaryDirectory() as tmp:
            self.assertIsNone(rs.users_picture('bryan', (1536, 512), Path(tmp)))


if __name__ == '__main__':
    unittest.main()
