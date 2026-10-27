#include "iso_reader.h"
#include "mod_runtime.h"
#include <switch.h>
#include <cstdio>
#include <cstdint>
#include <cstring>

#define PREFETCH_RING_SIZE 32
#define PREFETCH_AHEAD     16

struct PrefetchSlot {
    uint32_t lba;
    uint8_t  data[2352];
    bool     valid;
};

static PrefetchSlot s_prefetch_ring[PREFETCH_RING_SIZE];
static Mutex s_prefetch_mutex = INVALID_HANDLE;
static CondVar s_prefetch_cv = 0;
static Thread s_prefetch_thread;
static bool s_prefetch_thread_active = false;
static volatile bool s_prefetch_stop = false;
static volatile uint32_t s_prefetch_expected_lba = 0xFFFFFFFF;
static uint32_t s_prefetch_max_lba = 0;
static FILE* s_prefetch_fp = nullptr;
static uint8_t s_prefetch_vbuf[256 * 1024];

extern "C" {

extern int g_exec_phase;

uint64_t g_prof_iso_raw_calls = 0;
uint64_t g_prof_iso_raw_bytes = 0;
uint64_t g_prof_iso_raw_ns = 0;
uint64_t g_prof_iso_raw_max_ns = 0;

uint64_t g_prof_iso_cook_calls = 0;
uint64_t g_prof_iso_cook_bytes = 0;
uint64_t g_prof_iso_cook_ns = 0;
uint64_t g_prof_iso_cook_max_ns = 0;

uint64_t g_prof_iso_prefetch_hits = 0;
uint64_t g_prof_iso_prefetch_miss = 0;
uint64_t g_prof_iso_raw_seek_ns = 0;
uint64_t g_prof_iso_raw_read_ns = 0;
uint64_t g_prof_iso_cook_seek_ns = 0;
uint64_t g_prof_iso_cook_read_ns = 0;

#if defined(__aarch64__) || defined(__SWITCH__)
static inline uint64_t iso_prof_ticks(void) {
    uint64_t cnt;
    __asm__ __volatile__("mrs %0, cntpct_el0" : "=r"(cnt));
    return cnt;
}
static inline uint64_t iso_prof_ticks_to_ns(uint64_t tick) {
    return (tick * 625) / 12;
}
#else
static inline uint64_t iso_prof_ticks(void) { return 0; }
static inline uint64_t iso_prof_ticks_to_ns(uint64_t tick) { return tick; }
#endif

static void prefetch_worker(void* arg) {
    (void)arg;
    uint32_t consecutive_failures = 0;
    while (!s_prefetch_stop) {
        uint32_t target_lba = 0xFFFFFFFF;

        mutexLock(&s_prefetch_mutex);
        while (!s_prefetch_stop) {
            uint32_t base_lba = s_prefetch_expected_lba;
            if (base_lba != 0xFFFFFFFF && base_lba < s_prefetch_max_lba) {
                for (uint32_t i = 0; i < PREFETCH_AHEAD; ++i) {
                    uint32_t candidate = base_lba + i;
                    if (candidate >= s_prefetch_max_lba) break;
                    uint32_t idx = candidate % PREFETCH_RING_SIZE;
                    if (!s_prefetch_ring[idx].valid || s_prefetch_ring[idx].lba != candidate) {
                        target_lba = candidate;
                        break;
                    }
                }
            }
            if (target_lba != 0xFFFFFFFF) {
                break;
            }
            condvarWait(&s_prefetch_cv, &s_prefetch_mutex);
        }
        mutexUnlock(&s_prefetch_mutex);

        if (s_prefetch_stop || target_lba == 0xFFFFFFFF) {
            break;
        }

        if (s_prefetch_fp && target_lba < s_prefetch_max_lba) {
            off_t offset = (off_t)target_lba * 2352;
            uint8_t temp[2352];
            bool ok = false;
            if (fseek(s_prefetch_fp, offset, SEEK_SET) == 0) {
                if (fread(temp, 1, 2352, s_prefetch_fp) == 2352) {
                    ok = true;
                }
            }

            mutexLock(&s_prefetch_mutex);
            if (!s_prefetch_stop) {
                uint32_t idx = target_lba % PREFETCH_RING_SIZE;
                if (ok) {
                    consecutive_failures = 0;
                    uint32_t base_lba = s_prefetch_expected_lba;
                    if (base_lba != 0xFFFFFFFF && target_lba >= base_lba && target_lba < base_lba + PREFETCH_AHEAD) {
                        s_prefetch_ring[idx].lba = target_lba;
                        memcpy(s_prefetch_ring[idx].data, temp, 2352);
                        s_prefetch_ring[idx].valid = true;
                    }
                } else {
                    // Si falla la lectura, marcar el slot para no entrar en bucle infinito
                    s_prefetch_ring[idx].lba = target_lba;
                    s_prefetch_ring[idx].valid = true;
                    consecutive_failures++;
                    if (consecutive_failures >= 3) {
                        printf("[CDPrefetch] AVISO: %u fallos consecutivos de lectura en LBA %u. Pausando prefetch.\n",
                               consecutive_failures, target_lba);
                        fflush(stdout);
                        s_prefetch_expected_lba = 0xFFFFFFFF;
                    }
                }
            }
            mutexUnlock(&s_prefetch_mutex);
        }
    }
}

static void cd_prefetch_init(const char* bin_path) {
    if (!bin_path || bin_path[0] == '\0') return;

    mutexInit(&s_prefetch_mutex);
    condvarInit(&s_prefetch_cv);

    for (int i = 0; i < PREFETCH_RING_SIZE; ++i) {
        s_prefetch_ring[i].valid = false;
        s_prefetch_ring[i].lba = 0xFFFFFFFF;
    }
    s_prefetch_stop = false;
    s_prefetch_expected_lba = 0xFFFFFFFF;

    s_prefetch_fp = fopen(bin_path, "rb");
    if (!s_prefetch_fp) {
        printf("[CDPrefetch] No se pudo abrir descriptor propio para '%s' (fallback síncrono activo)\n", bin_path);
        return;
    }

    fseek(s_prefetch_fp, 0, SEEK_END);
    off_t fsz = ftello(s_prefetch_fp);
    fseek(s_prefetch_fp, 0, SEEK_SET);
    s_prefetch_max_lba = (fsz > 0) ? (uint32_t)(fsz / 2352) : 0;
    printf("[CDPrefetch] Archivo '%s' tamano: %lld bytes (%u sectores maximos para prefetch)\n",
           bin_path, (long long)fsz, s_prefetch_max_lba);

    setvbuf(s_prefetch_fp, (char*)s_prefetch_vbuf, _IOFBF, sizeof(s_prefetch_vbuf));

    // Crear hilo en Core 1 con prioridad 0x2E (menor que el main thread 0x2C en Core 0)
    Result rc = threadCreate(&s_prefetch_thread, prefetch_worker, nullptr, nullptr, 0x8000, 0x2E, 1);
    if (R_SUCCEEDED(rc)) {
        rc = threadStart(&s_prefetch_thread);
        if (R_SUCCEEDED(rc)) {
            s_prefetch_thread_active = true;
            printf("[CDPrefetch] Hilo de lectura anticipada activo en Core 1 (buffer 256 KB, anillo %d sectores)\n", PREFETCH_RING_SIZE);
        } else {
            threadClose(&s_prefetch_thread);
            fclose(s_prefetch_fp);
            s_prefetch_fp = nullptr;
            printf("[CDPrefetch] Advertencia: threadStart fallo con codigo 0x%08X\n", rc);
        }
    } else {
        fclose(s_prefetch_fp);
        s_prefetch_fp = nullptr;
        printf("[CDPrefetch] Advertencia: threadCreate fallo con codigo 0x%08X\n", rc);
    }
}

void cd_prefetch_shutdown() {
    if (s_prefetch_thread_active) {
        mutexLock(&s_prefetch_mutex);
        s_prefetch_stop = true;
        condvarWakeAll(&s_prefetch_cv);
        mutexUnlock(&s_prefetch_mutex);

        threadWaitForExit(&s_prefetch_thread);
        threadClose(&s_prefetch_thread);
        s_prefetch_thread_active = false;
    }
    if (s_prefetch_fp) {
        fclose(s_prefetch_fp);
        s_prefetch_fp = nullptr;
    }
    printf("[CDPrefetch] Hilo de lectura anticipada finalizado ordenadamente.\n");
}

static bool cd_prefetch_get(uint32_t lba, uint8_t* out_buffer) {
    if (!s_prefetch_thread_active || lba >= s_prefetch_max_lba) return false;

    mutexLock(&s_prefetch_mutex);
    uint32_t idx = lba % PREFETCH_RING_SIZE;
    if (s_prefetch_ring[idx].valid && s_prefetch_ring[idx].lba == lba) {
        memcpy(out_buffer, s_prefetch_ring[idx].data, 2352);
        s_prefetch_ring[idx].valid = false;
        if (lba + 1 < s_prefetch_max_lba) {
            s_prefetch_expected_lba = lba + 1;
            condvarWakeOne(&s_prefetch_cv);
        } else {
            s_prefetch_expected_lba = 0xFFFFFFFF;
        }
        mutexUnlock(&s_prefetch_mutex);
        return true;
    }
    mutexUnlock(&s_prefetch_mutex);
    return false;
}

void* iso_open(const char* path) {
    auto* reader = new PS1::ISOReader();
    if (!reader->Open(path)) {
        delete reader;
        return nullptr;
    }
    cd_prefetch_init(reader->GetBinPath().c_str());
    return reader;
}

int iso_read_sector(void* handle, uint32_t lba, uint8_t* buffer, int size) {
    if (!handle) return 0;
    auto* reader = static_cast<PS1::ISOReader*>(handle);
    (void)size; /* ReadSector always reads 2048 bytes */
    int prev_phase = g_exec_phase;
    g_exec_phase = 13; /* PROF: iso_io */
    uint64_t t0 = iso_prof_ticks();
    bool ok = reader->ReadSector(lba, buffer);
    uint64_t dt = iso_prof_ticks_to_ns(iso_prof_ticks() - t0);
    g_exec_phase = prev_phase;

    g_prof_iso_cook_calls++;
    g_prof_iso_cook_bytes += 2048;
    g_prof_iso_cook_ns += dt;
    if (dt > g_prof_iso_cook_max_ns) g_prof_iso_cook_max_ns = dt;

    if (!ok) return 0;
    mod_runtime_patch_disc_sector(lba, 0, buffer, 2048);
    return 1;
}

int iso_read_raw_sector(void* handle, uint32_t lba, uint8_t* buffer, int size) {
    if (!handle || size < 2352) return 0;
    auto* reader = static_cast<PS1::ISOReader*>(handle);
    int prev_phase = g_exec_phase;
    g_exec_phase = 13; /* PROF: iso_io */
    uint64_t t0 = iso_prof_ticks();

    bool ok = false;
    bool in_prefetch_range = (lba < s_prefetch_max_lba);
    if (in_prefetch_range && cd_prefetch_get(lba, buffer)) {
        ok = true;
        g_prof_iso_prefetch_hits++;
    } else {
        g_prof_iso_prefetch_miss++;
        ok = reader->ReadRawSector(lba, buffer);
        if (ok && s_prefetch_thread_active) {
            mutexLock(&s_prefetch_mutex);
            if (in_prefetch_range && (lba + 1 < s_prefetch_max_lba)) {
                if (s_prefetch_expected_lba != lba) {
                    for (int i = 0; i < PREFETCH_RING_SIZE; ++i) {
                        s_prefetch_ring[i].valid = false;
                        s_prefetch_ring[i].lba = 0xFFFFFFFF;
                    }
                }
                s_prefetch_expected_lba = lba + 1;
                condvarWakeOne(&s_prefetch_cv);
            } else {
                // Sectores en pistas multi-bin (CD-DA Track 2/3): desarmar prefetch para no ciclar en Track 1
                s_prefetch_expected_lba = 0xFFFFFFFF;
            }
            mutexUnlock(&s_prefetch_mutex);
        }
    }

    uint64_t dt = iso_prof_ticks_to_ns(iso_prof_ticks() - t0);
    g_exec_phase = prev_phase;

    g_prof_iso_raw_calls++;
    g_prof_iso_raw_bytes += 2352;
    g_prof_iso_raw_ns += dt;
    if (dt > g_prof_iso_raw_max_ns) g_prof_iso_raw_max_ns = dt;

    extern int g_post_fmv_sync_active;
    if (g_post_fmv_sync_active && (!in_prefetch_range || g_prof_iso_raw_calls <= 10)) {
        printf("[ISO_RawRead] lba=%u ok=%d time_us=%llu (in_prefetch=%d)\n",
               lba, ok ? 1 : 0, (unsigned long long)(dt / 1000), in_prefetch_range ? 1 : 0);
        fflush(stdout);
    }

    if (!ok) return 0;
    mod_runtime_patch_disc_sector(lba, 1, buffer, 2352);
    return 1;
}

uint32_t iso_sector_count(void* handle) {
    if (!handle) return 0;
    auto* reader = static_cast<PS1::ISOReader*>(handle);
    return reader->GetSectorCount();
}

/* CD-track TOC accessors (multi-track / CD-DA support). track is 1-based. */
int iso_track_count(void* handle) {
    if (!handle) return 1;
    return static_cast<PS1::ISOReader*>(handle)->TrackCount();
}

uint32_t iso_track_start_lba(void* handle, int track) {
    if (!handle) return 0;
    return static_cast<PS1::ISOReader*>(handle)->TrackStartLBA(track);
}

uint32_t iso_track_pregap_lba(void* handle, int track) {
    auto* reader = static_cast<PS1::ISOReader*>(handle);
    return reader ? reader->TrackPregapLBA(track) : 0;
}

int iso_track_is_audio(void* handle, int track) {
    if (!handle) return 0;
    return static_cast<PS1::ISOReader*>(handle)->TrackIsAudio(track) ? 1 : 0;
}

void iso_close(void* handle) {
    if (!handle) return;
    cd_prefetch_shutdown();
    auto* reader = static_cast<PS1::ISOReader*>(handle);
    reader->Close();
    delete reader;
}

} /* extern "C" */
