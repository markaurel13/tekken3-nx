#!/usr/bin/env python3
"""Capture a Tekken Tag Tournament (PS2) in-engine cutscene, frame by frame.

    python tools/ps2/ttt_capture.py OUT.jsonl [seconds] [--actors A,B] [--relink]

Reads this worktree's TTT PCSX2 instance through PINE (tools/ps2/pine.py
ttt_slot()). Run PCSX2 slowed (inis/PCSX2.ini [Framerate] NominalScalar < 1)
so every game frame is read. Format of the output: tools/ps2/ttt_capture_format.md.

What happens in a cutscene comes from its script (tools/ps2/ttt_script.py:
models, motions, camera, fades, sparks, effect sprites, devil textures); the
capture records what the script cannot give, each frame, in one PINE message
(Pine.many):
- each actor: nodes of 0xC0 bytes, world matrix at +0x00 (4x4 floats, row
  vectors, translation in the last row, y up, ~mm), world x view x
  projection at +0x40, parent node pointer at +0xB0; node 0 is the root.
  Actor A at 0x0087A2F0, actor B at 0x00885B90 (the two fighter objects,
  0x00879540 + 0xB8A0 * i, nodes at +0xDB0). Whenever a slot's skeleton
  changes (node count or tree: the Embu's characters taking turns, Angel's
  Devil becoming Kazuya), a {"relink": ...} line comes before the next frame;
- "t": the script clock (endings 0x005F6C3C, the Embu 0x005F6C22), the frame
  number the script's events use; "sp": the next script record. The old
  "s"/"f" (sequence, 0x00447F24) are kept: that counter is the index of the
  storm's Lorenz attractor (0x003D40D8), which wraps at 1024;
- "fade": the black and white full-screen GS quads (0x0043FEA8, 0x0043FEF0;
  alpha in the top byte, 0x80 = opaque) and the word 0x0043FEEC;
- "voices": the 22 SPU voices of the sound effects (0x008A7820, 8 bytes each:
  category, sound, side, volume; 0x003749D8 writes them when a sound starts);
- "music": the music/stream command packet (0x00436400, sent to the IOP once a
  frame by 0x00399FA0, ended by 0xFF), when it changes;
- "view": the frame's view x projection (0x004333C0), the camera even with
  nobody on stage (the Embu's storm); "hits": each fighter's 12 hit points
  (+0x668, 80 bytes apart: x, y, z integers, y and z negated), where the
  Embu's sparks go;
- "tasks" / "task_data": the engine's active tasks, everything drawn besides
  the actors (fades, screen filters, props, sparks, effect sprites, the
  stage's moving parts), as changes (see Tasks);
- "fighters" (character, costume, model slot, entry) and "texanim" (texture
  animation state, frame, count per model slot: the devil textures), when
  they change: model swaps and devil on/off as elements;
- "statics": the cutscene's figurants (Bruce's beaten soldiers: 0x90-byte
  records at 0x01FBF590, tools/ps2/ttt_static_bodies.py), when they change.
--after-load: wait for a disc load before the scene (launched from the
Theater grid, tools/ps2/ttt_capture_theater.py).
--length N: the script's length (ttt_script.py): a disc load before it is
mid-scene (Michelle's, Baek's endings), not the end.
--save-at T: also save state 9 when the script reaches frame T.
--load N: load save state N first, then capture the scene it starts (the
states taken as an ending starts: 4 Armor King, 3 Bruce...).
--actors A,B: other actor addresses (ttt_find_actors.py on a save state).
--relink: start with actor A alone (the TTT Embu: slot B sometimes empty).
--one-sequence stops when the old counter wraps (the Embu captured in parts,
tools/ps2/ttt_embu_capture.py); --window S:F-S:F, --every N:K and
--start-after F select frames by that counter, as before.
"""
import json, struct, sys, time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pine import Pine, OP, ttt_slot

COUNTER = 0x447F24
# CD loader status word (0x00398078; named the "Theater scene state" on
# 2026-10-03): 0x40 on the Theater grid, 0x40/0x41 while LOADING (bit 0 flips
# with the disc), 0 (a 0x10 blip) once the scene plays, back to 0x40 at the end.
SCENE = 0x5F6B60
ONE_SEQUENCE = False
WINDOW, EVERY = ((0, 0), (99, 99999)), (1, 0)
START_AFTER = 0
SAVE_AT = -1                                          # --save-at T: save state 9 at script frame T
LENGTH = 0                                            # --length: the script's length, the true end
EMBU = False                                          # --relink: the Embu, its own player and clock
ACTORS = [0x87A2F0, 0x885B90]
FADE = (0x43FEA8, 0x43FEF0, 0x43FEEC)
VOICES, VOICE_COUNT = 0x8A7820, 22
MUSIC, MUSIC_BYTES = 0x436400, 48
# Figurants, two groups of 0x90-byte records (0x003DB4D8, table 0x453EC8):
# A at 0x01FBF590 (Bruce's beaten bodies), B at 0x01FC0C10 (Tetsujin's 28
# Mokujin). +0x48 pose block (0 = unused), +0x6C animation frame (low half,
# the script frame) and variant (high half), +0x70 type.
STATICS, STATIC_SIZE, STATIC_COUNT = 0x1FBF590, 0x90, 30
STATICS_B = 0x1FC0C10
# The engine's tasks (0x003464A0): 512 objects of 352 bytes in a pool (pointer
# gp-29392), active ones in a ring from 0x008BF510 (+0 previous, +4 next).
# Everything drawn besides the actors is one: fades (0x3AC538), screen filters,
# props (0x3ACA70), sparks and effect sprites (created by 0x3A2B38 / 0x3A26D0),
# the stage's animated parts. +8 its function (its kind), +20 flags, +24/+26/+28
# parameters, then its own data.
TASK_HEAD, TASK_POOL_PTR, TASK_COUNT, TASK_SIZE = 0x8BF510, 0x5F5D20, 512, 352
# Each fighter's identity (0x0035C878): +32 model slot, +34 entry, +38 character
# * 8 + costume, +40, +42 character, +44. Texture animations (0x003A8640, the
# devil textures of script command 12): 0x008D5D20 + 32 * model slot + 16 * k,
# {u16 state, +4 frame << 8, +12 frame count}.
TEXANIM = 0x8D5D20
VIEW = 0x4333C0                                       # the frame's view x projection (row vectors)
FIGHTERS, FIGHTER_SIZE, HITS, HIT_SIZE, HIT_COUNT = 0x879540, 0xB8A0, 0x668, 80, 12
# 64-bit reads around the script players' words (gp-relative: gp = 0x005FCFF0).
CLOCKS = (0x5F6C18, 0x5F6C20, 0x5F6C30, 0x5F6C38, 0x5F6C40)
EXTRA = list(CLOCKS) + [0x43FEA8, 0x43FEF0, 0x43FEE8] + [VOICES + 8 * i for i in range(VOICE_COUNT)] \
    + [MUSIC + 8 * i for i in range(MUSIC_BYTES // 8)] \
    + [STATICS + 8 * i for i in range(STATIC_SIZE * STATIC_COUNT // 8)]     + [STATICS_B + 8 * i for i in range(STATIC_SIZE * STATIC_COUNT // 8)]     + [VIEW + 8 * i for i in range(8)]     + [FIGHTERS + FIGHTER_SIZE * a + HITS + HIT_SIZE * h + k for a in range(2) for h in range(HIT_COUNT) for k in (0, 8)]     + [FIGHTERS + FIGHTER_SIZE * a + k for a in range(2) for k in (32, 40)] + [TEXANIM + 8 * i for i in range(16)]     + [FIGHTERS + FIGHTER_SIZE * a + 0xB050 for a in range(2)]


def lo(w): return w & 0xFFFFFFFF
def hi(w): return w >> 32
def s16(v): return v - 0x10000 if v & 0x8000 else v


def parse_extra(w):
    """The words after the actors: clocks, fades, voices, music, statics."""
    sp2, embu, sp, clock, done = hi(w[0]), w[1], hi(w[2]), hi(w[3]), w[4] & 0xFFFF
    out = dict(t=clock, sp=sp, done=done)
    if EMBU:                                               # the Embu's player (its words stay set afterwards)
        out.update(t=s16((embu >> 16) & 0xFFFF), sp=sp2, embu_state=embu & 0xFFFF)
    out['fade'] = [lo(w[5]), lo(w[6]), hi(w[7])]
    v = w[8:8 + VOICE_COUNT]
    out['voices'] = [[(x >> k) & 0xFFFF for k in (0, 16, 32, 48)] for x in v]
    m = b''.join(struct.pack('<Q', x) for x in w[8 + VOICE_COUNT:8 + VOICE_COUNT + MUSIC_BYTES // 8])
    out['music'] = m[:m.index(0xFF)].hex() if 0xFF in m else m.hex()
    k = 8 + VOICE_COUNT + MUSIC_BYTES // 8
    out['statics_raw'] = b''.join(struct.pack('<Q', x) for x in w[k:k + STATIC_SIZE * STATIC_COUNT // 4])
    k += STATIC_SIZE * STATIC_COUNT // 4
    out['view'] = list(struct.unpack('<16f', struct.pack('<8Q', *w[k:k + 8]))); k += 8
    hits = struct.unpack(f'<{4 * 2 * HIT_COUNT}i', struct.pack(f'<{2 * HIT_COUNT * 2}Q', *w[k:k + 4 * HIT_COUNT]))
    out['hits'] = [[list(hits[4 * (HIT_COUNT * a + h):4 * (HIT_COUNT * a + h) + 3]) for h in range(HIT_COUNT)] for a in range(2)]
    k += 4 * HIT_COUNT
    who = [struct.unpack('<8h', struct.pack('<2Q', *w[k + 2 * a:k + 2 * a + 2])) for a in range(2)]; k += 4
    out['fighters'] = [dict(slot=h[0], entry=h[1], model=h[3], character=h[5], costume=h[3] & 7) for h in who]
    ta = b''.join(struct.pack('<Q', x) for x in w[k:k + 16])
    out['texanim'] = [[v[0], v[2] >> 8, v[4]] for v in (struct.unpack_from('<HhiII', ta, 16 * i) for i in range(8))]
    k += 16
    for a in range(2):                                 # +0xB052: script command 28 (Devil's wings, True Ogre)
        out['fighters'][a]['state'] = (w[k + a] >> 16) & 0xFFFF
    return out


def statics(raw):
    """The figurants in use: index, world matrix, pose block, base, angle, frame words."""
    out = []
    for i in range(2 * STATIC_COUNT):                  # group A 0..29, group B 30..59
        rec = raw[i * STATIC_SIZE:(i + 1) * STATIC_SIZE]
        block = struct.unpack_from('<I', rec, 0x48)[0]
        if not block: continue
        out.append(dict(i=i, m=list(struct.unpack_from('<16f', rec, 0)),
                        words=list(struct.unpack_from('<20I', rec, 0x40))))
    return out


def script_clock(p):
    """The running script's clock (the Embu's when its player is active)."""
    w = p.many(list(CLOCKS))
    return parse_extra(w + [0] * (len(EXTRA) - len(CLOCKS)))['t']


def fast_nodes(p, base, limit=200):
    """Node addresses of the skeleton at base, read in one PINE message."""
    n = 0
    for w in p.many([base + i * 0xC0 + 56 for i in range(limit)]):
        if w >> 32 != 0x3F800000: break
        n += 1
    return [base + i * 0xC0 for i in range(n)]


def read(p, actors):
    """One frame: every node's matrix and parent, the root's +0x40 matrix
    (view-projection), the checks that the skeletons did not change, and the
    extra words, in one round trip."""
    addrs = [o + k for a in actors for o in a for k in list(range(0, 64, 8)) + [0xB0]]
    checks = [(a[-1] if a else b) + 56 for a, b in zip(actors, ACTORS)] + [a[-1] + 0xC0 + 56 for a in actors if a]
    vp = [ACTORS[0] + 0x40 + k for k in range(0, 64, 8)]
    w = p.many(addrs + checks + vp + EXTRA)
    mats, pars, i = [], [], 0
    for a, b in zip(actors, ACTORS):
        m, q = [], []
        for o in a:
            m.append(list(struct.unpack('<16f', struct.pack('<8Q', *w[i:i + 8]))))
            ptr = w[i + 8] & 0xFFFFFFFF
            q.append((ptr - b) // 0xC0 if b <= ptr < b + len(a) * 0xC0 else -1)
            i += 9
        mats.append(m); pars.append(q)
    c = w[i:i + len(checks)]; i += len(checks)
    ok = all(((c[k] >> 32) == 0x3F800000) == bool(a) for k, a in enumerate(actors))
    ok = ok and all((x >> 32) != 0x3F800000 for x in c[len(actors):])
    vpm = list(struct.unpack('<16f', struct.pack('<8Q', *w[i:i + 8]))); i += 8
    return mats, pars, ok, vpm, parse_extra(w[i:])


def header(p):
    """Which scene: the Theater selection (gp-28296, ttt_script.py's selector
    index 8 * character + variant), the stage (gp-29754) and the scene file
    loaded in the first CD record (0x008BCC40: file number, address)."""
    sel, stage, rec = p.many([0x5F6168, 0x5F5BB0, 0x8BCC40])
    return dict(selection=s16(sel & 0xFFFF), stage=(stage >> 48) & 0xFFFF,
                scene_file=lo(rec), scene_address=hex(hi(rec) & 0x1FFFFFFF))


def save_state(p, copy):
    """Save state 9 of this worktree's data (its scratch slot), copied to copy."""
    import shutil
    state = Path(__file__).resolve().parents[2] / 'workspace/ttt-ps2/pcsx2-data/PCSX2/sstates/SLUS-20001 (E84C9242).09.p2s'
    before = state.stat().st_mtime if state.exists() else 0
    p.call(bytes([OP['save'], 9]))
    for _ in range(100):
        time.sleep(.2)
        if state.exists() and state.stat().st_mtime != before:
            time.sleep(1); break
    shutil.copyfile(state, copy)
    return copy


def dump_scene(p, out):
    """The scene's data file as the game holds it (decrypted and unpacked by
    the IOP; on the disc it is not readable as is): OUT.scene.bin, for
    ttt_scene.py. CD record 0x008BCC40: file number, address, sector, size.
    Read from a save state in slot 9 of this worktree's data (seconds; word by
    word through PINE it took two minutes)."""
    import zipfile
    rec = p.many([0x8BCC40, 0x8BCC48])
    addr, size = hi(rec[0]) & 0x1FFFFFFF, hi(rec[1])
    if not (0x100000 <= addr < 0x2000000 and 0 < size < 0x800000): return
    ee = zipfile.ZipFile(save_state(p, out.with_suffix('.end.p2s'))).read('eeMemory.bin')
    path = out.with_suffix('.scene.bin')
    path.write_bytes(ee[addr:addr + size])
    print(f'scene file {lo(rec[0]):#x}: {size} bytes -> {path}', flush=True)


class Tasks:
    """The active tasks each frame, written as changes only: "tasks" (address,
    function) when the set changes, "task_data" {address: 352 bytes hex} for
    the tasks whose bytes changed."""
    def __init__(self, p):
        self.p = p
        self.pool = lo(p.many([TASK_POOL_PTR])[0])
        self.last_set, self.last_data = None, {}

    def frame(self, line):
        p, pool = self.p, self.pool
        w = p.many([TASK_HEAD] + [pool + TASK_SIZE * i + k for i in range(TASK_COUNT) for k in (0, 8)])
        nxt = {pool + TASK_SIZE * i: hi(w[1 + 2 * i]) for i in range(TASK_COUNT)}
        fn = {pool + TASK_SIZE * i: lo(w[2 + 2 * i]) for i in range(TASK_COUNT)}
        active, a = [], hi(w[0])
        while a != TASK_HEAD and a in nxt and len(active) < TASK_COUNT:
            active.append(a); a = nxt[a]
        task_set = [[hex(a), hex(fn[a])] for a in active]
        if task_set != self.last_set: line['tasks'] = self.last_set = task_set
        if not active: self.last_data = {}; return
        words = p.many([a + k for a in active for k in range(0, TASK_SIZE, 8)])
        n, changed, data = TASK_SIZE // 8, {}, {}
        for i, a in enumerate(active):
            b = b''.join(struct.pack('<Q', x) for x in words[i * n:(i + 1) * n]).hex()
            data[a] = b
            if self.last_data.get(a) != b: changed[hex(a)] = b
        self.last_data = data
        if changed: line['task_data'] = changed


def capture(p, out, t0, span, actors, relink):
    tasks = Tasks(p)
    with out.open('w') as f:
        mats, pars, _, _, _ = read(p, actors)
        f.write(json.dumps(dict(version=2, actors=[hex(b) for b in ACTORS], parents=pars, **header(p))) + '\n')
        last, got, missed, seq, idle, last_t = -1, 0, 0, 0, time.monotonic(), 0
        scene, theater, last_music, last_statics, last_ident = 0, None, None, None, None
        quiet = 600 if relink else 5                     # the Embu loads between its parts
        while time.monotonic() - t0 < span:
            c = p.r32(COUNTER); s = p.r32(SCENE)
            if s != scene: print(f'scene word {s:#x} at frame {c}', flush=True); scene = s
            if s & 0x40:                                      # the Theater, or a disc load mid-scene (Michelle's
                if theater is None: theater = time.monotonic()  # ending loads at frame 39, the Embu between parts)
                over = LENGTH and last_t >= LENGTH - 3            # the script ran to its end
                if not LENGTH: over = not relink and p.r16(0x5F6C40)  # (the done flag can be stale)
                if time.monotonic() - theater > (1 if over else 120): break
                time.sleep(.02); continue
            theater = None
            if c == last:
                if time.monotonic() - idle > quiet: break
                time.sleep(.004)                               # polling flat out slowed the game: the Embu then gave up
                continue
            idle = time.monotonic()
            if last >= 0 and c < last and not ONE_SEQUENCE: seq += 1; print(f'sequence {seq} from frame {c}', flush=True); last = -1
            if not (WINDOW[0] <= (seq, c) <= WINDOW[1]) or c % EVERY[0] != EVERY[1]:
                last = c; continue                             # not captured on this pass
            m, q, ok, vp, x = read(p, actors)
            if not ok:                                        # node count changed: find the skeletons again
                actors = [fast_nodes(p, b) for b in ACTORS]
                m, q, ok, vp, x = read(p, actors)
            if p.r32(COUNTER) != c: continue
            if last >= 0 and c < last and ONE_SEQUENCE: break  # the next sequence: captured from its own state
            elif last >= 0 and c > last + 1 and EVERY[0] == 1: missed += c - last - 1
            if q != pars:
                pars = q
                print(f'frame {c}: nodes {[len(a) for a in actors]}', flush=True)
                f.write(json.dumps(dict(relink=c, t=x['t'], nodes=[len(a) for a in actors], parents=q)) + '\n')
            line = dict(s=seq, f=c, t=x['t'], sp=hex(x['sp']), actors=m, vp=vp, view=x['view'], hits=x['hits'],
                        fade=x['fade'], voices=x['voices'])
            tasks.frame(line)
            if SAVE_AT >= 0 and x['t'] >= SAVE_AT:               # a state mid-scene, for what the capture misses
                save_state(p, out.with_suffix('.mid.p2s')); print(f'state saved at frame {x["t"]}', flush=True)
                globals()['SAVE_AT'] = -1
            if x.get('embu_state') is not None: line['embu_state'] = x['embu_state']
            if x['done']: line['done'] = x['done']
            ident = [x['fighters'], x['texanim']]
            if ident != last_ident: last_ident = ident; line['fighters'], line['texanim'] = ident
            if x['music'] != last_music: line['music'] = last_music = x['music']
            st = statics(x['statics_raw'])
            key = [(o['i'], o['m'], o['words'][2]) for o in st]   # bookkeeping words move every frame
            if key != last_statics: last_statics = key; line['statics'] = st
            f.write(json.dumps(line) + '\n'); f.flush()
            last, got, last_t = c, got + 1, x['t']
            if got % 60 == 0: print(f'seq {seq} frame {c} (script {x["t"]}): {got} captured, {missed} missed', flush=True)
    print(f'done: {got} frames, {missed} missed, last {last}', flush=True)


def main():
    global ACTORS, ONE_SEQUENCE, WINDOW, EVERY, START_AFTER
    args = sys.argv[1:]
    global EMBU
    relink = EMBU = '--relink' in args
    if relink: args.remove('--relink')
    if '--start-after' in args:                         # F: first capture from frame F of sequence 0, quiet before
        i = args.index('--start-after'); START_AFTER = int(args[i + 1]); del args[i:i + 2]
    ONE_SEQUENCE = '--one-sequence' in args
    if ONE_SEQUENCE: args.remove('--one-sequence')
    if '--window' in args:                              # S:F-S:F, sequences and frames captured (both ends kept)
        i = args.index('--window'); a, b = args[i + 1].split('-')
        WINDOW = (tuple(map(int, a.split(':'))), tuple(map(int, b.split(':')))); del args[i:i + 2]
    if '--every' in args:                               # N:K, frames whose number is K modulo N
        i = args.index('--every'); EVERY = tuple(map(int, args[i + 1].split(':'))); del args[i:i + 2]
    after_load = '--after-load' in args                 # launched from the Theater: wait for its disc load first
    if after_load: args.remove('--after-load')
    global LENGTH, SAVE_AT
    if '--save-at' in args:
        i = args.index('--save-at'); SAVE_AT = int(args[i + 1]); del args[i:i + 2]
    if '--length' in args:
        i = args.index('--length'); LENGTH = int(args[i + 1]); del args[i:i + 2]
    load = None
    if '--load' in args:                                # N: load save state N first (one PINE client: this one)
        i = args.index('--load'); load = int(args[i + 1]); del args[i:i + 2]
    if '--actors' in args:
        i = args.index('--actors'); ACTORS = [int(a, 16) for a in args[i + 1].split(',')]; del args[i:i + 2]
    out = Path(args[0]); span = float(args[1]) if len(args) > 1 else 600
    p = Pine(ttt_slot()); p.wait()
    if load is not None:
        p.call(bytes([OP['load'], load])); time.sleep(.3)
    t0 = time.monotonic()
    if after_load:
        # Right after a save state the ending player's clock still runs a few
        # frames (the last scene's leftovers): the chosen scene comes after the
        # Theater grid or its disc load (bit 0x40 of the CD word).
        while not p.r32(SCENE) & 0x40:                    # the grid / the load
            if time.monotonic() - t0 > span: return
            time.sleep(.05)
        print('Theater or load seen', flush=True)
    # Wait for a scene to start: the counter back near 0 with the skeletons
    # in place (in the menu the counter or the nodes may hold anything).
    while True:
        if time.monotonic() - t0 > span: return
        if p.r32(SCENE) & 0x40:                           # LOADING: leave the game alone (polled fast
            time.sleep(.2); continue                      # there, the TTT Embu stopped short); 0x10 while it streams
        if START_AFTER:
            # The TTT Embu sets its disc streaming up in its first frames:
            # reads then (even ten) make it stop short in sequence 1.
            while p.r32(COUNTER) < START_AFTER: time.sleep(.2)
            actors = [fast_nodes(p, b) for b in ACTORS]
            if len(actors[0]) >= 90: break
            time.sleep(.2); continue
        c0 = script_clock(p)
        if c0 <= 30:                                    # not 0x447F24: it wraps at 1024 mid-scene
            time.sleep(.15)
            if script_clock(p) == c0: continue          # a stale clock (the Theater grid): not playing
            actors = [fast_nodes(p, b) for b in ACTORS]
            # One-actor endings (Bruce, Tetsujin) leave slot B empty.
            # The Embu opens on ~490 frames of storm without anyone: relinked when they come.
            if relink or (len(actors[0]) >= 90 and (len(actors[1]) >= 90 or not actors[1])): break
        time.sleep(.05)
    print('scene started at frame', p.r32(COUNTER), 'nodes', [len(a) for a in actors], flush=True)
    capture(p, out, t0, span, actors, relink)
    dump_scene(p, out)                                    # after: the reads would cost frames during the scene


if __name__ == '__main__':
    main()
