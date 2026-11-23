"""tools/t3_portraits.py: Tekken 3's LZ and a VS portrait TIM made up here; with
the Expanded's Tekken 3 disc beside the add-on, the 21 portraits from it."""
import struct, sys, tempfile, unittest, zlib
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))
sys.path.insert(0, str(ADDON / 'setup'))
import t3_portraits as t3  # noqa: E402


def packed(items):
    """Items (a byte: literal; (back, n): reference) in the game's LZ."""
    out = bytearray()
    for at in range(0, len(items), 7):
        group = items[at:at + 7]
        flags = 1 << len(group)                       # the highest set bit ends the group
        body = bytearray()
        for k, item in enumerate(group):
            if isinstance(item, int):
                flags |= 1 << k
                body.append(item)
            else:
                back, n = item
                body += struct.pack('>H', (n & 31) << 11 | (back & 0x7FF))
        out.append(flags); out += body
    return bytes(out + b'\0')


def tim(pal, pix):
    head = struct.pack('<2I', 0x10, 9) + struct.pack('<I4H', 12 + 512, 0, 0, 256, 1) + struct.pack('<256H', *pal)
    return head + struct.pack('<I4H', 12 + len(pix), 0, 0, t3.W // 2, t3.H) + bytes(pix)


def pixels(path):
    data = path.read_bytes()
    w, h = struct.unpack_from('>2I', data, 16)
    idat = data[33 + 8:-12 - 4]
    raw = zlib.decompress(idat)
    return w, h, b''.join(raw[r * (w * 3 + 1) + 1:(r + 1) * (w * 3 + 1)] for r in range(h))


class T3PortraitsTest(unittest.TestCase):
    def test_unpack_literals_and_references(self):
        items = [1, 2, 3, 4, (4, 6), (2048, 0)]       # 0: 32 bytes; back 2048: 0 in the field
        prefix = bytes(range(256)) * 8                # 2048 bytes for the far reference
        data = packed(list(prefix) + items)
        out = t3.unpack(data)
        self.assertEqual(out[:2048], prefix)
        self.assertEqual(out[2048:2052], bytes([1, 2, 3, 4]))
        self.assertEqual(out[2052:2058], bytes([1, 2, 3, 4, 1, 2]))
        self.assertEqual(out[2058:2090], (prefix + bytes([1, 2, 3, 4, 1, 2]))[2058 - 2048:2058 - 2048 + 32])
        self.assertEqual(len(out), 2090)

    def test_a_reference_before_the_start_is_refused(self):
        with self.assertRaises(ValueError):
            t3.unpack(packed([(1, 3)]))

    def test_portrait_bands_their_palette_quarters(self):
        pal = [0] + [0x8000 | (q * 64 + i) for q in range(4) for i in range(64)][1:]   # 0: see-through
        pix = [0 if (x, y) == (0, 0) else 5 for y in range(t3.H) for x in range(t3.W)]
        rgb = t3.portrait(tim(pal, pix))
        self.assertEqual(rgb[:3], bytes(t3.KEY_COLOUR))
        for y in (1, 64, 128, 251):
            c = pal[y // 64 * 64 + 5]
            want = bytes(((c & 31) * 255 // 31, (c >> 5 & 31) * 255 // 31, (c >> 10 & 31) * 255 // 31))
            self.assertEqual(rgb[(y * t3.W + 3) * 3:(y * t3.W + 3) * 3 + 3], want)

    def test_portrait_refuses_other_pictures(self):
        with self.assertRaises(ValueError):
            t3.portrait(struct.pack('<2I', 0x10, 8) + bytes(40))

    def test_png_is_the_caches(self):
        rgb = bytes((x * 7 + y) & 255 for y in range(t3.H) for x in range(t3.W * 3))
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'law.png'
            t3.png(path, rgb)
            self.assertEqual(path.read_bytes()[24:26], bytes([8, 2]))      # 8-bit RGB, as tmst_png reads
            self.assertEqual(pixels(path), (t3.W, t3.H, rgb))

    def test_setup_runs_it(self):
        import tmst_setup
        names = [s.__name__ for s in tmst_setup.STEPS]
        self.assertIn('tekken3_portraits', names)
        import tmst_gui
        self.assertIn('tekken3_portraits', tmst_gui.STEP_TEXT)

    def test_the_expanded_disc(self):
        disc = t3.find_disc(ADDON.parents[1])
        if not disc:
            self.skipTest('no Tekken 3 disc beside the add-on')
        with tempfile.TemporaryDirectory() as tmp:
            names = t3.convert(disc, Path(tmp), say=lambda _: None)
            self.assertEqual(names, [f'{k}.png' for k in t3.KEYS])
            w, h, rgb = pixels(Path(tmp) / 'law.png')
            self.assertEqual((w, h), (t3.W, t3.H))
            self.assertIn(bytes(t3.KEY_COLOUR), {rgb[i:i + 3] for i in range(0, len(rgb), 3)})


if __name__ == '__main__':
    unittest.main()
