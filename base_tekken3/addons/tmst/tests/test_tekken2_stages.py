"""tools/tekken2_stages.py without a disc: a stage made up here (its TIMs and
record as TEKKEN2.BNS holds them) through the pack, read back as the engine
reads it."""
import struct, sys, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))
import tekken2_stages as t2  # noqa: E402


def tim(x, y, w, h, words, clut=None):
    """A 4-bit TIM: its palette at clut (x, y), its image at x, y (w halfwords)."""
    out = struct.pack('<2I', 0x10, 8 if clut else 0)
    if clut:
        out += struct.pack('<I4H', 12 + 32, clut[0], clut[1], 16, 1) + struct.pack('<16H', *clut[2])
    return out + struct.pack('<I4H', 12 + 2 * w * h, x, y, w, h) + struct.pack(f'<{w * h}H', *words)


def packed(indices):
    return [indices[i] | indices[i + 1] << 4 | indices[i + 2] << 8 | indices[i + 3] << 12
            for i in range(0, len(indices), 4)]


PANO_CLUTS = [tuple(0x8000 | (k * 2 + n) for k in range(16)) for n in range(2)]
FLOOR_CLUT = tuple(0x8000 | 0x400 * k for k in range(16))


def pano_index(cell, u, v):
    return (u // 8 + v // 4 + cell) % 15 + 1


def stage_files(with_pano=True):
    """(textures, record): a panorama of 2 cells (64 x 32, at u 0 and 64 of the
    page at VRAM 512, 0), the map's places alternating them; a floor tile at
    VRAM 960, 0 with one bright texel at its top left, every tile its turn
    (i % 4)."""
    tex = b''
    if with_pano:
        idx = [pano_index(u // 64, u % 64, v) for v in range(32) for u in range(128)]
        words = packed(idx)
        tex += tim(512, 0, 32, 32, words, (0, 480, PANO_CLUTS[0]))
        tex += tim(1000, 500, 1, 1, [0], (16, 480, PANO_CLUTS[1]))
    floor_idx = [15 if (u, v) == (0, 0) else 1 for v in range(64) for u in range(64)]
    tex += tim(960, 0, 16, 64, packed(floor_idx), (0, 492, FLOOR_CLUT))
    floor = b''.join(struct.pack('<4H', 0, 15 | (i % 4) << 14, 0, 492 << 6) for i in range(100))
    cells = b''.join(struct.pack('<3H', u, (480 << 6) | n, 8) for n, u in enumerate((0, 64))) if with_pano else b''
    places = bytes([0, 1] * 12) if with_pano else b''
    o0 = 4 + 40
    o1 = o0 + 4 + len(floor)
    o2 = o1 + len(cells)
    o3 = o2 + 2 * len(places)
    rec = struct.pack('<I', 1) + struct.pack('<10I', o0, o1, o2, o3, o3, o3, o3, o3, o3, o3)
    rec += struct.pack('<I', 0x40) + floor + cells + bytes([0] * 24) + places
    return tex, rec


class FakeSource:
    def __init__(self, files):
        self.files = files

    def entry(self, i):
        return self.files[i]


def source(stage, with_pano=True):
    tex, rec = stage_files(with_pano)
    group = t2.GROUP0 + t2.GROUP * t2.FIGHTER_OF[stage]
    return FakeSource({group: tex, group + 1: rec})


class Tekken2StagesTest(unittest.TestCase):
    def test_every_stage_has_a_fighter(self):
        self.assertEqual(sorted(t2.FIGHTER_OF), list(range(len(t2.NAMES))))
        self.assertEqual(len(set(t2.FIGHTER_OF.values())), len(t2.NAMES))
        for stage in range(len(t2.NAMES)):
            lay = t2.layout(stage)
            self.assertEqual(2880 % (len(lay['places']) * lay['repeat']), 0, stage)
            self.assertLessEqual(t2.NUMBER0 + stage, 99)        # stage%02d

    def test_disc_data(self):
        st = t2.stage_data(source(0), 0)
        self.assertEqual((len(st['pano']), len(st['pano'][0])), (1, 24))
        idx, colours = st['pano'][0][1]
        self.assertEqual(colours, PANO_CLUTS[1])
        self.assertEqual(idx[3 * 64 + 10], pano_index(1, 10, 3))
        self.assertEqual(len(st['floor']), 100)
        self.assertEqual([f[2] for f in st['floor'][:5]], [0, 1, 2, 3, 0])
        self.assertEqual(st['floor'][0][0][0], 15)
        self.assertEqual(st['floor'][0][3], 492 << 6)

    def test_pack_as_the_engine_reads_it(self):
        pack = t2.build(t2.stage_data(source(2), 2))
        magic, version, number, n, size = struct.unpack_from('<4s2H2I', pack, 0)
        self.assertEqual((magic, version, number, size), (b'ASTG', 3, 22, len(pack)))
        s = t2.sections(pack)
        far = s['FARB']
        cols, rows, ring, yoff, degrees, pitch = struct.unpack_from('<6h', far, 0)
        # one row of cells, its end at y 160, then the ground's rows
        self.assertEqual((cols, rows, ring, yoff, degrees * ring), (24, 1 + t2.GROUND_ROWS, 48, 160 - 32, 2880))
        self.assertEqual((far[18], far[22]), (1, 1))                 # top and bottom bands on
        kinds = far[24 + 4 * cols * rows:]
        self.assertEqual(kinds[:ring * rows], bytes([0x31]) * ring + bytes([0x64]) * (ring * t2.GROUND_ROWS))
        # a cell word as the engine's far_cell reads it: its slot's page and uv
        d = struct.unpack_from('<H', far, 24)[0]
        self.assertEqual((d & 3, d >> 2 & 3, d >> 8 & 15, d >> 12 & 3), (0, 0, 0, 2))
        flor = s['FLOR']
        self.assertEqual(struct.unpack_from('<2hi', flor, 0), t2.FLOOR_PLAIN)
        self.assertEqual(len(flor), 5320)                           # as Tekken 3's
        cells = struct.unpack_from('<200I', flor, 8)
        self.assertEqual([w >> 30 for w in cells[0:8:2]], [0, 1, 2, 3])   # Tekken 2's turns
        o = 8 + 800 + 3200 + 100
        nv = struct.unpack_from('<H', flor, o)[0]
        o += (2 + 4 * nv + 3) & ~3
        self.assertEqual((nv, flor[o:o + 100]), (121, bytes([1]) * 100))
        quads = struct.unpack_from('<100I', flor, o + 100)
        verts = struct.unpack_from(f'<{2 * nv}h', flor, 8 + 800 + 3200 + 100 + 2)
        corner = lambda q, k: (verts[2 * (q >> 8 * k & 255)], verts[2 * (q >> 8 * k & 255) + 1])
        self.assertEqual([corner(quads[55], k) for k in range(4)], [(0, 2500), (2500, 2500), (0, 0), (2500, 0)])
        self.assertEqual(len(s['TRIG']), 18176)
        sine = struct.unpack_from('<4096h', s['TRIG'])
        cosine = struct.unpack_from('<4096h', s['TRIG'], 8192)
        self.assertEqual((sine[0], sine[1024], cosine[0], cosine[2048]), (0, 4096, 4096, -4096))
        self.assertEqual(struct.unpack_from('<2h', s['TRIG'], 16384), (4096, 4127))   # SquareRoot0's
        self.assertEqual(struct.unpack_from('<2H', s['T2FX']), (t2.NO_DIM, 0))
        self.assertEqual(struct.unpack_from('<2H', s['T2LT']), (t2.DAY_ARENA, 0))      # daylight on the fighters
        self.assertNotIn('T2CH', s)
        # everything in the upper half
        t2.upper_half(s['VRAM'])

    def test_pictures_read_back(self):
        pack = t2.build(t2.stage_data(source(2), 2))
        pics = t2.pictures(pack)
        w, h, img = pics['panorama']
        self.assertEqual((w, h), (24 * 64, 32))
        for place, x, y in ((0, 5, 7), (1, 40, 30), (3, 63, 0)):
            c = PANO_CLUTS[place % 2][pano_index(place % 2, x, y)]
            o = 3 * (y * w + place * 64 + x)
            self.assertEqual(img[o:o + 3], t2.rgb(c), (place, x, y))
        w, h, img = pics['floor']
        self.assertEqual((w, h), (1280, 1280))
        # tile i = row * 10 + column, turned (i % 4) quarters clockwise: its
        # bright texel (smoothed in the halves) goes from the cell's top left
        # to its top right, bottom right, bottom left
        for i, (qx, qy) in enumerate(((0, 0), (1, 0), (1, 1), (0, 1))):
            best = max(((sum(img[3 * (y * w + x):3 * (y * w + x) + 3]), x, y)
                        for y in range(128) for x in range(i * 128, i * 128 + 128)))
            self.assertEqual(((best[1] - i * 128) // 64, best[2] // 64), (qx, qy), i)

    def test_dark_room(self):
        pack = t2.build(t2.stage_data(source(22, with_pano=False), 22))
        s = t2.sections(pack)
        self.assertEqual(struct.unpack_from('<6h', s['FARB'], 0), (1, 8, 1, 0, 45, 0))
        self.assertEqual(s['FARB'][24 + 32:24 + 40], bytes([0x74]) * 8)   # black cells, the whole screen
        self.assertEqual(set(t2.pictures(pack)), {'floor'})
        self.assertEqual(struct.unpack_from('<2hi', s['FLOR'], 0), t2.FLOOR_LIT)
        self.assertEqual(struct.unpack_from('<2H', s['T2FX'])[0], t2.DARK['dim'])
        self.assertNotIn('T2LT', s)                                                     # a dark stage: arena 9's
        self.assertEqual(struct.unpack_from('<6i', s['T2MR']), t2.layout(22)['mirror'])

    def test_running_lights(self):
        # Hong Kong's own (issue #12): the disc's colours turned round, as Tekken 2 turns them
        self.assertEqual(t2.turned([0, 1, 2, 3, 4], (1, 2, 3), 1), [0, 2, 3, 1, 4])
        hk = t2.layout(5)['chase']
        self.assertEqual((hk['period'], hk['add'], hk['dots']), (16384, 1706, (10, 11, 12, 13)))
        self.assertNotIn('colour', hk)
        st = t2.stage_data(source(5), 5)
        st['layout'] = dict(st['layout'], chase=dict(cluts=(492 << 6,), dots=(1, 15), period=16384, add=1706),
                            blend=None)
        s = t2.sections(t2.build(st))
        period, frames, count, add = struct.unpack_from('<4H', s['T2CH'])
        self.assertEqual((period, frames, count, add), (16384, 2, 1, 1706))
        x, y = struct.unpack_from('<2H', s['T2CH'], 8)
        first = struct.unpack_from('<16H', s['T2CH'], 12)
        second = struct.unpack_from('<16H', s['T2CH'], 44)
        self.assertEqual(first, tuple(FLOOR_CLUT))                      # the disc's own first: no colour of ours
        self.assertEqual((second[1], second[15]), (FLOOR_CLUT[15], FLOOR_CLUT[1]))
        self.assertEqual(second[2:15], tuple(FLOOR_CLUT[2:15]))
        self.assertNotIn('T2BL', s)
        # the floor's cells and halves use that one palette, at x, y: the runtime changes it there
        clut = t2.clut_word(x, y)
        flor = s['FLOR']
        cells = struct.unpack_from('<200I', flor, 8)
        halves = struct.unpack_from('<800I', flor, 808)
        self.assertEqual({w >> 16 for w in cells[1::2]} | {w >> 16 for w in halves[1::2]}, {clut})

    def test_floor_fade(self):
        # issue #12: Hong Kong's 0x7B04 towards 0x7B09; here the floor's palette towards the panorama's second
        to = 480 << 6 | 1
        st = t2.stage_data(source(5), 5)
        st['layout'] = dict(st['layout'], chase=None, blend=dict(clut=492 << 6, to=to, first=0, last=14, add=17))
        st['blend_to'] = t2.palette(t2.vram_of(source(5).files[t2.GROUP0 + t2.GROUP * t2.FIGHTER_OF[5]]), to)
        s = t2.sections(t2.build(st))
        self.assertNotIn('T2CH', s)
        count, first, last, add = struct.unpack_from('<4H', s['T2BL'])
        self.assertEqual((count, first, last, add), (1, 0, 14, 17))
        x, y = struct.unpack_from('<2H', s['T2BL'], 8)
        self.assertEqual(struct.unpack_from('<16H', s['T2BL'], 12), tuple(FLOOR_CLUT))
        self.assertEqual(struct.unpack_from('<16H', s['T2BL'], 44), tuple(PANO_CLUTS[1]))
        # the cells and the halves keep the disc's indices on that one palette
        clut = t2.clut_word(x, y)
        flor = s['FLOR']
        cells = struct.unpack_from('<200I', flor, 8)
        halves = struct.unpack_from('<800I', flor, 808)
        self.assertEqual({w >> 16 for w in cells[1::2]} | {w >> 16 for w in halves[1::2]}, {clut})
        # Hong Kong's own: the disc's slot 2
        self.assertEqual(t2.layout(5)['blend'], dict(clut=0x7B04, to=0x7B09, first=0, last=14, add=17))

    def test_upper_half_room(self):
        """The slots and palettes never on Unknown's wolf or TTT Hitspark's
        fixed cells; a full pack leaves the cells of 16 x 64 that the common
        sparks (37) and the PS2 lasers (24) are placed in round it."""
        def cells(x, y, w, h):
            return {(cx, cy) for cx in range(x // 16, (x + w - 1) // 16 + 1) for cy in range(y // 64, (y + h - 1) // 64 + 1)}
        fixed = cells(*t2.WOLF).union(*(cells(*r) for r in t2.HIT_EFFECT_CELLS))
        slots = set().union(*(cells(x, y, 16, 64) for x, y in t2.SLOTS))
        self.assertEqual(len(slots), len(t2.SLOTS))
        self.assertFalse(slots & fixed)
        palettes = 16 * t2.CLUTS_A_ROW, 1024 - t2.CLUT_Y
        cluts = cells(t2.CLUT_X, t2.CLUT_Y, *palettes)
        self.assertFalse(cluts & (fixed | slots))
        self.assertGreaterEqual(t2.CLUTS_A_ROW * palettes[1], 256)
        free = 64 * 8 - len(cells(0, 512, 1024, 512) & (fixed | slots | cluts))
        self.assertGreaterEqual(free, 37 + 24)
        for x, y in t2.SLOTS:                       # one texture page each, as far_cell reads it
            self.assertEqual((x // 64, (y + 63) // 256), ((x + 15) // 64, y // 256))

    def test_launch_pad_places(self):
        lay = t2.layout(13)
        self.assertEqual(sorted(set(lay['places'])), list(range(24)))
        self.assertEqual(len(lay['places']) * lay['repeat'] * 72, 2880)

    def test_runtime_agrees(self):
        """src/tekken2/tmst_tekken2_stages.c: the same names (upper case, in
        order), the same first number, its packs' folder staged by addon.cmake."""
        import re
        c = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        table = re.search(r'names\[STAGES\] = \{(.*?)\};', c, re.S).group(1)
        self.assertEqual(re.findall(r'"([^"]+)"', table), [n.upper() for n in t2.NAMES])
        self.assertIn(f'STAGES = {len(t2.NAMES)}, NUMBER0 = {t2.NUMBER0}', c)
        self.assertIn(f'NO_DIM = {t2.NO_DIM:#x}', c)
        addon = (ADDON / 'addon.cmake').read_text(encoding='utf-8')
        self.assertIn(f'workspace/t2-stages/{t2.CLASSIC}|mods/t2-stages/{t2.CLASSIC}', addon)


if __name__ == '__main__':
    unittest.main()
