#!/usr/bin/env python3
"""Capture TTT (PS2) endings from the Theater, one after the other, alone.

    python tools/ps2/ttt_capture_theater.py OUT_DIR KEY [KEY ...] [-- ttt_capture.py options]

For each ending: load save state 1 of this worktree (the Theater grid), put
the cursor on its cell (0x01FC4814, the panel follows), start
ttt_capture.py waiting for the scene, press the panel's button on the
keyboard (PCSX2's pad 1 is bound to it too: Square J, Cross K, Circle L,
Triangle I, Start Return; tools/ps2/ttt_presskey.ps1 brings the PCSX2 window
up for the press, then gives the focus back), and once the scene is over
write the report (ttt_cine_report.py). The virtual pad of the .pnach patch
never launched a scene; the keyboard does. OUT_DIR/<key>.jsonl, .log,
.scene.bin, reports/<key>.report.md. Windows only (the key press).
"""
import os, subprocess, sys, time
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from pine import Pine, OP, ttt_slot
import ttt_script

HERE = Path(__file__).resolve().parent
CURSOR = 0x01FC4814
KEYS = dict(square=0x4A, cross=0x4B, circle=0x4C, triangle=0x49, start=0x0D, left=0x25, up=0x26, right=0x27, down=0x28)
# The Theater grid (ttt_theater.md): cell -> (row, column). Writing the cursor word
# moves the cursor on screen but not the menu's selection: move like a player.
GRID = {1: (0, 2), 2: (0, 3), 3: (0, 4), 4: (0, 5), 5: (0, 6), 6: (0, 7)}
GRID.update({8 + i: (1, i) for i in range(10)}); GRID.update({18 + i: (2, i) for i in range(10)})
GRID.update({34: (3, 1), 28: (3, 2), 29: (3, 3), 30: (3, 4), 31: (3, 5), 32: (3, 6), 33: (3, 7), 35: (3, 8)})
GRID.update({36: (0, 0), 37: (0, 8), 39: (3, 0), 38: (3, 9)})          # MOVIE, MUSIC, EXIT (overshoots)
# key: (cell, button), the Theater census (ttt_theater.md); guests of Tekken 3 Expanded first.
ENDINGS = {
    'kunimitsu': (1, 'square'), 'kazuya': (2, 'square'), 'bruce': (3, 'square'), 'jack2': (5, 'square'),
    'lee': (6, 'square'), 'baek': (8, 'square'), 'michelle': (9, 'square'), 'armorking': (10, 'square'),
    'ganryu': (15, 'square'), 'jun': (17, 'square'), 'wang': (28, 'square'), 'pjack': (29, 'square'),
    'devil': (30, 'square'), 'angel': (30, 'start'), 'roger': (33, 'square'), 'alex': (33, 'cross'),
    'tetsujin': (34, 'square'),
}


def pcsx2_pid():
    out = subprocess.run(['tasklist.exe', '/FI', 'IMAGENAME eq pcsx2-qt.exe', '/FO', 'CSV', '/NH'],
                         capture_output=True, text=True).stdout
    for line in out.splitlines():
        parts = line.strip('"').split('","')
        if len(parts) > 1 and parts[1].isdigit(): return int(parts[1])
    sys.exit('PCSX2 is not running (tools/ps2/ttt_pcsx2.py)')


def press(*buttons):
    r = subprocess.run(['powershell.exe', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File',
                        str(HERE / 'ttt_presskey.ps1'), '-ProcId', str(pcsx2_pid()),
                        '-Keys', ','.join(str(KEYS[b]) for b in buttons)], capture_output=True, text=True)
    if 'pressed' not in r.stdout: raise RuntimeError(f'key press failed: {r.stdout} {r.stderr}')


def move_to(cell):
    """One arrow press at a time until the cursor (read through PINE) is on
    the cell (presses in a row got lost)."""
    for _ in range(24):
        p = Pine(ttt_slot()); p.wait(10); cur = p.r32(CURSOR); p.sock.close()
        if cur == cell: return
        if cur not in GRID: raise RuntimeError(f'cursor {cur} off the grid')
        (r0, c0), (r1, c1) = GRID[cur], GRID[cell]
        press('down' if r1 > r0 else 'up' if r1 < r0 else 'right' if c1 > c0 else 'left')
        time.sleep(.3)
    raise RuntimeError(f'cell {cell} not reached')


def main():
    out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
    scenes = ttt_script.scenes(ttt_script.Elf(ttt_script.ELF_DEFAULT))
    args = sys.argv[2:]
    extra = args[args.index('--') + 1:] if '--' in args else []   # options for ttt_capture.py
    keys = args[:args.index('--')] if '--' in args else args
    for key in keys:
        cell, button = ENDINGS[key]
        p = Pine(ttt_slot()); p.wait(10)
        p.call(bytes([OP['load'], 1])); time.sleep(2)
        clock = None                                            # the state's leftover scene: let it stop
        for _ in range(60):
            c = p.r32(0x5F6C3C)
            if c == clock and p.r32(0x5F6B60) & 0x40: break
            clock = c; time.sleep(1)
        p.sock.close()                                          # PINE serves one client
        move_to(cell)
        log = open(out / f'{key}.log', 'w')
        cap = subprocess.Popen([sys.executable, str(HERE / 'ttt_capture.py'), str(out / f'{key}.jsonl'), '900', '--after-load',
                                '--length', str(ttt_script.find(scenes, {'alex': 'roger'}.get(key, key))['length']), *extra],
                               stdout=log, stderr=subprocess.STDOUT)
        time.sleep(3)
        for attempt in range(4):                                 # press again if the scene does not start
            try: press(button)
            except RuntimeError as e: print(key, e, flush=True)
            t0 = time.monotonic()
            while time.monotonic() - t0 < 40 and 'started' not in (out / f'{key}.log').read_text():
                time.sleep(1)
            if 'started' in (out / f'{key}.log').read_text(): break
        else:
            cap.kill(); log.close(); print(key, 'did not start', flush=True); continue
        cap.wait(); log.close()
        print(key, (out / f'{key}.log').read_text().strip().splitlines()[-2:], flush=True)
        import json
        head = json.loads((out / f'{key}.jsonl').open().readline())
        want = ttt_script.find(scenes, {'alex': 'roger'}.get(key, key))
        got = ttt_script.CHARACTERS.get(head['selection'] >> 3)
        print(key, 'selection', head['selection'], got, 'stage', head['stage'],
              'OK' if head['stage'] == want['stage'] else f"WRONG SCENE (stage {want['stage']} expected)", flush=True)
        subprocess.run([sys.executable, str(HERE / 'ttt_cine_report.py'), key, str(out / f'{key}.jsonl'), str(out / 'reports')])


if __name__ == '__main__':
    main()
