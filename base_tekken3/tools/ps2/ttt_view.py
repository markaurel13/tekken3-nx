#!/usr/bin/env python3
"""Wireframe viewer of a captured TTT (PS2) ending: the bones seen through
the game's own camera.

    python3 tools/ps2/ttt_view.py CAPTURE.jsonl OUT.html

Each frame's node positions (world matrix row 3) are projected with that
frame's view-projection (the root node's +0x40 matrix times the inverse of
its world matrix), so the page frames the scene as the PS2 did. The page
holds captured game data: keep it local. A --relink capture (the Embu: the
characters take turns in the slots) changes the skeletons on its "relink"
lines; each frame keeps the index of the skeleton set it is drawn with.
"""
import json, sys
from pathlib import Path


def inv_rigid(m):
    r = [[m[j * 4 + i] for j in range(3)] for i in range(3)]     # transpose of the 3x3
    t = m[12:15]
    it = [-sum(t[k] * r[k][j] for k in range(3)) for j in range(3)]
    return [r[0] + [0], r[1] + [0], r[2] + [0], it + [1]]


def mul(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(4)) for j in range(4)] for i in range(4)]


def main():
    src, dst = Path(sys.argv[1]), Path(sys.argv[2])
    lines = src.read_text().splitlines()
    head = json.loads(lines[0])
    sets, frames, vp = [head['parents']], [], None
    for line in lines[1:]:
        d = json.loads(line)
        if 'relink' in d:
            sets.append(d['parents']); continue
        if d['actors'][0]:                                  # slot A may be empty for a moment: keep the last camera
            root = d['actors'][0][0]
            vp = mul(inv_rigid(root), [d['vp'][i * 4:i * 4 + 4] for i in range(4)])
        if vp is None: continue
        pts = []
        for actor in d['actors']:
            for m in actor:
                x, y, z = m[12:15]
                v = [x * vp[0][c] + y * vp[1][c] + z * vp[2][c] + vp[3][c] for c in range(4)]
                pts += [round(v[0] / v[3], 1), round(v[1] / v[3], 1)] if v[3] > 1e-3 else [None, None]
        frames.append(dict(f=d['f'], s=d.get('s', 0), k=len(sets) - 1, p=pts))
    page = PAGE.replace('__DATA__', json.dumps(dict(sets=sets, frames=frames)))
    dst.write_text(page, encoding='utf-8')
    print(dst, len(frames), 'frames')


PAGE = r'''<!doctype html><html lang="fr"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1"><title>Capture TTT</title>
<style>body{margin:0;background:#111;color:#ddd;font:14px -apple-system,system-ui,sans-serif}
.wrap{max-width:900px;margin:0 auto;padding:12px}canvas{width:100%;background:#1b1b1f;border-radius:10px}
.bar{display:flex;gap:10px;align-items:center;margin-top:8px}input[type=range]{flex:1}button{font:inherit;padding:6px 12px;border-radius:8px;border:1px solid #444;background:#222;color:#ddd}
.leg span{display:inline-block;width:10px;height:10px;border-radius:2px;margin:0 4px 0 12px}</style></head><body><div class="wrap">
<canvas id="c" width="640" height="480"></canvas>
<div class="bar"><button id="play">❚❚</button><input id="t" type="range" min="0" value="0"><span id="n"></span></div>
<p class="leg">Os capturés, vus par la caméra du jeu :<span style="background:#f5f5f5"></span>emplacement A (0x87A2F0)<span style="background:#f0504a"></span>emplacement B (0x885B90)</p>
</div><script>
const D=__DATA__, c=document.getElementById('c'), g=c.getContext('2d'), t=document.getElementById('t');
t.max=D.frames.length-1; let playing=true, i=0, last=0;
function draw(){const f=D.frames[i];g.clearRect(0,0,640,480);
 const s=1, ox=320-2048, oy=240-2048*480/448;   // the PS2 screen: 640 x 448 around (2048, 2048), as in the game
 let base=0;D.sets[f.k].forEach((par,a)=>{g.strokeStyle=a?'#f0504a':'#f5f5f5';g.lineWidth=2;
  par.forEach((p,k)=>{if(p<0||k>=98||p>=98)return;   // 98+: hair, cloth, effect anchors (stretched lines), not the skeleton
  const x1=f.p[2*(base+k)],y1=f.p[2*(base+k)+1],x2=f.p[2*(base+p)],y2=f.p[2*(base+p)+1];
   if(x1===null||x2===null)return;g.beginPath();g.moveTo(ox+x1,oy+y1*480/448);g.lineTo(ox+x2,oy+y2*480/448);g.stroke()});base+=par.length});
 t.value=i;document.getElementById('n').textContent=`séquence ${f.s} · image ${f.f} · ${D.sets[f.k].map(x=>x.length).join(' / ')} nœuds`}
function loop(ts){if(playing&&ts-last>=1000/60){i=(i+1)%D.frames.length;last=ts;draw()}requestAnimationFrame(loop)}
t.oninput=()=>{i=+t.value;draw()};document.getElementById('play').onclick=e=>{playing=!playing;e.target.textContent=playing?'❚❚':'▶'};
draw();requestAnimationFrame(loop);
</script></body></html>'''

if __name__ == '__main__':
    main()
