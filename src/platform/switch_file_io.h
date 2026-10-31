#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void switch_audio_pump_if_needed(void);

/**
 * @brief Lee de un stream en bloques de 64 KB bombeando audio si transcurren >= 16.6 ms.
 * Evita el vaciado del anillo de audio (underrun) durante I/O pesada en MicroSD.
 */
static inline size_t switch_fread_pump(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t total_bytes = size * nmemb;
    if (total_bytes == 0) return 0;
    unsigned char *out = (unsigned char *)ptr;
    size_t bytes_read = 0;
    const size_t CHUNK = 64 * 1024;
    while (bytes_read < total_bytes) {
        size_t to_read = total_bytes - bytes_read;
        if (to_read > CHUNK) to_read = CHUNK;
        size_t r = fread(out + bytes_read, 1, to_read, stream);
        if (r > 0) {
            bytes_read += r;
        }
#ifdef __SWITCH__
        switch_audio_pump_if_needed();
#endif
        if (r < to_read) {
            break;
        }
    }
    return bytes_read / size;
}

/**
 * @brief Caché en RAM de archivos grandes leídos desde la MicroSD.
 * Si el archivo ya está en RAM, retorna el puntero existente de inmediato (0.001 ms).
 * Si no está, lo lee con switch_fread_pump, lo guarda en caché y retorna el puntero.
 * El búfer devuelto es de solo lectura y compartido; si el llamador necesita mutarlo,
 * debe hacer malloc + memcpy de out_size bytes.
 */
const unsigned char *switch_cached_file_get(const char *path, size_t *out_size);

#ifdef __cplusplus
}
#endif
