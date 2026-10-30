#!/usr/bin/env python3
"""Capture the TTT (PS2) Embu (Theater, MOVIE, Triangle) one sequence at a
time and join them.

    python3 tools/ps2/ttt_embu_capture.py OUT_DIR [START_SLOT]

Under a capture the Embu ends early (after about 1.2 of its 3 sequences;
2 290 frames without one: sequences 0 and 1 of 1 024 frames, then 2 to
frame 242), its music running on while the capture slows the game. So:
from save state START_SLOT (default 7: Omar's, the Embu's first frame
after LOADING), a light pass saves a state at the start of each sequence;
then each sequence is captured from its own state (ttt_capture.py
--relink --one-sequence) and OUT_DIR/embu.jsonl joins them (a "relink"
line where each part starts, "s" = its sequence).
"""
import json, shutil, subprocess, sys, time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pine import Pine, OP, ttt_slot
from ttt_capture import COUNTER, SCENE

ROOT = Path(__file__).resolve().parents[2]
SSTATES = ROOT / 'workspace/ttt-ps2/pcsx2-data/PCSX2/sstates'
slot_file = lambda n: SSTATES / f'SLUS-20001 (E84C9242).{n:02d}.p2s'
WORK = 9                                              # the slot each part is loaded from


def save(p, dst):
    f = slot_file(WORK); m = f.stat().st_mtime if f.exists() else 0
    p.call(bytes([OP['save'], WORK]))
    for _ in range(200):
        if f.exists() and f.stat().st_mtime != m: break
        time.sleep(.05)
    time.sleep(.8); shutil.copy2(f, dst)


def main():
    out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
    start = int(sys.argv[2]) if len(sys.argv) > 2 else 7
    shutil.copy2(slot_file(start), out / 'seq0.p2s')
    p = Pine(ttt_slot()); p.call(bytes([OP['load'], start])); time.sleep(1)
    # Right after a load the counter may still hold a stale value: start
    # counting sequences once the scene runs from its first frames.
    while not (p.r32(SCENE) == 0 and p.r32(COUNTER) < 50): time.sleep(.03)
    seq, last, theater = 0, p.r32(COUNTER), None
    while True:                                       # the light pass
        c, s = p.r32(COUNTER), p.r32(SCENE)
        if s & 0x40:
            theater = theater or time.monotonic()
            if time.monotonic() - theater > 8: break
        else: theater = None
        if c < last - 500:
            seq += 1; save(p, out / f'seq{seq}.p2s'); print('state at sequence', seq, flush=True)
        last = c; time.sleep(.03)
    p.sock.close()
    parts = []
    for k in range(seq + 1):
        shutil.copy2(out / f'seq{k}.p2s', slot_file(WORK))
        p = Pine(ttt_slot()); p.call(bytes([OP['load'], WORK])); p.sock.close(); time.sleep(.5)
        part = out / f'seq{k}.jsonl'
        subprocess.run([sys.executable, str(ROOT / 'tools/ps2/ttt_capture.py'), str(part), '600', '--relink', '--one-sequence'], check=True)
        parts.append(part); print('sequence', k, 'captured', flush=True)
    with open(out / 'embu.jsonl', 'w') as f:
        for k, part in enumerate(parts):
            lines = [json.loads(l) for l in part.read_text().splitlines()]
            head = lines[0]
            if k == 0: f.write(json.dumps(head) + '\n')
            else: f.write(json.dumps(dict(relink=0, part=k, nodes=[len(x) for x in head['parents']], parents=head['parents'])) + '\n')
            for d in lines[1:]:
                if 'f' in d: d['s'] = k
                f.write(json.dumps(d) + '\n')
    print(out / 'embu.jsonl', 'joined from', len(parts), 'sequences', flush=True)


if __name__ == '__main__':
    main()
