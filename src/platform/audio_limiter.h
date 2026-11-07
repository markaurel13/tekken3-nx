#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Umbral de limitación suave: 28500 (~ -1.2 dBFS).
 * Por debajo de este umbral (toda la música, efectos y voces en combate), el comportamiento
 * es 100% lineal y bit-exacto (cero compresión, máxima fidelidad y dinámica).
 * Para amplitudes |val| > 28500 (gritos extremos del anunciador / superposiciones masivas),
 * comprime el exceso asintóticamente hacia 32767 mediante una curva racional C1 continua,
 * eliminando el truncamiento plano de ondas (hard clipping), preservando nitidez y protegiendo
 * los altavoces de la consola. */
#define AUDIO_LIMITER_THRESHOLD 28500
#define AUDIO_LIMITER_HEADROOM  (32767 - AUDIO_LIMITER_THRESHOLD) /* 4267 */

/* Telemetria de diagnostico (definidos en switch_audio.cpp). Solo los escribe el hilo
 * principal (mezcla del SPU), el reporte los lee en la misma ventana. */
extern uint64_t g_audio_limiter_calls;   /* invocaciones totales */
extern uint64_t g_audio_limiter_active;  /* invocaciones con |val| >= AUDIO_LIMITER_THRESHOLD (curva activa) */

/**
 * @brief Limitador suave (soft-knee limiter) para mezcla de audio de 16 bits.
 */
static inline int16_t audio_soft_limit(int32_t val) {
    g_audio_limiter_calls++;
    if (val > -AUDIO_LIMITER_THRESHOLD && val < AUDIO_LIMITER_THRESHOLD) {
        return (int16_t)val;
    }
    g_audio_limiter_active++;
    if (val >= AUDIO_LIMITER_THRESHOLD) {
        int64_t excess = val - AUDIO_LIMITER_THRESHOLD;
        int32_t comp = AUDIO_LIMITER_THRESHOLD + (int32_t)((excess * AUDIO_LIMITER_HEADROOM) / (excess + AUDIO_LIMITER_HEADROOM));
        if (comp > 32766) comp = 32766;
        return (int16_t)comp;
    } else {
        int64_t excess = -val - AUDIO_LIMITER_THRESHOLD;
        int32_t comp = AUDIO_LIMITER_THRESHOLD + (int32_t)((excess * AUDIO_LIMITER_HEADROOM) / (excess + AUDIO_LIMITER_HEADROOM));
        if (comp > 32767) comp = 32767;
        return (int16_t)-comp;
    }
}

#ifdef __cplusplus
}
#endif
