#include "tek_block_prof.h"
#include <stdio.h>
#include <string.h>

#ifdef TEK_BLOCK_PROF

#define TEK_MAX_CALLERS 6

typedef struct {
    uint32_t pc;
    uint32_t count;
} TekCallerEntry;

typedef struct {
    const char* name;
    uint32_t block_pc;
    uint64_t calls;
    uint64_t total_ticks;
    uint64_t max_ticks;
    TekCallerEntry callers[TEK_MAX_CALLERS];
} TekBlockStats;

static TekBlockStats g_block_stats[TEK_BLOCK_COUNT] = {
    { "func_80036CAC", 0x80036CACu, 0, 0, 0, {{0}} },
    { "func_8007E078", 0x8007E078u, 0, 0, 0, {{0}} },
    { "func_80037484", 0x80037484u, 0, 0, 0, {{0}} },
    { "block_80029700", 0x80029700u, 0, 0, 0, {{0}} },
    { "block_80029720", 0x80029720u, 0, 0, 0, {{0}} },
};

void tek_block_prof_record_caller(uint32_t id, uint32_t caller_pc) {
    if (id >= TEK_BLOCK_COUNT) return;
    TekBlockStats* s = &g_block_stats[id];
    s->calls++;
    for (int i = 0; i < TEK_MAX_CALLERS; i++) {
        if (s->callers[i].pc == caller_pc) {
            s->callers[i].count++;
            return;
        }
        if (s->callers[i].pc == 0) {
            s->callers[i].pc = caller_pc;
            s->callers[i].count = 1;
            return;
        }
    }
    s->callers[0].count++; // Fallback if caller slots full
}

void tek_block_prof_record_time(uint32_t id, uint64_t ticks) {
    if (id >= TEK_BLOCK_COUNT) return;
    TekBlockStats* s = &g_block_stats[id];
    s->total_ticks += ticks;
    if (ticks > s->max_ticks) {
        s->max_ticks = ticks;
    }
}

void tek_block_prof_report_and_reset(uint64_t frame) {
    printf("[BlockProf] frame=%llu win120:\n", (unsigned long long)frame);
    for (int b = 0; b < TEK_BLOCK_COUNT; b++) {
        TekBlockStats* s = &g_block_stats[b];
        if (s->calls == 0) continue;
        double total_ms = (double)armTicksToNs(s->total_ticks) / 1e6;
        double avg_us = (double)armTicksToNs(s->total_ticks) / 1000.0 / (double)s->calls;
        double max_us = (double)armTicksToNs(s->max_ticks) / 1000.0;
        double per_frame_ms = total_ms / 120.0;

        char caller_str[256];
        int pos = 0;
        caller_str[0] = '\0';
        for (int i = 0; i < TEK_MAX_CALLERS && s->callers[i].pc != 0; i++) {
            pos += snprintf(caller_str + pos, sizeof(caller_str) - pos,
                            "%s%08X:%u", (i > 0 ? " " : ""),
                            s->callers[i].pc, s->callers[i].count);
        }
        printf("  %-14s(0x%08X): calls=%6llu (%5.1f/f) tot=%6.2fms (%5.2fms/f) avg=%6.1fus max=%6.1fus callers=[%s]\n",
               s->name, s->block_pc,
               (unsigned long long)s->calls, (double)s->calls / 120.0,
               total_ms, per_frame_ms, avg_us, max_us, caller_str);
    }
    fflush(stdout);

    for (int b = 0; b < TEK_BLOCK_COUNT; b++) {
        g_block_stats[b].calls = 0;
        g_block_stats[b].total_ticks = 0;
        g_block_stats[b].max_ticks = 0;
        memset(g_block_stats[b].callers, 0, sizeof(g_block_stats[b].callers));
    }
}

#endif
