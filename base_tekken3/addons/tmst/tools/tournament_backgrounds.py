"""The Tournament Mode screens' default backgrounds: data/tournament/<name>.png,
854 x 480 (16:9; the 4:3 screen is the middle 640). Plain drawing, nothing of
the game's. The player may replace any of them with a picture of their own
(any size, made to fit); data/tournament/guides/ shows where the mod draws.

    python tools/tournament_backgrounds.py [out_dir]
"""
import math
import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter

W, H, SAFE_X, SAFE_W = 854, 480, 107, 640
OUT = Path(__file__).resolve().parents[1] / 'data' / 'tournament'


def base(top, bottom):
    """A vertical gradient, a faint vignette."""
    im = Image.new('RGB', (W, H))
    px = im.load()
    for y in range(H):
        t = y / (H - 1)
        row = tuple(int(top[c] + (bottom[c] - top[c]) * t) for c in range(3))
        for x in range(W):
            d = min(1.0, abs(x - W / 2) / (W / 2)) ** 2 * 0.45
            px[x, y] = tuple(int(v * (1 - d)) for v in row)
    return im


def glow(im, cx, cy, r, colour, strength):
    layer = Image.new('RGB', (W, H))
    ImageDraw.Draw(layer).ellipse((cx - r, cy - r, cx + r, cy + r), fill=colour)
    layer = layer.filter(ImageFilter.GaussianBlur(r / 2))
    return Image.blend(im, Image.composite(layer, im, layer.convert('L')), strength)


def bands(im, y, h, colour):
    """The title band: dark in the middle, the colour fading in at the sides (as the selector's)."""
    d = ImageDraw.Draw(im)
    for x in range(W):
        t = min(1.0, abs(x - W / 2) / (SAFE_W / 2)) ** 1.5
        c = tuple(int(colour[k] * t) for k in range(3))
        d.line((x, y, x, y + h), fill=c)
    d.line((0, y, W, y), fill=(90, 70, 30)); d.line((0, y + h, W, y + h), fill=(90, 70, 30))


def streaks(im, colour, seed, n=26):
    d = ImageDraw.Draw(im)
    s = seed
    for _ in range(n):
        s = (s * 1103515245 + 12345) & 0x7fffffff
        x = s % W
        s = (s * 1103515245 + 12345) & 0x7fffffff
        length = 80 + s % 220
        d.line((x, 0, x - length, H), fill=colour, width=1)


def progress():
    im = base((14, 10, 16), (8, 8, 12))
    streaks(im, (24, 18, 22), 7)
    im = glow(im, W // 2, 92, 90, (90, 20, 20), 0.35)
    bands(im, 12, 48, (150, 20, 18))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 424, W, H), fill=(10, 8, 10)); d.line((0, 424, W, 424), fill=(120, 90, 30))
    return im


def match():
    im = base((10, 8, 14), (6, 6, 8))
    im = glow(im, 260, 220, 150, (120, 90, 20), 0.30)
    im = glow(im, W - 260, 220, 150, (130, 20, 20), 0.30)
    streaks(im, (26, 20, 20), 11, 40)
    bands(im, 14, 58, (150, 20, 18))
    return im


def champion():
    im = base((22, 16, 8), (8, 6, 4))
    for k in range(18):                                   # rays from behind the champion
        a = k / 18 * math.pi * 2
        ray = Image.new('L', (W, H))
        ImageDraw.Draw(ray).polygon((260, 230, 260 + 900 * math.cos(a - 0.05), 230 + 900 * math.sin(a - 0.05),
                                     260 + 900 * math.cos(a + 0.05), 230 + 900 * math.sin(a + 0.05)), fill=40)
        im = Image.composite(Image.new('RGB', (W, H), (120, 90, 30)), im, ray)
    im = glow(im, 260, 230, 170, (200, 150, 40), 0.35)
    bands(im, 14, 64, (170, 120, 20))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 404, W, H), fill=(12, 9, 4)); d.line((0, 404, W, 404), fill=(160, 120, 40))
    return im


def over():
    im = base((16, 6, 8), (6, 4, 6))
    streaks(im, (30, 12, 14), 23)
    im = glow(im, W // 2, 92, 90, (110, 10, 10), 0.40)
    bands(im, 12, 48, (140, 10, 10))
    d = ImageDraw.Draw(im)
    d.rectangle((0, 424, W, H), fill=(12, 6, 6)); d.line((0, 424, W, 424), fill=(140, 30, 30))
    return im


def main():
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else OUT
    out.mkdir(parents=True, exist_ok=True)
    for name, make in (('progress', progress), ('match', match), ('champion', champion), ('over', over)):
        make().save(out / f'{name}.png', optimize=True)
        print(out / f'{name}.png')


if __name__ == '__main__':
    main()
