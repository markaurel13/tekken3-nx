#!/usr/bin/env python3
"""Check retargets against their capture: an HTML page drawing, frame by
frame through the retarget's camera, each actor's captured PS2 body (grey,
moved into the retarget's frame of reference by its basis, scale and
origin) under its TTT1 skeleton (coloured) as Tekken 3 would pose it.

    venv-import python tools/ps2/ttt_retarget_view.py CAPTURE.jsonl OUT.html \\
        RETARGET.json=MODEL.3dm [RETARGET.json=MODEL.3dm ...]

A test without a build of the game: the bones only, no model, no head
check (the head basis is the retarget's own). The page holds captured game
data: keep it local (workspace/).
"""
import json, sys
from pathlib import Path
import numpy as np
sys.path.insert(0, str(Path(__file__).resolve().parent))
import ttt_retarget as R

BODY = 98                                   # PS2 nodes shared by every ending's skeleton (Kazuya's numbering)


def rows(sk, joints):
    """World point of every TTT1 row, the hips at 0."""
    local = [sk.rotation(joints[3 * b:3 * b + 3]) for b in range(18)]
    local[0][1:, :] *= -1
    world = {0: (local[0], np.zeros(3))}
    def get(r):
        if r not in world:
            Rp, T = get(R.AM.ROW_PARENT[r])
            world[r] = (Rp @ local[sk.bone[r]], T + Rp @ sk.off[r])
        return world[r]
    for r in R.AM.ROW_PARENT: get(r)
    return {r: world[r][1] for r in world}


def main():
    cap, out = Path(sys.argv[1]), Path(sys.argv[2])
    lines = cap.read_text().splitlines()
    parents = json.loads(lines[0])['parents']
    frames = [json.loads(l) for l in lines[1:] if '"relink"' not in l[:12]]
    actors, cams = [], None
    for spec in sys.argv[3:]:
        path, model = spec.split('=')
        r = json.loads(Path(path).read_text())
        sk = R.Skeleton(Path(model))
        c = r['calibration']; C, scale, origin = np.array(c['C']), c['scale'], np.array(c['origin'])
        order = sorted(R.AM.ROW_PARENT) if 0 in R.AM.ROW_PARENT else [0] + sorted(R.AM.ROW_PARENT)
        edges = [[order.index(k), order.index(p)] for k, p in R.AM.ROW_PARENT.items() if p in order and k != p]
        ttt, ps2 = [], []
        for d, p in zip(frames, r['poses']):
            heading, height, distance = R.s16(p[0]) * R.K, R.s16(p[1]), p[2]
            root = np.array([np.sin(heading) * distance, -height, np.cos(heading) * distance])
            pts = rows(sk, np.array([R.s16(v) for v in p[3:]], dtype=float))
            ttt.append([round(float(v)) for k in order for v in root + pts[k]])
            a = d['actors'][r['actor']]
            ps2.append([round(float(v)) for n in a[:BODY] for v in scale * (C @ (np.array(n[12:15]) - origin))])
        actors.append(dict(name=r['guest'], edges=edges, ps2edges=[[i, q] for i, q in enumerate(parents[r['actor']][:BODY]) if q >= 0],
                           ttt=ttt, ps2=ps2))
        cams = cams or r['camera']
    keys = [f"{d.get('s', 0)}:{d['f']}" for d in frames]
    data = json.dumps(dict(keys=keys, cams=cams, actors=actors), separators=(',', ':'))
    out.write_text(PAGE.replace('/*DATA*/', data), encoding='utf-8')
    print(out, len(frames), 'frames')


PAGE = """<!doctype html><meta charset="utf-8"><title>Retarget check</title>
<style>body{background:#111;color:#ddd;font:14px sans-serif;margin:12px}canvas{background:#000;display:block;max-width:100%}
#bar{display:flex;gap:10px;align-items:center;margin:8px 0}input[type=range]{flex:1}</style>
<canvas id=c width=960 height=720></canvas>
<div id=bar><button id=play>Play</button><input id=t type=range min=0 value=0><span id=k></span>
<label>FOV <input id=fov type=number value=50 min=10 max=120 style="width:4em"></label></div>
<div id=legend></div>
<script>
const D=/*DATA*/;const cv=document.getElementById('c'),g=cv.getContext('2d'),t=document.getElementById('t');
t.max=D.keys.length-1;const COL=['#ff6a3d','#3dc8ff','#9cff3d'];
document.getElementById('legend').innerHTML=D.actors.map((a,i)=>`<span style="color:${COL[i]}">${a.name} (TTT1)</span>`).join(' &middot; ')+' &middot; <span style="color:#888">grey: PS2 capture</span>';
function sub(a,b){return[a[0]-b[0],a[1]-b[1],a[2]-b[2]]}function cr(a,b){return[a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]]}
function nz(a){const l=Math.hypot(...a)||1;return a.map(v=>v/l)}function dt(a,b){return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]}
function draw(i){const c=D.cams[i],e=c.slice(0,3),f=nz(sub(c.slice(3,6),e));let r=nz(cr(f,[0,-1,0])),u=cr(r,f);
const F=cv.height/2/Math.tan(document.getElementById('fov').value*Math.PI/360);
const P=p=>{const q=sub(p,e),z=dt(q,f);return z<10?null:[cv.width/2+F*dt(q,r)/z,cv.height/2-F*dt(q,u)/z]};
g.fillStyle='#000';g.fillRect(0,0,cv.width,cv.height);
D.actors.forEach((a,ai)=>{[[a.ps2,a.ps2edges,'#777',1.5],[a.ttt,a.edges,COL[ai],3]].forEach(([pts,ed,col,w])=>{
const v=pts[i],pt=k=>P([v[3*k],v[3*k+1],v[3*k+2]]);g.strokeStyle=col;g.lineWidth=w;g.beginPath();
ed.forEach(([x,y])=>{const A=pt(x),B=pt(y);if(A&&B){g.moveTo(...A);g.lineTo(...B)}});g.stroke()})});
document.getElementById('k').textContent=D.keys[i]}
t.oninput=()=>draw(+t.value);document.getElementById('fov').oninput=()=>draw(+t.value);let run=null;
document.getElementById('play').onclick=e=>{if(run){clearInterval(run);run=null;e.target.textContent='Play';return}
e.target.textContent='Pause';run=setInterval(()=>{t.value=(+t.value+1)%D.keys.length;draw(+t.value)},1000/60)};draw(0);
</script>"""

if __name__ == '__main__':
    main()
