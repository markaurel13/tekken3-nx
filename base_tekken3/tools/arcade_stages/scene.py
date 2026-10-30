"""Arcade Stages E2: what the arcade stage drawing sends to the GPU, from the
decoded data and a camera. Python reference of the routines E4 ports.

A command is a dict: kind ('tile', 'sprite', 'f4', 'g4', 'ft3', 'ft4', 'gt3',
'gt4', 'tpage', 'window'), xy [(x, y)...], rgb [(r, g, b)...] (one, or one
per vertex), uv [(u, v)...], clut, tpage, w, h; exactly the fields of the
GPU packet, so validate.py compares them with the packets in RAM.

Far background (0x801A71AC set-up per stage, 0x801A7688 per frame, main OT
rank 1023): a strip of rows x 64-pixel cells scrolled by the camera yaw
around the fight, the top of the strip moved by the pitch; plain bands above
(top colour) and below (bottom colour, down to 3/4 of the screen). Data per
stage in the program: 0x801749A0 (20 bytes: columns, rows, pointers to the
cell textures, CLUTs and kinds, y offset, degrees per column period),
0x80174AA4 (pitch factor), 0x80174AC0 (12 bytes: cell, top, bottom colours
with their switches). Stage 3: a blue gradient instead of the cells; stage
6: one plain colour on the whole screen.
"""
from __future__ import annotations

import struct
from dataclasses import dataclass

FARBG, FARBG_PITCH, FARBG_COLOURS = 0x801749A0, 0x80174AA4, 0x80174AC0
GRAPH_TYPE = 4      # u8 0x80215C48 on the board: standard tpage, bit 11 = y + 512


def s12(a):
    """12-bit angle to -0x800..0x7FF."""
    a &= 0xFFF
    return a - 0x1000 if a >= 0x800 else a


def tdiv(a, b):
    """C division (towards zero), as the MIPS div the game uses."""
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b >= 0) else -q


def tmod(a, b):
    return a - tdiv(a, b) * b


@dataclass
class FarBg:
    stage: int
    cols: int          # cells of texture before it repeats (u16 0x894)
    rows: int
    ring: int          # cells of the kind map around the whole turn (0x8B0)
    desc: list         # rows x cols u16: tpage x (bits 8-11), y (12-13), u (0-1), v (2-3), 64 x 64 cells
    cluts: list        # rows x cols u16
    kinds: list        # rows x ring bytes: '1' textured, 't' cell colour, 'd' bottom colour, else empty
    yoff: int          # s16, halved in low resolution
    degrees: int       # u16 0x8AC: degrees of the turn per `cols` cells
    pitch: int         # s16 0x80174AA4, halved in low resolution
    cell_rgb: tuple
    top_rgb: tuple
    top_on: int
    bottom_rgb: tuple
    bottom_on: int

    def tpage(self, d):
        x, y = (d >> 2) & 0x3C0, (d >> 4) & 0x300
        if GRAPH_TYPE in (1, 2):
            return x >> 6 | y >> 3
        return ((y & 0x100) >> 4 | x >> 6) | (y & 0x200) << 2


def parse_farbg(arcade, stage: int, hires=False) -> FarBg:
    cols, rows, p_desc, p_clut, p_kind, yoff, degrees = struct.unpack(
        '<hhIIIhH', arcade.bytes_at(FARBG + 20 * stage, 20))
    ring = tdiv(0x2D000, degrees)
    ring = tdiv(ring, 64)
    pitch = arcade.word(FARBG_PITCH + 2 * stage, 1, 'h')
    if not hires:
        yoff, pitch = yoff >> 1, pitch >> 1
    c = arcade.bytes_at(FARBG_COLOURS + 12 * stage, 12)
    desc = arcade.word(p_desc, rows * cols, 'H')
    cluts = arcade.word(p_clut, rows * cols, 'H')
    kinds = arcade.bytes_at(p_kind, rows * ring)
    desc = desc if isinstance(desc, tuple) else (desc,)
    cluts = cluts if isinstance(cluts, tuple) else (cluts,)
    return FarBg(stage, cols, rows, ring, [list(desc[r * cols:(r + 1) * cols]) for r in range(rows)],
                 [list(cluts[r * cols:(r + 1) * cols]) for r in range(rows)],
                 [list(kinds[r * ring:(r + 1) * ring]) for r in range(rows)],
                 yoff, degrees, pitch, tuple(c[0:3]), tuple(c[4:7]), c[7], tuple(c[8:11]), c[11])


def farbg_commands(fb: FarBg, yaw: int, pitch: int, hires=False, black=False):
    """0x801A7688: the far background for camera yaw (minus the fight axis
    0x8021FB60 of the previous frame) and pitch, in drawing order (rank 1023,
    so the first command is drawn last: the list is in OT order reversed,
    i.e. drawing order)."""
    H = 0xF0 << int(hires)
    linked = []                     # AddPrim order (each one drawn before the previous)
    a = tdiv(((-yaw) & 0xFFF) << 9, per_degree(fb.degrees))
    scroll = tmod(a, fb.cols << 6)
    col0, kind0 = tdiv(scroll, 64), tdiv(tmod(a, fb.ring << 6), 64)
    y = tdiv(-s12(pitch) * fb.pitch, 256) + fb.yoff
    if black:
        return [dict(kind='tile', code=0x60, xy=[(0, 0)], w=0x200, h=H, rgb=[(0, 0, 0)])]
    s = fb.stage
    if s == 3:
        y = min(y, 0)
        y2 = y + (0xC0 if hires else 0x60)
        linked.append(dict(kind='g4', code=0x38, xy=[(0, y), (0x200, y), (0, y2), (0x200, y2)],
                           rgb=[(0, 0x14, 0x66), (0, 0x14, 0x66), (0, 0, 0), (0, 0, 0)]))
        linked.append(dict(kind='tile', code=0x60, xy=[(0, y2)], w=0x200, h=H * 75 // 100 - y2, rgb=[fb.bottom_rgb]))
        return linked[::-1]
    if s == 6:
        return [dict(kind='tile', code=0x60, xy=[(0, 0)], w=0x200, h=H, rgb=[fb.top_rgb])]
    if s in (11, 0, 5) and y > 0:
        y = 0
    if s == 11:
        y = max(y, -0xC0 if hires else -0x60)
    if y > 0 and fb.top_on:
        linked.append(dict(kind='tile', code=0x60, xy=[(0, 0)], w=0x200, h=y, rgb=[fb.top_rgb]))
    step = 0x40 if hires else 0x20
    for r in range(fb.rows):
        x = tdiv(scroll, 64) * 64 - scroll
        for c in range(9):
            if not (y < -0x40 or y > H):
                kind = fb.kinds[r][tmod(kind0 + c, fb.ring)]
                k = tmod(col0 + c, fb.cols)
                if kind == 0x31:
                    d = fb.desc[r][k]
                    tp, u, v = fb.tpage(d), (d & 3) << 6, (d << 4) & 0xC0
                    clut = fb.cluts[r][k]
                    if hires and not (s == 5 and r == 3):
                        linked.append(dict(kind='sprite', code=0x64, xy=[(x, y)], w=0x40, h=0x40, uv=[(u, v)], clut=clut,
                                           rgb=[(0x80, 0x80, 0x80)]))
                        linked.append(dict(kind='tpage', code=0xE1, value=tp & 0x27FF if GRAPH_TYPE in (1, 2) else tp))
                    else:
                        v0, v1 = (v + 0x3F, v) if s == 5 and r == 3 else (v, v + 0x3F)
                        linked.append(dict(kind='ft4', code=0x2C, xy=[(x, y), (x + 0x40, y), (x, y + step), (x + 0x40, y + step)],
                                           uv=[(u, v0), (u + 0x3F, v0), (u, v1), (u + 0x3F, v1)], clut=clut, tpage=tp,
                                           rgb=[(0x80, 0x80, 0x80)]))
                elif kind == 0x74:
                    linked.append(dict(kind='tile', code=0x60, xy=[(x, y)], w=0x40, h=step, rgb=[fb.cell_rgb]))
                elif kind == 0x64:
                    linked.append(dict(kind='tile', code=0x60, xy=[(x, y)], w=0x40, h=step, rgb=[fb.bottom_rgb]))
            x += 0x40
        y += step
    if y < H and fb.bottom_on:
        y = max(y, 0)
        linked.append(dict(kind='tile', code=0x60, xy=[(0, y)], w=0x200, h=H * 75 // 100 - y, rgb=[fb.bottom_rgb]))
    return linked[::-1]


def per_degree(degrees):
    """(degrees << 12) / 360 as 0x801A7688 computes it (signed multiply by
    0xB60B60B7, high word)."""
    v = degrees << 12
    hi = (v * (0xB60B60B7 - (1 << 32))) >> 32
    return ((hi + v) >> 8) - (v >> 31)


# ---------------------------------------------------------------------------
# Floor (not in the stage archive: per-stage data in the program). Set up by
# 0x801A6C30 at the stage load, drawn every frame by 0x801A6DBC through the
# pointer gp+0x88C: 0x801A91A8 -> 0x801A941C (kind 0), 0x801AA9C0 (kinds 1, 2).
# A 10 x 10 grid of 2500-unit cells, centred on the fighters' midpoint snapped
# to 2500, its texture map wrapping every 10 cells, GT4 quads in main OT ranks
# 1021 - (index of the cell's texture among the distinct ones).
# ---------------------------------------------------------------------------
FLOOR_STAGES = 0x801748EC        # 8 bytes per stage: u32 floor data, u32 ?
FLOOR_TMD = 0x801751B4           # LZ: TMD of the grid (121 vertices, u32 z | x << 16, y = 0)
FLOOR_COUNTS = {0: 0x80174F5C, 1: 0x80175088}   # 10 x 10 bytes: sub-quads per cell
FLOOR_LISTS = {0: 0x80174FC0, 1: 0x801750EC}    # 10 x 10 s16: byte offset of the cell's quads in the TMD primitives
FLOOR_FOG = 0x80174B5C           # 1024 bytes, kind 0
FLOOR_NEAR = 0x801FF390          # s32 per stage: added to the subdivision distance
SIN, COS = 0x80175944, 0x80176144

@dataclass
class Floor:
    stage: int
    kind: int          # 0: fog by depth; 1: a light under each fighter; 2: one light at their midpoint
    fog: int
    cells: list        # 10 x 10 (w0, w1): w0 = tpage << 16 | v << 8 | u, bits 25/26 mirror/flip, 30-31 rotation; w1 = clut << 16
    halves: list       # 20 x 20 (w0, w1), for the quads split near the camera
    unique: list       # 10 x 10: index of the cell's w0 & 0x39FFFFFF among the distinct ones (OT rank offset)
    verts: list        # 121 (x, z)
    quads: dict        # (row, col) -> [u32 of four vertex indices]
    near: int          # s32 FLOOR_NEAR[stage]
    uv: dict           # sub-quad count -> 4 rotations x n sub-quads x 4 corners (u | v << 8)


def lz(src, i=0):
    out = bytearray()
    while True:
        flags = src[i]; i += 1
        if flags == 0:
            return bytes(out)
        while flags >= 2:
            if flags & 1:
                out.append(src[i]); i += 1
            else:
                v = src[i] << 8 | src[i + 1]; i += 2
                dist = v & 0x7FF or 0x800
                for _ in range(v >> 11 & 0x1F or 0x20):
                    out.append(out[-dist])
            flags >>= 1


def parse_floor(arcade, stage: int) -> Floor:
    p = arcade.word(FLOOR_STAGES + 8 * stage)
    kind, fog, _flag = arcade.word(p + 0xFA4, 3, 'h')
    if kind not in (0, 1, 2):
        raise ValueError(f'stage {stage}: floor kind {kind}')
    words = arcade.word(p + 4, 200 + 800, 'I')
    cells = [[(words[2 * (10 * r + c)], words[2 * (10 * r + c) + 1]) for c in range(10)] for r in range(10)]
    halves = [[(words[200 + 2 * (20 * r + c)], words[201 + 2 * (20 * r + c)]) for c in range(20)] for r in range(20)]
    seen, unique = [], []
    for r in range(10):
        row = []
        for c in range(10):
            v = cells[r][c][0] & 0x39FFFFFF
            if v not in seen:
                seen.append(v)
            row.append(seen.index(v))
        unique.append(row)
    if len(seen) > 40:
        raise ValueError(f'stage {stage}: {len(seen)} distinct floor cells, the game gives up over 40')
    # the grid TMD (0x8026276C, mapped by GsMapModelingData)
    packed = arcade.bytes_at(FLOOR_TMD, 0x4000)
    tmd = lz(packed)
    vtop, nvert, _ntop, _nn, ptop = struct.unpack_from('<5I', tmd, 12)
    base = 12                                    # offsets count from the object table
    verts = []
    for i in range(nvert):
        w = struct.unpack_from('<I', tmd, base + vtop + 4 * i)[0]
        z, x = w & 0xFFFF, w >> 16
        verts.append((x - 0x10000 if x & 0x8000 else x, z - 0x10000 if z & 0x8000 else z))
    table = 0 if kind == 0 else 1
    counts = arcade.bytes_at(FLOOR_COUNTS[table], 100)
    offs = arcade.word(FLOOR_LISTS[table], 100, 'h')
    quads = {}
    for r in range(10):
        for c in range(10):
            n = counts[c + 10 * r]
            o = base + ptop + offs[c + 10 * r]
            quads[r, c] = list(struct.unpack_from('<%dI' % n, tmd, o)) if n else []
    return Floor(stage, kind, fog, cells, halves, unique, verts, quads,
                 arcade.word(FLOOR_NEAR + 4 * stage, 1, 'i'), floor_uv_tables(arcade, kind))


def constant_stores(arcade, start, end):
    """{address: u16} of the `sh` of constants between two code addresses:
    0x801A87C8 and 0x801AA424 fill their uv tables that way (ori/addiu of
    immediates, lui $at + sh). Read from the program, nothing copied here."""
    regs, out = {0: 0}, {}
    for a in range(start, end, 4):
        w = arcade.word(a)
        op, rs, rt, imm = w >> 26, w >> 21 & 31, w >> 16 & 31, w & 0xFFFF
        if op in (0x0D, 0x09) and rs == 0:           # ori / addiu rt, $zero, imm
            regs[rt] = imm if op == 0x0D else imm - (imm >> 15 << 16)
        elif op == 0x0F:                              # lui
            regs[rt] = imm << 16
        elif op == 0x29 and rs == 1 and rt in regs:   # sh rt, off($at)
            out[(regs[1] + imm - (imm >> 15 << 16)) & 0xFFFFFFFF] = regs[rt] & 0xFFFF
        elif op == 0x29 and rs == 1:
            raise ValueError(f'unknown register at {a:08X}')
    return out


def floor_uv_tables(arcade, kind):
    """The corner offsets (u | v << 8 per packet vertex 0..3) of a cell's
    sub-quads, per rotation, as the set-up writes them: kind 0 at 0x80265654
    (1 sub-quad), 0x80265674 (2), 0x802656B4 (4); kinds 1, 2 at 0x8026861C,
    0x8026863C, 0x8026867C."""
    if kind == 0:
        stores, bases = constant_stores(arcade, 0x801A87C8, 0x801A8BF0), (0x80265654, 0x80265674, 0x802656B4)
    else:
        stores, bases = constant_stores(arcade, 0x801AA424, 0x801AA8A0), (0x8026861C, 0x8026863C, 0x8026867C)
    out = {}
    for n, b in zip((1, 2, 4), bases):
        out[n] = [[tuple(stores.get(b + 8 * (rot * n + q) + 2 * k, 0) for k in range(4)) for q in range(n)]
                  for rot in range(4)]
    return out


@dataclass
class View:
    """What the floor needs from the frame: the world camera matrix (0x8036EBFC,
    built by 0x801A26A4 from the camera struct 0x802C6BD4: rotations -pitch,
    -yaw, -roll, row y scaled by (0xA00 << hires) * 95 / 100 / 4096), the
    camera struct (pitch, yaw, position), both fighters (x, z: +0x44C, +0x454
    of 0x8031E154 and 0x8031FC38) and the GTE screen (H, OFX, OFY)."""
    rot: tuple          # 3 x 3
    trans: tuple
    pitch: int
    yaw: int
    cam: tuple          # x, y, z
    p1: tuple
    p2: tuple
    h: int = 600
    ofx: int = 256 << 16
    ofy: int = 120 << 16
    hires: bool = False


SQRT_TABLE = 0x8021AA3C


@dataclass
class Tables:
    """Program tables the floor reads: sine and cosine (4096 s16 each, 4.12),
    PSY-Q SquareRoot0's table (192 s16), the kind 0 fog ramp (1024 bytes),
    0x801AEC2C's square root table (192 s16)."""
    sin: tuple
    cos: tuple
    sqrt: tuple
    fog: bytes
    sqrt2: tuple

    @classmethod
    def from_arcade(cls, arcade):
        return cls(arcade.word(SIN, 4096, 'h'), arcade.word(COS, 4096, 'h'),
                   arcade.word(SQRT_TABLE, 192, 'h'), arcade.bytes_at(FLOOR_FOG, 1024),
                   arcade.word(0x801AEC94, 192, 'h'))


def sqrt0(tables, a, table=None):
    """PSY-Q SquareRoot0 (0x801EFDF4), table at 0x8021AA3C; 0x801AEC2C is
    the same with its own table (0x801AEC94, Tables.sqrt2)."""
    if a <= 0:
        return 0
    lz = (32 - a.bit_length()) & ~1
    v = a >> (24 - lz) if lz < 24 else a << (lz - 24)
    t = (table or tables.sqrt)[v - 0x40]
    return ((t << ((31 - lz) >> 1)) & 0xFFFFFFFF) >> 12


def shift12(v):
    """v >> 12 rounded towards zero (the game adds 0xFFF to negatives)."""
    return (v + 0xFFF if v < 0 else v) >> 12


def floor_center(floor, view, tables):
    """0x801A91A8 (and the head of 0x801AA9C0): None when the floor is not
    drawn, else the point the grid follows."""
    if tables.sin[(view.pitch + 0xEF) & 0xFFF] <= 0:
        return None
    x = tdiv(view.p1[0] + view.p2[0], 2)
    z = tdiv(view.p1[1] + view.p2[1], 2)
    if floor.kind == 0:
        if (view.pitch & 0xFFF) and not view.pitch & 0x800 and view.cam[1] < -0x6B8:
            return floor_center_high(view, tables)
        if s12(view.pitch) < -0x25:
            a = (view.yaw + 0x400) & 0xFFF
            x -= shift12(tables.cos[a] * 0x9C4)
            z -= shift12(tables.sin[a] * 0x9C4)
    return x, z


def floor_center_high(view, tables):
    """0x801A8DD0: camera above y -0x6B8 looking down: the grid follows the
    point the camera looks at instead of the fighters."""
    cx, cy, cz = view.cam
    u = (view.pitch + 0xEF) & 0xFFF
    i8 = tdiv(abs(cy) * tables.cos[u], tables.sin[u])
    i2 = sqrt0(tables, i8 * i8 + cy * cy, tables.sqrt2)
    a = (view.yaw + 0x400) & 0xFFF
    c, s = tables.cos[a], tables.sin[a]
    i2 = tdiv(i2 << 8, 0x218)
    x0 = shift12(i8 * c) + cx
    z0 = shift12(i8 * s) + cz
    q = (view.yaw + 0x600) & 0xFFF
    quarter, odd = q >> 10, q >> 9 & 1
    d = min(shift12((-0x4CE - cy) * 0x8555), 0x30D4)
    if quarter in (1, 3):
        t = shift12(i2 * c)
        t = t if odd else -t
        z = z0 + t - 0x4E2 + d if quarter == 1 else z0 + t + 0x4E2 - d
        return tdiv(c * (z - cz), s) + cx, z
    t = shift12(i2 * s)
    t = -t if odd else t
    x = x0 + t - 0x4E2 + d if quarter == 0 else x0 + t + 0x4E2 - d
    return x, tdiv(s * (x - cx), c) + cz


def floor_commands(floor, view, tables):
    """0x801A941C / 0x801AA9C0: [(rank, command)] in link order (each one
    prepended to its main OT rank, so drawn before the ones linked earlier
    at the same rank). Commands carry 'packet' = (pool, index): pool 0 the
    cells (0x80262D50 or 0x80265D18, + 0x1450 per buffer, 0x34 each), pool 1
    the quads split near the camera (0x8036EF18 + 0x8F0 per buffer)."""
    import numpy as np
    import gte
    centre = floor_center(floor, view, tables)
    if centre is None:
        return []

    def snap(v):
        q = tdiv(v + 0x4E2, 0x9C4)
        return q - 1 if v + 0x4E2 < 0 else q
    cx, cz = snap(centre[0]), snap(centre[1])
    col0 = tmod(cx, 10) % 10
    row0 = 9 - tmod(cz, 10) % 10
    # 0x801AEB10: the world matrix moved to the snapped centre
    rot = np.array(view.rot, np.int64)
    off = (cx * 0x9C4, 0, cz * 0x9C4)
    t = [(sum(int(rot[i, k]) * off[k] for k in range(3)) >> 12) + view.trans[i] for i in range(3)]
    cam = gte.Camera(rot, np.array(t, np.int64), view.ofx, view.ofy, view.h)
    sx, sy, sz = gte.rtps(cam, np.array([(x, 0, z) for x, z in floor.verts], np.int64))
    H = 0xF0 << int(view.hires)
    mx, mz = tdiv(view.p1[0] + view.p2[0], 2), tdiv(view.p1[1] + view.p2[1], 2)
    e0 = sqrt0(tables, (view.cam[0] - mx) ** 2 + (view.cam[2] - mz) ** 2)
    sp = tables.sin[(view.pitch + 0xEF) & 0xFFF]
    e8 = (tdiv(abs(view.cam[1]) << 12, sp) if sp else 0) + floor.near
    fogtab = tables.fog if floor.kind == 0 and floor.fog else None
    shade = {}

    def colour(i):
        if i in shade:
            return shade[i]
        x, z = floor.verts[i]
        if floor.kind == 0:
            d = max(int(sz[i]) - e0, 0)
            k = (d * 8 * 0x1E0 + d * -0x1E5) * 2
            k = min((k + 0xFFFF if k < 0 else k) >> 16, 0x3FF)
            c = fogtab[k] * floor.fog
            c = ((c + 0x7F if c < 0 else c) >> 7) + 0x80
            c = 0 if c < 0 else min(c, 0xFE)
        elif floor.kind == 1:
            px1, pz1 = view.p1[0] - cx * 0x9C4, view.p1[1] - cz * 0x9C4
            px2, pz2 = view.p2[0] - cx * 0x9C4, view.p2[1] - cz * 0x9C4
            d1 = (px1 - x) ** 2 + (pz1 - z) ** 2 or 1
            d2 = (px2 - x) ** 2 + (pz2 - z) ** 2 or 1
            c = min(0xFE, tdiv(0xF200000, d1) + tdiv(0xF200000, d2))
        else:
            d = (mx - (x + cx * 0x9C4)) ** 2 + (mz - (z + cz * 0x9C4)) ** 2 or 1
            c = min(0xC0, tdiv(0x7A120000, d))
        shade[i] = c & 0xFF
        return shade[i]

    out, split, cell, rr = [], 0, 0, row0
    for r in range(10):
        cc = col0
        for c in range(10):
            w0, w1 = floor.cells[rr][cc]
            quads = floor.quads[r, c]
            rank = 1021 - floor.unique[rr][cc]
            for q, idx in enumerate(quads):
                vi = [idx >> (8 * k) & 0xFF for k in range(4)]
                packet, cell = cell, cell + 1
                if not all(sz[i] for i in vi):
                    continue
                xy = [(int(sx[i]), int(sy[i])) for i in vi]
                if not (any(0 <= y < H for x, y in xy) and any((x & 0xFFFF) < 0x200 for x, y in xy)):
                    continue
                uvo = floor.uv[len(quads)][w0 >> 30][q]
                if floor.kind != 0 or floor.fog:
                    rgb = [(colour(i),) * 3 for i in vi]
                else:
                    rgb = [(0x80, 0x80, 0x80)] * 4
                near = any(sz[i] < e8 for i in vi) or ((view.pitch - 0x400) & 0xFFFFFFFF) < 0x201
                if split < 11 and near:
                    for k, cmd in enumerate(split_quad(floor, rr, cc, xy, rgb, uvo)):
                        cmd['packet'] = (1, 4 * split + k)
                        out.append((rank, cmd))
                    split += 1
                else:
                    out.append((rank, dict(kind='gt4', code=0x3C, xy=xy, rgb=rgb, packet=(0, packet),
                                           **floor_uv(w0 & 0x3FFFFFFF, w1, uvo))))
            cc = 0 if cc == 9 else cc + 1
        rr = 9 if rr == 0 else rr - 1
    return out


def floor_uv(w0, w1, uvo):
    """0x801A9354: uv, clut and tpage of a floor quad from a cell's (w0, w1)
    and the corner offsets; bit 25 mirrors, bit 26 flips."""
    o = list(uvo)
    if w0 & 0x2000000:
        o = [o[1], o[0], o[3], o[2]]
    if w0 & 0x4000000:
        o = [o[2], o[3], o[0], o[1]]
    base = w0 & 0xFFFF
    uvs = [(base + k) & 0xFFFF for k in o]
    return dict(uv=[(u & 0xFF, u >> 8) for u in uvs], clut=(w1 >> 16) & 0xFFFF,
                tpage=((w0 & 0x39FF0000) >> 16) & 0xFFFF)


def split_quad(floor, rr, cc, xy, rgb, uvo):
    """The four quads of a cell split near the camera, each textured by one
    of the cell's four half cells (floor data + 0x324, 20 x 20)."""
    (x0, y0), (x1, y1), (x2, y2), (x3, y3) = xy
    c0, c1, c2, c3 = (c[0] for c in rgb)
    m01 = (tdiv(x0 + x1, 2), tdiv(y0 + y1, 2))
    m02 = (tdiv(x0 + x2, 2), tdiv(y0 + y2, 2))
    m23 = (tdiv(x2 + x3, 2), tdiv(y2 + y3, 2))
    m13 = (tdiv(x1 + x3, 2), tdiv(y1 + y3, 2))
    mid = (tdiv(m02[0] + m13[0], 2), tdiv(m01[1] + m23[1], 2))
    k01, k02, k13, k23 = (c0 + c1) >> 1, (c0 + c2) >> 1, (c1 + c3) >> 1, (c2 + c3) >> 1
    kc = (k01 + k02 + k13 + k23) >> 2
    g = lambda k: (k & 0xFF,) * 3
    parts = [([xy[0], m01, m02, mid], [rgb[0], g(k01), g(k02), g(kc)], (0, 0)),
             ([m01, xy[1], mid, m13], [g(k01), rgb[1], g(kc), g(k13)], (0, 1)),
             ([m02, mid, xy[2], m23], [g(k02), g(kc), rgb[2], g(k23)], (1, 0)),
             ([mid, m13, m23, xy[3]], [g(kc), g(k13), g(k23), rgb[3]], (1, 1))]
    out = []
    for pxy, prgb, (hr, hc) in parts:
        w0, w1 = floor.halves[2 * rr + hr][2 * cc + hc]
        out.append(dict(kind='gt4', code=0x3C, xy=pxy, rgb=prgb, **floor_uv(w0, w1, uvo)))
    return out


# ---------------------------------------------------------------------------
# Tiles (0x801D911C, 0x801DB560) and the whole frame.
# ---------------------------------------------------------------------------
def tex_window(x, y, w, h):
    """E2 word of PSY-Q SetTexWindow (0x801E9854), as validate.py measures it."""
    if not (w or h or x or y):
        return 0xE2000000
    return (0xE2000000 | (((~(w - 1)) & 0xFF) >> 3) | ((((~(h - 1)) & 0xFF) >> 3) << 5)
            | ((x >> 3) << 10) | ((y >> 3) << 15))


def prim_command(prim, xy):
    c = dict(kind=prim.kind.lower(), code=prim.code, xy=xy,
             rgb=[prim.rgb(k) for k in range(len(xy))] if prim.kind[0] == 'G' else [prim.rgb()])
    if prim.uv:
        c.update(uv=list(prim.uv), clut=prim.clut, tpage=prim.tpage)
    return c


CONE_DIRS = 0x8017CB78      # 8 bytes per 12-bit angle: s16 sine, cosine (1.15)
CONE_FOV = 0x80186858       # s16 per projection distance H (u32 0x8021FAF4): cone width


def cone_inputs(arcade, h, special=False):
    """The direction table and the cone width 0x801D911C passes for the
    projection distance h (u32 0x8021FAF4: 600 to 800 in the fights): the
    table's value x 9 / 10 (x 11 / 10 when u32 0x8021FD20 = 2)."""
    w = arcade.word(CONE_DIRS, 4 * 4096, 'h')
    dirs = [(w[4 * a], w[4 * a + 1]) for a in range(4096)]
    return dirs, tdiv(arcade.word(CONE_FOV + 2 * h, 1, 'h') * (11 if special else 9), 10)


def tile_visibility(dirs, fov, yaw, x, z, size):
    """0x801DA734, the tiles to draw: 256 ints (> 0: drawn), as the game
    leaves them on the stack. fov: cone width (12-bit angle); yaw: camera
    yaw minus the fight axis; x, z: the camera minus the fighters' midpoint
    (s16 each), turned by the axis (RotMatrix, ApplyMatrix); size: world
    units per tile (u32 0x8021FC14, 10 x the TMD tile size). Two rays from
    the camera's tile at yaw -/+ fov / 2 in steps of 0.7 tile mark what they
    cross, the grid's border is walked from the end of the first to the end
    of the second, every row is filled between its marks, then every tile
    as near to the camera's tile (Manhattan) as the nearest of the four
    centre tiles is dropped: nothing between the camera and the fighters."""
    vis = [0] * 256
    step = tdiv(size * 14, 20)
    if x < -7 * size:
        x = -8 * size
    if not x < 7 * size:
        x = 8 * size - 1
    if z < -7 * size:
        z = -8 * size
    if not z < 7 * size:
        z = 8 * size - 1

    def ray(a):
        sn, cs = dirs[a & 0xFFF]
        dx, dz = (step * sn) >> 15, (step * cs) >> 15
        px, pz, last = x + 8 * size, z + 8 * size, None
        while True:
            c, r = tdiv(px, size), tdiv(pz, size)
            if not (0 <= c < 16 and 0 <= r < 16):
                return last
            vis[16 * r + c], last = 1, (c, r)
            px, pz = px + dx, pz + dz
    c1, r1 = ray(-yaw - tdiv(fov, 2))
    c2, r2 = ray(-yaw + tdiv(fov, 2))
    for _ in range(64):                         # the border walk (64 steps go round)
        if r1 == 15 and c1 < 15:
            c1 += 1
            vis[0xF0 + c1] = 1
            if (c1, r1) == (c2, r2):
                break
        if c1 == 15 and r1 > 0:
            r1 -= 1
            vis[16 * r1 + 15] = 1
            if (c1, r1) == (c2, r2):
                break
        if r1 == 0 and c1 > 0:
            c1 -= 1
            vis[c1] = 1
            if (c1, r1) == (c2, r2):
                break
        if c1 == 0 and r1 < 15:
            r1 += 1
            vis[16 * r1] = 1
            if (c1, r1) == (c2, r2):
                break
    for r in range(16):
        row = [c for c in range(16) if vis[16 * r + c]]
        for c in range(row[0], row[-1]) if row else ():
            vis[16 * r + c] = 1
    cc, cr = tdiv(x + 8 * size, size), tdiv(z + 8 * size, size)
    vis[16 * cr + cc] = -2
    dist = lambda c, r: abs(c - cc) + abs(r - cr) if vis[16 * r + c] else 0x7FFF
    near = min(dist(c, r) for c, r in ((7, 7), (7, 8), (8, 7), (8, 8)))
    for r in range(16):
        for c in range(16):
            if vis[16 * r + c] and dist(c, r) <= near:
                vis[16 * r + c] = 0
    return vis


def geometry_commands(geo, cam, visible=None):
    """{main OT rank: [commands in drawing order]} of the tiles: rank 1021
    (OT B) and 1010 (OT A). visible(tile) -> bool is the game's tile culling
    (0x801DA734, a 2D frustum walk on the 16 x 16 grid); None keeps the tiles
    whose vertices are all in front of the camera."""
    import numpy as np
    import gte
    emitted = {False: [], True: []}
    for t in geo.tiles:
        if not t.prims or t.skipped or len(t.verts) > 0x100:
            continue
        if visible is not None and not visible(t.index):
            continue
        sx, sy, sz = gte.rtps(cam, np.array(t.verts))
        if visible is None and min(sz) < 16:
            continue
        for prim in t.prims:
            i = prim.idx
            a, b, c = (i[k] for k in prim.cull)
            if gte.nclip(sx[a], sy[a], sx[b], sy[b], sx[c], sy[c]) <= 0:
                continue
            cmds = [prim_command(prim, [(int(sx[j]), int(sy[j])) for j in i])]
            if prim.window:
                cmds = ([dict(kind='window', code=0xE2, value=tex_window(*prim.window) & 0xFFFFFF)] + cmds
                        + [dict(kind='window', code=0xE2, value=0)])
            emitted[t.layer_b].append((t.slot, cmds))
    out = {}
    for layer, rank in ((True, 1021), (False, 1010)):
        seq = []
        for slot in range(15, -1, -1):
            for s_, cmds in reversed(emitted[layer]):
                if s_ == slot:
                    seq += cmds
        out[rank] = seq
    return out


# ---------------------------------------------------------------------------
# Objects (stages 3 and 11): models = TMD files 1.. of archives 41 and 40,
# loaded by 0x801E2258, packets prepared by 0x801E2370 (pool 0x80370114 +
# 0x1A90 per buffer, in model and primitive order), drawn by 0x801E2A68 after
# the tiles: vertices through 0x8019C82C, a primitive kept when the cross
# product of its vertices 0, 1, 2 on screen is > 0, sorted by AVSZ4 / AVSZ3
# (ZSF4 0x100, ZSF3 0x155) of SZ >> 5 into a private OT of 128 (stage 3
# models 0-2: main rank 1011; stage 11 models 0-1: rank 1022), stage 3 model
# 3 into a single slot (rank 1009). Matrices: the tiles' camera times the
# model's rotation (0x802FECE4 + 8 i: RotMatrix, stage 11 RotMatrixYXZ, model
# 1 of stage 11 child of model 0), positions s16 at 0x8031E134 + 8 i (model 3
# of stage 3: 0x802FECFC, 0x8031E14C), animated by 0x801E3D0C (stage 11: a
# state machine reacting to the camera, 0x801E3F68 / 0x801E40C0; stage 3
# set up by 0x801E59FC).
# ---------------------------------------------------------------------------
OBJECTS = {3: (41, 4), 11: (40, 3)}
ROT_TABLE = 0x8021688C             # u32 sine | cosine << 16 x 4096 (RotMatrix, RotMatrixYXZ)
ZSF3, ZSF4 = 0x155, 0x100


def parse_objects(arcade, stage):
    import decode
    if stage not in OBJECTS:
        return []
    archive, n = OBJECTS[stage]
    files = arcade.archive_files(archive)
    return [decode.parse_geometry(f).tiles[0] for f in files[1:n + 1]]


def object_layout(stage):
    """[(model, main OT rank, private OT size)] in drawing call order."""
    if stage == 3:
        return [(0, 1011, 128), (1, 1011, 128), (2, 1011, 128), (3, 1009, 1)]
    if stage == 11:
        return [(0, 1022, 128), (1, 1022, 128)]
    return []


def object_commands(stage, models, cams):
    """{main OT rank: [commands in drawing order]}; cams[i]: gte.Camera of
    model i (its matrix and translation; H, OFX, OFY of the tiles).
    Commands carry 'packet': word index in the object pool."""
    import numpy as np
    import gte
    base, cursor = [], 0
    for m in models:                       # 0x801E2370: pool words per model
        base.append(cursor)
        cursor += sum(decode_words(p) for p in m.prims)
    ots = {}
    for i, rank, size in object_layout(stage):
        if i not in cams:
            continue
        m = models[i]
        sx, sy, sz = gte.rtps(cams[i], np.array(m.verts))
        z5 = sz >> 5
        ot = ots.setdefault(rank, [[] for _ in range(size)])
        word = base[i]
        for prim in m.prims:
            v = prim.idx
            x0, y0, x1, y1, x2, y2 = (int(q) for q in (sx[v[0]], sy[v[0]], sx[v[1]], sy[v[1]], sx[v[2]], sy[v[2]]))
            s16 = lambda q: ((q & 0xFFFF) ^ 0x8000) - 0x8000
            cross = s16(x1 - x0) * s16(y2 - y1) - s16(y1 - y0) * s16(x2 - x1)
            if cross > 0:
                cmd = prim_command(prim, [(int(sx[j]), int(sy[j])) for j in v])
                cmd['packet'] = word
                if size == 1:
                    slot = 0
                elif len(v) == 4:
                    slot = min(((sum(int(z5[j]) for j in v) * ZSF4) >> 12), 0xFFFF)
                else:
                    slot = min(((sum(int(z5[j]) for j in v) * ZSF3) >> 12), 0xFFFF)
                if slot >= size:
                    raise ValueError(f'object primitive at OT slot {slot}: outside the game\'s table')
                # 0x801E2A68 links the 128-slot OT to the main one by
                # overwriting slot 0's pointer: what landed there (OTZ 0,
                # model behind the camera) is never drawn.
                if size == 1 or slot:
                    ot[slot].append(cmd)
            word += decode_words(prim)
    out = {}
    for rank, ot in ots.items():
        seq = []
        for slot in range(len(ot) - 1, -1, -1):
            seq += list(reversed(ot[slot]))
        out[rank] = seq
    return out


def decode_words(prim):
    """Pool words 0x801E2370 gives a primitive (its GPU packet)."""
    return {'FT4': 10, 'FT3': 8, 'F4': 6, 'G4': 9, 'G3': 7, 'GT4': 13}[prim.kind]


class Stage:
    """Everything decoded for one arcade stage (from the ROM, or from a
    pack.py file with Stage.__new__ and the attributes set by pack.load)."""

    def __init__(self, arcade, stage, anim_frame=None):
        import decode
        self.stage = stage
        g, tims = arcade.stage(stage)
        self.geo = decode.parse_geometry(g, stage)
        if stage in OBJECTS:     # 0x801E21B0: file 0 of the objects' archive, TIMs
            tims = tims + decode.load_tims(arcade.archive_files(OBJECTS[stage][0])[:1])
        self.tims = tims
        self.vram, self.rects = decode.vram(tims, stage=stage, arcade=arcade, anim_frame=anim_frame)
        self.patches = []        # (x, y) of the program's palette patches (decode.CLUT_PATCHES)
        if stage in decode.CLUT_PATCHES:
            where, _, n = decode.CLUT_PATCHES[stage]
            self.patches = [arcade.word(where + 4 * i, 2, 'H') for i in range(n)]
        self.anim = decode.anim_frames(arcade) if stage in decode.ANIMATED else []
        self.farbg = parse_farbg(arcade, stage)
        self.floor = parse_floor(arcade, stage)
        self.objects = parse_objects(arcade, stage)
        # the objects' RotMatrix / RotMatrixYXZ table (E5b, tools/arcade_stages/anim.py)
        self.rot_table = arcade.word(ROT_TABLE, 4096, 'I') if stage in OBJECTS else None
        self.tables = Tables.from_arcade(arcade)

    def frame(self, cam, view, yaw, visible=None, objects=None):
        """All the stage's commands for one frame, in drawing order. cam:
        gte.Camera of the tiles; view: View of the floor; yaw: camera yaw
        minus the fight axis of the previous frame (far background);
        objects: {model: gte.Camera} of the objects drawn this frame.
        Link order of the game: floor, far background, objects, tiles; in a
        main OT rank, what is linked last is drawn first."""
        if objects:
            objects = object_commands(self.stage, self.objects, objects)
        ranks = {}

        def link(rank, cmds):
            ranks.setdefault(rank, []).append(cmds)
        for rank, cmd in floor_commands(self.floor, view, self.tables):
            link(rank, [cmd])
        link(1023, farbg_commands(self.farbg, yaw, view.pitch, view.hires))
        for rank, cmds in (objects or {}).items():
            link(rank, cmds)
        for rank, cmds in geometry_commands(self.geo, cam, visible).items():
            link(rank, cmds)
        out = []
        for rank in sorted(ranks, reverse=True):
            for cmds in reversed(ranks[rank]):
                out += cmds
        return out
