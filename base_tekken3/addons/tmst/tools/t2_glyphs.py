#!/usr/bin/env python3
"""Tekken 2 Content's Stages, Remake: the writing on the stages, drawn by
hand on pixel grids (and Hong Kong's neon sign in tube strokes), so that the
setup gives the same pictures on every machine, whatever fonts it has.

Each sign is in the language of its stage's country, a real word, said to
the user before it was drawn (the user's, 2026-11-05, after a player's
comment on the "玉" of a Great Wall banner and the "鐵拳" of Hong Kong):

- The Great Wall's war banners: 明 (míng, the Ming dynasty, who built most
  of the wall seen today) and 令 (lìng, "command": the commander's banner);
- Szechwan's gate (pailou) board: 天府之國 (Tiānfǔ zhī guó, "the land of
  abundance", Sichuan's old name for itself), traditional characters, read
  right to left as on old boards;
- Szechwan's prayer flags: no writing, the wind horse (lungta) in the middle;
- the Korean temple's guardian posts (jangseung), in Hangul: 천하대장군
  (Cheonha Daejanggun, "Great General under Heaven") on his, 지하여장군
  (Jiha Yeojanggun, "Woman General of the Underworld") on hers;
- Kyoto's paper lanterns: 祭 (matsuri, "festival");
- Hong Kong's rooftop sign: 香港 (Hong Kong, "fragrant harbour"), in neon;
- the English-speaking stages' signs: HOT DOGS (New York's cart), U.S. MAIL
  (its mailbox), TELEPHONE (London's K6 kiosk), FRAGILE (the Launch Pad's
  crate).

    python tools/t2_glyphs.py --preview <out.png>     (every sign, magnified)
"""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np


def grid(rows: str) -> np.ndarray:
    """'#' ink, '.' none; rows split by spaces or new lines."""
    lines = rows.split()
    return np.array([[c == '#' for c in line] for line in lines], bool)


# --- the CJK and Hangul signs, each on its own grid -----------------------

GLYPHS = {
    # 7 x 10: the Great Wall's banners
    '明': grid("""
        ....###
        ###.#.#
        #.#.###
        ###.#.#
        #.#.###
        ###.#.#
        ....#.#
        ...#..#
        ..#..##
        .......
    """),
    '令': grid("""
        ...#...
        ..#.#..
        .#...#.
        #.....#
        ..###..
        .......
        .#####.
        ...#.#.
        ...#.#.
        ...#...
    """),
    # 9 x 9: Szechwan's gate board
    '天': grid("""
        .#######.
        ....#....
        ....#....
        #########
        ....#....
        ...#.#...
        ..#...#..
        .#.....#.
        #.......#
    """),
    '府': grid("""
        ....#....
        #########
        #........
        #..#...#.
        #.#.####.
        #.##...#.
        #..#.#.#.
        #..#...#.
        #..#..##.
    """),
    '之': grid("""
        ...##....
        .....#...
        .######..
        .....#...
        ....#....
        ...#.....
        ..#......
        .##......
        #..######
    """),
    '國': grid("""
        #########
        #.....#.#
        #.#####.#
        #.##..#.#
        #.##..#.#
        #......##
        #.###..##
        #.......#
        #########
    """),
    # 9 x 9: the guardian posts (Hangul)
    '천': grid("""
        ..#....#.
        #####..#.
        ..#....#.
        .#.#.###.
        #...#..#.
        .......#.
        .#.......
        .#.......
        .#######.
    """),
    '하': grid("""
        ..##...#.
        ######.#.
        .......#.
        .####..##
        #....#.#.
        #....#.#.
        .####..#.
        .......#.
        .......#.
    """),
    '대': grid("""
        .....#.#.
        ####.#.#.
        #....#.#.
        #....###.
        #....#.#.
        ####.#.#.
        .....#.#.
        .....#.#.
        .....#.#.
    """),
    '장': grid("""
        #####..#.
        ...#...#.
        ..#.#..##
        .#...#.#.
        .......#.
        ..####...
        .#....#..
        .#....#..
        ..####...
    """),
    '군': grid("""
        #######..
        ......#..
        ......#..
        .........
        #########
        ....#....
        .#..#....
        .#.......
        .#######.
    """),
    '지': grid("""
        #####..#.
        ...#...#.
        ...#...#.
        ..#.#..#.
        .#...#.#.
        #.....##.
        .......#.
        .......#.
        .......#.
    """),
    '여': grid("""
        .......#.
        .##....#.
        #..#.###.
        #..#...#.
        #..#.###.
        .##....#.
        .......#.
        .......#.
        .......#.
    """),
    # 8 x 10: Kyoto's paper lanterns
    '祭': grid("""
        .#..###.
        .###..#.
        #..#.#..
        .#.#..#.
        ..#..#.#
        ..####..
        ........
        ########
        .#.#..#.
        #..#...#
    """),
    # 9 x 6: Szechwan's prayer flags, the wind horse
    'horse': grid("""
        .......#.
        ......###
        #....##..
        .######..
        .#....#..
        .#....#..
    """),
}

# --- Latin, two pixel fonts ------------------------------------------------

FONT3 = {   # 3 x 5
    'A': '.#. #.# ### #.# #.#', 'E': '### #.. ##. #.. ###', 'H': '#.# #.# ### #.# #.#',
    'I': '### .#. .#. .#. ###', 'L': '#.. #.. #.. #.. ###', 'M': '#.# ### ### #.# #.#',
    'N': '##. #.# #.# #.# #.#', 'O': '### #.# #.# #.# ###', 'P': '##. #.# ##. #.. #..',
    'S': '### #.. ### ..# ###', 'T': '### .#. .#. .#. .#.', 'U': '#.# #.# #.# #.# ###',
    '.': '. . . . #', ' ': '.. .. .. .. ..',
}
FONT5 = {   # 5 x 7
    'A': '.###. #...# #...# ##### #...# #...# #...#', 'D': '####. #...# #...# #...# #...# #...# ####.',
    'E': '##### #.... #.... ####. #.... #.... #####', 'F': '##### #.... #.... ####. #.... #.... #....',
    'G': '.###. #...# #.... #.### #...# #...# .###.', 'H': '#...# #...# #...# ##### #...# #...# #...#',
    'I': '.###. ..#.. ..#.. ..#.. ..#.. ..#.. .###.', 'L': '#.... #.... #.... #.... #.... #.... #####',
    'O': '.###. #...# #...# #...# #...# #...# .###.', 'R': '####. #...# #...# ####. #.#.. #..#. #...#',
    'S': '.#### #.... #.... .###. ....# ....# ####.', 'T': '##### ..#.. ..#.. ..#.. ..#.. ..#.. ..#..',
    ' ': '.. .. .. .. .. .. ..',
}


def text(words: str, font: dict, gap: int = 1) -> np.ndarray:
    """A line of words in one of the pixel fonts, gap columns between letters."""
    parts = [grid(font[c]) for c in words]
    h = parts[0].shape[0]
    cols = []
    for k, p in enumerate(parts):
        if k: cols.append(np.zeros((h, gap), bool))
        cols.append(p)
    return np.concatenate(cols, 1)


def stamp(img: np.ndarray, mask: np.ndarray, y: int, x: int, colour) -> None:
    """mask's ink onto img (H x W x 3, or H x W) at row y, column x."""
    h, w = mask.shape
    img[y:y + h, x:x + w][mask] = colour


# --- Hong Kong's neon: 香港 in tube strokes ---------------------------------
# Each character in a box of 12 x 12 (x right, y down), its strokes as
# polylines, drawn as tubes of one width (a neon sign's glass).
NEON = {
    '香': [
        [(8.6, 0.4), (3.4, 1.7)],                         # the grain's slant head
        [(1.4, 3.0), (10.6, 3.0)],                        # across
        [(6.0, 1.4), (6.0, 6.9)],                         # down
        [(6.0, 3.6), (1.4, 6.8)],                         # falling left
        [(6.0, 3.6), (10.6, 6.8)],                        # falling right
        [(3.0, 7.6), (3.0, 11.8)],                        # 日: its left
        [(3.0, 7.6), (9.0, 7.6), (9.0, 11.8)],            # its top and right
        [(3.0, 9.7), (9.0, 9.7)],                         # its middle
        [(3.0, 11.8), (9.0, 11.8)],                       # its foot
    ],
    '港': [
        [(0.8, 0.9), (2.0, 2.0)],                         # water: two drops
        [(0.4, 4.0), (1.6, 5.0)],
        [(0.6, 11.0), (2.3, 7.4)],                        # and the rising stroke
        [(4.0, 2.6), (11.0, 2.6)],                        # 共: across
        [(6.0, 0.4), (6.0, 5.2)],                         # its two downs
        [(9.0, 0.4), (9.0, 5.2)],
        [(3.4, 5.2), (11.8, 5.2)],                        # the long across
        [(6.2, 5.8), (4.2, 7.4)],                         # its two feet
        [(8.8, 5.8), (11.2, 7.4)],
        [(5.2, 7.8), (9.6, 7.8), (9.6, 9.5), (5.2, 9.5)],   # 巳
        [(5.2, 7.8), (5.2, 11.2), (6.0, 11.8), (11.6, 11.8), (11.6, 10.6)],
    ],
}


def neon_words(words: str = '香港', size=(128, 64)) -> np.ndarray:
    """The sign's words, white on black (L, 0..255), as tubes: the face's
    inner 104 x 40 (inside its cyan tube), the characters side by side."""
    from PIL import Image, ImageDraw
    w, h = size
    S = 4                                       # drawn 4 times larger, then smoothed down
    img = Image.new('L', (w * S, h * S), 0)
    d = ImageDraw.Draw(img)
    box = 40                                    # a character's box, pixels
    x0 = (w - box * len(words) - 12 * (len(words) - 1)) // 2
    y0 = (h - box) // 2
    for k, ch in enumerate(words):
        ox, oy = x0 + k * (box + 12), y0
        for stroke in NEON[ch]:
            pts = [((ox + px * box / 12) * S, (oy + py * box / 12) * S) for px, py in stroke]
            d.line(pts, fill=255, width=3 * S, joint='curve')
            for px, py in (pts[0], pts[-1]):
                r = 1.5 * S
                d.ellipse((px - r, py - r, px + r, py + r), fill=255)
    return np.asarray(img.resize((w, h), Image.LANCZOS))


def preview(out: Path) -> None:
    """Every sign magnified, on a grey sheet, with its name under it."""
    from PIL import Image, ImageDraw
    Z = 10
    items = [(k, v) for k, v in GLYPHS.items()]
    items += [(t, text(t, FONT5)) for t in ('HOT DOGS', 'FRAGILE')]
    items += [(t, text(t, FONT3)) for t in ('U.S. MAIL', 'TELEPHONE')]
    tiles = []
    for name, m in items:
        im = np.full((m.shape[0] + 2, m.shape[1] + 2), 220, np.uint8)
        im[1:-1, 1:-1][m] = 20
        tiles.append((name, Image.fromarray(im).resize(((m.shape[1] + 2) * Z, (m.shape[0] + 2) * Z), Image.NEAREST)))
    neon = Image.fromarray(neon_words()).resize((128 * 4, 64 * 4), Image.NEAREST)
    width = 1200
    x = y = 10
    row_h = 0
    places = []
    for name, t in tiles:
        if x + t.width > width - 10:
            x, y, row_h = 10, y + row_h + 30, 0
        places.append((name, t, x, y))
        x += t.width + 20
        row_h = max(row_h, t.height)
    y += row_h + 30
    sheet = Image.new('L', (width, y + neon.height + 20), 160)
    d = ImageDraw.Draw(sheet)
    for name, t, px, py in places:
        sheet.paste(t, (px, py))
        d.text((px, py + t.height + 4), name.encode('ascii', 'replace').decode() if not name.isascii() else name, fill=0)
    sheet.paste(neon, (10, y))
    sheet.save(out)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--preview', type=Path, required=True)
    preview(ap.parse_args(argv).preview)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
