#!/usr/bin/env bash
# ==============================================================================
# Prepara la estructura de archivos para la tarjeta SD de la Nintendo Switch
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

DEST_DIR="$ROOT_DIR/dist_sd/switch/tekken3"
DISC_DEST="$DEST_DIR/disc"

echo "[*] Creando estructura de carpetas para la tarjeta SD..."
mkdir -p "$DISC_DEST"

# 1. Copiar el ejecutable .nro
echo "[*] Copiando tekken3_nx.nro..."
cp -v "$ROOT_DIR/build-switch/tekken3_nx.nro" "$DEST_DIR/"

# 2. Copiar los archivos del juego (CUE y BINs)
echo "[*] Copiando imagen legal de Tekken 3 (CUE + BIN)..."
cp -v "$ROOT_DIR/base_tekken3/disc/Tekken 3 (USA).cue" "$DISC_DEST/"
cp -v "$ROOT_DIR/base_tekken3/disc/Tekken 3 (USA) (Track 1).bin" "$DISC_DEST/"
cp -v "$ROOT_DIR/base_tekken3/disc/Tekken 3 (USA) (Track 2).bin" "$DISC_DEST/"
cp -v "$ROOT_DIR/base_tekken3/disc/Tekken 3 (USA) (Track 3).bin" "$DISC_DEST/"

echo ""
echo "[+] Estructura preparada en: $ROOT_DIR/dist_sd/"
echo "    Solo copia el contenido de 'dist_sd/' a la raíz de tu tarjeta microSD."
ls -lh "$DEST_DIR"
ls -lh "$DISC_DEST"
