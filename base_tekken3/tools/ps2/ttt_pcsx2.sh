#!/bin/zsh
# Tekken Tag Tournament (PS2) in its own PCSX2 instance: its own data folder
# (workspace/ttt-ps2/pcsx2-data: BIOS, inis with PINE slot 28012, memory
# cards), so it never shares settings, saves or the PINE socket with another
# session's PCSX2. Through slot.py: one game.
ROOT=${0:A:h:h:h}
exec python3 "$ROOT/../ressources/slot.py" --kind game --who "embu-tests" --prio 2 -- \
    /Applications/PCSX2.app/Contents/MacOS/PCSX2 -datapath "$ROOT/workspace/ttt-ps2/pcsx2-data" -batch -- \
    "$HOME/TekkenProject/discs/Tekken Tag Tournament (USA) (v1.00).bin"
