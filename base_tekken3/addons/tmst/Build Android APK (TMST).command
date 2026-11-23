#!/bin/bash
# Tekken 3 Expanded for Android with the TMST add-on, on macOS (or Linux):
# the Expanded's APK build (Build Android APK) with the add-on built in
# (setup/tmst_android.py). Run it after the Expanded's setup and Setup TMST.
# Double-click it, or from Terminal with options:
#   --no-music  without the Jukebox's Tekken 1, 2, TTT and Tekken 5 DR music (about 800 MB)
#   --install   then install it on the phone connected by USB (adb)
#   --without   the APK without the add-on
cd "$(dirname "$0")" || exit 1
ROOT=$(cd ../.. && pwd)
python="$ROOT/.setup/venv/bin/python"
if [ ! -f "$ROOT/game.toml" ]; then
    echo "This folder is not inside Tekken 3 Expanded."
    echo "Put it exactly at <Tekken 3 Expanded>/addons/tmst and run this again."
    read -r -p "Press Return to close." _; exit 1
fi
if [ ! -x "$python" ]; then
    echo "Tekken 3 Expanded has not been set up yet."
    echo "Run \"Setup Tekken 3.command\" (or \"Build Android APK.command\") in $ROOT first, then this again."
    read -r -p "Press Return to close." _; exit 1
fi
"$python" setup/tmst_android.py "$@"
status=$?
[ -t 0 ] && read -r -p "Press Return to close." _
exit $status
