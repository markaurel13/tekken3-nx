#!/usr/bin/env python3
"""What a TTT (PS2) cutscene contains: its script (ttt_script.py) checked
against a capture (ttt_capture.py, format 2), as one events file and a report.

    python tools/ps2/ttt_cine_report.py SCENE CAPTURE.jsonl OUT_DIR

SCENE: as for ttt_script.py (armorking, kazuya, bruce, kunimitsu, embu...).
Writes OUT_DIR/<scene>.events.json (every element with its frame: script
events, with the world position of sparks and sprites read from the
captured nodes, camera cuts, sounds, music, skeleton changes, figurants) and
OUT_DIR/<scene>.report.md (what was found, and what the capture saw that the
script does not explain). Frames are script frames ("t" of the capture).
"""
import json, math, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import ttt_script

SPECIAL_NODES = {16384: 'stage point (0x3ACE48)', 16385: 'fixed point', 16386: 'camera', 16387: 'stage point', 98: 'root'}


def solve3(A, b):
    """Cramer's rule, 3x3."""
    def det(m): return (m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
                        + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]))
    d = det(A)
    if abs(d) < 1e-12: return None
    out = []
    for k in range(3):
        m = [row[:] for row in A]
        for i in range(3): m[i][k] = b[i]
        out.append(det(m) / d)
    return out


def mat(v): return [v[0:4], v[4:8], v[8:12], v[12:16]]
def mul(a, b): return [[sum(a[i][k] * b[k][j] for k in range(4)) for j in range(4)] for i in range(4)]


def eye(line):
    """The camera position: the point the view-projection (root +0x40 matrix
    times the inverse of the root's world matrix) sends to infinity."""
    if 'view' in line:                                  # format 2: the frame's own view x projection
        VP = mat(line['view'])
        A = [[VP[r][c] for r in range(3)] for c in (0, 1, 3)]
        return solve3(A, [-VP[3][c] for c in (0, 1, 3)])
    if not line['actors'] or not line['actors'][0]: return None   # the Embu's storm, nobody on stage
    W, M = mat(line['actors'][0][0]), mat(line['vp'])
    R = [r[:3] for r in W[:3]]; T = W[3][:3]
    inv = [[R[j][i] for j in range(3)] + [0] for i in range(3)]
    inv.append([-sum(T[k] * R[i][k] for k in range(3)) for i in range(3)] + [1])
    VP = mul(inv, M)
    A = [[VP[r][c] for r in range(3)] for c in (0, 1, 3)]
    b = [-VP[3][c] for c in (0, 1, 3)]
    return solve3(A, b)


def load(capture):
    lines = [json.loads(l) for l in Path(capture).read_text().splitlines() if l.strip()]
    head, frames, relinks = lines[0], [], []
    for l in lines[1:]:
        if 'relink' not in l and 't' not in l:      # format 1: the Lorenz index wraps at 1024 with the script
            l['t'] = 1024 * l.get('s', 0) + l['f']
        l.setdefault('fade', [0, 0, 0])
        (relinks if 'relink' in l else frames).append(l)
    return head, frames, relinks


def node_pos(line, actor, node):
    a = line['actors'][actor] if actor < len(line['actors']) else []
    if node < len(a): return [round(v, 1) for v in a[node][12:15]]
    return None


def fade_level(f):
    """Captured fade as the .cine level: -256 black .. 0 .. 256 white."""
    black, white, used = (f[0] >> 24) & 0xFF, (f[1] >> 24) & 0xFF, f[2]
    if used and white > 2: return 2 * white
    return -2 * black if black > 2 else 0


def script_fades(scene):
    """The script's fades as (start, end, from level, to level)."""
    out = []
    embu = scene['scene'] == 'embu'
    for e in scene['events']:
        t, c, a, b = e['t'], e['cmd'], e['a'], e['b']
        if c == 19 and not embu: out.append((t, t + a, -256, 0))
        elif c == 20 and not embu: out.append((t, t + a, 0, -256))
        elif c == 6: out.append((t, t + a, 0, 256))
        elif c == 5 and not embu: out.append((t, t + a, 2 * b if b else 256, 0))
        elif c == 5 and embu: out.append((t, t + a, 256, 0))
        elif c == 11 and embu: out.append((t, t + a, 256, 256))
    return out


def spans(values, frames):
    """(first, last, peak) runs of nonzero values."""
    out, run = [], None
    for t, v in zip(frames, values):
        if v:
            if run is None: run = [t, t, v]
            run[1] = t
            if abs(v) > abs(run[2]): run[2] = v
        elif run: out.append(tuple(run)); run = None
    if run: out.append(tuple(run))
    return out


def main():
    name, capture, out_dir = sys.argv[1], Path(sys.argv[2]), Path(sys.argv[3])
    scene = ttt_script.find(ttt_script.scenes(ttt_script.Elf(ttt_script.ELF_DEFAULT)), {'alex': 'roger'}.get(name, name))
    head, frames, relinks = load(capture)
    by_t = {}
    for l in frames: by_t.setdefault(l.get('t', l['f']), l)
    ts = sorted(by_t)
    events, notes = [], []

    # Script events, with positions for sparks and sprites.
    for e in scene['events']:
        ev = dict(e, source='script')
        line = by_t.get(e['t'] + 1) or by_t.get(e['t'])
        if e['kind'] == 'spark':
            actor = (e['a'] & 1) if scene['scene'] == 'embu' else e['c'] & 1
            node = e['b']
            ev.update(actor=actor, node=node, effect_of=e['a'] & 1)
            if scene['scene'] == 'embu':                    # a hit point of the actor, not a node
                ev.pop('node'); ev['hit_point'] = node
                if line and 'hits' in line:
                    x, y, z = line['hits'][actor][node]; ev['position'] = [x, -y, -z]
            elif line and node not in SPECIAL_NODES: ev['position'] = node_pos(line, actor, node)
        elif e['kind'] == 'sprite':
            actor = e['a'] & 1
            ev.update(actor=actor, node=e['b'], sprite=(e['a'] >> 1) & 3 if e['cmd'] == 17 else 3,
                      target=e['c'] if e['cmd'] == 17 else None, size=e['c'] if e['cmd'] == 18 else None)
            if line and e['b'] not in SPECIAL_NODES: ev['position'] = node_pos(line, actor, e['b'])
            elif e['b'] in SPECIAL_NODES: ev['where'] = SPECIAL_NODES[e['b']]
        events.append(ev)

    # Fades: captured quads against the script.
    # The white quad keeps its last alpha without being drawn (TIPS § 28):
    # it shows only while its alpha moves, and on the frames around.
    whites = [(by_t[t]['fade'][1] >> 24) & 0xFF for t in ts]
    levels = []
    for k, t in enumerate(ts):
        f = by_t[t]['fade']
        moving = any(whites[j] != whites[k] for j in (k - 1, k + 1) if 0 <= j < len(ts))
        black = (f[0] >> 24) & 0xFF
        levels.append(2 * whites[k] if moving and f[2] and whites[k] > 2 else (-2 * black if black > 2 else 0))
    measured = spans(levels, ts)
    expected = script_fades(scene)
    fade_rows = []
    for a, b, x, y in expected:
        hit = [m for m in measured if m[0] <= b + 3 and m[1] >= a - 3]
        fade_rows.append((a, b, x, y, hit))
    unexplained = [m for m in measured if not any(m[0] <= b + 3 and m[1] >= a - 3 for a, b, _, _ in expected)]

    # Camera cuts: a jump of the eye much larger than the usual motion.
    eyes = [(t, eye(by_t[t])) for t in ts]
    # Camera cuts: an eye jump far above the motion of the frames around it
    # (a fast pan moves a lot on several frames in a row, a cut on one).
    steps = {}
    for (t0, e0), (t1, e1) in zip(eyes, eyes[1:]):
        if e0 and e1 and t1 == t0 + 1: steps[t1] = math.dist(e0, e1)
    cuts = []
    for t, d in steps.items():
        around = sorted(steps.get(t + k, 0) for k in (-3, -2, -1, 1, 2, 3))
        if d > 300 and d > 4 * around[3] + 50: cuts.append(t)
    for t in cuts: events.append(dict(t=t, kind='cut', source='capture'))
    camera_cmds = [e['t'] for e in scene['events'] if e['kind'] == 'camera']

    # Sounds: a voice whose (category, sound, side) changes starts a sound.
    sounds, prev = [], None
    for t in ts:
        v = by_t[t].get('voices')
        if not v: continue
        if prev:
            for i, (a, b) in enumerate(zip(prev, v)):
                if a[:3] != b[:3] and b[1]:
                    sounds.append(dict(t=t, kind='sound', source='capture', voice=i, category=b[0], sound=b[1], side=b[2], volume=b[3]))
        prev = v
    events += sounds
    music = [dict(t=l.get('t', l['f']), kind='music', source='capture', packet=l['music']) for l in frames if 'music' in l]
    events += music
    for r in relinks: events.append(dict(t=r.get('t', r['relink']), kind='skeleton', source='capture', nodes=r['nodes']))
    statics = [l for l in frames if l.get('statics')]
    for l in statics: events.append(dict(t=l.get('t', l['f']), kind='figurants', source='capture', count=len(l['statics'])))
    # Tasks: every engine object besides the actors, by kind (its function).
    life, alive, data, samples = [], {}, {}, {}
    for l in frames:
        t = l.get('t', l['f'])
        if 'tasks' in l:
            now = {a: f for a, f in l['tasks']}
            for a, f in list(alive.items()):
                if a not in now or now[a] != f[0]:
                    life.append(dict(kind=f[0], address=a, start=f[1], end=t - 1)); del alive[a]
            for a, f in now.items():
                if a not in alive: alive[a] = (f, t)
        for a, b in l.get('task_data', {}).items():
            data.setdefault(a, []).append((t, b))
    for a, f in alive.items(): life.append(dict(kind=f[0], address=a, start=f[1], end=None))
    for e in life:
        d = [b for t, b in data.get(e['address'], []) if t >= e['start'] and (e['end'] is None or t <= e['end'])]
        if d:
            raw = bytes.fromhex(d[0])
            e['params'] = list(struct.unpack_from('<hhi', raw, 24))       # +24, +26, +28
            e['changes'] = len(d)
        events.append(dict(t=e['start'], kind='task', source='capture', task=e['kind'], end=e['end'],
                           params=e.get('params'), changes=e.get('changes', 0)))
    events.sort(key=lambda e: (e['t'], e['source']))

    out_dir.mkdir(parents=True, exist_ok=True)
    key = name                                          # alex: Roger's scene, its own report
    (out_dir / f'{key}.events.json').write_text(json.dumps(dict(scene=scene['scene'], key=key, stage=scene['stage'],
        length=scene['length'], actors=scene['actors'], capture=str(capture.name), header=head, events=events), indent=1))

    # Report.
    missing = [t for t in range(scene['length']) if t not in by_t]
    R = [f"# {key} : scène {scene['scene']}, stage {scene['stage']}, {scene['length']} images", '',
         'Acteurs (liste du script) : ' + ', '.join(f"{a['entry']} = {a['key']} (costume {a['costume']})" for a in scene['actors'] or []), '',
         f"Capture `{capture.name}` : {len(frames)} images, horloge du script {ts[0] if ts else '-'}..{ts[-1] if ts else '-'}, "
         f"{len(missing)} images du script manquantes" + (f" (premières : {missing[:8]})" if missing else '') + '.', '']
    R += ['## Événements du script', '', '| image | cmd | genre | a | b | c | détail |', '|---|---|---|---|---|---|---|']
    table = ttt_script.EMBU_COMMANDS if scene['scene'] == 'embu' else ttt_script.ENDING_COMMANDS
    for e in [e for e in events if e['source'] == 'script']:
        extra = ''
        if 'position' in e: extra = f"acteur {e['actor']} {'point de frappe ' + str(e['hit_point']) if 'hit_point' in e else 'nœud ' + str(e['node'])} en {e['position']}"
        elif 'where' in e: extra = e['where']
        R.append(f"| {e['t']} | {e['cmd']} | {e['kind']} | {e['a']} | {e['b']} | {e['c']} | {extra or table.get(e['cmd'], ('', ''))[1]} |")
    R += ['', '## Fondus : script contre carrés GS capturés', '', '| script | de → à | vu dans la capture |', '|---|---|---|']
    for a, b, x, y, hit in fade_rows:
        R.append(f"| {a}-{b} | {x} → {y} | {', '.join(f'{m[0]}-{m[1]} (pic {m[2]})' for m in hit) or '**non vu**'} |")
    if unexplained:
        R += ['', 'Fondus capturés sans commande du script : ' + ', '.join(f'{m[0]}-{m[1]} (pic {m[2]})' for m in unexplained)]
    R += ['', '## Caméra', '', f"Commandes caméra du script : {camera_cmds}.",
          f"Coupes vues (saut de l'œil > 400) : {cuts or 'aucune'}."]
    R += ['', '## Sons', '']
    if sounds:
        R += ['| image | voix | catégorie | son | côté | volume |', '|---|---|---|---|---|---|']
        R += [f"| {s['t']} | {s['voice']} | {s['category']} | {s['sound']} | {s['side']} | {s['volume']} |" for s in sounds]
    else:
        R.append('Aucun départ de son dans la table des voix.')
    R += ['', f"Paquets musique : {len(music)} " + (', '.join(f"{m['t']}:{m['packet']}" for m in music[:12]) if music else '')]
    first = ts[0] if ts else 0
    TASK_NAMES = {'0x3ac538': 'fondu (script 5/6/19/20)', '0x3ac728': 'fondu (script 21)', '0x3aca70': 'accessoire (script 25)',
                  '0x3ac818': 'écran (script 22)', '0x3ac8e0': 'écran (script 23)', '0x3aca18': 'écran (script 24)',
                  '0x3ac4c0': 'tâche (script 15)', '0x3aa420': 'écran (script 29)', '0x3ad140': 'écran (script 30)',
                  '0x3aa378': 'écran (script 32)'}
    from collections import defaultdict
    kinds = defaultdict(list)
    for e in life: kinds[e['kind']].append(e)
    R += ['', '## Éléments du moteur (tâches)', '',
          'Tout ce qui est dessiné en plus des acteurs. Présent dès le début et jamais fini = décor ou ambiance.', '',
          '| genre (fonction) | nom | nombre | vies (début-fin, paramètres +24 +26 +28) |', '|---|---|---|---|']
    for k, es in sorted(kinds.items(), key=lambda kv: min(e['start'] for e in kv[1])):
        static = all(e['start'] <= first and e['end'] is None for e in es)
        groups = defaultdict(list)                              # a burst = objects born on the same frame
        for e in es: groups[e['start']].append(e)
        parts = []
        for st, g in sorted(groups.items()):
            ends = [e['end'] for e in g if e['end'] is not None]
            span = f"{st}-{max(ends) if ends else 'fin'}"
            pr = g[0].get('params')
            parts.append(f"{span}" + (f" ×{len(g)}" if len(g) > 1 else '') + (f" {pr}" if pr and len(g) == 1 else ''))
        spans_ = ', '.join(parts[:16]) + (f' … (+{len(parts) - 16})' if len(parts) > 16 else '')
        R.append(f"| {k} | {TASK_NAMES.get(k, 'décor / ambiance' if static else '?')} | {len(es)} | {'toute la scène' if static else spans_} |")
    idents = [(l.get('t', l['f']), l['fighters'], l['texanim']) for l in frames if 'fighters' in l]
    R += ['', '## Personnages et textures (Devil)', '']
    for t, fi, ta in idents:
        who = ', '.join(f"acteur {i} = {ttt_script.CHARACTERS.get(f['character'], f['character'])} c{f['costume']} (modèle {f['slot']}{', état ' + str(f['state']) if f.get('state') else ''})" for i, f in enumerate(fi))
        anim = ', '.join(f"modèle {i // 2} : état {v[0]} image {v[1]}/{v[2]}" for i, v in enumerate(ta) if v[0] or v[2])
        R.append(f"- {t} : {who}" + (f" ; textures animées : {anim}" if anim else ''))
    R += ['', '## Squelettes et figurants', '',
          'Changements de squelette : ' + (', '.join(f"{r.get('t', r['relink'])} → {r['nodes']}" for r in relinks) or 'aucun'),
          'Figurants (quand ils apparaissent ou bougent) : ' + (', '.join(f"{l.get('t', l['f'])} ({len(l['statics'])})" for l in statics[:20]) or 'aucun')
          + (f' … ({len(statics)} changements)' if len(statics) > 20 else '')]
    (out_dir / f'{key}.report.md').write_text('\n'.join(R) + '\n', encoding='utf-8')
    print(out_dir / f'{key}.report.md')


if __name__ == '__main__':
    main()
