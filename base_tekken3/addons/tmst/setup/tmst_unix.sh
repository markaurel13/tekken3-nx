# Sourced by Setup TMST.sh (Linux, Steam Deck) and Setup TMST.command (macOS),
# from the add-on's folder: the add-on's setup with the Expanded's own Python
# (.setup/venv), in its window when Tk and a display are there, else with
# questions in the terminal. tmst_main "$@" runs it; a Tekken 2 disc given as
# the first argument is filled in. None of the Expanded's own files change.

TMST_ROOT=$(cd ../.. && pwd)
TMST_PYTHON="$TMST_ROOT/.setup/venv/bin/python"
if [ "$(uname)" = Darwin ]; then
    TMST_SCRIPT=command
else
    TMST_SCRIPT=sh
fi

tmst_pause() { [ -t 0 ] && read -r -p "Press Return to close." _; }

# A path dragged onto the terminal: quotes and escaped spaces taken off.
tmst_ask() {
    local answer
    read -r -p "$1: " answer
    answer=${answer%\'}; answer=${answer#\'}; answer=${answer%\"}; answer=${answer#\"}; answer=${answer%\ }
    printf '%s' "${answer//\\ / }"
}

tmst_window() {
    if [ "$(uname)" = Darwin ]; then
        "$TMST_PYTHON" -c 'import tkinter' 2>/dev/null
    else
        [ -n "${DISPLAY}${WAYLAND_DISPLAY}" ] && "$TMST_PYTHON" -c 'import tkinter; tkinter.Tk().destroy()' 2>/dev/null
    fi
}

tmst_main() {
    if [ ! -f "$TMST_ROOT/game.toml" ]; then
        echo "This folder is not inside Tekken 3 Expanded."
        echo "Put it exactly at <Tekken 3 Expanded>/addons/tmst and run this again."
        tmst_pause; return 1
    fi
    if [ ! -x "$TMST_PYTHON" ] || [ ! -f "$TMST_ROOT/.setup/ready.json" ] || [ ! -f "$TMST_ROOT/build-release/CMakeCache.txt" ]; then
        echo "Tekken 3 Expanded has not been set up yet."
        echo "Run \"Setup Tekken 3.$TMST_SCRIPT\" in $TMST_ROOT first and let it finish"
        echo "(the game starts). Close the game, then run this again."
        tmst_pause; return 1
    fi
    if tmst_window; then
        exec "$TMST_PYTHON" setup/tmst_gui.py "$@"
    fi

    echo "TMST add-on for Tekken 3 Expanded: Jukebox, Photo Mode, Tournament Mode, Tekken 2 Content."
    if "$TMST_PYTHON" -c 'import sys; sys.path.insert(0, "setup"); import tmst_gui; sys.exit(not tmst_gui.game_running())' 2>/dev/null; then
        echo "Tekken 3 Expanded is running: close the game first (a running game cannot be built again)."
        tmst_pause; return 1
    fi
    local args=() disc known
    if [ $# -gt 0 ]; then
        args=(--t2-disc "$1")
    else
        known=$("$TMST_PYTHON" -c 'import sys; sys.path.insert(0, "setup"); import tmst_gui; print(tmst_gui.known_t2_disc())' 2>/dev/null)
        if [ -n "$known" ]; then
            echo "Tekken 2 disc: $known"
        else
            echo "Optional: your Tekken 2 disc (USA SLUS-00213 or Europe SCES-00255; .cue, .bin or .chd) gives the"
            echo "Jukebox's Tekken 1 and 2 music and the Tekken 2 stages. Drag it onto this window (or type its path)."
            disc=$(tmst_ask "Tekken 2 disc, or Return to go on without it")
            if [ -n "$disc" ]; then args=(--t2-disc "$disc"); else args=(--no-t2-disc); fi
        fi
    fi
    # The add-on's steps, then the game built again with it (dev/build.py).
    "$TMST_PYTHON" -u setup/tmst_setup.py "${args[@]}"
    local status=$?
    if [ $status -ne 0 ]; then
        echo "The setup stopped (what it printed is above)."
        tmst_pause; return $status
    fi
    echo
    echo "The TMST add-on is installed. Switch on its mods in the game's Mods tab (TMST group), then restart the game."
    if [ -t 0 ]; then
        local play
        read -r -p "Open the game's launcher now? [Y/n] " play
        case "$play" in
        [nN]*) ;;
        *) cd "$TMST_ROOT" && exec "$TMST_PYTHON" -c 'import sys; sys.path.insert(0, "launcher"); import setup_backend as b; b.launch_game(settings=True)' ;;
        esac
    fi
    return 0
}
