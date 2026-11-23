#!/bin/bash
# The TMST add-on for Tekken 3 Expanded on Linux (experimental, as the
# Expanded's Linux build): its setup window (setup/tmst_gui.py), or questions
# here without one, then the game built again with the add-on. Run it after
# the Expanded's own setup (Setup Tekken 3.sh), from a terminal in this folder:
#     ./"Setup TMST.sh"             (or  bash "Setup TMST.sh")
#     ./"Setup TMST.sh" <disc>      with your Tekken 2 disc (.cue, .bin or .chd)
# Steam Deck: run it inside the Distrobox container the Expanded was set up in.
# It uses the Expanded's .setup/venv and the compilers its setup needs; none
# of the Expanded's own files change.
cd "$(dirname "$(readlink -f "$0")")" || exit 1
. setup/tmst_unix.sh
tmst_main "$@"
