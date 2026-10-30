#!/usr/bin/env python3
"""Jin's Omen mark for his Tekken 3 face: the Devil tattoo, gem and red eye of TTT1.

    python3 tools/ttt1_jin_omen_mark.py [--style full|thin|gem] [--out DIR] [--preview PNG]

In TTT1 every Omen move shows face expression 5 (timed property 0x2A): the
arcade copies the Devil face (TIM (32, 0)) over the shown face (32, 64) and
the tattooed forehead (64, 96) over the plain one (80, 96) -- TIPS § 30,
tools/ttt1_devil_jin.py. Jin on his TTT1 moves keeps his Tekken 3 model, whose
face is one 32 x 64 8-bit image (VRAM (384, 64) for player 1): the same half
face as TTT1's below the brows, row for row (T3 row y = TTT1 row y + 24,
forehead and face stacked), with a shorter forehead above them (hairline at
row 1 against 3, brows at row 14 against 38).

This tool takes the pixels where the Devil images differ from the plain ones
and lays them on the T3 face: rows from the brows down one for one, the
forehead squeezed between hairline and brows. --style: full keeps every
tattoo row the squeeze covers, thin one row in two or three (finer lines),
gem only the gem, the lines from it and the red eye.

Output (in --out, default workspace/ttt1-import/jin-omen/guest):
Jin-TTT1-omen-mark.bin = "OMEN", u16 width 32, u16 height 64, then one u16 per
pixel, row by row: 0 = leave the face alone, else 0x8000 | the 15-bit colour.
The runtime (src/tekken3_jin_omen.c) paints it with the face's own palette,
plus palette entries the face does not use for the reds. Game data never
leaves workspace/ (ignored by git).
"""
import argparse, struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT / 'tools')]
import ttt1_devil_jin as D

WIDTH, HEIGHT = 32, 64
# Tekken 3 face rows -> TTT1 rows (forehead 0..31 then face 32..95), measured
# on the row brightness of both images (brows: T3 18..22, TTT1 42..46).
HAIR_T3, HAIR_TTT1, BROW_T3, BROW_TTT1 = 1, 3, 14, 38
# Devil pixels: those that differ from the plain images by more than this (sum of 8-bit channels).
THRESHOLD = 60


def images():
    """TTT1 Jin costume 1: (plain, devil) as rows of (r, g, b), forehead above face."""
    ram = (D.WORK / 'captures/jun-select-ram.bin').read_bytes()
    bank = (D.WORK / 'ttt1/bankedroms.bin').read_bytes()
    to, tn = struct.unpack_from('<II', ram, D.DIRECTORY + D.SLOT * 8)
    tex = bank[to:to + tn]
    tims, p = {}, 0
    while p + 8 <= len(tex):
        magic, flags = struct.unpack_from('<II', tex, p)
        if magic != 16: break
        q, clut = p + 8, None
        if flags & 8:
            n = struct.unpack_from('<I', tex, q)[0]
            clut = struct.unpack_from(f'<{(n - 12) // 2}H', tex, q + 12)
            q += n
        size, x, y, w, h = struct.unpack_from('<I4H', tex, q)
        if flags & 7 == 1:                                 # 8-bit
            pix = tex[q + 12:q + size]
            tims[(x, y)] = [[clut[pix[r * w * 2 + c]] for c in range(w * 2)] for r in range(h)]
        p = q + size
    rgb = lambda v: ((v & 31) << 3, (v >> 5 & 31) << 3, (v >> 10 & 31) << 3)
    stack = lambda forehead, face: [[rgb(v) for v in row] for row in tims[forehead] + tims[face]]
    return stack(D.SHOWN_FOREHEAD, D.SHOWN_FACE), stack(D.TATTOO, D.DEVIL_FACE)


def source_row(y):
    if y >= BROW_T3: return y + BROW_TTT1 - BROW_T3
    return HAIR_TTT1 + (y - HAIR_T3) * (BROW_TTT1 - HAIR_TTT1) / (BROW_T3 - HAIR_T3)


def red(c): return c[0] > 120 and c[1] < 100


def mark(style):
    plain, devil = images()
    marked = [[sum(abs(a - b) for a, b in zip(p, d)) > THRESHOLD for p, d in zip(rp, rd)] for rp, rd in zip(plain, devil)]
    out = [[None] * WIDTH for _ in range(HEIGHT)]
    for y in range(HEIGHT):
        a, b = source_row(y), source_row(y + 1)
        rows = range(int(a), max(int(a) + 1, -int(-b // 1))) if style != 'thin' else [int(round((a + b) / 2 - .5))]
        for x in range(WIDTH):
            hits = [devil[r][x] for r in rows if 0 <= r < len(devil) and marked[r][x]]
            if style == 'gem' and y < BROW_T3 - 4: hits = [c for c in hits if red(c)]
            if hits: out[y][x] = min(hits, key=sum)     # the darkest: thin lines stay unbroken
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--style', choices=('full', 'thin', 'gem'), default='thin')
    ap.add_argument('--out', type=Path, default=D.WORK / 'jin-omen/guest')
    ap.add_argument('--preview', type=Path, help='also write the mark alone, x8, as a PNG')
    a = ap.parse_args()
    m = mark(a.style)
    words = [0 if c is None else 0x8000 | c[0] >> 3 | (c[1] >> 3) << 5 | (c[2] >> 3) << 10 for row in m for c in row]
    a.out.mkdir(parents=True, exist_ok=True)
    (a.out / 'Jin-TTT1-omen-mark.bin').write_bytes(b'OMEN' + struct.pack(f'<2H{len(words)}H', WIDTH, HEIGHT, *words))
    print(f'Jin Omen mark ({a.style}): {sum(1 for w in words if w)} pixels -> {a.out / "Jin-TTT1-omen-mark.bin"}')
    if a.preview:
        from PIL import Image
        im = Image.new('RGBA', (WIDTH, HEIGHT))
        im.putdata([(0, 0, 0, 0) if c is None else (*c, 255) for row in m for c in row])
        im.resize((WIDTH * 8, HEIGHT * 8), Image.NEAREST).save(a.preview)


if __name__ == '__main__':
    main()
