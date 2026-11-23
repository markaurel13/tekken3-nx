#!/usr/bin/env python3
"""Tekken 2's stage data as the Remake's generator (tools/t2_remake_stage.py)
reads it: the old fork's tools/t2_stage.py helpers, as they were (their
colours 5 bits shifted up, not scaled: the packs the same to the byte), on
the add-on's disc reader (tools/tekken2_stages.py Source: .cue, .bin, .chd).
"""
from __future__ import annotations

import struct

import numpy as np

GROUPS, GROUP = 24, 16                                 # BNS: a fighter's first group, groups per fighter
SKY_REPEAT = 2                                         # times the panorama goes round
CELL_W, CELL_H, COLUMNS = 64, 32, 24                   # the panorama's cells
TILES_PER_ROW, TILE_Y, CLUT_Y = 60, 512, 1000          # where the pack's source tiles lie (System 12 VRAM)
SAME_PART = 7                                          # mean texel difference of floor parts taken as one


# --- the disc ---------------------------------------------------------------

def tims(data: bytes) -> list[dict]:
    p, out = 0, []
    while p + 8 <= len(data) and struct.unpack_from('<I', data, p)[0] == 0x10:
        flags = struct.unpack_from('<I', data, p + 4)[0]; p += 8
        for block in ('clut', 'image') if flags & 8 else ('image',):
            n, x, y, w, h = struct.unpack_from('<I4H', data, p)
            out.append(dict(x=x, y=y, w=w, h=h, words=np.frombuffer(data, '<u2', w * h, p + 12).reshape(h, w)))
            p += n
    return out


def vram_of(textures: bytes) -> np.ndarray:
    """The PlayStation's VRAM (512 x 1024 halfwords) with the stage's textures."""
    vram = np.zeros((512, 1024), np.uint16)
    for t in tims(textures):
        vram[t['y']:t['y'] + t['h'], t['x']:t['x'] + t['w']] = t['words']
    return vram


def record(data: bytes) -> dict:
    offsets = struct.unpack_from('<10I', data, 4)
    floor = [struct.unpack_from('<4H', data, offsets[0] + 4 + 8 * i) for i in range(100)]
    cells = [struct.unpack_from('<3H', data, offsets[1] + 6 * i) for i in range((offsets[2] - offsets[1]) // 6)]
    # The panorama's map: rows of 24 places round the stage, each the cells'
    # column shown there, top to bottom (Launch Pad's: the hangar's wall,
    # columns 0..7, at every place in its upper half; below, the shuttle's
    # columns 8..15 at places 8..15 only).
    m = data[offsets[2]:offsets[3]]
    grid = [list(m[r:r + COLUMNS]) for r in range(0, len(m) - COLUMNS + 1, COLUMNS)] or [list(range(COLUMNS))]
    return dict(floor=floor, cells=cells, map=grid)


def texels(vram: np.ndarray, page: int, clut: int, u0: int, v0: int, w: int, h: int) -> np.ndarray:
    """A w x h rectangle of a 4-bit texture page as BGR555."""
    tx, ty = (page & 15) * 64, (page >> 4 & 1) * 256
    u, v = np.meshgrid(u0 + np.arange(w), v0 + np.arange(h))
    index = vram[(ty + v) % 512, (tx + u // 4) % 1024] >> ((u & 3) * 4) & 15
    return vram[clut >> 6 & 511, (clut & 63) * 16:(clut & 63) * 16 + 16][index]


def rgb(c: np.ndarray) -> np.ndarray:
    return np.dstack([(c & 31) << 3, (c >> 5 & 31) << 3, (c >> 10 & 31) << 3]).astype(np.uint8)


def panorama(vram: np.ndarray, rec: dict) -> np.ndarray:
    """The 24 places round the stage (the cells: the map's highest column
    + 1 a row)."""
    grid = rec['map']
    columns = max(max(row) for row in grid) + 1
    rows = len(rec['cells']) // columns
    pano = np.zeros((CELL_H * rows, CELL_W * COLUMNS, 3), np.uint8)
    # The map's last row, the one a fight shows (its upper rows, without the
    # shuttle, another view of the hangar).
    row = grid[-1]
    for r in range(rows):
        for p in range(COLUMNS):
            uv, clut, page = rec['cells'][r * columns + row[p]]
            pano[CELL_H * r:CELL_H * (r + 1), CELL_W * p:CELL_W * (p + 1)] = rgb(texels(vram, page, clut, uv & 255, uv >> 8,
                                                                                       CELL_W, CELL_H))
    return pano


def floor_tile(vram: np.ndarray, entry: tuple) -> np.ndarray:
    uv, page, _, clut = entry
    t = rgb(texels(vram, page & 255, clut, uv & 255, uv >> 8, 64, 64))
    # The high byte turns the tile: 0x40 a quarter clockwise, 0x80 a half,
    # 0xC0 three quarters (Launch Pad's logo and lines join only so).
    return np.rot90(t, (0, 3, 2, 1)[page >> 14 & 3])
