#include "tekken3_fmv_diag.h"
#include <stdio.h>
#include <switch.h>
#include "interrupts.h"
#include "psx_cycles.h"

typedef struct {
    uint64_t vsync_total_calls;
    uint64_t vsync_a0_neg1_calls;
    uint64_t vsync_ra_fed10;
    uint64_t vsync_ra_fee40;
    uint64_t vsync_ra_other;
    uint32_t vsync_last_other_ra;
    uint64_t vsync_would_skip_cycles;
    uint64_t vsync_total_host_ns;

    uint64_t getter_total_calls;
    uint64_t getter_ra_fecf8;
    uint64_t getter_ra_other;
    uint32_t getter_last_other_ra;

    uint64_t vblank_loop_iters_sum;
    uint32_t vblank_loop_iters_max;
    uint32_t vblank_count;
} FmvDiagWindow;

static FmvDiagWindow s_win = {0};
static uint32_t s_current_vblank_loop_iters = 0;
static u64 s_vsync_start_tick = 0;

void fmv_diag_pre_dispatch(CPUState* cpu, uint32_t addr) {
    if (addr == 0x800859A8u) {
        s_vsync_start_tick = armGetSystemTick();
        s_win.vsync_total_calls++;
        uint32_t a0 = cpu->gpr[4];
        uint32_t ra = cpu->gpr[31];
        if (a0 == 0xFFFFFFFFu) {
            s_win.vsync_a0_neg1_calls++;
            if (ra == 0x800FED10u) {
                s_win.vsync_ra_fed10++;
                uint32_t dist = cycles_to_next_event();
                if (dist > 64u && dist != 0xFFFFFFFFu && dist <= 564480u) {
                    psx_advance_cycles(dist);
                    s_win.vsync_would_skip_cycles += dist;
                }
            } else if (ra == 0x800FEE40u) {
                s_win.vsync_ra_fee40++;
                uint32_t dist = cycles_to_next_event();
                if (dist > 64u && dist != 0xFFFFFFFFu && dist <= 564480u) {
                    psx_advance_cycles(dist);
                    s_win.vsync_would_skip_cycles += dist;
                }
            } else {
                s_win.vsync_ra_other++;
                s_win.vsync_last_other_ra = ra;
            }
        }
    } else if (addr == 0x8008F74Cu) {
        s_win.getter_total_calls++;
        uint32_t ra = cpu->gpr[31];
        if (ra == 0x800FECF8u) {
            s_win.getter_ra_fecf8++;
        } else {
            s_win.getter_ra_other++;
            s_win.getter_last_other_ra = ra;
        }
    } else if (addr == 0x800FED10u || addr == 0x800FECE8u) {
        s_current_vblank_loop_iters++;
    }
}

void fmv_diag_post_dispatch(CPUState* cpu, uint32_t addr) {
    (void)cpu;
    if (addr == 0x800859A8u && s_vsync_start_tick != 0) {
        u64 end_tick = armGetSystemTick();
        s_win.vsync_total_host_ns += armTicksToNs(end_tick - s_vsync_start_tick);
        s_vsync_start_tick = 0;
    }
}

void fmv_diag_vblank(uint64_t frame_count) {
    s_win.vblank_count++;
    s_win.vblank_loop_iters_sum += s_current_vblank_loop_iters;
    if (s_current_vblank_loop_iters > s_win.vblank_loop_iters_max) {
        s_win.vblank_loop_iters_max = s_current_vblank_loop_iters;
    }
    s_current_vblank_loop_iters = 0;

    if (s_win.vblank_count >= 120) {
        if (s_win.vsync_total_calls > 100 || s_win.getter_total_calls > 100) {
            double avg_iters = (double)s_win.vblank_loop_iters_sum / (double)s_win.vblank_count;
            double vsync_ms = (double)s_win.vsync_total_host_ns / 1e6;
            double vsync_us_per_call = s_win.vsync_total_calls > 0 ?
                ((double)s_win.vsync_total_host_ns / 1e3 / (double)s_win.vsync_total_calls) : 0.0;
            uint64_t shortcut_hits = s_win.vsync_ra_fed10 + s_win.vsync_ra_fee40;
            double avg_skip_cyc = shortcut_hits > 0 ?
                ((double)s_win.vsync_would_skip_cycles / (double)shortcut_hits) : 0.0;

            printf("[FMV_LoopDiag] frame=%llu win%u: loop_iters=(avg=%.1f/f, max=%u/f, tot=%llu)\n"
                   "  [FMV_VSync] calls=%llu (a0=-1: %llu, host_time=%.2f ms, avg=%.2f us/call)\n"
                   "  [FMV_RA_Dist] ra_FED10=%llu, ra_FEE40=%llu, ra_other=%llu (last_other=0x%08X)\n"
                   "  [FMV_ShortcutCand] hits=%llu, would_skip_cyc=%llu (avg=%.0f cyc/hit)\n"
                   "  [FMV_Getter] calls=%llu (ra_FECF8=%llu, other=%llu, last_other=0x%08X)\n",
                   (unsigned long long)frame_count,
                   s_win.vblank_count,
                   avg_iters,
                   s_win.vblank_loop_iters_max,
                   (unsigned long long)s_win.vblank_loop_iters_sum,
                   (unsigned long long)s_win.vsync_total_calls,
                   (unsigned long long)s_win.vsync_a0_neg1_calls,
                   vsync_ms,
                   vsync_us_per_call,
                   (unsigned long long)s_win.vsync_ra_fed10,
                   (unsigned long long)s_win.vsync_ra_fee40,
                   (unsigned long long)s_win.vsync_ra_other,
                   s_win.vsync_last_other_ra,
                   (unsigned long long)shortcut_hits,
                   (unsigned long long)s_win.vsync_would_skip_cycles,
                   avg_skip_cyc,
                   (unsigned long long)s_win.getter_total_calls,
                   (unsigned long long)s_win.getter_ra_fecf8,
                   (unsigned long long)s_win.getter_ra_other,
                   s_win.getter_last_other_ra);
            fflush(stdout);
        }

        // Reset window stats
        s_win.vsync_total_calls = 0;
        s_win.vsync_a0_neg1_calls = 0;
        s_win.vsync_ra_fed10 = 0;
        s_win.vsync_ra_fee40 = 0;
        s_win.vsync_ra_other = 0;
        s_win.vsync_last_other_ra = 0;
        s_win.vsync_would_skip_cycles = 0;
        s_win.vsync_total_host_ns = 0;

        s_win.getter_total_calls = 0;
        s_win.getter_ra_fecf8 = 0;
        s_win.getter_ra_other = 0;
        s_win.getter_last_other_ra = 0;

        s_win.vblank_loop_iters_sum = 0;
        s_win.vblank_loop_iters_max = 0;
        s_win.vblank_count = 0;
    }
}
