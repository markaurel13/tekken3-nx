#!/usr/bin/env python3
"""Capture the Tekken Tag Tournament (PS2) endings one after the other from
the Theater, without a player.

    python3 tools/ps2/ttt_capture_all.py OUT_DIR [key ...]

The TTT PCSX2 instance (tools/ps2/ttt_pcsx2.sh, PINE slot 28012, run at a
quarter speed) with two things set up:
- save state slot 1: the Theater grid, the cursor on Kunimitsu (cell 1);
- the patch PCSX2/patches/SLUS-20001_E84C9242.pnach of its data folder,
  which makes pad 1 read its buttons from EE 0x010C0000 (the game's pad
  parse, 0x396E60..0x396E70): a button is pressed by writing its bit there
  (active high: Right 0x2000, Left 0x8000, Down 0x4000, Square 0x0080,
  Cross 0x0040). Writing code through PINE crashed PCSX2 (its recompiler's
  protected pages); a patch applied by PCSX2 itself does not.

For each ending: load slot 1, write the character's cell into the cursor
(0x01FC4814: the panel follows, tools/ps2/ttt_theater.md), press the
button of its panel line (Square 1P, Cross 2P, Start 3P),
then tools/ps2/ttt_capture.py records the scene (PINE serves one client at
a time: the presses first, then the capture). OUT_DIR/<key>.jsonl, .log.
"""
import os, subprocess, sys, time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pine import Pine, OP, ttt_slot

ROOT = Path(__file__).resolve().parents[2]
PAD, CURSOR = 0x010C0000, 0x01FC4814
SQUARE, CROSS = 0x0080, 0x0040
START = 0x0800  # standard PS2 pad bit, not tried yet
# key: (cursor, button), from the Theater census (tools/ps2/ttt_theater.json).
ENDINGS = {
    'kunimitsu': (1, SQUARE), 'kazuya': (2, SQUARE), 'bruce': (3, SQUARE), 'jack2': (5, SQUARE),
    'lee': (6, SQUARE), 'baek': (8, SQUARE), 'michelle': (9, SQUARE), 'armorking': (10, SQUARE),
    'ganryu': (15, SQUARE), 'jun': (17, SQUARE), 'wang': (28, SQUARE), 'devil': (30, SQUARE),
    'roger': (33, SQUARE), 'alex': (33, CROSS), 'tetsujin': (34, SQUARE),
    'pjack': (29, SQUARE), 'angel': (30, START), 'unknown': (35, SQUARE),
}


def press(p, mask, hold=0.12):
    p.w16(PAD, mask); time.sleep(hold); p.w16(PAD, 0); time.sleep(0.3)


def start(key):
    cell, button = ENDINGS[key]
    p = Pine(ttt_slot())
    p.w16(PAD, 0); p.call(bytes([OP['load'], 1])); time.sleep(2); p.w16(PAD, 0)
    p.w32(CURSOR, cell); time.sleep(1)
    if p.r32(CURSOR) != cell: raise RuntimeError(f'{key}: cell {cell} not reached ({p.r32(CURSOR)})')
    press(p, button, 0.2)
    p.sock.close()


def main():
    out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
    for key in sys.argv[2:] or list(ENDINGS):
        print(key, 'starting', flush=True)
        start(key)
        with open(out / f'{key}.log', 'w') as log:
            r = subprocess.run([sys.executable, str(ROOT / 'tools/ps2/ttt_capture.py'), str(out / f'{key}.jsonl'), '900'],
                               stdout=log, stderr=subprocess.STDOUT)
        print(key, 'done', r.returncode, (out / f'{key}.log').read_text().strip().splitlines()[-1:], flush=True)


if __name__ == '__main__':
    main()
