#!/usr/bin/env python3
"""Frame-by-frame comparison of a converted ending: Tekken 3's rendering
(tools/embu/play.py --video --keep-frames) beside the PS2 original's bones
seen through the PS2 camera (the capture), one image a frame, then a video.

    ~/TekkenProject/venv-import/bin/python3 tools/ps2/ttt_compare.py \\
        CAPTURE.jsonl T3_FRAMES_DIR OUT_DIR [--start 10]

T3 Embu frame start + i is capture frame i. Kazuya in white, Jin in red;
nodes 98 and above (effect anchors) left out. OUT_DIR/cmp_NNNNN.png and
OUT_DIR/compare.mp4 (game data: keep them local).

    ... CAPTURE.jsonl SHOTS_DIR OUT_DIR --zero F [--sheet]

The cinematic engine's: SHOTS_DIR holds screenshots named by game frame
(tools/cine_scene_drive.py --shots N), game frame F being the .cine's frame
0 (the frame phase 17 starts, plus the 30 frames of pre-roll), and a .cine's
frame is the script's (tools/ps2/ttt_to_cine.py): each shot goes beside the
capture line of script frame ("t") shot - F. --sheet: one contact sheet
(OUT_DIR/sheet.png, a pair a row) instead of the video.
"""
import json, subprocess, sys
from pathlib import Path
import numpy as np
from PIL import Image, ImageDraw


def vp_of(d):
    W = np.array(d['actors'][0][0]).reshape(4, 4); M = np.array(d['vp']).reshape(4, 4)
    inv = np.eye(4); inv[:3, :3] = W[:3, :3].T; inv[3, :3] = -W[3, :3] @ W[:3, :3].T
    return inv @ M


def main():
    cap, t3, out = Path(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3])
    start = int(sys.argv[sys.argv.index('--start') + 1]) if '--start' in sys.argv else 10
    zero = int(sys.argv[sys.argv.index('--zero') + 1]) if '--zero' in sys.argv else None
    out.mkdir(parents=True, exist_ok=True)
    lines = cap.read_text().splitlines()
    parents = json.loads(lines[0])['parents']
    frames = [d for d in map(json.loads, lines[1:]) if d.get('actors') and 'relink' not in d and not d.get('done')]
    pairs = [(t3 / f'{start + i:05d}.png', i, d) for i, d in enumerate(frames)]
    if zero is not None:                 # screenshots by game frame, capture lines by script frame
        by_t = {d['t']: d for d in frames}
        pairs = [(f, int(f.stem) - zero, by_t[int(f.stem) - zero]) for f in sorted(t3.glob('[0-9]*.png'))
                 if int(f.stem) - zero in by_t]
    made = []
    # The PS2 picture's extent in projected units, from all frames.
    xs, ys = [], []
    for d in frames[::20]:
        VP = vp_of(d)
        for a in d['actors']:
            for m in a[:98]:
                v = np.array([*m[12:15], 1.0]) @ VP
                if v[3] > 50: xs.append(v[0] / v[3]); ys.append(v[1] / v[3])
    cx, cy = np.median(xs), np.median(ys)
    for f3, i, d in pairs:
        if not f3.is_file(): continue
        left = Image.open(f3).convert('RGB')
        w, h = left.size
        right = Image.new('RGB', (w, h), (24, 24, 28)); g = ImageDraw.Draw(right)
        VP = vp_of(d)
        # The screen's centre: where the camera projects a point straight
        # ahead of it (eye from the view-projection, as ttt_retarget.camera).
        A = VP[:3][:, [0, 1, 3]].T; eye = np.linalg.solve(A, -VP[3, [0, 1, 3]])
        ahead = np.array([*(eye + 1000 * VP[:3, 3] / np.linalg.norm(VP[:3, 3])), 1.0]) @ VP
        cx, cy = ahead[0] / ahead[3], ahead[1] / ahead[3]
        for a, (actor, par) in enumerate(zip(d['actors'], parents)):
            col = (240, 240, 240) if a == 0 else (240, 80, 74)
            P = []
            for m in actor:
                v = np.array([*m[12:15], 1.0]) @ VP
                P.append(None if v[3] < 50 else (w / 2 + (v[0] / v[3] - cx) * w / 640, h / 2 + (v[1] / v[3] - cy) * h / 448))   # both screens 4:3
            for k, p in enumerate(par):
                if p < 0 or k >= 98 or P[k] is None or P[p] is None: continue
                g.line([P[k], P[p]], fill=col, width=2)
        g.text((6, 6), f'PS2 {i} (seq {d.get("s", 0)} f {d["f"]})' if zero is None else f'PS2 script frame {i}', fill=(200, 200, 200))
        both = Image.new('RGB', (2 * w, h)); both.paste(left, (0, 0)); both.paste(right, (w, 0))
        ImageDraw.Draw(both).text((6, 6), f'T3 {start + i}' if zero is None else f'T3 .cine frame {i}', fill=(255, 255, 0))
        both.save(out / f'cmp_{i:05d}.png'); made.append(both)
    if '--sheet' in sys.argv:
        w, h = made[0].size; k = 480 / h
        rows = [m.resize((int(w * k), 480)) for m in made]
        sheet = Image.new('RGB', (rows[0].width, 480 * len(rows)))
        for j, r in enumerate(rows): sheet.paste(r, (0, 480 * j))
        sheet.save(out / 'sheet.png'); print(out / 'sheet.png'); return
    subprocess.run(['ffmpeg', '-loglevel', 'error', '-y', '-framerate', '60', '-i', str(out / 'cmp_%05d.png'),
                    '-vf', 'scale=trunc(iw/2)*2:trunc(ih/2)*2', '-pix_fmt', 'yuv420p', str(out / 'compare.mp4')], check=True)
    print(out / 'compare.mp4')


if __name__ == '__main__':
    main()
