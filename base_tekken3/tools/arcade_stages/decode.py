#!/usr/bin/env python3
"""Arcade Stages, step E2: decoder of the Tekken 3 arcade stages (System 12).

    python tools/arcade_stages/decode.py [--tekken3 tekken3.zip] [--stages 0-12] [--summary]

Library of the E2 tools (validate.py, render.py, pack.py) and a summary of
every stage. Reads tekken3.zip through recon.Arcade; nothing extracted is
written in git. Routines read in the arcade program (TIPS section 91):

Geometry (file 0 of the stage archive, loaded at 0x802A1154 by 0x801D711C):
- u32 0x41, u32 tile size t (a tile is 10 t world units wide), u32 row count
  (256 = 16 x 16 tiles, row-major, z then x; < 256: the game builds nothing),
  then one row of 7 u32 per tile: vertex offset, vertex count, (row 0 only:
  end of the data), unused count, primitive offset, primitive count, 0.
  Offsets count from byte 12.
- vertices: 4 x s16 (x, y, z, 0), world coordinates / 10, y down. All the
  tiles share one GTE matrix (0x801D911C): the tile position is not added.
- primitives: one record each, u32 header first: byte 3 = kind (low 7 bits a
  GPU code, bit 7 = texture window), byte 2 = 1 when it carries colours,
  byte 1 + 1 = record size in words, byte 0 = GPU words of the packet. The
  builder 0x801D73E4 prepares the packets once (two buffers, 0x802FED04 +
  0xFA00 per buffer); the emitter 0x801DB560 writes the screen xy every frame.
  See KINDS for the field offsets.

Drawing order (0x801D911C, 0x801DB560): the vertices of a tile go through
RTPT (0x8019C82C) into the scratchpad, then each primitive in record order is
kept when NCLIP > 0 (vertices 0, 1 and Prim.cull) and prepended to slot
1 + SLOT_TABLE[tile] of a private OT of 16 (A, or B for the tiles of
LAYER_B), so later primitives and tiles are drawn first. Slot 15 is drawn
first, slot 0 last; OT B (main OT rank 1021) before OT A (1010). Tiles
outside the view are skipped (0x801DA734, visibility per tile on the stack).
"""
from __future__ import annotations

import argparse
import json
import pickle
import struct
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / 'tools/arcade_stages')]
import recon  # noqa: E402

WORK = ROOT / 'workspace/arcade-stages'
CACHE = WORK / 'e2/cache'
NSTAGE = 13
GP = 0x8021EEB0

# Header -> (name, vertices, textured, colour offsets, uv offsets, index
# offsets (u16), packet bytes). clut at +6, tpage at +0xA for the textured
# kinds. The emitter culls with NCLIP of vertices 0, 1 and CULL_LAST[header]
# (2 for the triangles and 0x3D010A0C, 3 for the other quads). Bit 31 of the header: texture window, bytes w, h at
# +0xE, +0xF and x, y at +0x12, +0x13 (SetTexWindow before the polygon, a
# reset after: packet + 0x18 bytes).
KINDS = {
    0x24000507: ('FT3', 3, True, (), (4, 8, 0xC), (0x12, 0x14, 0x16), 0x20),
    0x25010607: ('FT3', 3, True, (0x10,), (4, 8, 0xC), (0x14, 0x16, 0x18), 0x20),
    0x2C000709: ('FT4', 4, True, (), (4, 8, 0xC, 0x10), (0x16, 0x18, 0x1A, 0x1C), 0x28),
    0x2D010709: ('FT4', 4, True, (0x14,), (4, 8, 0xC, 0x10), (0x18, 0x1A, 0x1C, 0x1E), 0x28),
    0x29010305: ('F4', 4, False, (4,), (), (8, 0xA, 0xC, 0xE), 0x18),
    0x35010809: ('GT3', 3, True, (0x10, 0x14, 0x18), (4, 8, 0xC), (0x1C, 0x1E, 0x20), 0x28),
    0x3D010A0C: ('GT4', 4, True, (0x14, 0x18, 0x1C, 0x20), (4, 8, 0xC, 0x10), (0x24, 0x26, 0x28, 0x2A), 0x34),
}
# Kinds of the stage objects only (archives 40, 41; lit TMD primitives whose
# normals the game ignores: 0x801E2370 gives them a fixed colour).
KINDS.update({
    0x28000405: ('F4', 4, False, (4,), (), (0xA, 0xC, 0xE, 0x10), 0x18),
    0x30000406: ('G3', 3, False, (4,), (), (0xA, 0xE, 0x12), 0x1C),
    0x38000508: ('G4', 4, False, (4,), (), (0xA, 0xE, 0x12, 0x16), 0x24),
    0x3C00080C: ('GT4', 4, True, (), (4, 8, 0xC, 0x10), (0x16, 0x1A, 0x1E, 0x22), 0x34),
})
for _h in (0x2C000709, 0x2D010709, 0x3D010A0C):
    _k = KINDS[_h]
    KINDS[_h | 0x80000000] = _k[:6] + (_k[6] + 0x18,)
CULL_LAST = {h: 2 if KINDS[h][1] == 3 or h == 0x3D010A0C else 3 for h in KINDS}
GPU_CODE = {'FT3': 0x24, 'FT4': 0x2C, 'F4': 0x28, 'G3': 0x30, 'G4': 0x38, 'GT3': 0x34, 'GT4': 0x3C}

SLOT_TABLE = 0x8017C354      # 256 bytes: Manhattan distance of the tile to the centre
SKIPPED = {0: (0xC6, 0xC7, 0xD8), 5: (0x53, 0x63, 0x6B)}   # 0x801D73E4 builds nothing there


def layer_b(stage: int, tile: int) -> bool:
    """Tiles sent to OT B (main OT rank 1021, under OT A), 0x801D7520..0x801D7688."""
    if stage == 11:
        return tile in (0, 1, 0xE, 0xF, 0x10, 0x11, 0x1E, 0x1F, 0xE0, 0xE1, 0xEE, 0xEF, 0xF0, 0xF1, 0xFE, 0xFF)
    if stage == 4:
        return tile < 0x56
    if stage == 3:
        return tile >= 0xD2
    return False


@dataclass
class Prim:
    header: int
    kind: str
    idx: tuple            # vertex indices in the tile
    uv: tuple = ()        # (u, v) per vertex
    clut: int = 0
    tpage: int = 0
    colors: tuple = ()    # (r, g, b) per vertex (flat kinds: one, or none = 0x80)
    window: tuple | None = None   # texture window (x, y, w, h) in bytes of the record

    @property
    def cull(self):
        """The three vertices (positions in idx) whose NCLIP must be > 0."""
        return (0, 1, CULL_LAST[self.header])

    @property
    def code(self):
        return GPU_CODE[self.kind]

    def rgb(self, k=0):
        if not self.colors:
            return (0x80, 0x80, 0x80)
        return self.colors[k if len(self.colors) > 1 else 0]


@dataclass
class Tile:
    index: int
    verts: list
    prims: list
    slot: int = 0
    layer_b: bool = False
    skipped: bool = False
    raw_row: tuple = ()


@dataclass
class Geometry:
    tile_size: int
    tiles: list = field(default_factory=list)


def parse_prim(g: bytes, p: int) -> tuple[Prim, int]:
    hdr = struct.unpack_from('<I', g, p)[0]
    size = ((hdr >> 8 & 0xFF) + 1) * 4
    if hdr not in KINDS:
        raise ValueError(f'unknown primitive header {hdr:08X} at {p - 12:#x}')
    name, nv, textured, coffs, uvoffs, ioffs, _ = KINDS[hdr]
    u16 = lambda o: struct.unpack_from('<H', g, p + o)[0]
    prim = Prim(hdr, name, tuple(u16(o) for o in ioffs))
    if textured:
        prim.uv = tuple((g[p + o], g[p + o + 1]) for o in uvoffs)
        prim.clut, prim.tpage = u16(6), u16(0xA)
    prim.colors = tuple(tuple(g[p + o:p + o + 3]) for o in coffs)
    if hdr & 0x80000000:
        prim.window = (g[p + 0x12], g[p + 0x13], g[p + 0xE], g[p + 0xF])
    return prim, size


def parse_geometry(g: bytes, stage: int | None = None) -> Geometry:
    magic, tile, nrows = struct.unpack_from('<3I', g, 0)
    assert magic == 0x41, hex(magic)
    geo = Geometry(tile)
    slots = Arcade.slot_table() if nrows == 256 else bytes(nrows)
    for r in range(nrows):
        row = struct.unpack_from('<7I', g, 12 + 28 * r)
        vo, vn, _, _, po, pn, _ = row
        verts = [struct.unpack_from('<3h', g, 12 + vo + 8 * i) for i in range(vn)]
        prims, p = [], 12 + po
        for _ in range(pn):
            prim, size = parse_prim(g, p)
            for i in prim.idx:
                assert i < vn, (stage, r, i, vn)
            prims.append(prim)
            p += size
        geo.tiles.append(Tile(r, verts, prims, slot=1 + slots[r],
                              layer_b=stage is not None and layer_b(stage, r),
                              skipped=r in SKIPPED.get(stage, ()), raw_row=row))
    return geo


class Arcade(recon.Arcade):
    """recon.Arcade with a cache of the unpacked program and stage files under
    workspace/arcade-stages/e2/cache (never committed). cache=False (the
    setup, install.py): zip_path read as it is, nothing cached."""
    _program = None

    def __init__(self, zip_path: Path | None = None, cache: bool = True):
        if not cache:
            super().__init__(zip_path or default_zip())
            Arcade._program = self.program
            return
        CACHE.mkdir(parents=True, exist_ok=True)
        cache = CACHE / 'arcade.pkl'
        if cache.exists():
            self.__dict__.update(pickle.loads(cache.read_bytes()))
        else:
            super().__init__(zip_path or default_zip())
            cache.write_bytes(pickle.dumps(self.__dict__))
        Arcade._program = self.program

    @staticmethod
    def slot_table() -> bytes:
        return Arcade._program[SLOT_TABLE - recon.BASE:SLOT_TABLE - recon.BASE + 256]

    def word(self, a: int, n: int = 1, fmt: str = 'I'):
        v = struct.unpack_from('<%d%s' % (n, fmt), self.program, a - recon.BASE)
        return v if n > 1 else v[0]

    def bytes_at(self, a: int, n: int) -> bytes:
        return self.program[a - recon.BASE:a - recon.BASE + n]

    def stage(self, stage: int):
        """(geometry bytes, [TIMs as the game sends them]) of a stage."""
        (header, files), = self.archive(self.stage_table()[stage])
        raw = [self.rom(a, n) for a, n in files]
        return raw[0], load_tims(raw[1:], packed=stage == LZ_STAGE)

    def archive_files(self, k: int):
        (header, files), = self.archive(k)
        return [self.rom(a, n) for a, n in files]


def default_zip() -> Path:
    src = ROOT / 'workspace/difficulty/source.json'
    return Path(json.loads(src.read_text())['tekken3']) if src.exists() else Path('C:/TekkenProject/discs/tekken3.zip')


LZ_STAGE = 7
LOAD_BUFFER, LZ_BUFFER = 0x802A1154, 0x802AAD94   # 0x801D711C


def load_tims(files, packed=False):
    """The TIMs of files 1.. as 0x801D711C and 0x801DB408 send them.

    Each file is copied to 0x802A1154 (0x801BB648); stage 7 unpacks it to
    0x802AAD94 (0x80194C20, the LZ of tools/ttt1/extract.py), only 40000
    bytes further: a packed file longer than that (stage 7 file 5, 40651
    bytes) has its tail overwritten by its own output while it is read, and
    the arcade shows what comes out (20 lines of a 16 x 64 texture). The
    sender then walks TIMs while the word is 0x10, through whatever the
    buffer holds. Simulated here on one memory image, as on the board."""
    mem = bytearray(0x30000)
    written = bytearray(len(mem))
    gap = LZ_BUFFER - LOAD_BUFFER
    out = []
    for f in files:
        mem[:len(f)] = f
        written[:len(f)] = b'' * len(f)
        start = 0
        if packed:
            start, i, o = gap, 0, gap
            while True:
                flags = mem[i]; i += 1
                if flags == 0:
                    break
                while flags >= 2:
                    if flags & 1:
                        mem[o] = mem[i]; written[o] = 1; o += 1; i += 1
                    else:
                        v = mem[i] << 8 | mem[i + 1]; i += 2
                        dist = v & 0x7FF or 0x800
                        for _ in range(v >> 11 & 0x1F or 0x20):
                            mem[o] = mem[o - dist]; written[o] = 1; o += 1
                    flags >>= 1
        p = start
        while struct.unpack_from('<I', mem, p)[0] == 0x10:
            flags = struct.unpack_from('<I', mem, p + 4)[0]
            assert flags & 8, 'a TIM without CLUT: 0x801DB408 would loop'
            size, x, y, w, h = struct.unpack_from('<I4H', mem, p + 8)
            clut = (x, y, w, h, bytes(mem[p + 20:p + 20 + w * h * 2]))
            q = p + 8 + (size & ~3)
            isize, x, y, w, h = struct.unpack_from('<I4H', mem, q)
            image = (x, y, w, h, bytes(mem[q + 12:q + 12 + w * h * 2]))
            end = q + (isize & ~3)
            assert all(written[p:end]), 'a TIM read from memory the simulation never wrote'
            out.append(dict(bpp=(4, 8, 16)[flags & 3], clut=clut, image=image))
            p = end
    return out


# Stage 8: 0x801A70FC (called by the loader) loads 20 palettes of 16 colours
# from the program, positions at 0x80174950 (u16 x, y, 4 bytes each), colours
# at 0x80164C28 (32 bytes each).
CLUT_PATCHES = {8: (0x80174950, 0x80164C28, 20)}


# Animated textures, stages 6 and 12 (0x801E2090 at the load, 0x801E2114
# every frame): archive 39 holds 16 frames of 16 x 64 (4-bit, CLUT 512,140),
# sent to VRAM x 512..543, y 512..1023; every 4 frames the game moves the next
# one (u16 x, y at 0x8020456C + 8 * frame) onto the stage texture below.
ANIMATED = {6: (16, 896), 12: (16, 960)}
ANIM_ARCHIVE, ANIM_TABLE, ANIM_FRAMES, ANIM_PERIOD = 39, 0x8020456C, 16, 4


def anim_frames(arcade):
    """[(x, y)] of the 16 frames in VRAM."""
    return [arcade.word(ANIM_TABLE + 8 * i, 4, 'H')[0::2] for i in range(ANIM_FRAMES)]


def vram(tims, height=1024, stage=None, arcade=None, anim_frame=None):
    """System 12 VRAM (height x 1024 u16) after the stage load: the TIMs at
    their place, then the program's palette patches and, for stages 6 and
    12, the animation frames (and frame anim_frame moved in place); plus the
    rectangles."""
    import numpy as np
    v = np.zeros((height, 1024), np.uint16)
    rects = []
    if stage in ANIMATED:
        tims = list(tims) + load_tims(arcade.archive_files(ANIM_ARCHIVE))
    for t in tims:
        for kind, r in (('image', t['image']), ('clut', t['clut'])):
            x, y, w, h, raw = r
            v[y:y + h, x:x + w] = np.frombuffer(raw[:w * h * 2], '<u2').reshape(h, w)
            rects.append((kind, x, y, w, h, t['bpp']))
    if stage in CLUT_PATCHES:
        where, colours, n = CLUT_PATCHES[stage]
        for i in range(n):
            x, y = arcade.word(where + 4 * i, 2, 'H')
            v[y, x:x + 16] = arcade.word(colours + 32 * i, 16, 'H')
    if stage in ANIMATED and anim_frame is not None:
        (sx, sy), (dx, dy) = anim_frames(arcade)[anim_frame], ANIMATED[stage]
        v[dy:dy + 64, dx:dx + 16] = v[sy:sy + 64, sx:sx + 16].copy()
    return v, rects


def numbers(spec: str):
    out = []
    for part in spec.split(','):
        a, _, b = part.partition('-')
        out += range(int(a), int(b or a) + 1)
    return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--tekken3', type=Path)
    ap.add_argument('--stages', default='0-12')
    args = ap.parse_args(argv)
    from collections import Counter
    arcade = Arcade(args.tekken3)
    for s in numbers(args.stages):
        g, tims = arcade.stage(s)
        geo = parse_geometry(g, s)
        kinds = Counter(('w' if p.window else '') + p.kind + ('c' if p.colors else '') for t in geo.tiles for p in t.prims)
        nv = sum(len(t.verts) for t in geo.tiles)
        print(f'stage {s:2}: tile {geo.tile_size:4}, {nv:5} vertices, {sum(kinds.values()):5} primitives',
              ' '.join(f'{k}:{n}' for k, n in sorted(kinds.items())),
              f'| OT B tiles {sum(t.layer_b for t in geo.tiles)}, skipped {sum(t.skipped for t in geo.tiles)}')


if __name__ == '__main__':
    main()
