#include "switch_file_io.h"

typedef struct SwitchFileCacheEntry {
    char path[512];
    unsigned char *data;
    size_t size;
    struct SwitchFileCacheEntry *next;
} SwitchFileCacheEntry;

static SwitchFileCacheEntry *s_file_cache_head = NULL;

const unsigned char *switch_cached_file_get(const char *path, size_t *out_size) {
    if (!path || !path[0]) return NULL;

    // 1. Buscar en caché en RAM
    for (SwitchFileCacheEntry *e = s_file_cache_head; e; e = e->next) {
        if (strcmp(e->path, path) == 0) {
            if (out_size) *out_size = e->size;
            return e->data;
        }
    }

    // 2. Cargar desde disco / MicroSD
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (sz <= 0 || sz > 64 * 1024 * 1024) {
        fclose(f);
        return NULL;
    }

    unsigned char *buf = (unsigned char *)malloc((size_t)sz);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t r = switch_fread_pump(buf, 1, (size_t)sz, f);
    fclose(f);

    if (r != (size_t)sz) {
        free(buf);
        return NULL;
    }

    // 3. Registrar en la lista enlazada de caché
    SwitchFileCacheEntry *entry = (SwitchFileCacheEntry *)malloc(sizeof(SwitchFileCacheEntry));
    if (!entry) {
        if (out_size) *out_size = (size_t)sz;
        return buf;
    }

    strncpy(entry->path, path, sizeof(entry->path) - 1);
    entry->path[sizeof(entry->path) - 1] = '\0';
    entry->data = buf;
    entry->size = (size_t)sz;
    entry->next = s_file_cache_head;
    s_file_cache_head = entry;

    if (out_size) *out_size = (size_t)sz;
    return buf;
}
