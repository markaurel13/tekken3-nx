#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Telemetria de diagnostico (definidos en switch_audio.cpp). Solo los escribe el hilo
 * principal (mezcla del SPU), el reporte los lee en la misma ventana. */
extern uint64_t g_audio_limiter_calls;   /* invocaciones totales */
extern uint64_t g_audio_limiter_active;  /* invocaciones con |val| >= 24576 (curva activa) */

/**
 * @brief Limitador suave (soft-knee limiter) para mezcla de audio de 16 bits.
 * Para amplitudes |val| <= 24576 (-2.5 dBFS), el comportamiento es 100% lineal y bit-exacto.
 * Para amplitudes |val| > 24576, comprime el exceso asintóticamente hacia 32767 mediante
 * una curva racional C1 continua, eliminando el truncamiento plano de ondas (hard clipping),
 * la distorsión por armónicos impares y el raspeo/crujido en los altavoces de la consola.
 */
static inline int16_t audio_soft_limit(int32_t val) {
    g_audio_limiter_calls++;
    if (val > -24576 && val < 24576) {
        return (int16_t)val;
    }
    g_audio_limiter_active++;
    if (val >= 24576) {
        int64_t excess = val - 24576;
        int32_t comp = 24576 + (int32_t)((excess * 8191) / (excess + 8191));
        if (comp > 32766) comp = 32766;
        return (int16_t)comp;
    } else {
        int64_t excess = -val - 24576;
        int32_t comp = 24576 + (int32_t)((excess * 8191) / (excess + 8191));
        if (comp > 32767) comp = 32767;
        return (int16_t)-comp;
    }
}

#ifdef __cplusplus
}
#endif
