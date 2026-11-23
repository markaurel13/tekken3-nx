#!/bin/bash
# TEMPORARY developer build with the TMST add-on on macOS and Linux
# (dev/build.py --help), as dev\build.cmd on Windows.
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
PY="$ROOT/.setup/venv/bin/python"
if [ ! -x "$PY" ]; then
    echo "TMST build: run Setup Tekken 3 first (no .setup/venv)."
    exit 1
fi
exec "$PY" "$(dirname "$0")/build.py" "$@"
