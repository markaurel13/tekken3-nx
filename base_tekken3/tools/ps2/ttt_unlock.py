#!/usr/bin/env python3
"""Tekken Tag Tournament (PS2, SLUS-20001 v1.00): everything unlocked, through PINE.

    python3 tools/ps2/ttt_unlock.py [seconds]

Talks to the TTT instance's own PINE slot, 28012 (tools/ps2/ttt_pcsx2.sh).

Rewrites the unlock flags every second while PCSX2 runs the game (default: an
hour). Raw form of the public CodeBreaker "Everything Unlocked" code for v1.0
(Almar's Guides), decrypted with the CB v1-6 seeds of mlafeldt/codebreaker-rs:
32 bits 0x00439B70 / 74 / 78 = FFFFFFFF, 16 bits 0x00439B82 = 0x0103.
"""
import sys, time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pine import Pine, ttt_slot

end = time.monotonic() + (float(sys.argv[1]) if len(sys.argv) > 1 else 3600)
p = None
while time.monotonic() < end:
    try:
        p = p or Pine(ttt_slot())
        # PCSX2 has one PINE socket for whatever game it runs: never write
        # into another session's game.
        if 'TEKKEN TAG' not in p.title().upper() and 'SLUS-20001' not in p.title().upper():
            p = None; time.sleep(2); continue
        for a in (0x439B70, 0x439B74, 0x439B78): p.w32(a, 0xFFFFFFFF)
        p.w16(0x439B82, 0x0103)
    except (OSError, FileNotFoundError, RuntimeError):
        pass                                      # not up yet, or restarted
    # PINE serves one client at a time: let the capture tools in.
    if p and p.sock: p.sock.close()
    p = None
    time.sleep(1)
