#!/usr/bin/env python3
"""The PS2 ending and the engine's, frame by frame, side by side.

    venv-import python tools/ps2/ttt_compare_shots.py PS2_DIR T3_DIR OUT_DIR [--per-sheet 6]

PS2_DIR: <t>.png, screenshots of PCSX2 save states (t = the script clock read
in the state's own EE RAM, 0x5F6C3C). T3_DIR: cine_<frame>.png, the engine's
screenshots at those frames (tools/cine_scene_drive.py --hold). Each T3 frame
is paired with the nearest PS2 one (at most 2 frames apart), both shown 4:3
at 640 x 480, the frame numbers on top. OUT_DIR/compare_<k>.png, a few pairs a
sheet. Game pictures: keep them in workspace/, never published.
"""
import argparse, re
from pathlib import Path
from PIL import Image, ImageDraw


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('ps2', type=Path); ap.add_argument('t3', type=Path); ap.add_argument('out', type=Path)
    ap.add_argument('--per-sheet', type=int, default=6)
    a = ap.parse_args()
    a.out.mkdir(parents=True, exist_ok=True)
    ps2 = {int(p.stem): p for p in a.ps2.glob('*.png') if p.stem.isdigit()}
    t3 = {int(re.match(r'cine_(\d+)', p.stem).group(1)): p for p in a.t3.glob('cine_*.png')}
    pairs = []
    for f in sorted(t3):
        near = min(ps2, key=lambda t: abs(t - f), default=None)
        if near is not None and abs(near - f) <= 2: pairs.append((f, near))
    W, H = 640, 480
    for k in range(0, len(pairs), a.per_sheet):
        chunk = pairs[k:k + a.per_sheet]
        sheet = Image.new('RGB', (2 * W, H * len(chunk)))
        for j, (f, t) in enumerate(chunk):
            left = Image.open(ps2[t]).convert('RGB').resize((W, H))
            right = Image.open(t3[f]).convert('RGB').resize((W, H))
            ImageDraw.Draw(left).text((8, 8), f'PS2 script frame {t}', fill=(255, 255, 0))
            ImageDraw.Draw(right).text((8, 8), f'T3 .cine frame {f}', fill=(255, 255, 0))
            sheet.paste(left, (0, H * j)); sheet.paste(right, (W, H * j))
        sheet.save(a.out / f'compare_{k // a.per_sheet:02d}.png')
    print(f'{len(pairs)} pairs in {a.out}')


if __name__ == '__main__':
    main()
