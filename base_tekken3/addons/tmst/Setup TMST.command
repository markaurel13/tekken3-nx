#!/bin/bash
# The TMST add-on for Tekken 3 Expanded on macOS: its setup window
# (setup/tmst_gui.py; with Tk: brew install python-tk@3.13), or questions here
# without one, then the game built again with the add-on. Double-click it
# after the Expanded's own setup (Setup Tekken 3.command), or run it from
# Terminal:  ./"Setup TMST.command" [<Tekken 2 disc: .cue, .bin or .chd>]
# It uses the Expanded's .setup/venv and the Homebrew tools its setup needs
# (cmake, ninja, sdl3, the Xcode command line tools); none of the Expanded's
# own files change.
cd "$(dirname "$0")" || exit 1
. setup/tmst_unix.sh
tmst_main "$@"
