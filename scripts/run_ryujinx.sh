#!/usr/bin/env bash
# ==============================================================================
# Lanza el binario tekken3_nx.nro en el emulador Ryujinx
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

RYUJINX_BIN="$(which Ryujinx 2>/dev/null || which ryujinx 2>/dev/null || true)"

if [ -z "$RYUJINX_BIN" ]; then
    echo "[!] Ryujinx no se encuentra en el PATH."
    echo "    Si tienes el build de Chansey (ChanseyIsTheBest/ryujinx), colócalo en el PATH o define RYUJINX_PATH."
    exit 1
fi

echo "[*] Compilando cambios con Ninja..."
cd "$ROOT_DIR"
ninja -C build-switch

echo "[*] Iniciando Ryujinx con tekken3_nx.nro..."
"$RYUJINX_BIN" "$ROOT_DIR/build-switch/tekken3_nx.nro"
