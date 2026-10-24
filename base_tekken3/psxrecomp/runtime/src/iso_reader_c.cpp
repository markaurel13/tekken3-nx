/*
 * iso_reader_c.cpp — C wrapper for the C++ ISOReader class
 *
 * Provides iso_open() / iso_read_sector() / iso_close() for use by cdrom.c
 */

#include "iso_reader.h"
#include "mod_runtime.h"
#include <cstdio>

#include <cstdint>

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

void* iso_open(const char* path) {
    auto* reader = new PS1::ISOReader();
    if (!reader->Open(path)) {
        delete reader;
        return nullptr;
    }
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
    bool ok = reader->ReadRawSector(lba, buffer);
    uint64_t dt = iso_prof_ticks_to_ns(iso_prof_ticks() - t0);
    g_exec_phase = prev_phase;

    g_prof_iso_raw_calls++;
    g_prof_iso_raw_bytes += 2352;
    g_prof_iso_raw_ns += dt;
    if (dt > g_prof_iso_raw_max_ns) g_prof_iso_raw_max_ns = dt;

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
    auto* reader = static_cast<PS1::ISOReader*>(handle);
    reader->Close();
    delete reader;
}

} /* extern "C" */
