#!/usr/bin/env python3
"""The add-on's UI font: Fira Sans Medium (tools/data/fonts, SIL Open Font
License 1.1, OFL.txt there) drawn once into anti-aliased glyphs, written as
src/core/tmst_ui_font_data.c, so the game needs no font file and no font
library: tmst_ui_font.c draws them. It took the place of the add-on's 5 x 7
font (the user's, 2026-11-06: "too pixelated and digital"); where the game's
own Tekken 3 font is drawn, it stays.

Each face is a size for one kind of picture. Its texels need not be square:
a picture stretched over the screen (the Jukebox's list: 1024 x 392 texels
on 368 x 196 pixels of a 368 x 480 screen shown 4:3) has texels about 1.25
times as wide as tall, so the glyphs are drawn that much narrower, and look
right on the screen.

How: each glyph is drawn OVERSAMPLE times larger, its origin on a texel's
corner, and averaged down (box filter: the share of each texel the glyph
covers), 16 levels (4 bits a texel). Advances keep their fractions (1/64
texel); the glyphs land on whole texels. No kerning (Pillow here has no
shaping library to read the font's GPOS).

Characters: ASCII, Latin-1, Latin Extended-A (Hungarian o/u with double
acute among them) and a few punctuation marks (dashes, quotes, bullet,
ellipsis). A face without them (SCREEN: ASCII) draws a letter of Latin-1 or
Latin Extended-A as its plain letter (tmst_ui_fold_latin, from the
characters' decompositions: a for a-acute, o for o-double-acute), anything
else as '?'.

Icons (the SCREEN face, in the Private Use Area from U+E000, TmstUiIcon in
tmst_ui_font.h): the pad's symbols and arrows, drawn as the glyphs are (one
colour each: the text's tint), as tall as the capitals, square on the screen.

    python tools/ui_font.py                 (writes src/core/tmst_ui_font_data.c)
    python tools/ui_font.py --preview a.png (each face's glyphs, magnified)
"""
from __future__ import annotations

import argparse
import unicodedata
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ADDON = Path(__file__).resolve().parents[1]
FONT = ADDON / "tools" / "data" / "fonts" / "FiraSans-Medium.ttf"
OUT = ADDON / "src" / "core" / "tmst_ui_font_data.c"
OVERSAMPLE = 8

ASCII = list(range(0x20, 0x7F))
LATIN = (ASCII + list(range(0xA0, 0x100)) + list(range(0x100, 0x17F))   # U+017F (long s): not in the font
         + [0x2013, 0x2014, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2026])

# name (tmst_ui_face_<name> in tmst_ui_font.h), size in texels (the font's
# em, tall), width of a texel against its height, its characters.
FACES = [
    ("LIST", 22, 1.25, LATIN),              # the Jukebox's picture: 1024 x 392 on 368 x 196
    # the GPU's text (the overlay's sheet in the upper VRAM half, tmst_ui_prim_text): 2 texels to a screen
    # unit each way on the 368 x 480 screen shown 4:3, so a texel is 4 * 480 / (3 * 368) as wide as tall;
    # ASCII, the ellipsis and the icons only, to keep the sheet small
    ("SCREEN", 34, 4 * 480 / (3 * 368), ASCII + [0x2026]),
]
ICON_FACES = {"SCREEN"}
# Bolder: each stroke widened this much (hi-res pixels, OVERSAMPLE to a texel's
# height) on each side, the advances by twice that.
EMBOLDEN = {"SCREEN": 4}

# The icons, U+E000 on (tmst_ui_font.h's TmstUiIcon, in this order): name,
# width against the capitals' height (on the screen), its drawing (ImageDraw
# on the glyph's w x h hi-res pixels, which are square on the screen; t: the
# stroke).
ICON_BASE = 0xE000


def _triangle(d, w, h, t):
    m = t * 0.6
    d.line([(w / 2, m), (w - m, h - m), (m, h - m), (w / 2, m)], fill=255, width=round(t), joint="curve")


def _circle(d, w, h, t):
    r = min(w, h) / 2 - 1
    d.ellipse([w / 2 - r, h / 2 - r, w / 2 + r, h / 2 + r], outline=255, width=round(t))


def _cross(d, w, h, t):
    m = t * 0.9
    d.line([(m, m), (w - m, h - m)], fill=255, width=round(t))
    d.line([(w - m, m), (m, h - m)], fill=255, width=round(t))


def _square(d, w, h, t):
    m = h * 0.06
    d.rectangle([m, m, w - m, h - m], outline=255, width=round(t))


def _left_right(d, w, h, t):
    a = h * 0.62                                        # each arrow's width
    d.polygon([(0, h / 2), (a, h * 0.12), (a, h * 0.88)], fill=255)
    d.polygon([(w, h / 2), (w - a, h * 0.12), (w - a, h * 0.88)], fill=255)


def _play(d, w, h, t):
    d.polygon([(w * 0.1, 0), (w * 0.1, h), (w, h / 2)], fill=255)


def _up(d, w, h, t):
    d.polygon([(0, h * 0.78), (w, h * 0.78), (w / 2, h * 0.22)], fill=255)


def _down(d, w, h, t):
    d.polygon([(0, h * 0.22), (w, h * 0.22), (w / 2, h * 0.78)], fill=255)


ICONS = [("TRIANGLE", 1.0, _triangle), ("CIRCLE", 1.0, _circle), ("CROSS", 1.0, _cross), ("SQUARE", 1.0, _square),
         ("LEFT_RIGHT", 1.7, _left_right), ("PLAY", 0.9, _play), ("UP", 1.3, _up), ("DOWN", 1.3, _down)]

# A plain letter for those of U+00A0..U+017F the decompositions leave out.
FOLD_EXTRA = {"\u00df": "s", "\u00e6": "a", "\u00c6": "A", "\u00f8": "o", "\u00d8": "O", "\u0111": "d",
              "\u0110": "D", "\u0142": "l", "\u0141": "L", "\u0131": "i", "\u0127": "h", "\u0126": "H",
              "\u0167": "t", "\u0166": "T", "\u0153": "o", "\u0152": "O", "\u00f0": "d", "\u00d0": "D",
              "\u00fe": "t", "\u00de": "T", "\u0138": "k", "\u014b": "n", "\u014a": "N", "\u00a1": "!",
              "\u00bf": "?", "\u00ab": '"', "\u00bb": '"', "\u00d7": "x", "\u00b7": ".", "\u00a0": " "}
FOLD_FIRST, FOLD_LAST = 0xA0, 0x17F


def fold_table() -> list[int]:
    """U+00A0..U+017F: each one's plain ASCII letter (0: none)."""
    out = []
    for code in range(FOLD_FIRST, FOLD_LAST + 1):
        ch = chr(code)
        plain = FOLD_EXTRA.get(ch) or unicodedata.normalize("NFKD", ch)[:1]
        out.append(ord(plain) if plain and 0x20 <= ord(plain) < 0x7F else 0)
    return out


def bake_icons(cap: int, aspect: float, stroke: int = 0):
    """The icons as glyphs: from the baseline up to the capitals' height, their
    lines as much bolder as the face's (stroke: EMBOLDEN's)."""
    sx, sy = round(OVERSAMPLE * aspect), OVERSAMPLE
    glyphs = []
    for i, (name, wide, draw) in enumerate(ICONS):
        h = cap
        w = max(1, round(cap * wide / aspect))          # texels: square on the screen for wide 1
        img = Image.new("L", (w * sx, h * sy), 0)
        draw(ImageDraw.Draw(img), w * sx, h * sy, cap * sy * 0.14 + 2 * stroke)
        small = img.reduce((sx, sy))
        px = small.load()
        rows = [[(px[x, y] * 15 + 127) // 255 for x in range(w)] for y in range(h)]
        glyphs.append((ICON_BASE + i, 1, -cap, w, h, (w + 2) * 64, rows))
    return glyphs


def bake(size: int, aspect: float, codes, stroke: int = 0):
    """The face's glyphs: (code, left, top, w, h, advance in 1/64 texel, 4-bit rows)."""
    font = ImageFont.truetype(str(FONT), size * OVERSAMPLE)
    sx, sy = round(OVERSAMPLE * aspect), OVERSAMPLE      # hi-res pixels in a texel (aspect to 1/8)
    glyphs = []
    for code in codes:
        ch = chr(code)
        advance = round((font.getlength(ch) + 2 * stroke) / sx * 64)
        x0, y0, x1, y1 = font.getbbox(ch, anchor="ls", stroke_width=stroke)
        x0, x1 = x0 + stroke, x1 + stroke                 # centred in the wider advance
        if x1 <= x0 or y1 <= y0:
            glyphs.append((code, 0, 0, 0, 0, advance, []))
            continue
        left, right = x0 // sx, -(-x1 // sx)
        top, bottom = y0 // sy, -(-y1 // sy)
        w, h = right - left, bottom - top
        img = Image.new("L", (w * sx, h * sy), 0)
        ImageDraw.Draw(img).text((-left * sx + stroke, -top * sy), ch, font=font, fill=255, anchor="ls",
                                  stroke_width=stroke, stroke_fill=255)
        small = img.reduce((sx, sy))
        px = small.load()
        rows = [[(px[x, y] * 15 + 127) // 255 for x in range(w)] for y in range(h)]
        glyphs.append((code, left, top, w, h, advance, rows))
    cap = -font.getbbox("H", anchor="ls")[1] / sy
    ascent, descent = font.getmetrics()
    return glyphs, round(cap), round(ascent / sy), round(descent / sy)


def bake_face(name: str, size: int, aspect: float, codes):
    """bake(), and the icons for a face of ICON_FACES."""
    stroke = EMBOLDEN.get(name, 0)
    glyphs, cap, ascent, descent = bake(size, aspect, codes, stroke)
    if name in ICON_FACES:
        glyphs = glyphs + bake_icons(cap, aspect, stroke)
    return glyphs, cap, ascent, descent


def write_c(faces) -> str:
    out = ["/* Generated by tools/ui_font.py from Fira Sans Medium (tools/data/fonts,",
           " * SIL Open Font License 1.1): do not edit. tmst_ui_font.c draws it. */",
           '#include "tmst_ui_font.h"', ""]
    for name, size, aspect, (glyphs, cap, ascent, descent) in faces:
        lo = name.lower()
        bits, table = bytearray(), []
        for code, left, top, w, h, advance, rows in glyphs:
            table.append(f"{{0x{code:04x},{left},{top},{w},{h},{advance},{len(bits)}}}")
            for row in rows:
                row = row + [0] * (len(row) & 1)
                bits.extend(row[i] << 4 | row[i + 1] for i in range(0, len(row), 2))
        out.append(f"/* {name}: {size} texels to the em, a texel {aspect:.3f} times as wide as tall. */")
        out.append(f"static const uint8_t {lo}_bits[{len(bits)}] = {{")
        for i in range(0, len(bits), 24):
            out.append("    " + ",".join(f"0x{b:02x}" for b in bits[i:i + 24]) + ",")
        out.append("};")
        out.append(f"static const TmstUiGlyph {lo}_glyphs[{len(table)}] = {{")
        for i in range(0, len(table), 6):
            out.append("    " + ", ".join(table[i:i + 6]) + ",")
        out.append("};")
        out.append(f"const TmstUiFace tmst_ui_face_{lo} = {{{lo}_glyphs, {len(table)}, {lo}_bits, "
                   f"{cap}, {ascent}, {descent}}};")
        out.append("")
    fold = fold_table()
    out.append(f"/* U+{FOLD_FIRST:04X}..U+{FOLD_LAST:04X}: each one's plain letter (0: none), for a face without it. */")
    out.append(f"const char tmst_ui_fold_latin[{len(fold)}] = {{")
    for i in range(0, len(fold), 24):
        out.append("    " + ",".join(str(c) for c in fold[i:i + 24]) + ",")
    out.append("};")
    out.append("")
    return "\n".join(out)


def preview(faces, path: Path, zoom: int = 3) -> None:
    pics = []
    for name, size, aspect, (glyphs, cap, ascent, descent) in faces:
        line = ascent + descent + 4
        per = 40
        rows_n = -(-len(glyphs) // per)
        w = max(sum(g[5] for g in glyphs[i:i + per]) // 64 for i in range(0, len(glyphs), per)) + 8
        pic = Image.new("L", (w, rows_n * line + 4), 24)
        px = pic.load()
        for r in range(rows_n):
            pen = 4 * 64
            base = 2 + r * line + ascent
            for code, left, top, gw, gh, advance, rows in glyphs[r * per:(r + 1) * per]:
                x0 = (pen + 32) // 64 + left
                for y, row in enumerate(rows):
                    for x, v in enumerate(row):
                        if v and 0 <= x0 + x < w and 0 <= base + top + y < pic.height:
                            px[x0 + x, base + top + y] = 24 + v * 231 // 15
                pen += advance
        pics.append(pic.resize((round(pic.width * aspect * zoom), pic.height * zoom), Image.NEAREST))
    sheet = Image.new("L", (max(p.width for p in pics), sum(p.height + 8 for p in pics)), 0)
    y = 0
    for p in pics:
        sheet.paste(p, (0, y)); y += p.height + 8
    sheet.save(path)


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--preview", type=Path, help="write the glyphs, magnified, to this PNG instead")
    args = ap.parse_args()
    faces = [(n, s, a, bake_face(n, s, a, c)) for n, s, a, c in FACES]
    if args.preview:
        preview(faces, args.preview)
        print(f"wrote {args.preview}")
        return
    OUT.write_text(write_c(faces) + "\n", encoding="utf-8", newline="\n")
    print(f"wrote {OUT.relative_to(ADDON)}")


if __name__ == "__main__":
    main()
