#!/usr/bin/env python3
"""Tekken Tag Tournament (PS2) in its own PCSX2 instance, any platform
(ttt_pcsx2.sh is the macOS original).

    python tools/ps2/ttt_pcsx2.py [PCSX2 options...]

Its own data folder (workspace/ttt-ps2/pcsx2-data: BIOS, inis with their own PINE
slot, tools/ps2/pine.py ttt_slot(); save states), never the user's, so it shares no settings, saves or
PINE slot with another PCSX2. Through slot.py: one game. Extra options go to
PCSX2 (e.g. -state 1). $TTT_SLOT_WHO names the session in the slot.py queue
(default embu-tests). The disc: $TTT_DISC, else
<TekkenProject>/discs/Tekken Tag Tournament (USA) (v1.00).bin.
"""
import os, shutil, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DATA = ROOT / 'workspace/ttt-ps2/pcsx2-data'
DISC_NAME = 'Tekken Tag Tournament (USA) (v1.00).bin'


def pcsx2():
    if sys.platform == 'darwin':
        return '/Applications/PCSX2.app/Contents/MacOS/PCSX2'
    if os.name == 'nt':
        for base in (os.environ.get('ProgramFiles', r'C:\Program Files'), os.environ.get('LOCALAPPDATA', '')):
            exe = Path(base) / 'PCSX2/pcsx2-qt.exe'
            if exe.exists(): return str(exe)
    return shutil.which('pcsx2-qt') or shutil.which('pcsx2') or 'pcsx2-qt'


def disc():
    if os.environ.get('TTT_DISC'): return os.environ['TTT_DISC']
    for d in (ROOT.parent / 'discs', Path.home() / 'TekkenProject/discs'):
        if (d / DISC_NAME).exists(): return str(d / DISC_NAME)
    sys.exit(f'{DISC_NAME} not found (set TTT_DISC)')


def main():
    if not (DATA / 'PCSX2/inis/PCSX2.ini').exists():
        sys.exit(f'{DATA} has no PCSX2/inis/PCSX2.ini (copy pcsx2-data from the transfer)')
    cmd = [sys.executable, str(ROOT.parent / 'ressources/slot.py'), '--kind', 'game', '--who', os.environ.get('TTT_SLOT_WHO', 'embu-tests'),
           '--prio', '2', '--', pcsx2(), '-datapath', str(DATA), '-batch', *sys.argv[1:], '--', disc()]
    sys.exit(subprocess.run(cmd).returncode)


if __name__ == '__main__':
    main()
