# Reglas de Desarrollo y Optimización - Soulcalibur II NX

## 1. Regla de Oro: Cero Adivinanzas (Zero-Guesswork / Medición Primero)
- **PROHIBIDO ADIVINAR O ASUMIR CAUSAS**: Nunca modificar código, algoritmos, formatos o estructuras basándose en suposiciones, intuiciones o hipótesis no verificadas.
- **MEDICIÓN Y CERTEZA TOTAL PREVIA**: Antes de realizar cualquier optimización o cambio correctivo, es obligatorio instrumentar telemetría, contadores de hardware o logs precisos para medir y confirmar empíricamente la causa raíz con certeza del 100%.
- **VERIFICACIÓN CIENTÍFICA**: Todo cambio debe estar respaldado por datos empíricos de hardware (perfilado por nanosegundos/ciclos de CPU/GPU) obtenidos directamente de ejecuciones en la consola Switch.
- **EXPERIMENTOS DE BISECCIÓN Y PRUEBAS CONTROLADAS**: Está explícitamente permitido implementar pruebas temporales e interactivas en consola (como alternar rutas de renderizado, estados o aislamientos mediante botones `ZL`/`ZR`, pruebas A/B en tiempo real o bypasses diagnósticos) siempre que su propósito sea aislar y confirmar empíricamente la causa raíz de un fallo visual o de rendimiento. Estas pruebas deben retirarse una vez encontrada la solución definitiva.
- Si no se tiene certeza absoluta sobre el origen de un cuello de botella o fallo, el único paso permitido es solicitar o agregar herramientas de medición/diagnóstico o pruebas controladas de bisección.

## 2. Restricciones del Entorno Nintendo Switch
- **Cero Overclock**: Todo el rendimiento debe medirse y optimizarse a las frecuencias estándar de fábrica en modo Handheld (CPU 1020 MHz, GPU 307 MHz).
- **Integridad de Código**: Las modificaciones se limitan exclusivamente a las capas de plataforma en `switch/` y `tools/`. Nunca modificar `runtime/` o `third_party/`.
- **Telemetría no invasiva**: Los contadores de GPU deben usar hardware queries nativas de Deko3D (`dkCmdBufReportCounter`) sin bloquear el pipeline ni degradar el framerate.

## 3. Monitoreo Activo de Compilaciones
- **Monitoreo Periódico Cada 15 Minutos**: En cualquier compilación en segundo plano que no finalice inmediatamente, se debe verificar activamente el progreso de los procesos de compilación (`cc1`, `ninja`) cada 15 minutos mediante temporizadores (`schedule`), informando el estado y consumo de recursos para garantizar transparencia y evitar la sensación de congelamiento.

## 4. Gestión Dinámica de Recursos al Compilar (Anti-Congelamiento del Host)
- **Inspección Previa Obligatoria**: Antes de lanzar cualquier compilación (`ninja`, `make`, `cmake --build`), es obligatorio revisar la carga actual del sistema (`nproc`, `uptime` / load average, `free -h` para RAM/swap disponible y procesos activos como `cc1`, `cc1plus`, `clang`, `ninja`, `make`, `ld`, `rustc`).
- **Ajuste Dinámico de Hilos (`-j`)**:
  - **PC libre (sin otras compilaciones activas y RAM suficiente)**: Usar casi toda la capacidad disponible (por ejemplo, `nproc - 1` o `nproc - 2` hilos para dejar margen al sistema/IDE).
  - **Compilación concurrente o carga/RAM ajustada**: Reducir drásticamente el paralelismo (por ejemplo, `-j1`, `-j2` o como máximo el 25%–50% de los núcleos libres según la RAM disponible) y usar `nice -n 10` / `ionice -c2 -n7` si hace falta, evitando saturar CPU, memoria o swap y previniendo que el equipo se cuelgue.
