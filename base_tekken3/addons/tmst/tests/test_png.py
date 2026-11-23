"""src/core/tmst_png.c, built alone with the C compiler at hand (skipped
without one), its files decoded with Python's zlib."""
import os, shutil, struct, subprocess, tempfile, unittest, zlib
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]


def compiler():
    for name in (os.environ.get('CC'), 'clang', 'gcc', 'cc'):
        if name and shutil.which(name):
            return shutil.which(name)
    return None


def pattern(kind, w, h):
    """The same pixels as tests/c/png_main.c, as RGB rows."""
    rows, seed = [], 12345
    for y in range(h):
        row = bytearray()
        for x in range(w):
            if kind == 0:
                v = (x * 255 // max(w - 1, 1)) << 16 | (y * 255 // max(h - 1, 1)) << 8 | ((x ^ y) & 255)
            elif kind == 1:
                seed = (seed * 1103515245 + 12345) & 0xFFFFFFFF
                v = seed >> 8
            else:
                v = 0xFFFFFF if (x // 8 + y // 8) & 1 else 0x102030
            row += bytes(((v >> 16) & 255, (v >> 8) & 255, v & 255))
        rows.append(bytes(row))
    return rows


def decode(data):
    """RGB rows of an 8-bit RGB PNG, checking every chunk's CRC."""
    assert data[:8] == b'\x89PNG\r\n\x1a\n'
    at, idat, w, h = 8, b'', None, None
    while at < len(data):
        n, kind = struct.unpack('>I4s', data[at:at + 8])
        body = data[at + 8:at + 8 + n]
        assert struct.unpack('>I', data[at + 8 + n:at + 12 + n])[0] == zlib.crc32(kind + body)
        if kind == b'IHDR':
            w, h, depth, colour = struct.unpack('>IIBB', body[:10])
            assert (depth, colour) == (8, 2)
        elif kind == b'IDAT':
            idat += body
        at += 12 + n
    raw, stride, rows, up = zlib.decompress(idat), w * 3, [], bytes(w * 3)
    for y in range(h):
        f, line = raw[y * (stride + 1)], bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        for i in range(stride):
            a = line[i - 3] if i >= 3 else 0
            b, c = up[i], up[i - 3] if i >= 3 else 0
            if f == 1: line[i] = (line[i] + a) & 255
            elif f == 2: line[i] = (line[i] + b) & 255
            elif f == 3: line[i] = (line[i] + (a + b) // 2) & 255
            elif f == 4:
                p = a + b - c; pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                line[i] = (line[i] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 255
        rows.append(bytes(line)); up = bytes(line)
    return w, h, rows


@unittest.skipUnless(compiler(), 'no C compiler')
class PngTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.exe = Path(cls.tmp.name) / 'png_main.exe'
        subprocess.run([compiler(), '-O2', '-I', str(ADDON / 'src/core'), str(ADDON / 'tests/c/png_main.c'),
                        str(ADDON / 'src/core/tmst_png.c'), '-o', str(cls.exe)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def roundtrip(self, kind, w, h):
        path = Path(self.tmp.name) / f'p{kind}_{w}x{h}.png'
        subprocess.run([str(self.exe), str(path), str(w), str(h), str(kind)], check=True)
        data = path.read_bytes()
        self.assertEqual(decode(data), (w, h, pattern(kind, w, h)))
        return len(data)

    def test_gradient(self):
        self.roundtrip(0, 368, 480)

    def test_noise(self):
        self.roundtrip(1, 97, 33)

    def test_tiles_compress(self):
        self.assertLess(self.roundtrip(2, 736, 480), 736 * 480 * 3 // 20)

    def test_one_pixel(self):
        self.roundtrip(0, 1, 1)

    def read(self, data):
        src, out = Path(self.tmp.name) / 'in.png', Path(self.tmp.name) / 'out.raw'
        src.write_bytes(data)
        r = subprocess.run([str(self.exe), 'r', str(src), str(out)])
        if r.returncode: return None
        raw = out.read_bytes()
        w, h = struct.unpack('<II', raw[:8])
        return w, h, list(struct.unpack('<%dI' % (w * h), raw[8:]))

    def test_reader(self):
        """Every colour type the reader takes, each filter, stored / fixed / dynamic deflate."""
        w, h, seed = 37, 23, 7
        argb = []
        for i in range(w * h):
            seed = (seed * 1103515245 + 12345) & 0xFFFFFFFF
            v = seed >> 8 if i % 3 else (i * 977) & 0xFFFFFF
            argb.append((((i * 13) & 255) << 24) | v)
        def chunk(kind, body):
            return struct.pack('>I', len(body)) + kind + body + struct.pack('>I', zlib.crc32(kind + body))
        def png(colour, rows, level, extra=b''):
            ihdr = struct.pack('>IIBBBBB', w, h, 8, colour, 0, 0, 0)
            return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', ihdr) + extra +
                    chunk(b'IDAT', zlib.compress(b''.join(rows), level)) + chunk(b'IEND', b''))
        def filtered(lines, bpp):
            out, up = [], bytes(len(lines[0]))
            for y, line in enumerate(lines):
                f, enc = y % 5, bytearray()
                for i, x in enumerate(line):
                    a = line[i - bpp] if i >= bpp else 0
                    b, c = up[i], up[i - bpp] if i >= bpp else 0
                    p = a + b - c; pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                    pr = a if pa <= pb and pa <= pc else b if pb <= pc else c
                    enc.append((x - (0, a, b, (a + b) // 2, pr)[f]) & 255)
                out.append(bytes([f]) + bytes(enc)); up = line
            return out
        px = [argb[y * w:(y + 1) * w] for y in range(h)]
        cases = {
            6: ([b''.join(bytes((p >> 16 & 255, p >> 8 & 255, p & 255, p >> 24)) for p in r) for r in px], 4,
                lambda p: p),
            2: ([b''.join(bytes((p >> 16 & 255, p >> 8 & 255, p & 255)) for p in r) for r in px], 3,
                lambda p: 0xFF000000 | p & 0xFFFFFF),
            0: ([bytes(p & 255 for p in r) for r in px], 1, lambda p: 0xFF000000 | (p & 255) * 0x010101),
            4: ([b''.join(bytes((p & 255, p >> 24)) for p in r) for r in px], 2,
                lambda p: (p >> 24) << 24 | (p & 255) * 0x010101),
        }
        for colour, (lines, bpp, want) in cases.items():
            for level in (0, 1, 9):
                self.assertEqual(self.read(png(colour, filtered(lines, bpp), level)),
                                 (w, h, [want(p) for p in argb]), (colour, level))
        pal = bytes((i * 7) & 255 for i in range(768))
        trns = bytes(255 - i for i in range(256))
        idx = [bytes((p >> 8) & 255 for p in r) for r in px]
        got = self.read(png(3, filtered(idx, 1), 6, chunk(b'PLTE', pal) + chunk(b'tRNS', trns)))
        self.assertEqual(got, (w, h, [(255 - (p >> 8 & 255)) << 24 | pal[(p >> 8 & 255) * 3] << 16 |
                                      pal[(p >> 8 & 255) * 3 + 1] << 8 | pal[(p >> 8 & 255) * 3 + 2] for p in argb]))
        self.assertIsNone(self.read(b'not a png'))


if __name__ == '__main__':
    unittest.main()
