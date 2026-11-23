"""tools/t2_remake_stages.py and the add-on's parts of tools/t2_remake_stage.py
without a disc or tekken3.zip: made-up packs, frames and recipes."""
import json, struct, sys, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))
try:
    import numpy as np
    import t2_remake_stage as gen
    import t2_remake_stages as rs
    import tekken2_stages as classic
except ImportError as error:                     # numpy, Pillow: the setup's python_packages
    raise unittest.SkipTest(str(error))


def pack(parts):
    table, body, off = b'', b'', 16 + 12 * len(parts)
    for sid, data in parts:
        data = data + bytes(-len(data) % 4)
        table += struct.pack('<4s2I', sid, off, len(data))
        body += data
        off += len(data)
    return b'ASTG' + struct.pack('<2H2I', 3, 215, len(parts), off) + table + body


def rect(x, y, w, h, value):
    return struct.pack('<4H', x, y, w, h) + struct.pack(f'<{w * h}H', *([value] * (w * h)))


class Frames(unittest.TestCase):
    def test_frames_out_of_the_vram(self):
        vram = struct.pack('<I', 4) + rect(0, 512, 16, 64, 7) + rect(0, 4608, 16, 64, 1) \
            + rect(16, 4608, 16, 64, 2) + rect(32, 960, 16, 1, 9)
        anim = struct.pack('<6H', 0, 512, 16, 64, 10, 3) + struct.pack('<6H', 0, 4608, 16, 4608, 0, 4608)
        out = dict(rs.sections(rs.rewritten(pack([(b'VRAM', vram), (b'ANIM', anim), (b'OBJS', b'1234')]), [])))
        self.assertNotIn(b'ANIM', out)
        self.assertNotIn(b'OBJS', out)
        self.assertEqual(out[b'T2OB'], b'1234')
        self.assertEqual(struct.unpack_from('<I', out[b'VRAM'])[0], 2)          # the frames gone, the rest kept
        t = out[b'T2AN']
        self.assertEqual(struct.unpack_from('<I', t)[0], 1)
        self.assertEqual(t[4:8], b'ANIM')
        x, y, w, h, period, entries, frames, mode = struct.unpack_from('<8H', t, 8)
        self.assertEqual((x, y, w, h, period, entries, frames, mode), (0, 512, 16, 64, 10, 3, 2, 0))
        self.assertEqual(struct.unpack_from('<3H', t, 24), (0, 1, 0))
        pixels = 24 + 8                                                         # 3 entries, padded to 4 bytes
        self.assertEqual(struct.unpack_from('<H', t, pixels)[0], 1)
        self.assertEqual(struct.unpack_from('<H', t, pixels + 2 * 16 * 64)[0], 2)

    def test_frames_in_the_vram_stay(self):
        vram = struct.pack('<I', 1) + rect(0, 512, 16, 64, 7)
        anim = struct.pack('<6H', 0, 512, 16, 64, 10, 1) + struct.pack('<2H', 0, 512)
        out = dict(rs.sections(rs.rewritten(pack([(b'VRAM', vram), (b'ANIM', anim)]), [])))
        self.assertIn(b'ANIM', out)
        self.assertNotIn(b'T2AN', out)


class Allocation(unittest.TestCase):
    def tearDown(self):
        gen.RESERVED[:] = []

    def test_frames_go_past_the_vram_in_the_addons_packs(self):
        gen.RESERVED[:] = list(classic.HIT_EFFECT_CELLS)
        vr = gen.Vram()
        idx = np.zeros((64, 64), np.uint8)
        x, y, _ = vr.frame(idx, [0] * 16)
        self.assertGreaterEqual(y, 1024)
        self.assertLess(vr.picture(idx, [0] * 16)[1], 1024)

    def test_the_old_forks_allocation_unchanged(self):
        vr = gen.Vram()
        idx = np.zeros((64, 64), np.uint8)
        self.assertEqual(vr.frame(idx, [0] * 16)[:2], (0, 512))
        self.assertEqual(vr.picture(idx, [0] * 16)[:2], (16, 512))           # no picture taken twice

    def test_the_same_picture_once(self):
        gen.RESERVED[:] = list(classic.HIT_EFFECT_CELLS)
        vr = gen.Vram()
        idx = (np.arange(64 * 64).reshape(64, 64) % 16).astype(np.uint8)
        a, b = vr.picture(idx, list(range(16))), vr.picture(idx, list(range(16)))
        self.assertEqual(a, b)
        self.assertNotEqual(a, vr.picture(idx, list(range(1, 17))))


class Frame(unittest.TestCase):
    def test_classic_cfg(self):
        cfg = {'crop': [0, 100, 2000, 500], 'rows': 6, 'columns': 24, 'roll': -500, 'yoff': -70, 'seam': 96,
               'materials': {'cliff': {'box': [500, 300, 700, 340]}},
               'sea': {'row': 4, 'foam_from': 30, 'water_to': 40, 'wash': 4, 'reach': 10},
               'lights': {'windows': {'rows': [2, 6]}, 'beacons': {'below': 285}, 'neon': [[528, 284, 26, 44]]},
               'grade': {'horizon_row': 0.5, 'floor': [1, 1, 1]}}
        picture = np.zeros((576, 1536, 3), np.float32)
        frame = {'shift': 100, 'y': [1.5, -20], 'old': [1536, 384], 'hd': [2000, 667]}
        c = gen.classic_cfg(cfg, picture, 64, frame)
        self.assertEqual((c['rows'], c['columns'], c['degrees'], c['yoff']), (9, 24, 60, gen.CLASSIC_BOTTOM - 288))
        for k in ('roll', 'seam'):
            self.assertNotIn(k, c)
        self.assertEqual(c['grade'], {'floor': [1, 1, 1]})
        Y = lambda y: 1.5 * y - 20 + 64
        self.assertEqual(c['sea']['row'], int(Y(4 * 64 + 32) // 64))
        self.assertEqual(c['sea']['foam_from'], round(Y(4 * 64 + 30) - 64 * c['sea']['row']))
        self.assertEqual(c['lights']['beacons']['below'], round(Y(285)))
        self.assertEqual(c['lights']['neon'][0], [528, round(Y(284)), 26, round(44 * 1.5)])
        # an HD box: past the crop, the roll (in the old panorama's pixels), then the frame
        roll = round(-500 * 1536 / 2000)
        self.assertEqual(c['materials']['cliff']['box'][0], round(((500 * 1536 / 2000) + roll) % 1536))
        self.assertEqual(c['materials']['cliff']['box'][1], round(Y((300 - 100) * 384 / 400)))
        self.assertEqual(c['_picture'].shape, picture.shape)

    def test_frames_for_every_remake(self):
        data = json.loads((ADDON / 'tools/data/t2_remake_stages.json').read_text(encoding='utf-8'))
        for owner, cfg in data['arcade'].items():
            if not cfg.get('night') and owner not in rs.OWN_FRAME:
                self.assertIn(owner, data['frames'], owner)
        by_stage = rs.owners()
        self.assertEqual(sorted(by_stage), list(range(23)))
        # the old fork's last five in their own frame (the user's, 2026-11-07)
        self.assertEqual(sorted(gen.stage_of(k) for k in rs.OWN_FRAME), [10, 11, 12, 14, 21])


class WolfCut(unittest.TestCase):
    """The stages of "wolf_cut": top rows of their background off rather than
    their pictures on Unknown's wolf (the user's, 2026-11-05)."""
    FRAME = {'shift': 0, 'y': [1.0, 0], 'old': [1536, 576], 'hd': [2000, 667]}
    BASE = {'clouds': {'row': 3}}

    def tearDown(self):
        gen.RESERVED[:] = []

    def test_the_stages(self):
        by_owner = {k: gen.stage_of(k) for k in rs.WOLF_CUT}
        self.assertEqual(sorted(by_owner.values()), [1, 5, 8, 9, 13, 15, 19, 21])

    def test_on_wolf(self):
        def vram(*rects):
            return pack([(b'VRAM', struct.pack('<I', len(rects)) + b''.join(rect(*r, 1) for r in rects))])
        self.assertEqual(rs.on_wolf(vram((768, 740, 256, 1), (0, 512, 16, 64))), 0)     # the wolf's empty strip
        self.assertEqual(rs.on_wolf(vram((640, 512, 16, 64))), 16 * 64)
        self.assertEqual(rs.on_wolf(vram((704, 767, 64, 1), (676, 740, 92, 1))), 0)
        self.assertEqual(rs.on_wolf(vram((640, 767, 64, 1), (704, 739, 64, 1))), 36 + 64)
        self.assertEqual(rs.on_wolf(pack([(b'FLOR', b'1234')])), 0)

    def test_font_place(self):
        def vram(*rects):
            return pack([(b'VRAM', struct.pack('<I', len(rects)) + b''.join(rect(*r, 1) for r in rects))])
        self.assertEqual(rs.font_place(vram()), (832, 963))
        # the bottom right taken: the next place up, never on the wolf
        self.assertEqual(rs.font_place(vram((768, 960, 256, 64))), (576, 963))
        full = [(x, y, 16, 64) for x in range(0, 1024, 16) for y in range(512, 1024, 64)
                if not (640 <= x and y < 768)]
        self.assertIsNone(rs.font_place(vram(*full)))
        self.assertEqual(rs.font_place(vram(*[r for r in full if not rs.FONT_ROOM[0] <= r[0] or r[1] < 960])),
                         (832, 963))
        # as the overlay seeks it
        c = (ADDON / 'src/core/tmst_overlay.c').read_text(encoding='utf-8')
        for s in ('ATLAS_PAGES = 3, ATLAS_W = ATLAS_PAGES * 64, ATLAS_H = 61,',
                  'for (int y = 1024 - ATLAS_H; y >= 512 && atlas_x < 0; y -= 8)',
                  'for (int x = 1024 - ATLAS_W; x >= 0; x -= 64)',
                  '(y - 512) / 256 == (y + ATLAS_H - 1 - 512) / 256'):
            self.assertIn(s, c)

    def test_cut_fitted_keeps_room_for_the_letters(self):
        import tempfile
        built = []

        def build(owner, src, tekken3, out, cfg):
            built.append(list(gen.RESERVED))
            room = rs.FONT_ROOM in gen.RESERVED
            full = [(x, y, 16, 64) for x in range(0, 1024, 16) for y in range(512, 1024, 64)
                    if not (640 <= x and y < 768) and not (room and x >= rs.FONT_ROOM[0] and y >= 960)]
            out.write_bytes(pack([(b'VRAM', struct.pack('<I', len(full)) + b''.join(rect(*r, 1) for r in full))]))
            return {}
        real = gen.build
        gen.build = build
        try:
            with tempfile.TemporaryDirectory() as tmp:
                make = rs.cutter(self.BASE, np.zeros((9 * 64, 1536, 3), np.float32), 64, self.FRAME, 2)
                _, wolf, _, cut = rs.cut_fitted('lee', None, None, Path(tmp) / 'x.part', make)
                self.assertEqual((wolf, cut), ('free', 0))
                self.assertEqual(len(built), 2)
                self.assertNotIn(rs.FONT_ROOM, built[0])
                self.assertIn(rs.FONT_ROOM, built[1])
        finally:
            gen.build = real

    def test_cutter(self):
        picture = np.zeros((9 * 64, 1536, 3), np.float32)
        make = rs.cutter(self.BASE, picture, 64, self.FRAME, 2)
        whole, cut = make(0), make(2)
        self.assertEqual((whole['rows'], cut['rows']), (9, 7))
        # the foot where it was, the animated row with the picture
        self.assertEqual(whole['yoff'] + 32 * whole['rows'], cut['yoff'] + 32 * cut['rows'])
        self.assertEqual(cut['clouds']['row'], whole['clouds']['row'] - 2)
        self.assertEqual(cut['_picture'].shape[0], 7 * 64)
        self.assertIsNone(make(whole['clouds']['row'] + 1))                   # never the clouds' row
        self.assertIsNone(make(9))
        # Hong Kong's windows from the top row when their first was cut
        lit = rs.cutter({'lights': {'windows': {'rows': [1, 6]}, 'beacons': {'below': 400}}},
                        picture, 64, self.FRAME, 2)
        self.assertEqual(lit(0)['lights']['windows']['rows'], [2, 7])
        self.assertEqual(lit(3)['lights']['windows']['rows'], [0, 4])

    def test_cut_fitted(self):
        import tempfile
        built = []

        def build(owner, src, tekken3, out, cfg):
            built.append((cfg['rows'], list(gen.RESERVED)))
            if cfg['rows'] > 7 and rs.WOLF in gen.RESERVED:
                raise SystemExit('the pictures do not fit the high half of the VRAM')
            out.write_bytes(pack([(b'VRAM', struct.pack('<I', 0))]))
            return {'far_cells': f'24 x {cfg["rows"]}'}
        real = gen.build
        gen.build = build
        try:
            with tempfile.TemporaryDirectory() as tmp:
                temp = Path(tmp) / 'x.part'
                make = rs.cutter(self.BASE, np.zeros((9 * 64, 1536, 3), np.float32), 64, self.FRAME, 2)
                report, wolf, cfg, cut = rs.cut_fitted('panda', None, None, temp, make)
                self.assertEqual((wolf, cut, cfg['rows']), ('free', 2, 7))
                self.assertTrue(all(rs.WOLF in r for _, r in built))           # never on the wolf
                # a picture that never fits off the wolf: as before, uncut
                built.clear()
                make = rs.cutter({'clouds': {'row': 0}}, np.zeros((9 * 64, 1536, 3), np.float32), 0,
                                 self.FRAME, 2)
                report, wolf, cfg, cut = rs.cut_fitted('panda', None, None, temp, make)
                self.assertEqual((wolf, cut, cfg['rows']), ('used', 0, 9))
        finally:
            gen.build = real


class Runtime(unittest.TestCase):
    def test_runtime_and_staging_agree(self):
        c = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        self.assertIn('"T2AN"', c)
        self.assertIn('order = at + 20', c)                                    # the section's 4 + 16 bytes a head
        self.assertIn('"workspace/t2-stages/remake|mods/t2-stages/remake"',
                      (ADDON / 'addon.cmake').read_text(encoding='utf-8'))


class Storm(unittest.TestCase):
    """The lightning's LGHT and THND made from a TTT stage pack (storm_like)."""

    def ttt_pack(self, cols=2, rows=1):
        seqs = [[1, 0, 1, -1], [1, -1], [1, 1, -1], [1, 1, 0, -1], [1, 1, -1], [1, 0, 1, -1]]
        strm = struct.pack('<6h', 3000, 3000, 4000, 5000, 6000, 0)
        strm += b''.join(bytes([v, v, v, 0]) for v in (0x20, 0x20, 0x40, 0xFF, 0xFF, 0xFF))
        seq = b''.join(bytes([len(q)]) + bytes(v & 0xFF for v in q) for q in seqs)
        strm += seq + bytes(-len(seq) % 4) + bytes(6 * 4 * cols * rows)
        strm += struct.pack('<10H', 0x121, 0x1C, 0x123, 0x1C, 0x124, 0x1C, 0x122, 0x1D, 0x122, 0x1D)
        strm += struct.pack('<4H', 60, 4, 130, 3)
        pcm = {0x121: [1, 2, 3], 0x122: [4], 0x123: [5, 6], 0x124: [7]}
        head = 4 + 12 * len(pcm)
        thnd, data = struct.pack('<I', len(pcm)), b''
        for sound, samples in pcm.items():
            thnd += struct.pack('<2H2I', sound, 0, len(samples), head + len(data))
            data += struct.pack(f'<{len(samples)}h', *samples)
        thnd += data
        parts = [(b'STRM', strm), (b'THND', thnd)]
        table, body, off = b'', b'', 24 + 12 * len(parts)
        for sid, d in parts:
            d = d + bytes(-len(d) % 4)
            table += struct.pack('<4s2I', sid, off, len(d))
            body += d
            off += len(d)
        return b'ASTG' + struct.pack('<2H2I', 4, 4, len(parts), off) + b'TTTA' + bytes(4) + table + body

    def test_lght(self):
        lg = rs.storm_like(self.ttt_pack())[b'LGHT']
        self.assertEqual(struct.unpack_from('<5h', lg), (3000, 3000, 4000, 5000, 6000))
        self.assertEqual(struct.unpack_from('<2H', lg, 10), (60, 130))
        self.assertEqual(tuple(lg[14:18]), (4, 3, 6, 0))
        self.assertEqual(tuple(lg[18:24]), (3, 1, 2, 3, 2, 3))                 # the patterns without their -1
        self.assertEqual(tuple(lg[24:30]), (0xFF, 0xFF, 0xFF, 0xFF, 5, 0xFF))  # 4 runs into 5
        self.assertEqual(tuple(lg[30:44]), (1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1))
        at = 44                                                                  # 44: on 4 already
        self.assertEqual(struct.unpack_from('<2H', lg, at), (6, rs.LIGHT_FRAMES))
        self.assertEqual(struct.unpack_from('<I', lg, at + 4)[0], 0x202020)
        self.assertEqual(len(lg), at + 4 + 24)

    def test_thnd(self):
        th = rs.storm_like(self.ttt_pack())[b'THND']
        self.assertEqual(struct.unpack_from('<2H', th), (4, 6))
        self.assertEqual(tuple(th[4:10]), (0, 1, 2, 3, 3, 0xFF))               # by first use; type 5 none
        o = 12
        frames = struct.unpack_from('<I', th, o)[0]
        self.assertEqual(frames, 3)
        self.assertEqual(struct.unpack_from('<6h', th, o + 4), (1, 1, 2, 2, 3, 3))   # mono on both sides

    def test_old_forks_lght(self):
        """The Expanded's TTT stage 4 gives the old fork's LGHT to the byte (when both are here)."""
        storm = rs.storm_pack()
        old = Path(r'C:\Games\UltimateTekken3\tekken3-expanded\workspace\export\tajmahal-full.zip')
        if not storm or not old.is_file():
            self.skipTest('no TTT stage 4 pack or no old export here')
        import zipfile
        z = zipfile.ZipFile(old)
        pack = z.read(next(n for n in z.namelist() if n.endswith('.ast')))
        self.assertEqual(rs.storm_like(storm.read_bytes())[b'LGHT'], dict(rs.sections(pack))[b'LGHT'])

    def test_runtime_reads_it(self):
        c = (ADDON / 'src/tekken2/tmst_tekken2_extras.c').read_text(encoding='utf-8')
        for sid in ('FARB', 'FARV', 'LGHT', 'THND'):
            self.assertIn('"%s"' % sid, c)
        self.assertIn('__wrap_tekken3_arcade_stages_thunder_mix', c)
        cm = (ADDON / 'addon.cmake').read_text(encoding='utf-8')
        self.assertIn('"tekken3_arcade_stages_thunder_mix|src/tekken2/tmst_tekken2_extras.c|STAGE_ENGINE"', cm)


class Mirror(unittest.TestCase):
    """Mirror Darkness: the Remake's MIRR style read, the screen cleared, the laser moved before the copy."""

    def test_runtime(self):
        s = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        self.assertIn('section(b, size, "MIRR", &len)', s)
        m = (ADDON / 'src/tekken2/tmst_tekken2_mirror.c').read_text(encoding='utf-8')
        self.assertIn('tmst_tekken2_mirror_style(raw)', m)
        self.assertIn('clear_black(base)', m)
        self.assertIn('__tmst_chain_func_80036CAC', m)
        self.assertIn('"func_80036CAC|src/tekken2/tmst_tekken2_mirror.c"',
                      (ADDON / 'addon.cmake').read_text(encoding='utf-8'))

    def test_made_remake_has_the_style(self):
        """The Remake's Mirror Darkness carries MIRR (its place as T2MR's, then the style); the Classic's none."""
        remake = ADDON / 'workspace/t2-stages/remake/stage22.ast'
        classic_pack = ADDON / 'workspace/t2-stages/classic/stage22.ast'
        if not remake.is_file() or not classic_pack.is_file():
            self.skipTest('no packs made here')
        r = dict(rs.sections(remake.read_bytes()))
        if b'MIRR' not in r:
            self.skipTest('the Remake is the Classic here (no tekken3.zip)')
        self.assertGreaterEqual(len(r[b'MIRR']), 24 + 17)
        self.assertEqual(r[b'MIRR'][:24], r[b'T2MR'][:24])
        self.assertGreater(struct.unpack_from('<h', r[b'MIRR'], 24)[0], 0)       # the frame
        self.assertNotIn(b'MIRR', dict(rs.sections(classic_pack.read_bytes())))


# src/tekken2/tmst_tekken2_extras.c: the least of each section it reads
EXTRAS_MIN = {b'DUST': 6, b'FTHR': 15, b'FFLY': 21, b'RAIN': 23, b'DIYA': 22, b'ANI2': 12, b'ANI3': 12,
              b'ANI4': 12}


class Extras(unittest.TestCase):
    """The Remake's extras (phase 3): the runtime reads what the generator writes."""

    def test_runtime_reads_them(self):
        c = (ADDON / 'src/tekken2/tmst_tekken2_extras.c').read_text(encoding='utf-8')
        for sid, least in EXTRAS_MIN.items():
            self.assertIn('"%s"' % sid.decode(), c)
            if not sid.startswith(b'ANI'):
                self.assertIn('"%s", &len)) && len >= %d' % (sid.decode(), least), c)
        self.assertIn('psx_mod_gpu_high_vram_source', c)                       # their textures in the upper half
        self.assertIn('src/tekken2/tmst_tekken2_extras.c', (ADDON / 'addon.cmake').read_text(encoding='utf-8'))
        m = (ADDON / 'src/tekken2/tmst_tekken2_mirror.c').read_text(encoding='utf-8')
        self.assertIn('tmst_tekken2_extras_draw(cpu)', m)                      # once the engine drew the stage
        s = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        self.assertIn('tmst_tekken2_extras_load(b, size)', s)
        self.assertIn('"FDIM"', s)

    def test_the_generators_sections_are_long_enough(self):
        self.assertGreaterEqual(struct.calcsize('<2H2B2x'), EXTRAS_MIN[b'DUST'])
        self.assertGreaterEqual(struct.calcsize('<6h2H2B') + 4, EXTRAS_MIN[b'FFLY'])
        self.assertGreaterEqual(struct.calcsize('<7h2H2B') + 3, EXTRAS_MIN[b'RAIN'])
        self.assertGreaterEqual(struct.calcsize('<5h2H4B2h'), EXTRAS_MIN[b'DIYA'])
        self.assertGreaterEqual(struct.calcsize('<6h') + 3, EXTRAS_MIN[b'FTHR'])

    def test_made_packs(self):
        """The packs the setup made (when there): each extra long enough, the
        palettes' frames in the pack's own VRAM rectangles."""
        made = sorted((ADDON / 'workspace/t2-stages/remake').glob('stage*.ast'))
        if not made:
            self.skipTest('no Remake packs made here')
        seen = set()
        for path in made:
            parts = dict(rs.sections(path.read_bytes()))
            vram = parts[b'VRAM']
            n, o, rects = struct.unpack_from('<I', vram)[0], 4, []
            for _ in range(n):
                x, y, w, h = struct.unpack_from('<4H', vram, o)
                rects.append((x, y, w, h))
                o += 8 + 2 * w * h
            for sid, least in EXTRAS_MIN.items():
                if sid not in parts:
                    continue
                seen.add(sid)
                data = parts[sid]
                self.assertGreaterEqual(len(data), least, f'{path.name} {sid}')
                if sid == b'DIYA':
                    self.assertGreaterEqual(len(data), 22 + 6 * struct.unpack_from('<h', data)[0])
                if sid == b'FTHR' and len(data) >= 18 and struct.unpack_from('<h', data, 16)[0] in (3, 5):
                    self.assertGreaterEqual(len(data), 26, f'{path.name} FTHR picture')
                if sid.startswith(b'ANI'):
                    x, y, w, h, period, count = struct.unpack_from('<6H', data)
                    self.assertGreaterEqual(len(data), 12 + 4 * count)
                    for k in range(count):
                        fx, fy = struct.unpack_from('<2H', data, 12 + 4 * k)
                        inside = any(rx <= fx and fx + w <= rx + rw and ry <= fy and fy + h <= ry + rh
                                     for rx, ry, rw, rh in rects)
                        covered = all(any(rx <= fx + i < rx + rw and ry <= fy < ry + rh for rx, ry, rw, rh in rects)
                                      for i in range(w))
                        self.assertTrue(inside or covered, f'{path.name} {sid} frame {k}')
        if len(made) == 23:
            self.assertEqual(seen, set(EXTRAS_MIN))

    def test_made_packs_objects(self):
        """The objects as the runtime takes them (objects_load): at most 6
        models of at most 256 vertices, 12 drawing calls of known models,
        T2RT and TRIG whole, a machine it moves."""
        made = sorted((ADDON / 'workspace/t2-stages/remake').glob('stage*.ast'))
        if not made:
            self.skipTest('no Remake packs made here')
        machines = {}
        for path in made:
            parts = dict(rs.sections(path.read_bytes()))
            if b'T2OB' not in parts:
                continue
            self.assertNotIn(b'OBJS', parts)                                   # the engine would take them
            d = parts[b'T2OB']
            nm, o, verts = struct.unpack_from('<I', d)[0], 4, []
            self.assertLessEqual(nm, 6, path.name)
            for _ in range(nm):
                nv, npr = struct.unpack_from('<2H', d, o)
                self.assertLessEqual(nv, 256)
                o += 4 + 8 * nv + 40 * npr
                verts.append(nv)
            nl = struct.unpack_from('<I', d, o)[0]
            self.assertTrue(0 < nl <= 12, path.name)
            self.assertLessEqual(o + 4 + 8 * nl, len(d))
            for k in range(nl):
                model, _, rank, size = struct.unpack_from('<BBHH', d, o + 4 + 8 * k)
                self.assertLess(model, nm)
                self.assertTrue(rank <= 1023 and size <= 128, path.name)
            self.assertGreaterEqual(len(parts[b'T2RT']), 4 * 4096)
            self.assertGreaterEqual(len(parts[b'TRIG']), 2 * 4096)
            machines[int(path.name[5:7])] = struct.unpack_from('<H', parts[b'T2OA'])[0]
            c = (ADDON / 'src/tekken2/tmst_tekken2_extras.c').read_text(encoding='utf-8')
            self.assertIn('s_obj.ostage == %d' % machines[int(path.name[5:7])], c)
        if len(made) == 23:
            self.assertEqual(machines[0], 200)      # the eagle
            self.assertEqual(machines[5], 11)       # the helicopter
            self.assertEqual(machines[8], 202)      # the ship
            self.assertEqual(machines[19], 201)     # the grass


class Bugfix(unittest.TestCase):
    """The user's bugfix round (2026-11-05): the fighters' light, the grass,
    the writing."""

    def test_light_section(self):
        for stage in range(23):
            parts = dict(rs.ours(stage, {}))
            if stage in rs.DARK_LIGHT or stage in rs.REMAKE_DARK:
                self.assertNotIn(b'T2LT', parts)
            else:
                self.assertEqual(struct.unpack('<2H', parts[b'T2LT']), (rs.DAY_ARENA, 0))
        self.assertEqual(rs.DARK_LIGHT, {4, 6, 7, 16, 21, 22})  # Church, Pagoda, Forest, Taj, Eternal, Mirror (D15 b)
        self.assertEqual(rs.REMAKE_DARK, {12})                  # Nowhere Desert's Remake at night (D3 a, 2026-11-07)
        s = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        self.assertIn('"T2LT"', s)
        self.assertIn('LIGHT_TABLE = 0x80096c78', s)
        self.assertIn('light_saved', s)                          # arena 9's own put back

    def test_classic_light_as_the_remakes(self):
        """The Classic lit as the Remake (the user's, 2026-11-05): T2LT on the
        same stages, the dark ones dimmed as the Remake's recipe dims them."""
        recipe = json.loads((ADDON / 'tools/data/t2_remake_stages.json').read_text(encoding='utf-8'))
        # (not the last five's own night light, 2026-11-07: the Classic's stays)
        light = {stage: cfg.get('fighter_light') for stage, cfg in
                 ((gen.stage_of(owner), cfg) for owner, cfg in recipe['arcade'].items() if owner not in rs.OWN_FRAME)}
        for stage in range(23):
            lay = classic.layout(stage)
            if stage in classic.DARK_LIGHT:
                if light.get(stage):
                    self.assertEqual(lay['dim'], round(0x80 * light[stage]), stage)
                else:
                    self.assertEqual(lay['dim'], 0x50, stage)
            else:
                self.assertEqual(lay.get('dim', classic.NO_DIM), classic.NO_DIM, stage)

    def test_made_classic_light(self):
        made = sorted((ADDON / 'workspace/t2-stages/classic').glob('stage*.ast'))
        if len(made) != 23:
            self.skipTest('no Classic packs made here')
        for path in made:
            stage, parts = int(path.name[5:7]), dict(rs.sections(path.read_bytes()))
            self.assertEqual(b'T2LT' in parts, stage not in classic.DARK_LIGHT, path.name)

    def test_made_packs_light(self):
        made = sorted((ADDON / 'workspace/t2-stages/remake').glob('stage*.ast'))
        if len(made) != 23:
            self.skipTest('no Remake packs made here')
        for path in made:
            stage, parts = int(path.name[5:7]), dict(rs.sections(path.read_bytes()))
            self.assertEqual(b'T2LT' in parts, stage not in rs.DARK_LIGHT and stage not in rs.REMAKE_DARK, path.name)

    def test_grass_round(self):
        c = (ADDON / 'src/tekken2/tmst_tekken2_extras.c').read_text(encoding='utf-8')
        self.assertIn('GRASS_SIDE = 7', c)
        self.assertIn('GRASS_REACH = 5', c)                       # a round of 2.5 squares, each tuft cut on its own
        self.assertIn('if (dx * dx + dz * dz > s_grass_r2) continue;', c)

    def test_ground_fill(self):
        """The pause band (the user's, 2026-11-05): FARB's bottom colour under
        all of the stage, every Tekken 2 pack, read where the engine reads it."""
        s = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        self.assertIn('if (o && len >= 24 && o[22]) { memcpy(ground, o + 19, 3); ground_on = 1; }', s)
        m = (ADDON / 'src/tekken2/tmst_tekken2_mirror.c').read_text(encoding='utf-8')
        self.assertIn('ground_fill(base,', m)
        self.assertIn('rank = base + 4u * 1023', m)
        # the pause's drawing area given back to the stage (Tekken 3's E3 at rank 1023: y 234 / 358)
        self.assertIn('AREA_TOP_LEFT = 0xe3005000u, AREA_BOTTOM_RIGHT = 0xe4074d6fu', m)
        self.assertIn('pause_area(base,', m)
        self.assertIn('if (!psx_mod_read_word(PAUSED) && !s_area_seen) return;', m)   # and while Tekken 3 cuts it (menus opening, closing)
        self.assertIn('area_look();', m)
        self.assertIn('src/tekken2/tmst_tekken2_trace.c', (ADDON / 'addon.cmake').read_text(encoding='utf-8'))

    def test_floor_under_shadows(self):
        """No floor cell drawn after the fighters' shadows (rank 1007): FLOR's
        ranks capped at 9 (the engine: main rank 1021 - rank; 1011 the
        Church's window light, 1010 the tiles)."""
        flor = bytes(8 + 4000) + bytes(range(0, 100)) + b'tail'
        out = rs.floor_under_shadows(flor)
        self.assertEqual(len(out), len(flor))
        self.assertEqual(out[-4:], b'tail')
        ranks = out[4008:4108]
        self.assertEqual(max(ranks), 9)
        self.assertEqual(list(ranks[:10]), list(range(10)))
        self.assertEqual(1021 - max(ranks), 1012)
        made = sorted((ADDON / 'workspace/t2-stages/remake').glob('stage*.ast'))
        for path in made:
            f = dict(rs.sections(path.read_bytes()))[b'FLOR']
            self.assertLessEqual(max(f[4008:4108]), 9, path.name)

    def test_frame_lift(self):
        """A frame's "lift": the user's picture that many texels higher
        (London: its buses out of the sand; its sea waving as the Beach's)."""
        self.assertEqual(gen.frame_y({'y': [1.5, 48]}), (1.5, 48))
        self.assertEqual(gen.frame_y({'y': [1.5, 48], 'lift': 50}), (1.5, -2))
        data = json.loads((ADDON / 'tools/data/t2_remake_stages.json').read_text(encoding='utf-8'))
        self.assertEqual(data['frames']['pjack']['lift'], 50)
        self.assertEqual(data['arcade']['pjack']['sea']['row'], 5)

    def test_scenery_anchor(self):
        """The scenery stands at an anchor of ours (the fighters moved to it
        for the stage routine's call, given back after the extras): pushed
        only past the leash, unturned by the axis (2026-11-08, "A")."""
        c = (ADDON / 'src/tekken2/tmst_tekken2_anchor.c').read_text(encoding='utf-8')
        for key in ('LEASH = 4000.0', '} else if (!psx_mod_read_word(PAUSED) && d > LEASH) {', 'shift(dx, dz);', 'shift(-dx, -dz);',
                    'HISTORY_P1 + 8 + 16 * k', 'TMST_T2_ANCHOR',
                    # the Church fixed to the floor (FIXW): grid anchor, unturned, its walls (COLL)
                    'fabs(m[k] - *a[k]) > P / 10 * 6', 'psx_mod_write_word(AXIS, 0);', 'ROOT_P1 = 0x800a9228',
                    'if (!psx_mod_read_word(PAUSED)) collide();', 'if (!moved || !frozen) return;'):
            self.assertIn(key, c)
        self.assertNotIn('FOLLOW', c)
        self.assertEqual(c.count('freeze();'), 2)
        m = (ADDON / 'src/tekken2/tmst_tekken2_mirror.c').read_text(encoding='utf-8')
        self.assertLess(m.index('anchored = tmst_tekken2_anchor_begin();'), m.index('        tmst_tekken2_stage_entry();'))
        self.assertLess(m.index('tmst_tekken2_extras_draw(cpu); }'), m.index('tmst_tekken2_anchor_end();'))
        self.assertIn('src/tekken2/tmst_tekken2_anchor.c', (ADDON / 'addon.cmake').read_text(encoding='utf-8'))
        self.assertIn('if (cpu->pc == 0x8006d110u) tmst_tekken2_anchor_step(cpu->gpr[2]);', m)
        st = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        self.assertIn('section(b, size, "FIXW", &len)', st)
        self.assertIn('section(b, size, "COLL", &len)', st)

    def test_fixed_stage_tiles_drawn_by_the_addon(self):
        """A stage fixed to the floor: the engine gets no tiles (its cone drops
        those near the camera), the add-on draws them all (T2TL)."""
        parts = [(b'VRAM', b'v'), (b'TILE', struct.pack('<2I', 200, 3) + b'tiles'), (b'FIXW', struct.pack('<i', 25000))]
        out = dict(rs.own_tiles(parts))
        self.assertEqual(out[b'TILE'], struct.pack('<2I', 200, 0))
        self.assertEqual(out[b'T2TL'], parts[1][1])
        self.assertEqual(rs.own_tiles(parts[:2]), parts[:2])             # not fixed: the engine's
        c = (ADDON / 'src/tekken2/tmst_tekken2_extras.c').read_text(encoding='utf-8')
        self.assertIn('section(b, size, "T2TL", &len)', c)
        self.assertIn('own_tiles_draw(&g);', c)
        self.assertNotIn('keep_in_view', (ADDON / 'src/tekken2/tmst_tekken2_anchor.c').read_text(encoding='utf-8'))
        made = ADDON / 'workspace/t2-stages/remake/stage04.ast'
        if made.is_file():
            parts = dict(rs.sections(made.read_bytes()))
            self.assertEqual(struct.unpack_from('<I', parts[b'TILE'], 4)[0], 0)
            self.assertEqual(struct.unpack_from('<I', parts[b'T2TL'], 4)[0], 256)

    def test_church_glass(self):
        """D17: the Church's window light as T2GL (the runtime draws it), not
        in the tiles; the runtime reads what the generator writes."""
        src = (ADDON / 'tools/t2_remake_stage.py').read_text(encoding='utf-8')
        self.assertIn("extra.append((b'T2GL', glass_section(cfg)))", src)
        self.assertIn("faces += glass_light(spec.get('glass', {}), g, patch_on=not RESERVED)", src)   # the shafts stay
        self.assertEqual(struct.calcsize('<7h4B'), 18)
        c = (ADDON / 'src/tekken2/tmst_tekken2_extras.c').read_text(encoding='utf-8')
        self.assertIn('section(b, size, "T2GL", &len)', c)
        self.assertIn('memcpy(&s_glass.p[k], o + 8 + 3 * panes + 18 * k, 18);', c)
        self.assertIn('GLASS_RANK = 1011', c)
        made = ADDON / 'workspace/t2-stages/remake/stage04.ast'
        if made.is_file():
            g = dict(rs.sections(made.read_bytes())).get(b'T2GL')
            self.assertIsNotNone(g)
            n, rows, cols, period = struct.unpack_from('<4H', g)
            self.assertEqual((n, rows, cols, period), (4, 8, 4, 2500))

    def test_glyphs(self):
        import t2_glyphs as g
        sizes = {'明': (10, 7), '令': (10, 7), '祭': (10, 8), 'horse': (6, 9)}
        for ch in '天府之國천하대장군지여':
            sizes[ch] = (9, 9)
        for ch, shape in sizes.items():
            self.assertEqual(g.GLYPHS[ch].shape, shape, ch)
            self.assertTrue(g.GLYPHS[ch].any(), ch)
        for words, font, most in (('HOT DOGS', g.FONT5, 52), ('FRAGILE', g.FONT5, 46), ('U.S. MAIL', g.FONT3, 36),
                                  ('TELEPHONE', g.FONT3, 52)):
            self.assertLessEqual(g.text(words, font).shape[1], most, words)
        neon = g.neon_words('香港')
        self.assertEqual(neon.shape, (64, 128))
        self.assertGreater((neon > 110).sum(), 400)
        self.assertFalse((neon[:6] > 110).any() or (neon[:, :8] > 110).any())   # inside the sign's tube

    def test_no_made_up_writing_in_the_addons_packs(self):
        """With RESERVED (the add-on's packs) the pictures with writing take
        tools/t2_glyphs.py's; the user's "鐵拳" picture is not used."""
        src = (ADDON / 'tools/t2_remake_stage.py').read_text(encoding='utf-8')
        for key in ("GLYPHS['令' if k == 2 else '明']", "GLYPHS['國之府天'[k]]", "GLYPHS['horse']", "GLYPHS['祭']",
                    "'지하여장군' if kind == 'jangseung_body_f' else '천하대장군'", "neon_words('香港')",
                    "glyph_text('HOT DOGS'", "glyph_text('U.S. MAIL'", "glyph_text('TELEPHONE'",
                    "glyph_text('FRAGILE'"):
            self.assertIn(key, src)


if __name__ == '__main__':
    unittest.main()


class LastFive(unittest.TestCase):
    """The old fork's last five (2026-11-07): their own frame, their
    animations' frames and lights' palettes past the VRAM, Eternal
    Darkness's top rows cut rather than the wolf's room taken."""

    def tearDown(self):
        gen.RESERVED[:] = []
        gen.PALETTE_RAM = False

    def test_own_cutter(self):
        base = {'crop': [0, 40, 2000, 640], 'rows': 6, 'yoff': -40, 'weave': {'cell': [2, 15]},
                'flicker': {'fires': [[1265, 575, 0.4]], 'rain': {'windows': [[1, 70, 9, 150], [1, 210, 9, 360]]}},
                'lightning': {'windows': {'rects': [[1, 70, 9, 150], [1, 120, 9, 360]]}}, 'webs': {'at': [[985, 300, 60, 'tr']]}}
        make = rs.own_cutter(base)
        self.assertIs(make(0), base)
        c = make(1)
        self.assertEqual((c['crop'], c['rows'], c['yoff'], c['weave']['cell']), ([0, 140, 2000, 640], 5, -8, [1, 15]))
        self.assertEqual(c['flicker']['rain']['windows'], [[1, 210, 9, 360]])          # 10 pixels left: gone
        self.assertEqual(c['lightning']['windows']['rects'], [[1, 140, 9, 360]])       # clipped
        self.assertEqual(base['rows'], 6)                                              # its own recipe kept
        self.assertIsNone(make(3))                                                     # the web's row: no
        self.assertIsNone(make(6))

    def test_palette_frames(self):
        vr = gen.Vram()
        frames = [[[f] * 16, [f + 1] * 16] for f in range(3)]
        self.assertEqual(gen.palette_frames(vr, 400, frames), [399, 398, 397])          # the old fork's packs
        gen.RESERVED[:] = [(0, 0, 1, 1)]
        self.assertEqual(gen.palette_frames(gen.Vram(), 400, frames), [399, 398, 397])  # the Pagoda's: as they were
        gen.PALETTE_RAM = True
        vr = gen.Vram()
        places = gen.palette_frames(vr, 400, frames)
        self.assertTrue(all(y >= 1024 for _, y in places))
        x, y = places[2]
        self.assertEqual([int(v) for v in vr.v[y, x:x + 32]], [2] * 16 + [3] * 16)

    def test_frames_out_palette_rows_and_shown_frame(self):
        def rect(x, y, w, h, v):
            return struct.pack('<4H', x, y, w, h) + struct.pack(f'<{w * h}H', *([v] * (w * h)))
        vram = (b'VRAM', struct.pack('<I', 4) + rect(16, 512, 16, 64, 7) + rect(0, 4608, 32, 1, 8) + rect(0, 4609, 32, 1, 9)
                + rect(0, 4672, 16, 64, 5))
        ani = (b'ANI4', struct.pack('<6H', 640, 1000, 32, 1, 3, 2) + struct.pack('<4H', 0, 4608, 0, 4609) + struct.pack('<H', 0))
        web = (b'ANI3', struct.pack('<6H', 16, 512, 16, 64, 30, 2) + struct.pack('<4H', 16, 512, 0, 4672) + struct.pack('<H', 0))
        out = dict(rs.frames_out([vram, ani, web]))
        self.assertNotIn(b'ANI4', out)
        t = out[b'T2AN']
        self.assertEqual(struct.unpack_from('<I', t)[0], 2)
        self.assertEqual(struct.unpack_from('<4s7H', t, 4), (b'ANI4', 640, 1000, 32, 1, 3, 2, 2))
        self.assertEqual(struct.unpack_from('<64H', t, 4 + 20 + 4), tuple([8] * 32 + [9] * 32))
        self.assertEqual(struct.unpack_from('<I', out[b'VRAM'])[0], 1)               # the picture stays

    def test_made_last_five(self):
        made = [ADDON / f'workspace/t2-stages/remake/stage{st:02d}.ast' for st in (10, 11, 12, 14, 21)]
        if not all(p.is_file() for p in made):
            self.skipTest('no Remake packs made here')
        for path in made:
            pack = path.read_bytes()
            parts = dict(rs.sections(pack))
            if b'T2OB' not in parts:
                self.skipTest('the Classic copies (no tekken3.zip)')
            # Eternal Darkness's whole: the wolf's area too (stage21w.ast hers, WolfVariant)
            wolf = (path.with_name(rs.wolf_name(path.name))).is_file()
            if not wolf:
                self.assertEqual(rs.on_wolf(pack), 0, path.name)
            self.assertIsNotNone(rs.font_place(pack, wolf=not wolf), path.name)
            self.assertIn(b'T2AN', parts, path.name)
            self.assertFalse(set(parts) & {b'ANIM', b'ANI2', b'ANI4'}, path.name)
            v = parts[b'VRAM']
            n, o = struct.unpack_from('<I', v)[0], 4
            for _ in range(n):
                x, y, w, h = struct.unpack_from('<4H', v, o)
                o += 8 + 2 * w * h
                self.assertLessEqual(y + h, 1024, path.name)

    def test_runtime_room(self):
        s = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        self.assertIn('ANIMS_MAX = 32', s)

    def test_runtime_parts(self):
        """Phase 2: the old fork's e454a7e runtime parts of the last five."""
        c = (ADDON / 'src/tekken2/tmst_tekken2_extras.c').read_text(encoding='utf-8')
        for part in ('static void desert_wind(const Gte *g)', 'static void dandelion_seeds(const Gte *g)',
                     'if (s_pack.feather_shape == 6) { desert_wind(g); return; }',
                     'if (s_pack.feather_shape == 7) { dandelion_seeds(g); return; }',
                     'int16_t gull[16];', 'static void dangle_step(int k, int slot)',
                     'spider = g[13] == 4, lizard = g[13] == 3 || spider', 'section(b, size, "FLIT", &len)',
                     'int lit = s_bolt_on && s_bolt >= 0;'):
            self.assertIn(part, c)
        made = ADDON / 'workspace/t2-stages/remake'
        if not (made / 'stage21.ast').is_file():
            self.skipTest('no Remake packs made here')
        parts = {st: dict(rs.sections((made / f'stage{st:02d}.ast').read_bytes())) for st in (12, 14, 21)}
        if b'T2OB' not in parts[21]:
            self.skipTest('the Classic copies (no tekken3.zip)')
        self.assertEqual(struct.unpack_from('<h', parts[12][b'FTHR'], 16)[0], 6)        # tumbleweeds
        self.assertGreaterEqual(len(parts[12][b'FTHR']), 34)
        self.assertEqual(struct.unpack_from('<h', parts[14][b'FTHR'], 16)[0], 7)        # dandelions
        gull = struct.unpack_from('<16h', parts[21][b'GULL'])
        self.assertEqual(gull[13], 4)                                                     # spiders
        self.assertTrue(0 < gull[14] <= gull[9])                                          # some on threads
        self.assertEqual(struct.unpack_from('<h', parts[12][b'GULL'], 26)[0], 3)         # lizards
        self.assertGreaterEqual(len(parts[21][b'FLIT']), 36)


class WolfVariant(unittest.TestCase):
    """A "wolf_cut" stage's two packs (the user's, 2026-11-08): stageNNw.ast,
    its background's top cut, while Unknown fights; stageNN.ast whole."""

    def tearDown(self):
        gen.RESERVED[:] = []

    def test_wolf_name(self):
        self.assertEqual(rs.wolf_name('stage21.ast'), 'stage21w.ast')

    def test_full_fitted(self):
        import tempfile
        seen = []
        cells = gen.pack.HIT_EFFECT_64_CELLS

        def build(owner, src, tekken3, out, cfg, letters=True):
            seen.append((list(gen.RESERVED), tuple(gen.pack.HIT_EFFECT_64_CELLS)))
            if rs.WOLF in gen.RESERVED:
                raise SystemExit('the pictures do not fit the high half of the VRAM')
            rects = [(640, 512, 16, 64)] if letters else                 [(x, y, 16, 64) for x in range(0, 1024, 16) for y in range(512, 1024, 64)]
            out.write_bytes(pack([(b'VRAM', struct.pack('<I', len(rects)) + b''.join(rect(*r, 1) for r in rects))]))
            return {'far_cells': '20 x 6'}
        real = gen.build
        gen.build = build
        try:
            with tempfile.TemporaryDirectory() as tmp:
                temp = Path(tmp) / 'x.part'
                report, wolf, cfg = rs.full_fitted('kazuya', None, None, temp, lambda cut: {'rows': 6 - cut})
                self.assertEqual((wolf, cfg['rows']), ('used', 6))
                self.assertIn(b'VRAM', dict(rs.sections(report['_pack'])))
                self.assertFalse(temp.exists())
                # the letters' room kept, from the palettes too, then given back
                self.assertTrue(all(rs.FONT_ROOM in r and rs.FONT_ROOM in c for r, c in seen))
                self.assertEqual(gen.pack.HIT_EFFECT_64_CELLS, cells)
                # the letters left no place: none
                gen.build = lambda *a: build(*a, letters=False)
                self.assertIsNone(rs.full_fitted('kazuya', None, None, temp, lambda cut: {'rows': 6 - cut}))
                self.assertEqual(gen.pack.HIT_EFFECT_64_CELLS, cells)
        finally:
            gen.build = real

    def test_runtime(self):
        s = (ADDON / 'src/tekken2/tmst_tekken2_stages.c').read_text(encoding='utf-8')
        for part in ('"%s/stage%02dw.ast"', 'static int wolf_pack(int stage, char *out, size_t size)',
                     'int wolf = held_as == REMAKE && wolf_pack(stage, from, sizeof from);',
                     'held_wolf != wolf_pack(fight, NULL, 0)', 'extras_wolf = drawn_version() == REMAKE && wolf_pack('):
            self.assertIn(part, s)
        c = (ADDON / 'src/compat/tmst_compat.c').read_text(encoding='utf-8')
        self.assertIn('tekken3_guest_key(psx_mod_read_half(LOADING_FIGHTERS + 2 * p))', c)
        self.assertIn('!strcmp(key, "unknown")', c)

    def test_hills_on_the_floor(self):
        """Hills' picture down to the floor's far edge (the user's, 2026-11-08)."""
        self.assertEqual(gen.DATA['arcade']['alex']['yoff'], 16)

    def test_made_packs(self):
        made = ADDON / 'workspace/t2-stages/remake'
        stages = sorted(gen.stage_of(k) for k in rs.WOLF_CUT)
        paths = [made / f'stage{st:02d}.ast' for st in stages]
        if not all(p.is_file() for p in paths):
            self.skipTest('no Remake packs made here')
        if b'T2OB' not in dict(rs.sections(paths[-1].read_bytes())):
            self.skipTest('the Classic copies (no tekken3.zip)')
        for path in paths:
            whole = path.read_bytes()
            hers = path.with_name(rs.wolf_name(path.name))
            if hers.is_file():
                cut = hers.read_bytes()
                self.assertEqual(rs.on_wolf(cut), 0, hers.name)
                self.assertIsNotNone(rs.font_place(cut), hers.name)
                self.assertIsNotNone(rs.font_place(whole, wolf=False), path.name)
            else:
                self.assertEqual(rs.on_wolf(whole), 0, path.name)
                self.assertIsNotNone(rs.font_place(whole), path.name)


class HongKongFloor(unittest.TestCase):
    """Issue #12: the Remake's running lights and floor fade, Tekken 2's own."""

    def test_recipe(self):
        hk = gen.DATA['arcade']['gunjack']
        self.assertEqual({k: hk['floor_chase'][k] for k in ('cluts', 'dots', 'period', 'add')},
                         dict(cluts=[0x7B02, 0x7B03], dots=[10, 11, 12, 13], period=16384, add=1706))
        self.assertNotIn('colour', hk['floor_chase'])                  # no lit colour of ours
        self.assertEqual(hk['floor_blend'], dict(clut=0x7B04, to=0x7B09, first=0, last=15, add=17))
        self.assertLessEqual(gen.BLEND_MAX, 64)                          # the runtime's

    def test_faded(self):
        idx = np.array([[0, 1], [1, 3]])
        img_b = np.array([[[8, 16, 24], [80, 0, 0]], [[160, 0, 0], [0, 0, 0]]], float)
        colours = [0x1111, 0x2222, 0x3333, 0x4444]
        out = gen.faded(img_b, idx, colours)
        self.assertEqual(out[0], int(gen.bgr555(np.array([8, 16, 24], float))))
        self.assertEqual(out[1], int(gen.bgr555(np.array([120, 0, 0], float))))     # the mean of its texels
        self.assertEqual(out[2], 0x3333)                                             # no texel: as it was
        self.assertEqual(out[3], 0x0421)                                             # never colour 0
