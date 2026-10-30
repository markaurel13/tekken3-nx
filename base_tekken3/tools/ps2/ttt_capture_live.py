#!/usr/bin/env python3
"""Capture a TTT (PS2) in-engine ending the player starts from the Theater,
its actors found on the way.

    python3 tools/ps2/ttt_capture_live.py OUT.jsonl [seconds]

The TTT PCSX2 instance (tools/ps2/ttt_pcsx2.sh: PINE slot 28012, a quarter
speed) with the real pad (no virtual pad patch). The player starts the
ending; as soon as the cutscene counter (0x00447F24) runs, this saves state
slot 4, finds the actors in it (ttt_find_actors.py: they are not where
Kazuya's ending had them), loads that state back and records the scene from
there (ttt_capture.py --actors). OUT.state.p2s keeps that state, to capture
the ending again without the player.
"""
import shutil, subprocess, sys, time, zipfile
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pine import Pine, OP, ttt_slot
from ttt_find_actors import actors, COUNTER
from ttt_capture import SCENE

ROOT = Path(__file__).resolve().parents[2]
SSTATES = ROOT / 'workspace/ttt-ps2/pcsx2-data/PCSX2/sstates'
SLOT = 4


def save(p):
    path = SSTATES / f'SLUS-20001 (E84C9242).{SLOT:02d}.p2s'
    before = path.stat().st_mtime if path.exists() else 0
    p.call(bytes([OP['save'], SLOT]))
    for _ in range(200):                            # the file lands a moment later
        if path.exists() and path.stat().st_mtime != before:
            time.sleep(.5)
            return path
        time.sleep(.1)
    raise RuntimeError('save state not written')


def main():
    out = Path(sys.argv[1]); span = sys.argv[2] if len(sys.argv) > 2 else '900'
    p = Pine(ttt_slot())
    print('waiting for the ending to start (Theater: the character, then Square)', flush=True)
    last = p.r32(COUNTER)
    while True:                                     # the counter running from near 0
        c = p.r32(COUNTER)
        if p.r32(SCENE) == 0 and c != last and 0 < c <= 20: break   # not a stale counter during LOADING
        last = c; time.sleep(.02)
    first = None                                    # (state copy, actors) of the first state with an actor
    for _ in range(10):
        state = save(p)
        ee = zipfile.ZipFile(state).read('eeMemory.bin')
        found = [(a, n) for a, n in actors(ee) if n >= 90]
        counter = int.from_bytes(ee[COUNTER:COUNTER + 4], 'little')
        print('state at frame', counter, 'actors', ' '.join(f'{a:#x}:{n}' for a, n in found), flush=True)
        if len(found) >= 2: break
        if found and not first:
            first = (out.with_suffix('.first.p2s'), found); shutil.copy2(state, first[0])
        time.sleep(.5)
    else:
        if not first: raise RuntimeError('no actors found')
        # a single actor all along (Bruce plays his ending alone): capture from its first state
        print('one actor only: capturing from the first state', flush=True)
        shutil.copy2(first[0], state); found = first[1]
    shutil.copy2(state, out.with_suffix('.state.p2s'))
    p.call(bytes([OP['load'], SLOT])); p.sock.close()
    time.sleep(.5)
    # --relink reads each frame in one PINE message: the word-by-word reads
    # kept only 28 of 213 frames over Windows' TCP PINE (Kunimitsu, 2026-10-13).
    cmd = [sys.executable, str(ROOT / 'tools/ps2/ttt_capture.py'), str(out), span, '--relink',
           '--actors', ','.join(f'{a:x}' for a, n in found)]
    sys.exit(subprocess.run(cmd).returncode)


if __name__ == '__main__':
    main()
