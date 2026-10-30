#!/usr/bin/env python3
"""Arcade Stages, step E1: which Tekken 3 arcade stage stands for which PS1 arena.

    python tools/arcade_stages/recon.py [--tekken3 tekken3.zip] [--disc "Tekken 3 (Track 1).bin"]

Reads the user's own tekken3.zip (Tekken 3 arcade, TET2/VER.E1) and PS1 disc,
and writes under workspace/arcade-stages/recon/ (never committed):

- stages.csv: one row per PS1 arena 0..14 and its arcade counterpart (same
  number), with the files, VRAM rectangles and geometry of both;
- palettes.csv: palettes shared between every PS1 arena and arcade stage
  (16-colour CLUT rows, identical 15-bit colours), the measure behind the pairing;
- with --png: textures/ps1-NN.png and textures/arcade-NN.png, each stage's
  TIMs laid out in its VRAM (PS1 1024x512, System 12 1024x1024), 4/8-bit
  images shown through their own palette, for E2 and the side-by-side boards.

PS1 (SLUS_004.02, U.S.):
- arena = u16 0x800ADC84; the fight loader 0x80036974 (twin at 0x800368A4)
  reads file 0x22 + arena (BNS 36 + arena, an ARC: member 0 = TIMs sent to
  VRAM by 0x8006E37C, member 1 = far background data copied to 0x800ADC88's
  buffer and set up by 0x8004808C), then file 0x36 + arena (BNS 56 + arena,
  the geometry) into 0x8006C800's buffer, cut into sections by 0x8006C95C.
  File n -> BNS id through the u16 table 0x80098574.
- per frame (0x8002B020..0x8002B044): 0x80048240 (far background through the
  pointer 0x8009E728), then 0x8006D7CC -> 0x8006D014 (geometry).

Arcade (program unpacked at 80010000 as tools/difficulty/import.py does):
- archive directory 0x801FF738: 44 pairs (header list, count); a header is
  u32 n then n x (offset from the header, size). ROM < 0x2000000 = banked
  ROMs, 0x2xxxxxx = program ROM - 0x2000000.
- stage = u16 0x8021FBB8; loader 0x801D711C reads archive u32 0x8017C320[stage]
  (13 stages), sends files 1.. to VRAM (0x801DB408; stage 7 LZ-unpacks them
  first, 0x80194C20, the LZ of tools/ttt1/extract.py), then file 0 (geometry)
  to 0x802A1154.
- geometry, both machines: u32 0x41, tile size, row count (PS1 36 = 6x6,
  arcade 256 = 16x16), then rows of 7 u32 (vertex offset, vertex count,
  -, primitive count, primitive offset, -, -), offsets from byte 12.
"""
from __future__ import annotations

import argparse
import csv
import importlib.util
import json
import struct
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / 'tools'), str(ROOT / 'tools/ttt1')]
import arcade_model_probe as probe  # noqa: E402
import bns_tool  # noqa: E402
import extract as ttt1  # noqa: E402

OUT = ROOT / 'workspace/arcade-stages/recon'
BASE = 0x80010000
ARC_DIRECTORY, ARC_COUNT = 0x801FF738, 44
ARC_STAGES, ARC_NSTAGE = 0x8017C320, 13
PS1_REMAP, PS1_TEX, PS1_GEO, PS1_NARENA = 0x80098574, 0x22, 0x36, 15
# Owner of each PS1 arena (a fighter record's byte 10, TIPS section 25).
PS1_NAMES = ['Paul', 'Law', 'Yoshimitsu', 'Xiaoyu', 'Hwoarang', 'Lei', 'Ogre',
             'Xiaoyu/Jin alt', 'Jin', 'Nina', 'Eddy', 'King', 'Julia', 'Gon', 'Dr. B']


def load_program_module():
    spec = importlib.util.spec_from_file_location('difficulty_import', ROOT / 'tools/difficulty/import.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class Arcade:
    def __init__(self, zip_path: Path):
        self.program = load_program_module().program('tekken3', zip_path)
        regions = [r for r in json.loads(probe.MANIFEST.read_text())['sets']['tekken3']['regions']
                   if r['name'] in ('maincpu:rom', 'bankedroms')]
        rebuilt = probe.reconstruct(zip_path, dict(regions=regions))[0]
        self.main, self.bank = rebuilt['maincpu:rom'], rebuilt['bankedroms']

    def u32(self, a: int) -> int:
        return struct.unpack_from('<I', self.program, a - BASE)[0]

    def rom(self, a: int, n: int) -> bytes:
        data = self.main[a - 0x2000000:a - 0x2000000 + n] if a >= 0x2000000 else self.bank[a:a + n]
        assert len(data) == n, hex(a)
        return data

    def archive(self, k: int):
        """[(header ROM address, [(file ROM address, size)])] of archive k."""
        lst, count = self.u32(ARC_DIRECTORY + 8 * k), self.u32(ARC_DIRECTORY + 8 * k + 4)
        out = []
        for h in (self.u32(lst + 4 * i) for i in range(count)):
            n = struct.unpack('<I', self.rom(h, 4))[0]
            pairs = struct.unpack('<%dI' % (2 * n), self.rom(h + 4, 8 * n))
            out.append((h, [(h + o, s) for o, s in zip(pairs[0::2], pairs[1::2])]))
        return out

    def stage_table(self):
        return [self.u32(ARC_STAGES + 4 * i) for i in range(ARC_NSTAGE)]

    def stage_files(self, archive: int):
        """(header, [bytes]) with files 1.. LZ-unpacked when they do not read as TIMs."""
        (header, files), = self.archive(archive)
        out, packed = [], False
        for i, (a, size) in enumerate(files):
            data = self.rom(a, size)
            if i and not tims(data):
                data, packed = ttt1.lz(data, 0), True
            out.append(data)
        return header, out, packed


def tims(data: bytes):
    """TIMs one after another (up to 16 bytes of padding at the end)."""
    out, i = [], 0
    while i + 8 <= len(data):
        magic, flags = struct.unpack_from('<2I', data, i)
        # PS1 arena 12 (Julia) has one TIM with its magic zeroed; 0x8006E37C
        # sends it all the same.
        if magic not in (0x10, 0) or flags & ~0xF or flags & 3 == 3 or i + 20 > len(data):
            break
        i += 8
        clut = None
        if flags & 8:
            size, x, y, w, h = struct.unpack_from('<I4H', data, i)
            clut = (x, y, w, h, data[i + 12:i + size])
            i += size
        size, x, y, w, h = struct.unpack_from('<I4H', data, i)
        out.append(dict(bpp=(4, 8, 16)[flags & 3], clut=clut, image=(x, y, w, h, data[i + 12:i + size])))
        i += size
    if i < len(data) - 16 or (out and any(data[i:])) and data[i:i + 4] not in (b'AAAA', b'TIMZ'):
        return []
    return out


def palettes(timlist):
    """Set of 16-colour rows of every CLUT, colours without the STP bit."""
    rows = set()
    for t in timlist:
        if t['clut']:
            x, y, w, h, raw = t['clut']
            words = struct.unpack('<%dH' % (len(raw) // 2), raw)
            for r in range(h):
                for c in range(0, w, 16):
                    row = tuple(v & 0x7FFF for v in words[r * w + c:r * w + c + 16])
                    if len(row) == 16 and len(set(row)) > 2:
                        rows.add(row)
    return rows


def vram_rects(timlist):
    rects = []
    for t in timlist:
        x, y, w, h, _ = t['image']
        rects.append(('img%d' % t['bpp'], x, y, w, h))
        if t['clut']:
            x, y, w, h, _ = t['clut']
            rects.append(('clut', x, y, w, h))
    return rects


def pages(rects):
    """Texture area in 64 x 256 pages (image rectangles only)."""
    return sum(w * h for kind, x, y, w, h in rects if kind != 'clut') / (64 * 256)


def bounds(rects, kind):
    sel = [(x, y, x + w, y + h) for k, x, y, w, h in rects if (k == 'clut') == (kind == 'clut')]
    if not sel:
        return ''
    return '%d..%d x %d..%d' % (min(r[0] for r in sel), max(r[2] for r in sel) - 1,
                                min(r[1] for r in sel), max(r[3] for r in sel) - 1)


def geometry(data: bytes):
    """Header and size of the tiles' records. A tile's primitive record size is
    (next block - its offset) / its count: PS1 mostly 20 bytes (0x24 / 0x2C
    polygons), arcade mostly 32, with other sizes in
    some tiles of both (other primitive kinds, left to E2)."""
    magic, tile, nrows = struct.unpack_from('<3I', data, 0)
    assert magic == 0x41, hex(magic)
    rows = [struct.unpack_from('<7I', data, 12 + 28 * r) for r in range(nrows)]
    starts = sorted({r[0] for r in rows} | {r[4] for r in rows} | {len(data) - 12})
    after = lambda o: min(x for x in starts if x > o)
    prim, vert = Counter(), Counter()
    for r in rows:
        if r[3]:
            prim[round((after(r[4]) - r[4]) / r[3], 1)] += 1
        if r[1]:
            vert[round((after(r[0]) - r[0]) / r[1], 1)] += 1
    side = int(round(nrows ** 0.5))
    fmt = lambda c: ' '.join(f'{k:g}:{n}' for k, n in c.most_common(3))
    return dict(tile=tile, grid=f'{side}x{side}', rows=nrows, used=sum(r[3] > 0 for r in rows),
                vertices=sum(r[1] for r in rows), prims=sum(r[3] for r in rows),
                prim_bytes=fmt(prim), vertex_bytes=fmt(vert))


class Ps1:
    def __init__(self, exe: Path, disc: Path):
        self.exe = exe.read_bytes()
        self.table, _, _ = bns_tool.load_us_table(exe)
        self.source = bns_tool.open_bns_source(disc)

    def u16(self, a: int) -> int:
        return struct.unpack_from('<H', self.exe, 0x800 + a - BASE)[0]

    def record(self, file_number: int):
        bns = self.u16(PS1_REMAP + 2 * file_number)
        e = self.table[bns]
        with self.source as s:
            return bns, s.read_at(e.offset, e.size)


def arc_members(data: bytes):
    n = struct.unpack_from('<I', data, 0)[0]
    pairs = struct.unpack_from('<%dI' % (2 * n), data, 4)
    return [data[o:o + s] for o, s in zip(pairs[0::2], pairs[1::2])]


def render(timlist, height):
    """RGB picture of a stage's TIMs at their VRAM place, each through its own
    palette. One pixel = one 4-bit texel: VRAM x is drawn 4 times wider (4096),
    an 8-bit texel takes 2 pixels and a 16-bit one 4."""
    import numpy as np
    from PIL import Image
    img = np.full((height, 4096, 3), 24, np.uint8)
    rgb = lambda v: np.stack([(v & 31) * 255 // 31, (v >> 5 & 31) * 255 // 31, (v >> 10 & 31) * 255 // 31], -1)
    for t in timlist:
        x, y, w, h, raw = t['image']
        words = np.frombuffer(raw[:w * h * 2], '<u2').reshape(h, w).astype(np.uint32)
        lut = None
        if t['clut']:
            cx, cy, cw, ch, craw = t['clut']
            lut = rgb(np.frombuffer(craw[:cw * ch * 2], '<u2').astype(np.uint32))
        if t['bpp'] == 16:
            col = np.repeat(rgb(words), 4, 1)
        else:
            per, bits = (4, 4) if t['bpp'] == 4 else (2, 8)
            idx = np.stack([(words >> (bits * k)) & ((1 << bits) - 1) for k in range(per)], -1).reshape(h, w * per)
            col = lut[np.minimum(idx, len(lut) - 1)] if lut is not None else np.repeat(idx[..., None] * 16 % 256, 3, -1)
            col = np.repeat(col, 4 // per, 1).astype(np.uint8)
        y1, x1 = min(height, y + h), min(4096, 4 * x + col.shape[1])
        if y < height:
            img[y:y1, 4 * x:x1] = col[:y1 - y, :x1 - 4 * x]
    return Image.fromarray(img)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--tekken3', type=Path, help='tekken3.zip (default: workspace/difficulty/source.json, '
                    'then C:/TekkenProject/discs/tekken3.zip)')
    ap.add_argument('--disc', type=Path, default=ROOT / 'disc/Tekken 3 (Track 1).bin')
    ap.add_argument('--exe', type=Path, default=ROOT / 'disc/SLUS_004.02')
    ap.add_argument('--output', type=Path, default=OUT)
    ap.add_argument('--png', action='store_true', help='also draw each stage\'s textures in its VRAM')
    args = ap.parse_args(argv)
    zip_path = args.tekken3
    if zip_path is None:
        src = ROOT / 'workspace/difficulty/source.json'
        zip_path = Path(json.loads(src.read_text())['tekken3']) if src.exists() else Path('C:/TekkenProject/discs/tekken3.zip')
    args.output.mkdir(parents=True, exist_ok=True)

    arcade, ps1 = Arcade(zip_path), Ps1(args.exe, args.disc)
    table = arcade.stage_table()
    arc_rows, arc_pal, arc_tims = {}, {}, {}
    for stage, archive in enumerate(table):
        header, files, packed = arcade.stage_files(archive)
        tl = [t for f in files[1:] for t in tims(f)]
        rects = vram_rects(tl)
        arc_tims[stage], arc_pal[stage] = tl, palettes(tl)
        arc_rows[stage] = dict(archive=archive, header=f'{header:07X}', files=len(files),
                               lz='yes' if packed else 'no',
                               tims=len(tl), pages=round(pages(rects), 2), images=bounds(rects, 'img'),
                               cluts=bounds(rects, 'clut'), **{'geo_' + k: v for k, v in geometry(files[0]).items()})
    ps1_rows, ps1_pal, ps1_tims = {}, {}, {}
    for arena in range(PS1_NARENA):
        tex_id, tex = ps1.record(PS1_TEX + arena)
        geo_id, geo = ps1.record(PS1_GEO + arena)
        members = arc_members(tex)
        tl = tims(members[0])
        rects = vram_rects(tl)
        ps1_tims[arena], ps1_pal[arena] = tl, palettes(tl)
        ps1_rows[arena] = dict(name=PS1_NAMES[arena], bns_textures=tex_id, bns_geometry=geo_id,
                               members=len(members), tims=len(tl), pages=round(pages(rects), 2),
                               images=bounds(rects, 'img'), cluts=bounds(rects, 'clut'),
                               **{'geo_' + k: v for k, v in geometry(geo).items()})

    with open(args.output / 'palettes.csv', 'w', newline='') as f:
        w = csv.writer(f)
        w.writerow(['ps1 arena'] + [f'arcade {s}' for s in range(ARC_NSTAGE)])
        best = {}
        for arena in range(PS1_NARENA):
            shared = [len(ps1_pal[arena] & arc_pal[s]) for s in range(ARC_NSTAGE)]
            w.writerow([arena] + shared)
            top = max(range(ARC_NSTAGE), key=lambda s: shared[s])
            best[arena] = (top, shared[top], len(ps1_pal[arena])) if shared[top] else (None, 0, len(ps1_pal[arena]))
    with open(args.output / 'stages.csv', 'w', newline='') as f:
        fields = (['ps1_arena'] + ['ps1_' + k for k in ps1_rows[0]] + ['shared_palettes', 'best_palette_match']
                  + ['arcade_' + k for k in arc_rows[0]])
        w = csv.DictWriter(f, fieldnames=fields)
        w.writeheader()
        for arena in range(PS1_NARENA):
            row = dict(ps1_arena=arena, **{'ps1_' + k: v for k, v in ps1_rows[arena].items()})
            top, n, total = best[arena]
            row['shared_palettes'] = f'{len(ps1_pal[arena] & arc_pal[arena]) if arena in arc_pal else 0}/{total}'
            row['best_palette_match'] = '' if top is None else f'{top} ({n})'
            if arena in arc_rows:
                row.update({'arcade_' + k: v for k, v in arc_rows[arena].items()})
            w.writerow(row)
    others = sorted(set(range(23, 39)) - set(table))
    print(f'arcade stage table 0x{ARC_STAGES:08X}: {table}; stage archives not in it: {others}')
    for arena in range(PS1_NARENA):
        p, a = ps1_rows[arena], arc_rows.get(arena)
        top, n, total = best[arena]
        print(f'{arena:2} {p["name"]:15} BNS {p["bns_textures"]}/{p["bns_geometry"]} {p["pages"]:5} pages '
              f'{p["geo_grid"]} tile {p["geo_tile"]:4} {p["geo_prims"]:5} prims | '
              + (f'arcade {arena:2} archive {a["archive"]} {a["pages"]:5} pages {a["geo_grid"]} tile {a["geo_tile"]:4} '
                 f'{a["geo_prims"]:5} prims | palettes shared {len(ps1_pal[arena] & arc_pal[arena])}/{total}, '
                 f'best {top} ({n})' if a else f'no arcade stage {arena} | best palette match {top} ({n})'))
    if args.png:
        (args.output / 'textures').mkdir(exist_ok=True)
        for arena, tl in ps1_tims.items():
            render(tl, 512).save(args.output / f'textures/ps1-{arena:02}.png')
        for stage, tl in arc_tims.items():
            render(tl, 1024).save(args.output / f'textures/arcade-{stage:02}.png')
    print(f'written: {args.output}')


if __name__ == '__main__':
    main()
