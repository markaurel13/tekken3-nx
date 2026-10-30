#!/usr/bin/env python3
"""Fit a converted TTT (PS2) ending into a small Tekken 3 arena: each shot
moved to the arena's centre, the camera brought nearer in the wide shots.

    python3 tools/ps2/ttt_recentre.py MAIN.json OUT_MAIN.json [OTHER.json OUT_OTHER.json ...] [--max-camera 5000] [--face DX,DZ] [--face-shots 0,1,...] [--shot K:DX,DZ[:MAXCAM];...]

MAIN.json and the others: tools/ps2/ttt_retarget.py (or ttt_body_actor.py)
files of one ending. The PS2's temples are far larger than a Tekken 3 arena
(Bruce's: the camera up to 16000 units from the centre, Bruce himself up to
11700): out of the arena nothing is drawn behind them and the screen keeps
the previous frames. A shot (cut where the main actor's root jumps by more
than 500 or the sequence changes) is moved as a whole, actors and camera,
by its main actor's mean position, so the framing does not change. Then a
camera eye further than --max-camera from the arena's centre slides along
its line of sight towards its target until it is that far (or as near the
target as 200): same angle, a closer shot; the actors never move for the
camera (Omar's rule for Kazuya's ending).

--face DX,DZ also turns each shot as a whole about the arena's centre so
that its camera looks, on average, along (DX, DZ): towards the part of the
arena that has scenery (Heihachi's: the steps the fight camera looks at),
not into the black where the arena ends. Actors turn with it: their roots,
and their hips' rotation (bone 0, as tools/ps2/ttt_body_actor.py does).
--face-shots: only these shots (from 0) turn. --shot sets one shot's own
direction and camera limit (Bruce on Heihachi's arena, measured as the
scenery's share of the screen: "0:1,0:2500;1:0,-1;2:0,-1;3:0,1;4:0,1;5:0,1").
"""
import json, math, sys
from pathlib import Path
import numpy as np
sys.path.insert(0, str(Path(__file__).resolve().parent))
import ttt_retarget as R

K = 2 * math.pi / 65536
s16 = lambda v: v - 65536 if v > 32767 else v


def root_xz(p):
    return math.sin(s16(p[0]) * K) * s16(p[2]), math.cos(s16(p[0]) * K) * s16(p[2])


def set_root(p, x, z):
    p[0] = int(round(math.atan2(x, z) / K)) & 0xffff
    p[2] = int(round(math.hypot(x, z))) & 0xffff


def main():
    args = [a for a in sys.argv[1:]]
    limit = 5000.0
    if '--max-camera' in args:
        i = args.index('--max-camera'); limit = float(args[i + 1]); del args[i:i + 2]
    face = None
    if '--face' in args:
        i = args.index('--face'); face = [float(v) for v in args[i + 1].split(',')]; del args[i:i + 2]
    per = {}
    if '--shot' in args:
        i = args.index('--shot')
        for item in args[i + 1].split(';'):
            k, d, *m = item.split(':'); per[int(k)] = ([float(v) for v in d.split(',')], float(m[0]) if m else None)
        del args[i:i + 2]
    only = None
    if '--face-shots' in args:
        i = args.index('--face-shots'); only = {int(v) for v in args[i + 1].split(',')}; del args[i:i + 2]
    pairs = list(zip(args[0::2], args[1::2]))
    files = [json.loads(Path(src).read_text()) for src, _ in pairs]
    main_r = files[0]
    pos = [root_xz(p) for p in main_r['poses']]
    seq = main_r['sequences']
    n = len(pos)
    cuts = [0] + [i for i in range(1, n) if math.dist(pos[i], pos[i - 1]) > 500 or seq[i] != seq[i - 1]]
    shift = [None] * n
    for a, b in zip(cuts, cuts[1:] + [n]):
        cx = sum(x for x, _ in pos[a:b]) / (b - a); cz = sum(z for _, z in pos[a:b]) / (b - a)
        for i in range(a, b): shift[i] = (cx, cz)
    # Each shot's turn (about the vertical, after the move to the centre).
    turn, lim = [0.0] * n, [limit] * n
    for k, (a, b) in enumerate(zip(cuts, cuts[1:] + [n])):
        d = per[k][0] if k in per else face if face and (only is None or k in only) else None
        if k in per and per[k][1]: lim[a:b] = [per[k][1]] * (b - a)
        if not d: continue
        fx = sum(c[3] - c[0] for c in main_r['camera'][a:b]); fz = sum(c[5] - c[2] for c in main_r['camera'][a:b])
        for i in range(a, b): turn[i] = math.atan2(d[0], d[1]) - math.atan2(fx, fz)
    def rot(x, z, ang):                                       # x' = x cos + z sin, z' = -x sin + z cos: heading + ang
        return x * math.cos(ang) + z * math.sin(ang), -x * math.sin(ang) + z * math.cos(ang)
    N = np.diag([1.0, -1.0, -1.0])
    for r in files:
        for i, p in enumerate(r['poses']):
            if s16(p[2]) >= 20000: continue                    # a body kept out of sight (ttt_body_actor.py)
            x, z = root_xz(p); x, z = rot(x - shift[i][0], z - shift[i][1], turn[i]); set_root(p, x, z)
            if turn[i]:
                a = turn[i]; G = np.array([[math.cos(a), 0, math.sin(a)], [0, 1, 0], [-math.sin(a), 0, math.cos(a)]])
                hips = R.euler(N @ G @ N @ R.Skeleton.rotation([s16(v) for v in p[3:6]]))
                p[3:6] = [int(round(v)) & 0xffff for v in hips]
    moved = 0
    for i, c in enumerate(main_r['camera']):
        sx, sz = shift[i]
        ex, ez = rot(c[0] - sx, c[2] - sz, turn[i]); tx, tz = rot(c[3] - sx, c[5] - sz, turn[i])
        e = [ex, c[1], ez]; t = [tx, c[4], tz]
        d = math.hypot(e[0], e[2]); limit = lim[i]
        if d > limit:
            v = [e[k] - t[k] for k in range(3)]; L = math.sqrt(sum(x * x for x in v)) or 1.0
            lo, hi = 0.0, 1.0                                    # eye = t + s v, the largest s with |eye_xz| <= limit
            for _ in range(40):
                mid = (lo + hi) / 2
                if math.hypot(t[0] + mid * v[0], t[2] + mid * v[2]) <= limit: lo = mid
                else: hi = mid
            s = max(lo, min(1.0, 200.0 / L))
            e = [t[k] + s * v[k] for k in range(3)]; moved += 1
        main_r['camera'][i] = [round(v) for v in e + t]
    for r in files[1:]: r['camera'] = main_r['camera']
    for (src, dst), r in zip(pairs, files):
        r['recentred'] = dict(cuts=cuts, max_camera=limit, face=face)
        Path(dst).write_text(json.dumps(r))
    far = max(math.hypot(c[0], c[2]) for c in main_r['camera'])
    print(f'{len(cuts)} shots from {cuts}; {moved} frames with the camera brought nearer; camera now within {far:.0f}')


if __name__ == '__main__':
    main()
