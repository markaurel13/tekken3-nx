#!/usr/bin/env bash
# ==============================================================================
# Script de Despliegue y Depuración Remota para Nintendo Switch
# Uso:
#   ./scripts/deploy_switch.sh [IP_DE_TU_SWITCH]
# Si no pasas IP, intentará leerla del archivo .switch_ip o de la variable SWITCH_IP.
# ==============================================================================

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
IP_FILE="$ROOT_DIR/.switch_ip"

NXLINK="/opt/devkitpro/tools/bin/nxlink"
if [ ! -x "$NXLINK" ]; then
    NXLINK="$(which nxlink 2>/dev/null || true)"
fi

if [ -z "$NXLINK" ]; then
    echo "[-] Error: no se encontró nxlink en el sistema."
    exit 1
fi

# Determinar la IP de la consola
SWITCH_IP="$1"
if [ -z "$SWITCH_IP" ] && [ -n "$SWITCH_IP_ENV" ]; then
    SWITCH_IP="$SWITCH_IP_ENV"
fi

if [ -z "$SWITCH_IP" ] && [ -f "$IP_FILE" ]; then
    SWITCH_IP="$(cat "$IP_FILE" | tr -d '[:space:]')"
fi

if [ -z "$SWITCH_IP" ]; then
    echo "[!] No se especificó la IP de la Nintendo Switch."
    read -rp "Ingresa la dirección IP de tu Switch (ej. 192.168.1.45): " SWITCH_IP
fi

# Guardar la IP para futuras ejecuciones
echo "$SWITCH_IP" > "$IP_FILE"

echo "[*] IP de Nintendo Switch configurada: $SWITCH_IP"

# 1. Asegurar que los últimos cambios estén compilados
echo "[*] Verificando compilación con Ninja..."
cd "$ROOT_DIR"
ninja -C build-switch

NRO_PATH="$ROOT_DIR/build-switch/tekken3_nx.nro"
if [ ! -f "$NRO_PATH" ]; then
    echo "[-] Error: no existe el archivo $NRO_PATH"
    exit 1
fi

echo "[+] Binario listo: $(ls -lh "$NRO_PATH" | awk '{print $9, "(" $5 ")"}')"
echo ""
echo "======================================================================"
echo "  ENVIANDO A NINTENDO SWITCH POR WI-FI (NXLINK)"
echo "  1. Asegúrate de tener abierto el Homebrew Menu en tu Switch."
echo "  2. Pulsa el botón 'Y' en el Homebrew Menu para activar NetLoader."
echo "  [Tip] Para recibir logs en vivo: sudo ufw allow 28280/tcp"
echo "======================================================================"
echo ""

# 2. Ejecutar nxlink con servidor interactivo de salida stdio
# -s : Inicia servidor para recibir los printf en consola
# -a : Dirección IP de la Switch
"$NXLINK" -s -a "$SWITCH_IP" "$NRO_PATH"
