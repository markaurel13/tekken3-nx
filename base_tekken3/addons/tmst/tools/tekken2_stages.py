#!/usr/bin/env python3
"""Tekken 2 Content's Stages, Classic: the 23 Tekken 2 stages as packs the
Expanded's stage engine draws (src/tekken3_arcade_stages.c, its Tekken 3 path),
from the player's Tekken 2 PlayStation disc (tools/tekken2_disc.py,
remembered by the add-on's setup in workspace/tekken2-disc.json) into
addons/tmst/workspace/t2-stages/classic/stageNN.ast (NN: Tekken 2's stage
number, 00..22), with pictures to check them by in t2-stages/preview/ (run
by the add-on's setup, setup/tmst_setup.py).

    python tools/tekken2_stages.py --disc <Tekken 2 disc> [--root <expanded folder>]

Classic: Tekken 2's own background and floor, as the PlayStation draws them,
nothing added (the Remake comes later, in t2-stages/remake).

The disc (Europe SCES-00255 and USA SLUS-00213 alike):
- TEKKEN2/TEKKEN2.BNS, the archive; its entries in the program's table at
  file offset 0x964 (12 bytes each: sector in the file, size, 0), 400 of them;
- a fighter's group of 16 entries from entry 24 + 16 * fighter (Tekken 2's own
  ids, the order of the program's fighter records): the first its stage's
  textures (TIMs: the panorama's six 256 x 256 4-bit blocks at VRAM x 512..,
  32 palettes each from y 480, the floor's 64 x 64 4-bit tiles at x 960..), the
  second its stage's record: a count, ten offsets, then at the first u32 0x40
  and the floor's 10 x 10 tiles, 8 bytes each (u | v << 8; texture page with
  a turn in bits 14-15: 1 a quarter clockwise, 2 a half, 3 three quarters; 0;
  CLUT); at the second the panorama's cells, 64 x 32 texels each (u | v << 8;
  CLUT; page), row by row; at the third its map, rows of the cells' column
  shown at each of 24 places round the stage (the last row is the fight's).
  Eternal Darkness and Mirror Darkness have a floor and no panorama.

The pack (format 3, as the Expanded's tools/arcade_stages/pack.py save writes
Tekken 3 arcade's: the engine's Tekken 3 path, its floor and its far
background; its number 20 + the stage, which the runtime makes the arena it
gives the fight as it hands the pack over):
- VRAM: the cells and floor textures in the upper half of VRAM (y 512..1023),
  each in a 64 x 64 texel slot of its own, 4-bit, their palettes below them
  (a CLUT word's bit 15: y + 512), all where the Expanded's stage packs put
  theirs (SLOTS: never on Unknown's wolf); a panorama cell's 32 rows each
  twice (the engine draws 64 x 64 texels into a cell of 64 x 32 pixels);
- TILE: no scenery tiles;
- FARB: the panorama, its 24 places (or a stage's own order, "places") going
  round the fight "repeat" times, a cell a 64-pixel column of the arcade's
  512 x 240 screen, Tekken 2's one texel a pixel, its top "yoff" at a level
  camera; the bands above and below it the colours of its top and bottom
  lines; no panorama: black;
- FLOR: the floor's 10 x 10 tiles, turned as Tekken 2 turns them, on the
  engine's 10 x 10 cells of 2 500 round the fighters, lit as Tekken 3's stage
  0 (darker far off) or, "lit", round each fighter only (the remake's Mirror
  Darkness); the cells nearest the camera in halves, each a quarter of its
  tile twice as large (cubic, 16 colours of its own; numpy);
- TRIG: the sine, cosine, square root and fog tables, computed;
- the add-on's own (src/tekken2/tmst_tekken2_stages.c; the engine reads only
  its own sections): T2FX the fighters' light ("dim", 0x80 as the game
  sets it), T2LT the arena whose light the fighters take (DAY_ARENA, but
  on DARK_LIGHT), T2CH Hong Kong's running lights ("chase"), T2BL its floor's
  fade ("blend"), T2MR Mirror Darkness's mirror ("mirror").

Nothing from the disc goes into git; the packs are staged beside the game as
mods/t2-stages/classic (addon.cmake TMST_STAGED) and read there through
src/tekken2/tmst_tekken2_stages.c. Pure Python, a few seconds.
"""
from __future__ import annotations

import argparse
import math
import struct
import sys
import zlib
from array import array
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
EXPANDED = ADDON.parents[1]                 # <expanded>/addons/tmst
sys.path.insert(0, str(ADDON / 'tools'))
from disc_image import DiscError, open_disc  # noqa: E402
from tekken2_disc import REGIONS  # noqa: E402

OUT = 'addons/tmst/workspace/t2-stages'
CLASSIC, PREVIEW = 'classic', 'preview'
# Increase whenever the packs this writes change: the setup makes them again.
VERSION = 6

BNS_FILE, BNS_TABLE, BNS_ENTRIES = 'TEKKEN2/TEKKEN2.BNS', 0x964, 400
GROUP0, GROUP = 24, 16                      # a fighter's group of BNS entries
NUMBER0 = 20                                # a pack's number: 20 + Tekken 2's stage (the runtime gives the arena's)
PLACES = 24                                 # round the stage, a panorama cell each
CELL_W, CELL_H = 64, 32                     # a panorama cell's texels
FLOOR_N = 10                                # the floor's tiles a side
NAMES = ('Grand Canyon', 'Szechwan Fields', 'Morning Fields', 'New York City', 'Church Sanctuary',
         'Hong Kong Rooftop', 'Pagoda Temple', 'Serene Forest', 'Akihabara', 'The Great Wall',
         'Cambodian Wetlands', 'Athens', 'Nowhere Desert', 'Launch Pad', 'Hills', 'Kyoto at Sunset',
         'Taj Mahal', 'Beach', 'London Ruins', 'Arizona Desert', 'Korean Temple', 'Eternal Darkness',
         'Mirror Darkness')
# Each stage's fighter (Tekken 2's id, its group): the fighter records' stage
# byte (Europe RAM 0x800A39E4, byte 6), a fighter a stage.
FIGHTER_OF = {21: 0, 3: 1, 9: 2, 8: 3, 11: 4, 4: 5, 7: 6, 19: 7, 1: 8, 6: 9, 13: 10, 18: 11, 12: 12,
              16: 13, 15: 14, 14: 15, 10: 16, 22: 17, 5: 18, 2: 19, 0: 20, 20: 21, 17: 22}
# A stage's own settings beside DEFAULT: "places" the panorama's places in
# order (Launch Pad: the shuttle once round, between the hangar's walls),
# "repeat" its turns round the fight, "bottom" its last row's end on the
# arcade's 240 lines at a level camera (or "yoff" its top row's), "pitch" how
# far it moves with the camera's;
# "lit": the floor lit round the fighters only, "dim" the fighters' light
# (0x80 the game's; the user's, 2026-11-03: the dark stages); "chase" Tekken
# 2's running lights round Hong Kong's yin and yang, as its program turns them
# (issue #12: its table at 0x800A3010, slots 0 and 1): the palettes' "dots"
# colours turned round one place (colour i takes i + 1's), a turn whenever a
# clock adding "add" a frame passes a multiple of "period" (0x800C4928:
# 1706, bit 14); "blend" its floor's fade (slot 2): palette "clut" towards
# palette "to" and back, colours "first".."last", the clock adding "add" a
# frame (4096 a turn; src/tekken2/tmst_tekken2_stages.c blend_tick());
# "mirror" Mirror Darkness's (devil-full.zip's MIRR: radius, centre angle,
# width, top, bottom, panels round the camera).
# bottom 160: the panorama's last row ends at y 160 at a level camera, its
# ground on the floor's far edge (headless, 2026-11-03: yoff -96 for the 8
# rows of most; the taller Church, Forest and Launch Pad the same way).
DEFAULT = dict(places=list(range(PLACES)), repeat=2, bottom=160, pitch=144)
# Rows of the bottom colour under the panorama, to below the screen: a frame
# the floor misses (the game's drawing rate changing, pausing) shows the
# ground's colour there, not black (the user's, 2026-11-03).
GROUND_ROWS = 4
DARK = dict(lit=True, dim=0x50)
# The fighters' light, as the Remake's (the user's, 2026-11-05: "the same on
# the Classic as on the Remake; dark light only on the dark stages named"):
# our fights are Tekken 3's arena 9's, whose light is dim and orange; every
# stage but DARK_LIGHT takes arena DAY_ARENA's (T2LT: Tekken 3's daylight,
# back 0x898, a white light; src/tekken2/tmst_tekken2_stages.c light_tick()),
# without a dim. DARK_LIGHT keeps arena 9's, dimmed as the Remake dims it
# (its recipe's "fighter_light": Church and Taj 0.62, Pagoda and Forest 0.8;
# Mirror and Eternal Darkness 0x50). Launch Pad's floor stays lit, its
# fighters no longer dimmed (the Remake's are not).
DAY_ARENA = 8
DARK_LIGHT = {4, 6, 7, 16, 21, 22}
LAYOUTS = {
    4: dict(DARK, dim=0x4F), 6: dict(DARK, dim=0x66), 7: dict(DARK, dim=0x66), 16: dict(dim=0x4F), 21: DARK,
    5: dict(chase=dict(cluts=(0x7B02, 0x7B03), dots=(10, 11, 12, 13), period=16384, add=1706),
            blend=dict(clut=0x7B04, to=0x7B09, first=0, last=14, add=17)),
    22: dict(DARK, mirror=(6400, 5, 2700, -1260, 400, 13)),
    13: dict(lit=True, places=[*range(12, 16), *range(16, 24), *range(8), *range(16, 24), *range(8), *range(8, 12)],
             repeat=1),
}


def layout(stage: int) -> dict:
    return {**DEFAULT, **LAYOUTS.get(stage, {})}


# --- the disc -------------------------------------------------------------

class Source:
    """The disc's program and TEKKEN2.BNS."""

    def __init__(self, disc):
        self.disc = disc
        exe = next((e for e in (disc.read_file(name) for name in REGIONS) if e), None)
        if exe is None:
            raise DiscError('not a Tekken 2 disc (no SCES_002.55 or SLUS_002.13)')
        found = disc.locate(BNS_FILE)
        if not found:
            raise DiscError(f'not a Tekken 2 disc (no {BNS_FILE})')
        self.lba, size = found
        self.entries = [struct.unpack_from('<2I', exe, BNS_TABLE + 12 * i) for i in range(BNS_ENTRIES)]
        if any(sec * 2048 + n > size for sec, n in self.entries):
            raise DiscError('a Tekken 2 version whose archive table is not the one known '
                            '(Europe SCES-00255, USA SLUS-00213)')

    def entry(self, i: int) -> bytes:
        sec, size = self.entries[i]
        return b''.join(self.disc.read(self.lba + sec + n) for n in range(-(-size // 2048)))[:size]


def vram_of(textures: bytes) -> array:
    """The PlayStation's VRAM (1024 x 512 halfwords, row by row) with the TIMs."""
    vram = array('H', bytes(2 * 1024 * 512))
    p = 0
    while p + 8 <= len(textures) and struct.unpack_from('<I', textures, p)[0] == 0x10:
        flags = struct.unpack_from('<I', textures, p + 4)[0]
        p += 8
        for _ in range(2 if flags & 8 else 1):
            n, x, y, w, h = struct.unpack_from('<I4H', textures, p)
            if n < 12 or p + n > len(textures):
                raise DiscError('a stage texture the disc does not hold whole')
            words = array('H', textures[p + 12:p + 12 + 2 * w * h])
            for row in range(h):
                if y + row < 512 and x + w <= 1024:
                    vram[(y + row) * 1024 + x:(y + row) * 1024 + x + w] = words[row * w:(row + 1) * w]
            p += n
    return vram


def record(data: bytes) -> dict:
    offsets = struct.unpack_from('<10I', data, 4)
    if struct.unpack_from('<I', data, offsets[0])[0] != 0x40 or not offsets[0] <= offsets[1] <= offsets[2] <= offsets[3]:
        raise DiscError('a stage record that is not one')
    floor = [struct.unpack_from('<4H', data, offsets[0] + 4 + 8 * i) for i in range(FLOOR_N * FLOOR_N)]
    cells = [struct.unpack_from('<3H', data, offsets[1] + 6 * i) for i in range((offsets[2] - offsets[1]) // 6)]
    m = data[offsets[2]:offsets[3]]
    rows = [list(m[r:r + PLACES]) for r in range(0, len(m) - PLACES + 1, PLACES)]
    return dict(floor=floor, cells=cells, map=rows)


def texels(vram: array, page: int, u0: int, v0: int, w: int, h: int) -> bytes:
    """A w x h rectangle of a 4-bit texture page: its indices, row by row."""
    tx, ty = (page & 15) * 64, (page >> 4 & 1) * 256
    out = bytearray(w * h)
    for v in range(h):
        row = ((ty + v0 + v) % 512) * 1024
        for u in range(w):
            uu = u0 + u
            out[v * w + u] = vram[row + (tx + uu // 4) % 1024] >> ((uu & 3) * 4) & 15
    return bytes(out)


def palette(vram: array, clut: int) -> tuple:
    x, y = (clut & 63) * 16, clut >> 6 & 511
    return tuple(vram[y * 1024 + x:y * 1024 + x + 16])


def stage_data(src: Source, stage: int) -> dict:
    """A stage's panorama (rows of places: (indices 64 x 32, palette)) and floor
    (10 x 10: (indices 64 x 64, palette, turn, Tekken 2's CLUT word)), as the
    disc holds them."""
    group = GROUP0 + GROUP * FIGHTER_OF[stage]
    vram, rec = vram_of(src.entry(group)), record(src.entry(group + 1))
    lay = layout(stage)
    pano = []
    if rec['cells'] and rec['map']:
        fight = rec['map'][-1]
        columns = max(max(r) for r in rec['map']) + 1
        if len(rec['cells']) % columns:
            raise DiscError(f'stage {stage}: a panorama of {len(rec["cells"])} cells in {columns} columns')
        for r in range(len(rec['cells']) // columns):
            row = []
            for p in lay['places']:
                uv, clut, page = rec['cells'][r * columns + fight[p]]
                row.append((texels(vram, page, uv & 255, uv >> 8, CELL_W, CELL_H), palette(vram, clut)))
            pano.append(row)
    floor = []
    for uv, page, _, clut in rec['floor']:
        floor.append((texels(vram, page & 0xFF, uv & 255, uv >> 8, 64, 64), palette(vram, clut), page >> 14 & 3, clut))
    st = dict(stage=stage, name=NAMES[stage], pano=pano, floor=floor, layout=lay)
    if lay.get('blend'):
        st['blend_to'] = palette(vram, lay['blend']['to'])
    return st


# --- the pack ---------------------------------------------------------------

FORMAT = 3
# 64 x 64 texel slots of the upper half where the Expanded's own stage packs
# put theirs, page by page: x 0..543 at y 512..959, x 544..639 at y 512..767.
# Never Unknown's wolf (tekken3_ttt1_wolf.c, while she fights) nor TTT
# Hitspark's fixed cells (tekken3_ttt1_combat.c hfx_taken); the rest is left
# for what is placed round the stage (the common sparks, the PS2 lasers, the
# add-on's letters). Palettes in a column at x 0..63 from y 960, four a row.
WOLF = (640, 512, 384, 256)
HIT_EFFECT_CELLS = ((544, 768, 32, 64), (592, 768, 432, 64), (544, 832, 480, 64), (544, 896, 224, 64))


def stage_area(x: int, y: int) -> bool:
    return x < 544 and y < 960 or x < 640 and y < 768


SLOTS = [(px * 64 + 16 * (k % 4), band + 64 * (k // 4)) for band in (512, 768) for px in range(16)
         for k in range(16) if stage_area(px * 64 + 16 * (k % 4), band + 64 * (k // 4))]
CLUT_X, CLUT_Y, CLUTS_A_ROW = 0, 960, 4
# The uv of a floor cell's corners (x-, z+), (x+, z+), (x-, z-), (x+, z-) for
# each turn (the engine's luv[1], its cell word's bits 30-31): Tekken 2's
# turns, a quarter clockwise more each.
TURNS = ((0, 0x3F, 0x3F00, 0x3F3F), (0x3F00, 0, 0x3F3F, 0x3F), (0x3F3F, 0x3F00, 0x3F, 0), (0x3F, 0x3F3F, 0, 0x3F00))
TILE_SIZE = 800                             # world / 10: the empty TILE's grid, for the engine's cone walk
# FLOR kind, fog, near: Tekken 3's stage 0 (lit evenly, darker far off), and
# the remake's Mirror Darkness (lit round each fighter, dark beyond).
FLOOR_PLAIN, FLOOR_LIT = (0, -51, -700), (1, -51, -800)
NO_DIM = 0x80                               # T2FX: the fighters' light as the game sets it


def pad4(b: bytes) -> bytes:
    return b + bytes(-len(b) % 4)


def page_word(x: int, y: int) -> int:
    """The texture page (4-bit) of upper-half VRAM x, y: bits 0-3 x / 64, bit 4
    y + 256, bit 11 y + 512."""
    return x // 64 | (y >> 8 & 1) << 4 | (y >> 9 & 1) << 11


def clut_word(x: int, y: int) -> int:
    return (y << 6 | x >> 4) & 0xFFFF


def mean(colours) -> tuple:
    rgb = [(c & 31, c >> 5 & 31, c >> 10 & 31) for c in colours]
    return tuple(min(255, round(sum(c[k] for c in rgb) / max(1, len(rgb))) * 8) for k in range(3))


def trig() -> bytes:
    """TRIG, computed: the 4.12 sine and cosine, PSY-Q SquareRoot0's table
    (sqrt(64 + i) << 9, twice: the engine's sqrt1 and sqrt2) and the floor's
    fog ramp (0 .. 127 over 1 024 distances, as Tekken 3 arcade's)."""
    sine = [round(4096 * math.sin(2 * math.pi * a / 4096)) for a in range(4096)]
    cosine = [round(4096 * math.cos(2 * math.pi * a / 4096)) for a in range(4096)]
    root = [int(math.sqrt(64 + i) * 512) for i in range(192)]
    fog = bytes(round(127 * (i / 1024) ** 1.5) for i in range(1024))
    return struct.pack('<4096h4096h192h', *sine, *cosine, *root) + fog + struct.pack('<192h', *root)


class Vram:
    """The pack's upper half: slots of 64 x 64 texels and palettes, each kept
    once (a palette by its colours, or by a key of its own: one the runtime
    changes)."""

    def __init__(self):
        self.slots, self.cluts, self.rects = {}, {}, []

    def slot(self, idx: bytes) -> tuple:
        if idx not in self.slots:
            if len(self.slots) == len(SLOTS):
                raise ValueError('no room for another 64 x 64 texture in the upper half')
            x, y = SLOTS[len(self.slots)]
            words = array('H', (idx[i] | idx[i + 1] << 4 | idx[i + 2] << 8 | idx[i + 3] << 12
                                for i in range(0, len(idx), 4)))
            self.rects.append((x, y, 16, 64, words))
            self.slots[idx] = (x, y)
        return self.slots[idx]

    def clut(self, colours: tuple, key=None) -> int:
        key = key or colours
        if key not in self.cluts:
            n = len(self.cluts)
            x, y = CLUT_X + 16 * (n % CLUTS_A_ROW), CLUT_Y + n // CLUTS_A_ROW
            if y >= 1024:
                raise ValueError('no room for another palette in the upper half')
            self.rects.append((x, y, 16, 1, array('H', colours)))
            self.cluts[key] = clut_word(x, y)
        return self.cluts[key]

    def section(self) -> bytes:
        return struct.pack('<I', len(self.rects)) + b''.join(
            struct.pack('<4H', x, y, w, h) + words.tobytes() for x, y, w, h, words in self.rects)


def texture_word(x: int, y: int) -> int:
    """A floor cell's word without its turn: uv of slot x, y, its page."""
    return (y % 256) << 8 | (x % 64) * 4 | page_word(x, y) << 16


def doubled(idx: bytes) -> bytes:
    """A 64 x 32 cell with each row twice: 64 x 64."""
    return b''.join(idx[r * 64:(r + 1) * 64] * 2 for r in range(CELL_H))


# --- the floor's near halves: Tekken 2's tile twice as large -----------------

def cubic_weights(n: int):
    """(2n x n) the cubic convolution (Keys, a = -0.5) that doubles n samples,
    the edges held."""
    import numpy as np
    w = np.zeros((2 * n, n))
    for i in range(2 * n):
        s = (i + 0.5) / 2 - 0.5
        x0 = math.floor(s)
        t = s - x0
        for k, d in enumerate((-1, 0, 1, 2)):
            x = abs(d - t)
            weight = 1.5 * x ** 3 - 2.5 * x ** 2 + 1 if x <= 1 else -0.5 * x ** 3 + 2.5 * x ** 2 - 4 * x + 2
            w[i, min(max(x0 + d, 0), n - 1)] += weight
    return w


def colour_of(r: float, g: float, b: float) -> int:
    c = min(31, round(r)) | min(31, round(g)) << 5 | min(31, round(b)) << 10
    return c or 0x8000                      # black, not the transparent 0


def magnified(idx: bytes, colours: tuple) -> list:
    """Tekken 2's 64 x 64 tile twice as large (cubic), as its four 64 x 64
    quarters (v, u: 00, 01, 10, 11), each of 16 colours of its own (k-means
    from the tile's own colours): [(indices, palette)]."""
    import numpy as np
    pal = np.array([(c & 31, c >> 5 & 31, c >> 10 & 31) for c in colours], float)
    img = pal[np.frombuffer(idx, np.uint8).reshape(64, 64)]
    w = cubic_weights(64)
    big = np.clip(np.einsum('ij,jkc->ikc', w, np.einsum('kj,ijc->ikc', w, img)), 0, 31)
    used = pal[sorted(set(idx))]
    out = []
    for qv in (0, 1):
        for qu in (0, 1):
            px = big[64 * qv:64 * qv + 64, 64 * qu:64 * qu + 64].reshape(-1, 3)
            centres = used.copy()
            for _ in range(8):
                near = ((px[:, None, :] - centres[None]) ** 2).sum(2).argmin(1)
                for k in range(len(centres)):
                    if (near == k).any():
                        centres[k] = px[near == k].mean(0)
            near = ((px[:, None, :] - centres[None]) ** 2).sum(2).argmin(1)
            palette = tuple(colour_of(*c) for c in centres) + (0,) * (16 - len(centres))
            out.append((bytes(near.astype(np.uint8)), palette))
    return out


def blown_up(idx: bytes, colours: tuple) -> list:
    """The same quarters, each texel twice as large: the indices kept (a
    palette the runtime changes)."""
    out = []
    for qv in (0, 1):
        for qu in (0, 1):
            out.append((bytes(idx[(32 * qv + v // 2) * 64 + 32 * qu + u // 2] for v in range(64) for u in range(64)),
                        colours))
    return out


# --- the sections ------------------------------------------------------------

def farb(st: dict, vram: Vram) -> bytes:
    """FARB: the panorama, a 64 x 64 texel cell (Tekken 2's 64 x 32, its rows
    twice) a 64 x 32 cell of the arcade's 512 x 240 screen; the bands above
    and below it its top and bottom lines' colours, GROUND_ROWS cells of the
    bottom colour under it. No panorama: eight rows of black cells, fixed,
    over the whole screen."""
    lay, pano = st['layout'], st['pano']
    if not pano:
        head = struct.pack('<6h', 1, 8, 1, 0, 45, 0)
        colours = bytes(3) + bytes(3) + b'\1' + bytes(3) + b'\1\0'
        return pad4(head + colours + bytes(2 * 8) + bytes(2 * 8) + bytes([0x74]) * 8)
    cols, rows = len(pano[0]), len(pano)
    ring = cols * lay['repeat']
    degrees = 2880 // ring                  # 512 pixels' turn: the ring of 64-pixel columns is 360 degrees
    if degrees * ring != 2880:
        raise ValueError(f'stage {st["stage"]}: {ring} columns round the fight do not make whole degrees')
    desc, cluts = [], []
    for row in pano:
        for idx, colours in row:
            x, y = vram.slot(doubled(idx))
            desc.append((x % 64) // 16 | (y % 256) // 64 << 2 | (x // 64) << 8 | (y >> 8 & 1) << 12 | (y >> 9 & 1) << 13)
            cluts.append(vram.clut(colours))
    top = mean(colours[i] for idx, colours in pano[0] for i in idx[:64])
    bottom = mean(colours[i] for idx, colours in pano[-1] for i in idx[-64:])
    yoff = lay.get('yoff', lay['bottom'] - 32 * rows)
    desc += [0] * (cols * GROUND_ROWS)
    cluts += [0] * (cols * GROUND_ROWS)
    head = struct.pack('<6h', cols, rows + GROUND_ROWS, ring, yoff, degrees, lay['pitch'])
    colours = bytes(bottom) + bytes(top) + b'\1' + bytes(bottom) + b'\1\0'
    out = head + colours + struct.pack(f'<{len(desc)}H', *desc) + struct.pack(f'<{len(cluts)}H', *cluts)
    return pad4(out + bytes([0x31]) * (ring * rows) + bytes([0x64]) * (ring * GROUND_ROWS))


def flor(st: dict, vram: Vram) -> bytes:
    """FLOR: Tekken 2's 10 x 10 floor tiles on the engine's 10 x 10 cells
    (2 500 world units), turned as Tekken 2 turns them; the near-camera
    halves (20 x 20) each a quarter of its tile twice as large; one quad a
    cell."""
    lay = st['layout']
    chase = lay.get('chase') or {}
    blend = lay.get('blend') or {}
    kind, fog, near = FLOOR_LIT if lay.get('lit') else FLOOR_PLAIN
    cells, halves, made = [], [None] * 400, {}
    for r in range(FLOOR_N):
        for c in range(FLOOR_N):
            idx, colours, turn, clut = st['floor'][r * FLOOR_N + c]
            key = ('chase', clut) if clut in chase.get('cluts', ()) else ('blend', clut) if clut == blend.get('clut') else None
            x, y = vram.slot(idx)
            cells.append((texture_word(x, y) | turn << 30, vram.clut(colours, key) << 16))
            if (idx, colours, key) not in made:
                made[idx, colours, key] = (blown_up if key else magnified)(idx, colours)
            quarters = made[idx, colours, key]
            for p in range(4):
                hr, hc = divmod(p, 2)
                t = TURNS[turn][p]          # the cell's corner p: the quarter round its uv
                qidx, qcolours = quarters[2 * (t >> 8 > 0) + (t & 0xFF > 0)]
                qx, qy = vram.slot(qidx)
                halves[(2 * r + hr) * 20 + 2 * c + hc] = (texture_word(qx, qy), vram.clut(qcolours, key) << 16)
    verts, quads = {}, []
    for r in range(FLOOR_N):
        for c in range(FLOOR_N):
            x0, z0 = -12500 + 2500 * c, -10000 + 2500 * r
            corners = ((x0, z0), (x0 + 2500, z0), (x0, z0 - 2500), (x0 + 2500, z0 - 2500))
            quads.append(sum(verts.setdefault(v, len(verts)) << 8 * k for k, v in enumerate(corners)))
    out = struct.pack('<2hi', kind, fog, near)
    out += b''.join(struct.pack('<2I', *w) for w in cells) + b''.join(struct.pack('<2I', *w) for w in halves)
    out += pad4(bytes(100))                 # every cell at OT rank 1021
    out += pad4(struct.pack('<H', len(verts)) + b''.join(struct.pack('<2h', *v) for v in verts))
    out += pad4(bytes([1]) * 100) + struct.pack('<100I', *quads)
    out += b''.join(struct.pack('<4H', *TURNS[t]) for t in range(4))
    return out + bytes(2 * 4 * 4 * (2 + 4))  # the 2 and 4 sub-quad corners: no cell has them


def turned(colours, dots, f: int) -> list:
    """A palette with its "dots" colours turned round f places, as Tekken 2
    turns them (issue #12): colour dots[j] takes dots[j + f]'s."""
    out = list(colours)
    for j, d in enumerate(dots):
        out[d] = colours[dots[(j + f) % len(dots)]]
    return out


def chase_section(st: dict, vram: Vram) -> bytes | None:
    """T2CH (the add-on's, src/tekken2/tmst_tekken2_stages.c): Tekken 2's
    running lights, its palettes "cluts" with their "dots" colours turned
    round, a turn whenever the clock (adding "add" a frame) passes "period":
    u16 period, frames, palettes, add; (u16 x, y) a palette; then each
    frame's palettes (the first the disc's own)."""
    chase = st['layout'].get('chase')
    if not chase:
        return None
    places = []
    for (idx, colours, turn, clut) in st['floor']:
        if clut in chase['cluts'] and ('chase', clut) in vram.cluts and clut not in [p[0] for p in places]:
            w = vram.cluts['chase', clut]
            places.append((clut, (w & 63) * 16, w >> 6 & 0x3FF, colours))
    if not places:
        return None
    dots = chase['dots']
    out = struct.pack('<4H', chase['period'], len(dots), len(places), chase.get('add', 1))
    out += b''.join(struct.pack('<2H', x, y) for _, x, y, _ in places)
    for f in range(len(dots)):
        for *_, colours in places:
            out += struct.pack('<16H', *turned(colours, dots, f))
    return out


def blend_section(st: dict, vram: Vram) -> bytes | None:
    """T2BL (the add-on's, src/tekken2/tmst_tekken2_stages.c): Tekken 2's
    floor fade (issue #12), palette "clut" towards "to" and back: u16
    palettes, first, last, add; each palette (u16 x, y), A[16], B[16]."""
    blend = st['layout'].get('blend')
    if not blend or ('blend', blend['clut']) not in vram.cluts:
        return None
    w = vram.cluts['blend', blend['clut']]
    colours = next(c for (_, c, _, clut) in st['floor'] if clut == blend['clut'])
    out = struct.pack('<4H', 1, blend['first'], blend['last'], blend['add'])
    return out + struct.pack('<2H', (w & 63) * 16, w >> 6 & 0x3FF) + struct.pack('<16H', *colours)         + struct.pack('<16H', *st['blend_to'])


def build(st: dict) -> bytes:
    """The stage's format 3 pack."""
    vram = Vram()
    far, floor = farb(st, vram), flor(st, vram)
    sections = [(b'VRAM', vram.section()), (b'TILE', struct.pack('<2I', TILE_SIZE, 0)), (b'FARB', far),
                (b'FLOR', floor), (b'TRIG', trig()),
                (b'T2FX', struct.pack('<2H', st['layout'].get('dim', NO_DIM), 0))]
    if st['stage'] not in DARK_LIGHT:
        sections.append((b'T2LT', struct.pack('<2H', DAY_ARENA, 0)))
    chase = chase_section(st, vram)
    if chase:
        sections.append((b'T2CH', chase))
    fade = blend_section(st, vram)
    if fade:
        sections.append((b'T2BL', fade))
    if st['layout'].get('mirror'):
        sections.append((b'T2MR', struct.pack('<6i', *st['layout']['mirror'])))
    head = 16 + 12 * len(sections)
    table, body, off = b'', b'', head
    for sid, data in sections:
        data = pad4(data)
        table += struct.pack('<4s2I', sid, off, len(data))
        body += data
        off += len(data)
    return struct.pack('<4s2H2I', b'ASTG', FORMAT, NUMBER0 + st['stage'], len(sections), off) + table + body


# --- reading a pack back (the engine's checks; the pictures) ---------------

def sections(pack: bytes) -> dict:
    magic, version, number, n, size = struct.unpack_from('<4s2H2I', pack, 0)
    if magic != b'ASTG' or version != FORMAT or size != len(pack):
        raise ValueError('not a format 3 pack')
    out = {'number': number}
    for i in range(n):
        sid, o, ln = struct.unpack_from('<4s2I', pack, 16 + 12 * i)
        out[sid.decode('ascii')] = pack[o:o + ln]
    for need in ('VRAM', 'TILE', 'FARB', 'FLOR', 'TRIG'):
        if need not in out:
            raise ValueError(f'a pack without {need}')
    return out


def upper_half(vram_section: bytes) -> array:
    v = array('H', bytes(2 * 1024 * 512))
    n, o = struct.unpack_from('<I', vram_section)[0], 4
    for _ in range(n):
        x, y, w, h = struct.unpack_from('<4H', vram_section, o)
        o += 8
        words = array('H', vram_section[o:o + 2 * w * h])
        o += 2 * w * h
        if not (512 <= y and y + h <= 1024 and x + w <= 1024):
            raise ValueError(f'a rectangle outside the upper half: {x},{y} {w}x{h}')
        for r in range(h):
            v[(y - 512 + r) * 1024 + x:(y - 512 + r) * 1024 + x + w] = words[r * w:(r + 1) * w]
    return v


def sample(v: array, x0: int, y0: int, u: int, vv: int, clut: int) -> int:
    """A 4-bit texel of the upper half (x0, y0: the page's corner) through a palette."""
    i = v[(y0 - 512 + vv) * 1024 + x0 + u // 4] >> ((u & 3) * 4) & 15
    cy, cx = clut >> 6 & 0x3FF, (clut & 63) * 16
    return v[(cy - 512) * 1024 + cx + i]


def rgb(c: int) -> bytes:
    return bytes(((c & 31) << 3, (c >> 5 & 31) << 3, (c >> 10 & 31) << 3))


def png(path: Path, w: int, h: int, pixels: bytes) -> None:
    raw = b''.join(b'\0' + pixels[r * w * 3:(r + 1) * w * 3] for r in range(h))
    chunk = lambda t, d: struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d))
    path.write_bytes(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>2I5B', w, h, 8, 2, 0, 0, 0))
                     + chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b''))


def pictures(pack: bytes) -> dict:
    """{'panorama': (w, h, rgb), 'floor': (w, h, rgb)} as the engine reads the
    pack: the panorama's cells at 64 x 32 (every other texel row), the floor
    from above (+z up) as the near halves draw it, 20 x 20 of 64 x 64.
    Transparent texels magenta."""
    s = sections(pack)
    v = upper_half(s['VRAM'])
    out = {}
    far = s['FARB']
    cols, rows, ring = struct.unpack_from('<3h', far, 0)
    kinds = far[24 + 4 * cols * rows:]
    textured = next((r for r in range(rows) if kinds[r * ring] != 0x31), rows)
    if textured:
        desc = struct.unpack_from(f'<{cols * rows}H', far, 24)
        cluts = struct.unpack_from(f'<{cols * rows}H', far, 24 + 2 * cols * rows)
        rows = textured
        w, h = 64 * cols, 32 * rows
        img = bytearray(3 * w * h)
        for k in range(cols * rows):
            d = desc[k]
            x0, y0 = (d >> 8 & 15) * 64, 512 + (d >> 12 & 1) * 256
            u0, v0 = (d & 3) * 64, (d >> 2 & 3) * 64
            for yy in range(32):
                for xx in range(64):
                    c = sample(v, x0, y0, u0 + xx, v0 + 2 * yy, cluts[k])
                    o = 3 * ((k // cols * 32 + yy) * w + k % cols * 64 + xx)
                    img[o:o + 3] = rgb(c) if c else b'\xff\0\xff'
        out['panorama'] = (w, h, bytes(img))
    f = s['FLOR']
    cells = struct.unpack_from('<200I', f, 8)
    halves = struct.unpack_from('<800I', f, 808)
    w = h = 64 * 20
    img = bytearray(3 * w * h)
    for r in range(20):
        for c in range(20):
            turn = cells[2 * (10 * (r // 2) + c // 2)] >> 30
            w0, w1 = halves[2 * (20 * r + c)], halves[2 * (20 * r + c) + 1]
            base, tp, clut = w0 & 0xFFFF, w0 >> 16 & 0x39FF, w1 >> 16
            x0, y0 = (tp & 15) * 64, 512 + (tp >> 4 & 1) * 256
            # corners (x-, z+), (x+, z+), (x-, z-), (x+, z-): the picture's top left, top right, bottom left, bottom right
            uv = [((base + k) & 0xFF, (base + k) >> 8 & 0xFF) for k in TURNS[turn]]
            for yy in range(64):
                for xx in range(64):
                    a, b = xx / 63, yy / 63
                    uu = round((1 - a) * (1 - b) * uv[0][0] + a * (1 - b) * uv[1][0] + (1 - a) * b * uv[2][0] + a * b * uv[3][0])
                    vv = round((1 - a) * (1 - b) * uv[0][1] + a * (1 - b) * uv[1][1] + (1 - a) * b * uv[2][1] + a * b * uv[3][1])
                    col = sample(v, x0, y0, uu, vv, clut)
                    o = 3 * ((r * 64 + yy) * w + c * 64 + xx)
                    img[o:o + 3] = rgb(col) if col else b'\xff\0\xff'
    out['floor'] = (w, h, bytes(img))
    return out


# --- the setup's side --------------------------------------------------------

def find_disc(state: Path) -> Path | None:
    """The Tekken 2 disc the add-on's setup remembered (workspace/tekken2-disc.json), if it is still there."""
    import json
    try:
        disc = json.loads((state / 'tekken2-disc.json').read_text(encoding='utf-8')).get('disc') or {}
    except (OSError, ValueError):
        return None
    path = disc.get('path')
    return Path(path) if path and Path(path).is_file() else None


def pack_name(stage: int) -> str:
    return f'stage{stage:02d}.ast'


def slug(stage: int) -> str:
    return f'{stage:02d}-' + NAMES[stage].lower().replace(' ', '-')


def convert(disc: Path, out: Path, say=print, previews: bool = True) -> list[str]:
    """The 23 Classic packs (and their pictures) into out/classic (out/preview).
    Returns the packs' names relative to out. A folder of its own made next to
    it first, then put in its place."""
    classic, temp = out / CLASSIC, out / (CLASSIC + '.new')
    if temp.exists():
        for f in temp.iterdir():
            f.unlink()
    temp.mkdir(parents=True, exist_ok=True)
    pv = out / PREVIEW
    if previews:
        pv.mkdir(parents=True, exist_ok=True)
    names = []
    with open_disc(disc) as d:
        src = Source(d)
        for stage in range(len(NAMES)):
            st = stage_data(src, stage)
            pack = build(st)
            (temp / pack_name(stage)).write_bytes(pack)
            names.append(f'{CLASSIC}/{pack_name(stage)}')
            if previews:
                for kind, (w, h, img) in pictures(pack).items():
                    png(pv / f'{slug(stage)}-{kind}.png', w, h, img)
            say(f'Tekken 2 stage {stage:2} {st["name"]}: {len(pack)} bytes, '
                f'panorama {len(st["pano"][0]) if st["pano"] else 0} x {len(st["pano"])} cells')
    if classic.exists():
        for f in classic.iterdir():
            f.unlink()
        classic.rmdir()
    temp.replace(classic)
    return names


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--disc', type=Path, help='the Tekken 2 disc (default: the one the setup remembered)')
    ap.add_argument('--root', type=Path, default=EXPANDED, help='the Expanded folder')
    ap.add_argument('--no-previews', action='store_true', help='no pictures')
    args = ap.parse_args(argv)
    disc = args.disc or find_disc(ADDON / 'workspace')
    if not disc:
        print('no Tekken 2 disc (give --disc, or run the setup)')
        return 1
    convert(disc, args.root / OUT, previews=not args.no_previews)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
