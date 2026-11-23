#!/bin/bash
# The add-on's unit tests and repository check with the Expanded's own Python
# (its .setup/venv has numpy), on macOS and Linux, as dev\test.cmd on Windows.
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
PY="$ROOT/.setup/venv/bin/python"
if [ ! -x "$PY" ]; then
    echo "TMST tests: run Setup Tekken 3 first (no .setup/venv)."
    exit 1
fi
cd "$(dirname "$0")/.." || exit 1
"$PY" -m unittest discover -s tests "$@" || exit $?
exec "$PY" tools/check_repo.py
