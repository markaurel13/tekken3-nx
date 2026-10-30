#!/usr/bin/env python3
"""Series of PCSX2 save states of the TTT instance (PINE slot 28012).

    python3 tools/ps2/ttt_states.py OUT [seconds] [interval]

Every `interval` seconds (default 3, for 90 s), saves state slot 9 through
PINE and copies the new .p2s to OUT/<seconds>.p2s: each holds the whole EE
RAM (eeMemory.bin) and a Screenshot.png of that moment.
"""
import shutil, struct, sys, time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pine import Pine, ttt_slot

ROOT = Path(__file__).resolve().parents[2]
SSTATES = ROOT / 'workspace/ttt-ps2/pcsx2-data/PCSX2/sstates'
out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
span = float(sys.argv[2]) if len(sys.argv) > 2 else 90
step = float(sys.argv[3]) if len(sys.argv) > 3 else 3
t0 = time.monotonic()
while (t := time.monotonic() - t0) < span:
    before = {p: p.stat().st_mtime for p in SSTATES.glob('*.09.p2s')}
    p = Pine(ttt_slot())
    p.call(bytes([9, 9]))                       # save state, slot 9
    p.sock.close()
    for _ in range(100):                        # the file lands a moment later
        new = [q for q in SSTATES.glob('*.09.p2s') if q.stat().st_mtime != before.get(q)]
        if new: break
        time.sleep(.1)
    if new:
        time.sleep(.3)
        shutil.copy2(new[0], out / f'{t:06.1f}.p2s'); print(f'{t:6.1f} saved', flush=True)
    time.sleep(max(0, step - (time.monotonic() - t0 - t)))
