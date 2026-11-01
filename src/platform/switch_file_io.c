#include "switch_file_io.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define CACHE_HASH_SIZE 256

typedef struct SwitchFileCacheEntry {
    char path[512];
    unsigned char *data;
    size_t size;
    struct SwitchFileCacheEntry *next;
} SwitchFileCacheEntry;

static SwitchFileCacheEntry *s_file_cache_buckets[CACHE_HASH_SIZE] = {0};

static inline unsigned int cache_hash(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c;
    return hash % CACHE_HASH_SIZE;
}

const unsigned char *switch_cached_file_get(const char *path, size_t *out_size) {
    if (!path || !path[0]) return NULL;

    unsigned int bucket = cache_hash(path);

    // 1. Buscar en caché en RAM (O(1))
    for (SwitchFileCacheEntry *e = s_file_cache_buckets[bucket]; e; e = e->next) {
        if (strcmp(e->path, path) == 0) {
            if (out_size) *out_size = e->size;
            return e->data;
        }
    }

    // 2. Cargar desde MicroSD con open/fstat directo (sin sobrecarga stdio ni cluster seeks)
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;

    struct stat st;
    if (fstat(fd, &st) != 0 || st.st_size <= 0 || st.st_size > 64 * 1024 * 1024) {
        close(fd);
        return NULL;
    }

    size_t sz = (size_t)st.st_size;
    unsigned char *buf = (unsigned char *)malloc(sz);
    if (!buf) {
        close(fd);
        return NULL;
    }

    size_t bytes_read = 0;
    const size_t CHUNK = 512 * 1024;
    while (bytes_read < sz) {
        size_t to_read = sz - bytes_read;
        if (to_read > CHUNK) to_read = CHUNK;
        ssize_t n = read(fd, buf + bytes_read, to_read);
        if (n <= 0) break;
        bytes_read += (size_t)n;
#ifdef __SWITCH__
        switch_audio_pump_if_needed();
#endif
    }
    close(fd);

    if (bytes_read != sz) {
        free(buf);
        return NULL;
    }

    // 3. Registrar en hash table de caché
    SwitchFileCacheEntry *entry = (SwitchFileCacheEntry *)malloc(sizeof(SwitchFileCacheEntry));
    if (!entry) {
        if (out_size) *out_size = sz;
        return buf;
    }

    strncpy(entry->path, path, sizeof(entry->path) - 1);
    entry->path[sizeof(entry->path) - 1] = '\0';
    entry->data = buf;
    entry->size = sz;
    entry->next = s_file_cache_buckets[bucket];
    s_file_cache_buckets[bucket] = entry;

    if (out_size) *out_size = sz;
    return buf;
}
