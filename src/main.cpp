#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <vector>
#include <algorithm>
#include <utility>
#include <sys/stat.h>
#include <sys/iosupport.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <strings.h>
#include <switch.h>

#include "renderer/deko3d_renderer.h"
#include "platform/switch_audio.h"
#include "platform/switch_input.h"
#include "iso_reader.h"
#include "disc_path.h"
#include "tek_block_prof.h"

// Referencias a los componentes de psxrecomp
extern "C" {
extern void cd_prefetch_shutdown(void);
extern uint32_t g_game_entry_hits[];
extern uint32_t psx_game_entry_addr(uint32_t);
extern uint32_t psx_game_entry_count(void);
#include "cpu_state.h"
#include "psx_scheduler.h"
#include "psx_bios_backend.h"
#include "bios_hle_plan.h"
#include "bios_hle.h"
#include "fntrace.h"
#include "dirty_ram_interp.h"
#include "memcard.h"
#include "gpu_render.h"
#include "gpu.h"
#include "sio.h"
#include "spu.h"
#include "dma.h"
#include "timers.h"
#include "interrupts.h"
#include "mdec.h"
#include "cdrom.h"
#include "psx_interpreter.h"
#include "crc32.h"

// Funciones de memoria e IRQ de PS1
void memory_init(const char* bios_path);
void memory_set_sr_ptr(const uint32_t *p);
void psx_irq_set_cause_ptr(uint32_t *p);
uint32_t psx_read_word(uint32_t addr);
void psx_write_word(uint32_t addr, uint32_t val);
uint16_t psx_read_half(uint32_t addr);
void psx_write_half(uint32_t addr, uint16_t val);
uint8_t psx_read_byte(uint32_t addr);
void psx_write_byte(uint32_t addr, uint8_t val);
void psx_icache_reset(void);
void psx_hle_scheduler_set_default(int on);
uint8_t* memory_get_ram_ptr(void);

// Forward declaration de la función de despacho generada para Tekken 3
void psx_dispatch_init(void);
extern uint32_t g_pc0_reason;
extern uint32_t g_overlay_region_floor;
extern uint64_t psx_cycle_count;
extern int g_idle_skip_enabled;
extern uint64_t g_idle_skip_count;
extern uint64_t g_idle_skip_cycles;
extern uint64_t g_dirty_ram_blocks_run;
extern uint64_t g_dirty_ram_insns_run;
extern uint64_t g_dirty_ram_unsupported_midblock;
extern uint32_t g_dirty_ram_last_unsupported_pc;
extern uint32_t g_dirty_ram_last_unsupported_insn;

// Configuración gráfica dinámica (config.txt)
struct GameConfig {
    int supersampling = 2; // 1 = 1x (Nativo PS1), 2 = 2x HD (Recomendado), 3 = 3x Ultra HD
    bool widescreen_16_9 = false; // false = 4:3 con bandas negras, true = 16:9 pantalla completa
};
static GameConfig g_config;
extern const char* g_dirty_ram_last_unsupported_reason;
void dirty_ram_text_exact_mismatch_stats(uint64_t *count, uint32_t out[5]);

int psx_overlay_static_code_matches(const uint32_t *lo_len_pairs,
                                    uint32_t count,
                                    uint32_t expected_crc) {
    const uint8_t *ram = memory_get_ram_ptr();
    if (!ram || !lo_len_pairs || count == 0 || count > 4096) {
        return 0;
    }

    uint32_t crc = 0xFFFFFFFFu;
    for (uint32_t i = 0; i < count; i++) {
        const uint32_t lo = lo_len_pairs[i * 2u] & 0x1FFFFFu;
        const uint32_t len = lo_len_pairs[i * 2u + 1u];
        if (len == 0 || lo >= 0x200000u || len > 0x200000u - lo) {
            return 0;
        }
        crc = crc32_update(crc, ram + lo, len);
    }
    return (crc ^ 0xFFFFFFFFu) == expected_crc;
}

// Variables globales requeridas por el runtime generado
int g_psx_cps_mode = 0;
uint64_t s_frame_count = 0;
int debug_server_fmv_quiet = 0;
uint32_t g_debug_last_store_pc = 0;
uint32_t g_debug_current_func_addr = 0;
int g_ws_bd_stretch_on = 0;
int g_ws_bd_stretch_pct = 0;
void event_ring_record_aux(uint32_t a, uint32_t b) { (void)a; (void)b; }
void debug_server_poll(void) {}
void card_data_writes_arm(int a) { (void)a; }

// Stubs de debug server / telemetry para Switch release
int g_ram_read_watch_active = 0;
volatile uint32_t g_psx_last_fn_entry = 0;
#ifndef PSX_NO_DEBUG_TOOLS
void debug_server_log_call_entry(uint32_t func_addr) { g_psx_last_fn_entry = func_addr; }
#endif
void debug_server_cyc_observe(uint32_t block_leader_phys) { (void)block_leader_phys; }
void debug_server_trace_mmio_read(uint32_t a, uint32_t b, int c) { (void)a; (void)b; (void)c; }
void debug_server_trace_ram_read_watch(uint32_t a, uint32_t b) { (void)a; (void)b; }
void card_read_summary_record(int a, int b) { (void)a; (void)b; }
int spu_shadow_enabled(void) { return 0; }
void spu_shadow_process(const int16_t* a, int b) { (void)a; (void)b; }

// Netplay y selfcheck stubs para single-player offline en Switch
int psx_netplay_active(void) { return 0; }
int psx_netplay_is_resimulating(void) { return 0; }
int psx_selfcheck_resim_active(void) { return 0; }
int psx_selfcheck_replay_input(void) { return 0; }
void psx_selfcheck_finish_frame(int a) { (void)a; }
int psx_selfcheck_input_locked(void) { return 0; }
int psx_selfcheck_enabled(void) { return 0; }
void psx_netplay_diag_snapshot(void* a) { (void)a; }
void load_transition_note(int a, int b, int c, int d) { (void)a; (void)b; (void)c; (void)d; }
void post_load_probe_on_vblank(int a, int b) { (void)a; (void)b; }

// Crash trace / overlay stubs
int g_call_unit_depth = 0;
uint64_t g_dispatch_static_hits = 0;
void (*g_overlay_flush_pending_cycles)(void) = NULL;
CPUState* debug_cpu_ptr = NULL;
uint32_t debug_guest_ra(void) { return debug_cpu_ptr ? debug_cpu_ptr->gpr[31] : 0; }
uint32_t debug_guest_sp(void) { return debug_cpu_ptr ? debug_cpu_ptr->gpr[29] : 0; }
uint32_t psx_nd_wood_batch_ot(void) { return 0; }
uint32_t psx_nd_sibling_ot_hint(void) { return 0; }
uint32_t psx_netplay_sim_tick(void) { return 0; }
int psx_netplay_cd_bisect_active(void) { return 0; }
uint32_t psx_netplay_rb_sticky_bb_pc(void) { return 0; }
void psx_netplay_poll_snap(CPUState* a) { (void)a; }
void psx_selfcheck_poll(CPUState* a) { (void)a; }
void savestate_poll(CPUState* a) { (void)a; }
void event_ring_record(uint32_t a, uint32_t b) { (void)a; (void)b; }
void spu_shadow_reset(void) {}
void text_xlate_vram_upload(int a, int b, int c, int d, const uint16_t* e) { (void)a; (void)b; (void)c; (void)d; (void)e; }
int overlay_loader_is_candidate(uint32_t a) { (void)a; return 0; }
void overlay_regs_snap(CPUState* a) { (void)a; }
int overlay_loader_dispatch(CPUState* a, uint32_t b) { (void)a; (void)b; return 0; }
int overlay_loader_call_native(CPUState* a, uint32_t b, uint32_t c) { (void)a; (void)b; (void)c; return 0; }
void overlay_fp_log(uint32_t a, uint32_t b) { (void)a; (void)b; }
int overlay_fp_enabled(void) { return 0; }
void overlay_loader_active_write_check(uint32_t a, uint32_t b) { (void)a; (void)b; }
void overlay_loader_note_code_write(uint32_t a, uint32_t b) { (void)a; (void)b; }
void overlay_capture_before_dma(uint32_t a, uint32_t b);
void overlay_capture_on_dma(uint32_t a, uint32_t b, const uint8_t* c);
void card_data_writes_check(uint32_t a, uint32_t b) { (void)a; (void)b; }
void debug_server_trace_dispatch(uint32_t a) { (void)a; }
void debug_server_trace_mmio_write(uint32_t a, uint32_t b, uint8_t c) { (void)a; (void)b; (void)c; }
void debug_server_trace_write_check(uint32_t a, uint32_t b, uint32_t c, uint8_t d) { (void)a; (void)b; (void)c; (void)d; }
void debug_server_log_sio_write(uint32_t a, uint32_t b, uint8_t c) { (void)a; (void)b; (void)c; }
void debug_server_log_restore_event(uint32_t a, uint32_t b, uint32_t c) { (void)a; (void)b; (void)c; }
int debug_server_dirty_break_maybe_pause(uint32_t a, CPUState* b) { (void)a; (void)b; return 0; }
void debug_server_wait_if_paused(void) {}
void boot_state_trigger_capture(void) {}
void text_xlate_on_dispatch(uint32_t a) { (void)a; }
void overlay_loader_shadow_scheduler_escape_fixup(void) {}
uint32_t psx_netplay_rb_recover_null_pc(CPUState* a) { (void)a; return 0; }
void debug_server_log_thread_event(uint32_t a, CPUState *b, uint32_t c, uint32_t d, uint32_t e) {
    (void)a; (void)b; (void)c; (void)d; (void)e;
}
void overlay_loader_shadow_native_thread_switch_bail(void) {}
void overlay_loader_shadow_escape_fixup(void) {}
void psx_fatal_halt(const char* reason) {
    printf("[PSX FATAL] %s\n", reason ? reason : "unknown");
    abort();
}
void psx_unknown_dispatch_record(uint32_t a, uint32_t b) { (void)a; (void)b; }
void psx_crash_trace_dump(const char* a) { (void)a; }
uint64_t crash_trace_dispatch_seq_get(void) { return 0; }
const void* crash_trace_dispatch_ring_get(void) { return 0; }
}

struct SwitchOverlayCaptureEntry {
    uint32_t load_addr;
    uint32_t size;
    uint32_t seq;
    std::vector<uint8_t> bytes;
};

static std::vector<SwitchOverlayCaptureEntry> s_switch_overlay_captures;
static uint32_t s_switch_overlay_capture_seq = 0;
static uint64_t s_switch_overlay_capture_bytes = 0;
static void switch_overlay_capture_mkdirs(void);
static void switch_overlay_capture_write_bin(const SwitchOverlayCaptureEntry& e);

static constexpr uint32_t kSwitchOverlayHotPcs[] = {
    0x0007B5A4u,
    0x000B19B0u,
    0x000B1AE0u,
    0x000B1CA8u,
    0x000B37F0u,
    0x000DBCF8u,
    0x000E0F20u,
    0x000E11DCu,
    0x000E1D4Cu,
    0x000FDE0Cu,
    0x000FEA08u,
    0x000FEBE0u,
    0x000FECF8u,
    0x0010EB70u,
    0x0010ED3Cu,
    0x0010F838u,
    0x0010FBE8u,
    0x0010FD00u,
};

static bool switch_overlay_capture_contains_pc(const SwitchOverlayCaptureEntry& e, uint32_t pc) {
    return pc >= e.load_addr && pc < e.load_addr + e.size;
}

static bool switch_overlay_span_contains_pc(uint32_t load_addr, uint32_t size, uint32_t pc) {
    return pc >= load_addr && pc < load_addr + size;
}

extern "C" void overlay_capture_before_dma(uint32_t load_addr, uint32_t size) {
    (void)load_addr;
    (void)size;
}

extern "C" int fntrace_is_game_started(void);

extern "C" void overlay_capture_on_dma(uint32_t load_addr, uint32_t size, const uint8_t* bytes) {
    if (!bytes || size == 0) return;
    // CRITICAL: Never capture during boot / before game handoff!
    // Capturing boot-time executable sectors caused thousands of synchronous SD writes,
    // freezing boot DMA at 1.6 FPS and causing a black screen before Namco Presents.
    if (!fntrace_is_game_started()) return;

    uint32_t phys = load_addr & 0x1FFFFFFFu;
    // Only track genuine dynamic runtime overlays (0x800F0000 - 0x80100000)
    if (phys < 0x000F0000u || phys >= 0x00100000u) return;

    // Limit in-memory captures
    if (s_switch_overlay_captures.size() >= 32) return;

    static uint32_t s_log_dma_cap_cnt = 0;
    if (s_log_dma_cap_cnt++ < 30) {
        printf("[OverlayCapture] DMA CD overlay transfer to phys 0x%08X (size %u bytes)\n", phys, size);
        fflush(stdout);
    }
    SwitchOverlayCaptureEntry e;
    e.seq = ++s_switch_overlay_capture_seq;
    e.load_addr = phys;
    e.size = size;
    e.bytes.assign(bytes, bytes + size);
    s_switch_overlay_capture_bytes += size;
    s_switch_overlay_captures.push_back(std::move(e));
}


// Sistema de logging unificado (MicroSD + nxlink over Wi-Fi)
static FILE* s_sd_log = NULL;
static int s_nxlink_sock = -1;
static uint64_t s_last_main_loop_tick = 0;
static uint64_t s_last_main_loop_frame = 0;
static uint64_t s_win_loop_ns = 0;
static uint64_t s_win_input_ns = 0;
static uint64_t s_win_audio_ns = 0;
static uint64_t s_win_render_ns = 0;
static uint64_t s_win_other_ns = 0;
static uint64_t s_win_guest_ns = 0;
static uint64_t s_win_samples = 0;
static uint64_t s_last_cb_exit_tick = 0;
static uint64_t s_last_hot_pc_frame = 0;
static uint32_t s_last_hot_pc = 0;


static void switch_overlay_capture_mkdirs(void) {
    mkdir("sdmc:/switch/tekken3/overlay_captures_switch", 0777);
}

static void switch_overlay_capture_write_bin(const SwitchOverlayCaptureEntry& e) {
    switch_overlay_capture_mkdirs();
    char path[256];
    snprintf(path, sizeof(path),
             "sdmc:/switch/tekken3/overlay_captures_switch/entry_%04u_%06X_%u.bin",
             e.seq, e.load_addr, e.size);
    FILE* f = fopen(path, "wb");
    if (!f) return;
    if (!e.bytes.empty()) fwrite(e.bytes.data(), 1, e.bytes.size(), f);
    fclose(f);
}

static void switch_overlay_capture_write_json(void) {
    if (s_switch_overlay_captures.empty()) return;
    switch_overlay_capture_mkdirs();
    for (const auto& e : s_switch_overlay_captures) {
        switch_overlay_capture_write_bin(e);
    }
    const char* path = "sdmc:/switch/tekken3/overlay_captures_switch/overlay_captures.json";
    FILE* f = fopen(path, "w");
    if (!f) {
        printf("[OverlayCapture] No se pudo escribir %s\n", path);
        return;
    }
    fprintf(f, "{\n");
    fprintf(f, "  \"schema\": \"tekken3-nx-switch-overlay-captures-v1\",\n");
    fprintf(f, "  \"note\": \"DMA captures after game handoff; file points to live RAM bytes loaded over the static SLUS image.\",\n");
    fprintf(f, "  \"count\": %zu,\n", s_switch_overlay_captures.size());
    fprintf(f, "  \"total_bytes\": %llu,\n", (unsigned long long)s_switch_overlay_capture_bytes);
    fprintf(f, "  \"entries\": [\n");
    for (size_t i = 0; i < s_switch_overlay_captures.size(); i++) {
        const SwitchOverlayCaptureEntry& e = s_switch_overlay_captures[i];
        fprintf(f, "    {\"seq\":%u,\"load_addr\":\"0x%06X\",\"size\":%u,\"file\":\"entry_%04u_%06X_%u.bin\",\"covered_hot_pcs\":[",
                e.seq, e.load_addr, e.size, e.seq, e.load_addr, e.size);
        bool first_pc = true;
        for (uint32_t pc : kSwitchOverlayHotPcs) {
            if (!switch_overlay_capture_contains_pc(e, pc)) continue;
            fprintf(f, "%s\"0x%06X\"", first_pc ? "" : ",", pc);
            first_pc = false;
        }
        fprintf(f, "]}%s\n", (i + 1 < s_switch_overlay_captures.size()) ? "," : "");
    }
    fprintf(f, "  ]\n}\n");
    fclose(f);
    printf("[OverlayCapture] Escrito %s entries=%zu bytes=%llu\n",
           path, s_switch_overlay_captures.size(),
           (unsigned long long)s_switch_overlay_capture_bytes);
}

int main(int argc, char* argv[]);

extern "C" {
u32 __nx_exception_ignoredebug = 1;
alignas(16) u8 __nx_exception_stack[0x10000];
u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);

void __libnx_exception_handler(ThreadExceptionDump *ctx) {
    printf("\n\n[CRASH] === EXCEPCION DE HARDWARE CAPTURADA EN SWITCH ===\n");
    if (ctx) {
        u64 main_addr = (u64)&main;
        printf("[CRASH] error_desc=0x%08X esr=0x%08X (EC=0x%02X ISS=0x%07X) pstate=0x%08X\n",
               ctx->error_desc, ctx->esr, (ctx->esr >> 26) & 0x3Fu, ctx->esr & 0x1FFFFFFu, ctx->pstate);
        printf("[CRASH] PC=0x%016llX (main%+lld) LR=0x%016llX (main%+lld)\n",
               (unsigned long long)ctx->pc.x, (long long)(ctx->pc.x - main_addr),
               (unsigned long long)ctx->lr.x, (long long)(ctx->lr.x - main_addr));
        printf("[CRASH] SP=0x%016llX FP=0x%016llX FAR=0x%016llX main_addr=0x%016llX\n",
               (unsigned long long)ctx->sp.x, (unsigned long long)ctx->fp.x,
               (unsigned long long)ctx->far.x, (unsigned long long)main_addr);
        for (int i = 0; i < 28; i += 4) {
            printf("[CRASH] X%02d..X%02d: %016llX %016llX %016llX %016llX\n",
                   i, i + 3,
                   (unsigned long long)ctx->cpu_gprs[i].x,
                   (unsigned long long)ctx->cpu_gprs[i + 1].x,
                   (unsigned long long)ctx->cpu_gprs[i + 2].x,
                   (unsigned long long)ctx->cpu_gprs[i + 3].x);
        }
    }
    printf("[CRASH] PSX state: last_fn_entry=0x%08X cur_func=0x%08X last_store_pc=0x%08X cycles=%llu frame=%llu\n",
           g_psx_last_fn_entry, g_debug_current_func_addr, g_debug_last_store_pc,
           (unsigned long long)psx_cycle_count, (unsigned long long)s_frame_count);
    if (debug_cpu_ptr) {
        printf("[CRASH] PSX CPU: pc=0x%08X ra=0x%08X sp=0x%08X gp=0x%08X fp=0x%08X sr=0x%08X cause=0x%08X epc=0x%08X\n",
               debug_cpu_ptr->pc, debug_cpu_ptr->gpr[31], debug_cpu_ptr->gpr[29],
               debug_cpu_ptr->gpr[28], debug_cpu_ptr->gpr[30],
               debug_cpu_ptr->cop0[12], debug_cpu_ptr->cop0[13], debug_cpu_ptr->cop0[14]);
        for (int r = 0; r < 32; r += 4) {
            printf("[CRASH] PSX GPR[%02d..%02d]: %08X %08X %08X %08X\n",
                   r, r + 3,
                   debug_cpu_ptr->gpr[r], debug_cpu_ptr->gpr[r + 1],
                   debug_cpu_ptr->gpr[r + 2], debug_cpu_ptr->gpr[r + 3]);
        }
    }
    printf("[CRASH] Ultimos dispatches (g_disp_tail_seq=%llu):\n", (unsigned long long)g_disp_tail_seq);
    uint64_t tcount = g_disp_tail_seq < DISP_TAIL_CAP ? g_disp_tail_seq : DISP_TAIL_CAP;
    for (uint64_t i = g_disp_tail_seq - tcount; i < g_disp_tail_seq; i++) {
        const DispTailEntry* t = &g_disp_tail[i % DISP_TAIL_CAP];
        printf("  [tail %llu] target=0x%08X ra=0x%08X sp=0x%08X cyc=%llu\n",
               (unsigned long long)i, t->target, t->ra, t->sp, (unsigned long long)t->cycle);
    }
    if (s_sd_log) {
        fflush(s_sd_log);
        fclose(s_sd_log);
        s_sd_log = NULL;
    }
}
}

extern "C" {
int g_post_fmv_sync_active = 0;
}

static ssize_t log_devoptab_write(struct _reent *r, void *fd, const char *ptr, size_t len) {
    (void)r; (void)fd;
    if (s_sd_log) {
        fwrite(ptr, 1, len, s_sd_log);
        if (memmem(ptr, len, "FATAL", 5) || memmem(ptr, len, "ERROR", 5) || memmem(ptr, len, "ABORT", 5) || memmem(ptr, len, "PANIC", 5)) {
            fflush(s_sd_log);
            fsync(fileno(s_sd_log));
        }
    }
    if (s_nxlink_sock >= 0) {
        send(s_nxlink_sock, ptr, len, 0);
    }
    return (ssize_t)len;
}

static devoptab_t s_log_devoptab = {
    "logtee",
    0,
    NULL, NULL,
    log_devoptab_write,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    0,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL
};

// Callback de VBlank invocado por la GPU de PS1 a 60 FPS
static u64 s_last_vblank_tick = 0;
static uint64_t s_last_vblank_frame = 0;
static uint64_t s_last_dirty_insns = 0;
static uint64_t s_last_static_hits = 0;
static uint64_t s_last_dirty_dispatch_calls = 0;
static uint64_t s_last_dirty_dispatch_handled = 0;
static uint64_t s_last_dirty_dispatch_rejected = 0;
static uint64_t s_last_dirty_dispatch_time_ns = 0;
static uint64_t s_last_dirty_native_game_hits = 0;
static uint64_t s_last_dirty_native_game_misses = 0;
static uint64_t s_last_dirty_native_overlay_hits = 0;
static uint64_t s_last_dirty_clean_rejects = 0;
static uint64_t s_last_dirty_native_handoffs = 0;
static uint64_t s_last_dirty_aborts = 0;
static uint64_t s_last_dirty_guard_yields = 0;
static uint32_t s_last_mismatch_info[5] = {0};
static uint64_t s_last_mismatch_count = 0;

extern "C" {
    extern DirtyRamPcEntry g_dirty_ram_pc_window_table[DIRTY_RAM_PC_TABLE_SIZE];
    extern DirtyRamPcEntry g_dirty_native_game_miss_pc_window_table[DIRTY_RAM_PC_TABLE_SIZE];
    extern DirtyRamPcEntry g_dirty_native_game_miss_range_window_table[DIRTY_RAM_PC_TABLE_SIZE];
    extern uint64_t g_dirty_native_game_miss_reason_window[DIRTY_NATIVE_MISS_COUNT];
    extern DirtyRamPcEntry g_dirty_native_game_miss_reason_pc_window_table[DIRTY_NATIVE_MISS_COUNT][DIRTY_RAM_PC_TABLE_SIZE];
    const char *dirty_native_game_miss_reason_name(uint32_t reason);
    uint32_t openbios_entry_count(void);
    uint32_t openbios_entry_addr(uint32_t idx);
    uint32_t openbios_entry_hits(uint32_t idx);
    void openbios_clear_entry_hits(void);
    void psx_interrupt_check_path_diag(uint64_t *entry, uint64_t *fast_sr,
                                       uint64_t *fast_none, uint64_t *mid,
                                       uint64_t *eval, uint64_t *irq_deliv);
}

static bool is_bios_vector_noise(uint32_t pc) {
    return pc == 0x000000A0u || pc == 0x000000B0u || pc == 0x000000C0u;
}

extern "C" void psx_kernel_bless_stats(uint64_t out[6]);

static void collect_top_window_entries(const DirtyRamPcEntry* table,
                                       bool use_insns,
                                       uint32_t top_pc[3],
                                       uint64_t top_val[3]) {
    for (uint32_t i = 0; i < DIRTY_RAM_PC_TABLE_SIZE; i++) {
        uint32_t pc = table[i].pc;
        uint64_t val = use_insns ? table[i].insns : table[i].hits;
        if (!pc || !val) continue;
        if (is_bios_vector_noise(pc)) continue;
        for (int slot = 0; slot < 3; ++slot) {
            if (val > top_val[slot]) {
                for (int move = 2; move > slot; --move) {
                    top_val[move] = top_val[move - 1];
                    top_pc[move] = top_pc[move - 1];
                }
                top_val[slot] = val;
                top_pc[slot] = pc;
                break;
            }
        }
    }
}

static void collect_top_dirty_window_entries(const DirtyRamPcEntry* table,
                                             int max_entries,
                                             uint32_t* top_pc,
                                             uint64_t* top_insns,
                                             uint64_t* top_hits,
                                             uint64_t* out_total_insns,
                                             uint32_t* out_active_pcs) {
    uint64_t total_insns = 0;
    uint32_t active = 0;
    for (uint32_t i = 0; i < DIRTY_RAM_PC_TABLE_SIZE; i++) {
        uint32_t pc = table[i].pc;
        uint64_t insns = table[i].insns;
        uint64_t hits = table[i].hits;
        if (!pc || (!insns && !hits)) continue;
        if (is_bios_vector_noise(pc)) continue;
        total_insns += insns;
        active++;
        for (int slot = 0; slot < max_entries; ++slot) {
            if (insns > top_insns[slot]) {
                for (int move = max_entries - 1; move > slot; --move) {
                    top_insns[move] = top_insns[move - 1];
                    top_hits[move]  = top_hits[move - 1];
                    top_pc[move]    = top_pc[move - 1];
                }
                top_insns[slot] = insns;
                top_hits[slot]  = hits;
                top_pc[slot]    = pc;
                break;
            }
        }
    }
    if (out_total_insns) *out_total_insns = total_insns;
    if (out_active_pcs) *out_active_pcs = active;
}

/* ===== PROFILER: wall-time sampler (phase + guest PC histogram) ===== */
extern "C" {
extern int g_exec_phase;
extern volatile uint32_t g_prof_pc;
extern uint64_t g_prof_irq_calls;
extern uint64_t g_prof_svc_calls;

extern uint64_t g_prof_dev_sio_ns;
extern uint64_t g_prof_dev_cdrom_ns;
extern uint64_t g_prof_dev_dma_ns;
extern uint64_t g_prof_dev_timers_ns;
extern uint64_t g_prof_dev_irq_ns;
extern uint64_t g_prof_dev_sched_ns;
extern uint64_t g_prof_dev_total_ns;
extern uint64_t g_prof_dev_adv_calls;

extern uint64_t g_prof_iso_raw_calls;
extern uint64_t g_prof_iso_raw_bytes;
extern uint64_t g_prof_iso_raw_ns;
extern uint64_t g_prof_iso_raw_max_ns;
extern uint64_t g_prof_iso_cook_calls;
extern uint64_t g_prof_iso_cook_bytes;
extern uint64_t g_prof_iso_cook_ns;
extern uint64_t g_prof_iso_cook_max_ns;

extern uint64_t g_prof_iso_prefetch_hits;
extern uint64_t g_prof_iso_prefetch_miss;
extern uint64_t g_prof_iso_raw_seek_ns;
extern uint64_t g_prof_iso_raw_read_ns;

extern uint64_t g_prof_audio_spu_ns;
extern uint64_t g_prof_audio_audren_ns;
extern uint64_t g_prof_audio_underruns;
}

#define PROF_PHASES 14
static volatile uint64_t s_prof_phase_cnt[PROF_PHASES];
#define PROF_PC_SLOTS 8192
static uint32_t s_prof_pc_key[PROF_PC_SLOTS];
static uint32_t s_prof_pc_cnt[PROF_PC_SLOTS];
static Thread s_prof_thread;
static volatile bool s_prof_run = false;
static u64 s_prof_last_cb_end = 0, s_prof_guest_ticks = 0, s_prof_cb_ticks = 0, s_prof_frames = 0;
static uint64_t s_prof_last_irq = 0, s_prof_last_svc = 0;

static uint64_t s_prof_last_dev_sio = 0;
static uint64_t s_prof_last_dev_cdrom = 0;
static uint64_t s_prof_last_dev_dma = 0;
static uint64_t s_prof_last_dev_timers = 0;
static uint64_t s_prof_last_dev_irq = 0;
static uint64_t s_prof_last_dev_sched = 0;
static uint64_t s_prof_last_dev_total = 0;
static uint64_t s_prof_last_dev_adv = 0;

static uint64_t s_prof_last_iso_raw_calls = 0;
static uint64_t s_prof_last_iso_raw_bytes = 0;
static uint64_t s_prof_last_iso_raw_ns = 0;
static uint64_t s_prof_last_iso_cook_calls = 0;
static uint64_t s_prof_last_iso_cook_bytes = 0;
static uint64_t s_prof_last_iso_cook_ns = 0;

static uint64_t s_prof_last_prefetch_hits = 0;
static uint64_t s_prof_last_prefetch_miss = 0;
static uint64_t s_prof_last_iso_raw_seek_ns = 0;
static uint64_t s_prof_last_iso_raw_read_ns = 0;

static uint64_t s_prof_last_audio_spu = 0;
static uint64_t s_prof_last_audio_audren = 0;
static uint64_t s_prof_last_audio_underrun = 0;

static void prof_thread_fn(void*) {
    uint64_t last_seen_frame = 0;
    u64 last_frame_change_tick = armGetSystemTick();
    uint32_t watchdog_counter = 0;
    uint32_t stall_alert_count = 0;

    while (s_prof_run) {
        int ph = g_exec_phase;
        if (ph < 0 || ph >= PROF_PHASES) ph = 0;
        uint64_t c = s_prof_phase_cnt[ph];
        s_prof_phase_cnt[ph] = c + 1;
        if (ph != 7) {
            uint32_t pc = g_prof_pc & ~0xFu;   /* 16-byte buckets */
            uint32_t h = (pc >> 4) * 2654435761u;
            for (int i = 0; i < 16; i++) {
                uint32_t s = (h + i) & (PROF_PC_SLOTS - 1);
                if (s_prof_pc_key[s] == pc || s_prof_pc_cnt[s] == 0) {
                    s_prof_pc_key[s] = pc; s_prof_pc_cnt[s]++; break;
                }
            }
        }

        // Watchdog activo cada ~200 ms (1000 iteraciones x 0.2 ms)
        if (++watchdog_counter >= 1000) {
            watchdog_counter = 0;

            // Salida de emergencia inmediata incondicional con Plus + Minus desde el hilo Watchdog (Core 2)
            // Invocamos svcExitProcess() a nivel de kernel para salir instantáneamente a hbmenu sin deadlocks de libc.
            if (switch_input_check_immediate_exit()) {
                svcExitProcess();
            }

            u64 now_tick = armGetSystemTick();
            uint64_t cur_frame = s_frame_count;

            if (cur_frame != last_seen_frame) {
                last_seen_frame = cur_frame;
                last_frame_change_tick = now_tick;
                stall_alert_count = 0;
            } else if (cur_frame > 100) { // Evaluar unicamente tras el arranque
                u64 stall_ms = armTicksToNs(now_tick - last_frame_change_tick) / 1000000ULL;
                if (stall_ms >= 1500) { // Main thread bloqueado por mas de 1.5 segundos
                    stall_alert_count++;
                    if (stall_alert_count <= 10 || (stall_alert_count % 10) == 0) {
                        printf("\n[WATCHDOG_ALERT #%u] Main thread STALL! stall=%llu ms at frame=%llu (phase=%d g_prof_pc=0x%08X)\n",
                               stall_alert_count, (unsigned long long)stall_ms, (unsigned long long)cur_frame, ph, g_prof_pc);
                        if (debug_cpu_ptr) {
                            printf("  [CPU] pc=0x%08X ra=0x%08X sp=0x%08X v0=0x%08X a0=0x%08X\n",
                                   debug_cpu_ptr->pc, debug_cpu_ptr->gpr[31], debug_cpu_ptr->gpr[29],
                                   debug_cpu_ptr->gpr[2], debug_cpu_ptr->gpr[4]);
                        }
                        if (g_disp_tail_seq > 0) {
                            printf("  [Tail Dispatches]:\n");
                            uint64_t tcount = g_disp_tail_seq < 5 ? g_disp_tail_seq : 5;
                            for (uint64_t i = g_disp_tail_seq - tcount; i < g_disp_tail_seq; i++) {
                                const DispTailEntry* t = &g_disp_tail[i % DISP_TAIL_CAP];
                                printf("    [%llu] target=0x%08X ra=0x%08X sp=0x%08X cyc=%llu\n",
                                       (unsigned long long)i, t->target, t->ra, t->sp, (unsigned long long)t->cycle);
                            }
                        }
                        if (s_sd_log) fflush(s_sd_log);
                    }
                }
            }
        }

        svcSleepThread(200000);   /* ~0.2 ms */
    }
}

static void prof_start(void) {
    s_prof_run = true;
    if (R_SUCCEEDED(threadCreate(&s_prof_thread, prof_thread_fn, NULL, NULL, 0x4000, 0x3B, 2)))
        threadStart(&s_prof_thread);
    else
        printf("[Prof] threadCreate failed\n");
}

static void prof_report(uint64_t frame) {
    static const char* names[PROF_PHASES] = {
        "guest0", "dirtyinterp", "?2", "guest3", "gpu",
        "dev_other", "irqchk", "vblankcb",
        "cdrom_adv", "dma_adv", "timers_adv", "sio_adv", "irq_adv", "iso_io"
    };
    uint64_t tot = 0; for (int i = 0; i < PROF_PHASES; i++) tot += s_prof_phase_cnt[i];
    if (!tot) tot = 1;
    char line[512]; int n = 0;
    for (int i = 0; i < PROF_PHASES; i++)
        n += snprintf(line + n, sizeof(line) - n, "%s=%.1f%% ", names[i], 100.0 * s_prof_phase_cnt[i] / tot);
    double fr = s_prof_frames ? (double)s_prof_frames : 1.0;
    printf("[PROF] frame=%llu samples=%llu %s\n", (unsigned long long)frame, (unsigned long long)tot, line);
    printf("[PROF] per-frame: guest_between_vblanks=%.2fms vblank_cb=%.2fms irq_checks=%.0f dev_svc=%.0f\n",
           armTicksToNs(s_prof_guest_ticks) / 1e6 / fr, armTicksToNs(s_prof_cb_ticks) / 1e6 / fr,
           (double)(g_prof_irq_calls - s_prof_last_irq) / fr, (double)(g_prof_svc_calls - s_prof_last_svc) / fr);

    // Exact devsvc breakdown
    double d_tot = (double)(g_prof_dev_total_ns - s_prof_last_dev_total) / 1e6 / fr;
    double d_cdrom = (double)(g_prof_dev_cdrom_ns - s_prof_last_dev_cdrom) / 1e6 / fr;
    double d_dma = (double)(g_prof_dev_dma_ns - s_prof_last_dev_dma) / 1e6 / fr;
    double d_timers = (double)(g_prof_dev_timers_ns - s_prof_last_dev_timers) / 1e6 / fr;
    double d_sio = (double)(g_prof_dev_sio_ns - s_prof_last_dev_sio) / 1e6 / fr;
    double d_irq = (double)(g_prof_dev_irq_ns - s_prof_last_dev_irq) / 1e6 / fr;
    double d_sched = (double)(g_prof_dev_sched_ns - s_prof_last_dev_sched) / 1e6 / fr;
    double d_sub = d_cdrom + d_dma + d_timers + d_sio + d_irq + d_sched;
    double d_other = (d_tot > d_sub) ? (d_tot - d_sub) : 0.0;
    double d_adv = (double)(g_prof_dev_adv_calls - s_prof_last_dev_adv) / fr;

    printf("[PROF_DEVSVC] per-frame: total=%.2fms (cdrom=%.2fms dma=%.2fms timers=%.2fms sio=%.2fms irq_adv=%.2fms sched=%.2fms other=%.2fms) adv_calls=%.0f\n",
           d_tot, d_cdrom, d_dma, d_timers, d_sio, d_irq, d_sched, d_other, d_adv);

    // Exact IRQ check vs delivery diagnostic
    static uint64_t s_prof_last_irq_entry = 0, s_prof_last_irq_sr = 0, s_prof_last_irq_none = 0, s_prof_last_irq_mid = 0, s_prof_last_irq_deliv = 0;
    uint64_t irq_entry = 0, irq_sr = 0, irq_none = 0, irq_mid = 0, irq_eval = 0, irq_deliv = 0;
    psx_interrupt_check_path_diag(&irq_entry, &irq_sr, &irq_none, &irq_mid, &irq_eval, &irq_deliv);
    double d_irq_entry = (double)(irq_entry - s_prof_last_irq_entry) / fr;
    double d_irq_none  = (double)(irq_none - s_prof_last_irq_none) / fr;
    double d_irq_sr    = (double)(irq_sr - s_prof_last_irq_sr) / fr;
    double d_irq_mid   = (double)(irq_mid - s_prof_last_irq_mid) / fr;
    double d_irq_deliv = (double)(irq_deliv - s_prof_last_irq_deliv) / fr;
    printf("[IrqChkDiag] per-frame: checks=%.0f (none_pending=%.0f sr_masked=%.0f mid=%.0f) delivered=%.2f/f (%.2f%% actual work)\n",
           d_irq_entry, d_irq_none, d_irq_sr, d_irq_mid, d_irq_deliv,
           d_irq_entry > 0.0 ? (100.0 * d_irq_deliv / d_irq_entry) : 0.0);
    s_prof_last_irq_entry = irq_entry; s_prof_last_irq_sr = irq_sr; s_prof_last_irq_none = irq_none;
    s_prof_last_irq_mid = irq_mid; s_prof_last_irq_deliv = irq_deliv;

    // Exact CDROM ISO I/O breakdown
    uint64_t delta_raw_calls = g_prof_iso_raw_calls - s_prof_last_iso_raw_calls;
    uint64_t delta_raw_bytes = g_prof_iso_raw_bytes - s_prof_last_iso_raw_bytes;
    uint64_t delta_raw_ns = g_prof_iso_raw_ns - s_prof_last_iso_raw_ns;

    uint64_t delta_cook_calls = g_prof_iso_cook_calls - s_prof_last_iso_cook_calls;
    uint64_t delta_cook_bytes = g_prof_iso_cook_bytes - s_prof_last_iso_cook_bytes;
    uint64_t delta_cook_ns = g_prof_iso_cook_ns - s_prof_last_iso_cook_ns;

    uint64_t delta_prefetch_hits = g_prof_iso_prefetch_hits - s_prof_last_prefetch_hits;
    uint64_t delta_prefetch_miss = g_prof_iso_prefetch_miss - s_prof_last_prefetch_miss;
    uint64_t delta_raw_seek_ns = g_prof_iso_raw_seek_ns - s_prof_last_iso_raw_seek_ns;
    uint64_t delta_raw_read_ns = g_prof_iso_raw_read_ns - s_prof_last_iso_raw_read_ns;

    double d_iso_ms = (double)(delta_raw_ns + delta_cook_ns) / 1e6 / fr;
    double d_seek_ms = (double)delta_raw_seek_ns / 1e6 / fr;
    double d_read_ms = (double)delta_raw_read_ns / 1e6 / fr;
    double d_raw_calls = (double)delta_raw_calls / fr;
    double d_cook_calls = (double)delta_cook_calls / fr;
    double d_kb = (double)(delta_raw_bytes + delta_cook_bytes) / 1024.0 / fr;
    double avg_raw_ms = delta_raw_calls ? ((double)delta_raw_ns / 1e6 / (double)delta_raw_calls) : 0.0;
    double max_raw_ms = (double)g_prof_iso_raw_max_ns / 1e6;
    double avg_seek_ms = delta_raw_calls ? ((double)delta_raw_seek_ns / 1e6 / (double)delta_raw_calls) : 0.0;
    double avg_read_ms = delta_raw_calls ? ((double)delta_raw_read_ns / 1e6 / (double)delta_raw_calls) : 0.0;

    printf("[PROF_ISO_IO] per-frame: io_time=%.2fms (seek=%.2fms read=%.2fms) sectors=%.1f (raw=%.1f cook=%.1f) data=%.1fKB prefetch(hit=%llu miss=%llu) avg_raw(seek=%.3fms read=%.3fms tot=%.3fms) max_raw=%.2fms\n",
           d_iso_ms, d_seek_ms, d_read_ms, d_raw_calls + d_cook_calls, d_raw_calls, d_cook_calls, d_kb,
           (unsigned long long)delta_prefetch_hits, (unsigned long long)delta_prefetch_miss,
           avg_seek_ms, avg_read_ms, avg_raw_ms, max_raw_ms);

    uint64_t delta_spu_ns = g_prof_audio_spu_ns - s_prof_last_audio_spu;
    uint64_t delta_audren_ns = g_prof_audio_audren_ns - s_prof_last_audio_audren;
    uint64_t delta_underrun = g_prof_audio_underruns - s_prof_last_audio_underrun;
    double d_spu_ms = (double)delta_spu_ns / 1e6 / fr;
    double d_audren_ms = (double)delta_audren_ns / 1e6 / fr;
    printf("[AudioBreakdown] per-frame: spu_mix=%.3fms (Core 0) audren_dsp=%.3fms (Core 2) ring_underrun=%llu\n",
           d_spu_ms, d_audren_ms, (unsigned long long)delta_underrun);

    s_prof_last_dev_sio = g_prof_dev_sio_ns;
    s_prof_last_dev_cdrom = g_prof_dev_cdrom_ns;
    s_prof_last_dev_dma = g_prof_dev_dma_ns;
    s_prof_last_dev_timers = g_prof_dev_timers_ns;
    s_prof_last_dev_irq = g_prof_dev_irq_ns;
    s_prof_last_dev_sched = g_prof_dev_sched_ns;
    s_prof_last_dev_total = g_prof_dev_total_ns;
    s_prof_last_dev_adv = g_prof_dev_adv_calls;

    s_prof_last_iso_raw_calls = g_prof_iso_raw_calls;
    s_prof_last_iso_raw_bytes = g_prof_iso_raw_bytes;
    s_prof_last_iso_raw_ns = g_prof_iso_raw_ns;
    s_prof_last_iso_cook_calls = g_prof_iso_cook_calls;
    s_prof_last_iso_cook_bytes = g_prof_iso_cook_bytes;
    s_prof_last_iso_cook_ns = g_prof_iso_cook_ns;
    s_prof_last_prefetch_hits = g_prof_iso_prefetch_hits;
    s_prof_last_prefetch_miss = g_prof_iso_prefetch_miss;
    s_prof_last_iso_raw_seek_ns = g_prof_iso_raw_seek_ns;
    s_prof_last_iso_raw_read_ns = g_prof_iso_raw_read_ns;
    s_prof_last_audio_spu = g_prof_audio_spu_ns;
    s_prof_last_audio_audren = g_prof_audio_audren_ns;
    s_prof_last_audio_underrun = g_prof_audio_underruns;
    g_prof_iso_raw_max_ns = 0;
    g_prof_iso_cook_max_ns = 0;

    /* top 10 guest block PCs by samples */
    uint32_t tk[10] = {0}, tc[10] = {0};
    for (int i = 0; i < PROF_PC_SLOTS; i++) {
        uint32_t c = s_prof_pc_cnt[i]; if (!c) continue;
        for (int k = 0; k < 10; k++) if (c > tc[k]) {
            for (int m = 9; m > k; m--) { tc[m] = tc[m-1]; tk[m] = tk[m-1]; }
            tc[k] = c; tk[k] = s_prof_pc_key[i]; break;
        }
    }
    n = 0;
    for (int k = 0; k < 10 && tc[k]; k++)
        n += snprintf(line + n, sizeof(line) - n, "%08X:%u ", tk[k], tc[k]);
    printf("[PROF] topPC %s\n", line);
    memset((void*)s_prof_phase_cnt, 0, sizeof(s_prof_phase_cnt));
    memset(s_prof_pc_key, 0, sizeof(s_prof_pc_key));
    memset(s_prof_pc_cnt, 0, sizeof(s_prof_pc_cnt));
    s_prof_guest_ticks = s_prof_cb_ticks = s_prof_frames = 0;
    s_prof_last_irq = g_prof_irq_calls; s_prof_last_svc = g_prof_svc_calls;
}

extern "C" {
extern void memory_log_frame_prim_diag(uint32_t frame);
extern uint32_t g_mesh_3d_calls_frame;
extern uint32_t g_mesh_3d_prim_calls_frame;
extern uint32_t g_mesh_3d_prim_emitted_frame;
}

static void check_combat_stall_diagnostic(uint32_t frame) {
    static bool s_combat_ever_seen = false;
    static uint32_t s_stall_frames = 0;
    uint32_t vtx = Deko3DRenderer::Instance().GetLastFrameVtx();

    static uint32_t s_prev_vtx = 0;
    static uint32_t s_vtx_same_frames = 0;

    if (vtx > 3000) {
        s_combat_ever_seen = true;
    }

    bool is_stalled = false;
    if (s_combat_ever_seen) {
        if (vtx <= 50) {
            is_stalled = true;
        } else if (vtx == s_prev_vtx && g_mesh_3d_prim_calls_frame == 0) {
            s_vtx_same_frames++;
            if (s_vtx_same_frames >= 60) {
                is_stalled = true;
            }
        } else {
            s_vtx_same_frames = 0;
        }
    }
    s_prev_vtx = vtx;

    if (is_stalled) {
        s_stall_frames++;
        if (s_stall_frames == 1 || s_stall_frames == 60 || (s_stall_frames % 300) == 0) {
            printf("[CombatStallDiag] frame=%u stall_duration=%u vtx=%u mesh_calls=%u prim_calls=%u prim_emitted=%u\n",
                   frame, s_stall_frames, vtx,
                   g_mesh_3d_calls_frame, g_mesh_3d_prim_calls_frame, g_mesh_3d_prim_emitted_frame);
            printf("  [DMA2] total_nodes=%u empty_nodes=%u packets=%u words=%u start_addr=0x%08X chcr=0x%08X madr=0x%08X gpustat=0x%08X\n",
                   g_dma2_last_total_nodes, g_dma2_last_empty_nodes, g_dma2_last_packet_count, g_dma2_last_actual_words,
                   g_dma2_last_start_addr, dma_read(0x1F8010A8), dma_read(0x1F8010A0), gpu_read_gpustat());
            if (debug_cpu_ptr) {
                printf("  [CPU] pc=0x%08X ra=0x%08X sp=0x%08X fp=0x%08X v0=0x%08X a0=0x%08X a1=0x%08X\n",
                       debug_cpu_ptr->pc, debug_cpu_ptr->gpr[31], debug_cpu_ptr->gpr[29], debug_cpu_ptr->gpr[30],
                       debug_cpu_ptr->gpr[2], debug_cpu_ptr->gpr[4], debug_cpu_ptr->gpr[5]);
            }
            uint32_t p1_ptr = psx_read_word(0x800ADC20);
            uint32_t p2_ptr = psx_read_word(0x800ADC24);
            printf("  [P1 Actor 0x%08X] ", p1_ptr);
            if (p1_ptr >= 0x80000000 && p1_ptr < 0x80200000) {
                printf("anim=0x%08X f=%u hp=%d pos=(%d,%d,%d) facing=%d\n",
                       psx_read_word(p1_ptr + 0x54), (uint32_t)psx_read_half(p1_ptr + 0x58),
                       (int16_t)psx_read_half(p1_ptr + 0x3F4),
                       (int32_t)psx_read_word(p1_ptr + 0x908), (int32_t)psx_read_word(p1_ptr + 0x90C), (int32_t)psx_read_word(p1_ptr + 0x910),
                       (int16_t)psx_read_half(p1_ptr + 0x2A));
            } else {
                printf("invalid\n");
            }
            printf("  [P2 Actor 0x%08X] ", p2_ptr);
            if (p2_ptr >= 0x80000000 && p2_ptr < 0x80200000) {
                printf("anim=0x%08X f=%u hp=%d pos=(%d,%d,%d) facing=%d\n",
                       psx_read_word(p2_ptr + 0x54), (uint32_t)psx_read_half(p2_ptr + 0x58),
                       (int16_t)psx_read_half(p2_ptr + 0x3F4),
                       (int32_t)psx_read_word(p2_ptr + 0x908), (int32_t)psx_read_word(p2_ptr + 0x90C), (int32_t)psx_read_word(p2_ptr + 0x910),
                       (int16_t)psx_read_half(p2_ptr + 0x2A));
            } else {
                printf("invalid\n");
            }
            printf("  [Recent Dispatches (last 10)]:\n");
            uint64_t seq = g_fntrace_seq;
            uint32_t count = (seq > 10) ? 10 : (uint32_t)seq;
            for (uint32_t i = count; i > 0; --i) {
                uint64_t idx = (seq - i) % FNTRACE_RING_CAP;
                const FntraceEntry& e = g_fntrace_ring[idx];
                printf("    #%llu f=%u tgt=0x%08X ra=0x%08X a0=0x%08X a1=0x%08X sp=0x%08X\n",
                       (unsigned long long)(seq - i), e.frame, e.target, e.ra, e.a0, e.a1, e.sp);
            }
        }
    } else {
        s_stall_frames = 0;
    }

    g_mesh_3d_calls_frame = 0;
    g_mesh_3d_prim_calls_frame = 0;
    g_mesh_3d_prim_emitted_frame = 0;
}

static void switch_vblank_callback(void) {
    u64 frame_start_tick = armGetSystemTick();
    if (s_last_cb_exit_tick != 0) {
        s_win_guest_ns += armTicksToNs(frame_start_tick - s_last_cb_exit_tick);
    }
    if (s_prof_last_cb_end) s_prof_guest_ticks += frame_start_tick - s_prof_last_cb_end;
    int prof_prev_phase = g_exec_phase;
    g_exec_phase = 7;
    struct ProfGuard { u64 st; int prev; ~ProfGuard() {
        g_exec_phase = prev;
        u64 e = armGetSystemTick();
        s_prof_cb_ticks += e - st; s_prof_last_cb_end = e; s_prof_frames++;
    } } prof_guard{frame_start_tick, prof_prev_phase};
    s_frame_count++;
    memory_log_frame_prim_diag((uint32_t)s_frame_count);
    check_combat_stall_diagnostic((uint32_t)s_frame_count);
    static bool s_game_started_logged = false;
    if (!s_game_started_logged && fntrace_is_game_started()) {
        s_game_started_logged = true;
        printf("[Display] Juego iniciado (entry_pc alcanzado) frame %llu! Modo %s con %ux Supersampling activo\n",
               (unsigned long long)s_frame_count,
               g_config.widescreen_16_9 ? "16:9 Widescreen" : "4:3 HD",
               g_config.supersampling);
        fflush(stdout);
    }

    u64 now_tick = armGetSystemTick();
    if (s_last_vblank_tick == 0) {
        s_last_vblank_tick = now_tick;
        s_last_vblank_frame = s_frame_count;
    }

    const uint64_t report_interval = (s_frame_count <= 360) ? 120 : 600;

    if (s_frame_count <= 5 || s_frame_count == 60 || (s_frame_count % report_interval) == 0) {
        extern uint64_t g_dirty_dispatch_calls;
        extern uint64_t g_dirty_dispatch_handled;
        extern uint64_t g_dirty_dispatch_rejected;
        extern uint64_t g_dirty_dispatch_time_ns;
        extern uint64_t g_dirty_native_game_hits;
        extern uint64_t g_dirty_native_game_misses;
        extern uint64_t g_dirty_native_overlay_hits;
        extern uint64_t g_dirty_clean_rejects;
        extern uint64_t g_dirty_ram_aborts;
        extern uint64_t g_dirty_ram_guard_yields;
        extern uint64_t g_dirty_ram_native_handoffs;
        u64 elapsed_ns = armTicksToNs(now_tick - s_last_vblank_tick);
        uint64_t frames_delta = s_frame_count - s_last_vblank_frame;
        double avg_ms = (frames_delta > 0) ? ((double)elapsed_ns / 1e6 / (double)frames_delta) : 0.0;
        double fps = (avg_ms > 0.0) ? (1000.0 / avg_ms) : 0.0;
        uint64_t d_insns = g_dirty_ram_insns_run - s_last_dirty_insns;
        uint64_t d_static = g_dispatch_static_hits - s_last_static_hits;
        uint64_t d_dirty_calls = g_dirty_dispatch_calls - s_last_dirty_dispatch_calls;
        uint64_t d_dirty_handled = g_dirty_dispatch_handled - s_last_dirty_dispatch_handled;
        uint64_t d_dirty_rejected = g_dirty_dispatch_rejected - s_last_dirty_dispatch_rejected;
        uint64_t d_dirty_time_ns = g_dirty_dispatch_time_ns - s_last_dirty_dispatch_time_ns;
        uint64_t d_native_game_hits = g_dirty_native_game_hits - s_last_dirty_native_game_hits;
        uint64_t d_native_game_misses = g_dirty_native_game_misses - s_last_dirty_native_game_misses;
        uint64_t d_native_overlay_hits = g_dirty_native_overlay_hits - s_last_dirty_native_overlay_hits;
        uint64_t d_dirty_clean_rejects = g_dirty_clean_rejects - s_last_dirty_clean_rejects;
        uint64_t d_dirty_native_handoffs = g_dirty_ram_native_handoffs - s_last_dirty_native_handoffs;
        uint64_t d_dirty_aborts = g_dirty_ram_aborts - s_last_dirty_aborts;
        uint64_t d_dirty_guard_yields = g_dirty_ram_guard_yields - s_last_dirty_guard_yields;
        uint64_t mism_cnt = 0;
        uint32_t mism_info[5] = {0};
        dirty_ram_text_exact_mismatch_stats(&mism_cnt, mism_info);

        printf("[VBlank] frame=%llu fps=%.1f (%.2f ms/f) static_d=%llu dirty_insns_d=%llu (tot=%llu) dirty_calls=%llu handled=%llu rejected=%llu dirty_ms=%.3f native(game h/m=%llu/%llu ovl=%llu handoff=%llu) abort=%llu yield=%llu clean_reject=%llu idle_skips=%llu mism=%llu\n",
               (unsigned long long)s_frame_count,
               fps, avg_ms,
               (unsigned long long)d_static,
               (unsigned long long)d_insns,
               (unsigned long long)g_dirty_ram_insns_run,
               (unsigned long long)d_dirty_calls,
               (unsigned long long)d_dirty_handled,
               (unsigned long long)d_dirty_rejected,
               (double)d_dirty_time_ns / 1e6,
               (unsigned long long)d_native_game_hits,
               (unsigned long long)d_native_game_misses,
               (unsigned long long)d_native_overlay_hits,
               (unsigned long long)d_dirty_native_handoffs,
               (unsigned long long)d_dirty_aborts,
               (unsigned long long)d_dirty_guard_yields,
               (unsigned long long)d_dirty_clean_rejects,
               (unsigned long long)g_idle_skip_count,
               (unsigned long long)mism_cnt);
        if (mism_cnt > 0 && (mism_cnt != s_last_mismatch_count || memcmp(mism_info, s_last_mismatch_info, sizeof(mism_info)) != 0)) {
            printf("  [Mismatch] range=0x%08X+%u at=0x%08X live=0x%02X ref=0x%02X\n",
                   mism_info[0], mism_info[1], mism_info[2], mism_info[3], mism_info[4]);
            memcpy(s_last_mismatch_info, mism_info, sizeof(mism_info));
        }
        if ((s_frame_count % report_interval) == 0) {
            uint32_t n = psx_game_entry_count(), t[5] = {0,0,0,0,0}, c[5] = {0,0,0,0,0};
            for (uint32_t i = 0; i < n; i++) {
                uint32_t h = g_game_entry_hits[i];
                if (!h) continue;
                for (int k = 0; k < 5; k++) if (h > c[k]) {
                    for (int m = 4; m > k; m--) { c[m] = c[m-1]; t[m] = t[m-1]; }
                    c[k] = h; t[k] = psx_game_entry_addr(i); break;
                }
            }
            printf("  [HotFn] %08X:%u %08X:%u %08X:%u %08X:%u %08X:%u\n", t[0],c[0],t[1],c[1],t[2],c[2],t[3],c[3],t[4],c[4]);
            memset(g_game_entry_hits, 0, n * sizeof(uint32_t));

            // Hot functions in OpenBIOS (measures the 3M static calls during boot/BIOS)
            uint32_t ob_n = openbios_entry_count(), ob_t[5] = {0,0,0,0,0}, ob_c[5] = {0,0,0,0,0};
            for (uint32_t i = 0; i < ob_n; i++) {
                uint32_t h = openbios_entry_hits(i);
                if (!h) continue;
                for (int k = 0; k < 5; k++) if (h > ob_c[k]) {
                    for (int m = 4; m > k; m--) { ob_c[m] = ob_c[m-1]; ob_t[m] = ob_t[m-1]; }
                    ob_c[k] = h; ob_t[k] = openbios_entry_addr(i); break;
                }
            }
            if (ob_c[0] > 0) {
                printf("  [OpenBiosHotFn] %08X:%u %08X:%u %08X:%u %08X:%u %08X:%u\n",
                       ob_t[0], ob_c[0], ob_t[1], ob_c[1], ob_t[2], ob_c[2], ob_t[3], ob_c[3], ob_t[4], ob_c[4]);
                openbios_clear_entry_hits();
            }
        }
        if (d_native_game_misses > 0 && (s_frame_count % report_interval) == 0) {
            uint32_t miss_pc[3] = {0, 0, 0};
            uint64_t miss_hits[3] = {0, 0, 0};
            uint32_t miss_range[3] = {0, 0, 0};
            uint64_t miss_range_hits[3] = {0, 0, 0};
            collect_top_window_entries(g_dirty_native_game_miss_pc_window_table, false, miss_pc, miss_hits);
            collect_top_window_entries(g_dirty_native_game_miss_range_window_table, false, miss_range, miss_range_hits);
            printf("  [NativeMissTop] pc1=0x%08X h1=%llu pc2=0x%08X h2=%llu pc3=0x%08X h3=%llu "
                   "range1=0x%08X h1=%llu range2=0x%08X h2=%llu range3=0x%08X h3=%llu",
                   miss_pc[0], (unsigned long long)miss_hits[0],
                   miss_pc[1], (unsigned long long)miss_hits[1],
                   miss_pc[2], (unsigned long long)miss_hits[2],
                   miss_range[0], (unsigned long long)miss_range_hits[0],
                   miss_range[1], (unsigned long long)miss_range_hits[1],
                   miss_range[2], (unsigned long long)miss_range_hits[2]);
            if (mism_cnt > 0) {
                printf(" mism_range=0x%08X+%u at=0x%08X", mism_info[0], mism_info[1], mism_info[2]);
            }
            printf("\n");
            printf("  [NativeMissReasonTop]");
            for (uint32_t r = 0; r < DIRTY_NATIVE_MISS_COUNT; ++r) {
                uint32_t reason_pc[3] = {0, 0, 0};
                uint64_t reason_hits[3] = {0, 0, 0};
                collect_top_window_entries(g_dirty_native_game_miss_reason_pc_window_table[r], false, reason_pc, reason_hits);
                printf(" %s=%llu pc1=0x%08X h1=%llu pc2=0x%08X h2=%llu pc3=0x%08X h3=%llu",
                       dirty_native_game_miss_reason_name(r),
                       (unsigned long long)g_dirty_native_game_miss_reason_window[r],
                       reason_pc[0], (unsigned long long)reason_hits[0],
                       reason_pc[1], (unsigned long long)reason_hits[1],
                       reason_pc[2], (unsigned long long)reason_hits[2]);
            }
            printf("\n");
            memset(g_dirty_native_game_miss_pc_window_table, 0, sizeof(g_dirty_native_game_miss_pc_window_table));
            memset(g_dirty_native_game_miss_range_window_table, 0, sizeof(g_dirty_native_game_miss_range_window_table));
            memset(g_dirty_native_game_miss_reason_window, 0, sizeof(uint64_t) * DIRTY_NATIVE_MISS_COUNT);
            memset(g_dirty_native_game_miss_reason_pc_window_table, 0, sizeof(DirtyRamPcEntry) * DIRTY_NATIVE_MISS_COUNT * DIRTY_RAM_PC_TABLE_SIZE);
        }
        if (d_insns > 0 && (s_frame_count % report_interval) == 0 && s_frame_count != s_last_hot_pc_frame) {
            uint32_t top_pc[8] = {0};
            uint64_t top_in[8] = {0};
            uint64_t top_hits[8] = {0};
            uint64_t win_insns_tot = 0;
            uint32_t win_active_pcs = 0;
            collect_top_dirty_window_entries(g_dirty_ram_pc_window_table, 8, top_pc, top_in, top_hits, &win_insns_tot, &win_active_pcs);
            printf("  [DirtyTop] win (tot_insns=%llu in %u PCs):\n", (unsigned long long)win_insns_tot, win_active_pcs);
            for (int k = 0; k < 8 && top_pc[k] != 0; ++k) {
                double pct = win_insns_tot ? (100.0 * (double)top_in[k] / (double)win_insns_tot) : 0.0;
                printf("    #%d pc=0x%08X insns=%llu (%.1f%%) calls=%llu\n",
                       k + 1, top_pc[k], (unsigned long long)top_in[k], pct, (unsigned long long)top_hits[k]);
            }
            if (top_pc[0] != 0 && top_pc[0] != s_last_hot_pc) {
                printf("[HotPC] current=0x%08X insns=%llu frame=%llu\n",
                       top_pc[0], (unsigned long long)top_in[0], (unsigned long long)s_frame_count);
                s_last_hot_pc = top_pc[0];
            }
            memset(g_dirty_ram_pc_window_table, 0, sizeof(g_dirty_ram_pc_window_table));
            s_last_hot_pc_frame = s_frame_count;
        }
        if ((s_frame_count % report_interval) == 0) {
            uint64_t kb[6] = {0};
            psx_kernel_bless_stats(kb);
            printf("  [KBless] total=%llu clean=%llu mism=%llu hits=%llu verif=%llu invals=%llu\n",
                   (unsigned long long)kb[0], (unsigned long long)kb[1], (unsigned long long)kb[2],
                   (unsigned long long)kb[3], (unsigned long long)kb[4], (unsigned long long)kb[5]);
        }

        s_last_vblank_tick = now_tick;
        s_last_vblank_frame = s_frame_count;
        s_last_dirty_insns = g_dirty_ram_insns_run;
        s_last_static_hits = g_dispatch_static_hits;
        s_last_dirty_dispatch_calls = g_dirty_dispatch_calls;
        s_last_dirty_dispatch_handled = g_dirty_dispatch_handled;
        s_last_dirty_dispatch_rejected = g_dirty_dispatch_rejected;
        s_last_dirty_dispatch_time_ns = g_dirty_dispatch_time_ns;
        s_last_dirty_native_game_hits = g_dirty_native_game_hits;
        s_last_dirty_native_game_misses = g_dirty_native_game_misses;
        s_last_dirty_native_overlay_hits = g_dirty_native_overlay_hits;
        s_last_dirty_clean_rejects = g_dirty_clean_rejects;
        s_last_dirty_native_handoffs = g_dirty_ram_native_handoffs;
        s_last_dirty_aborts = g_dirty_ram_aborts;
        s_last_dirty_guard_yields = g_dirty_ram_guard_yields;
        s_last_mismatch_count = mism_cnt;
        if (s_sd_log) fflush(s_sd_log);
    }

    // Flush regular cada 60 frames (1 segundo) para no perder logs ante cualquier freeze inesperado
    static uint32_t s_regular_flush_counter = 0;
    if (++s_regular_flush_counter >= 60) {
        s_regular_flush_counter = 0;
        if (s_sd_log) fflush(s_sd_log);
    }

    // 1. Actualizar mandos y estados de botones siempre primero
    u64 input_start = armGetSystemTick();
    switch_input_update();
    u64 input_end = armGetSystemTick();
    u64 input_ns = armTicksToNs(input_end - input_start);
    s_win_input_ns += input_ns;

    // 2. Manejo de bucle principal y salida rápida con botones (Plus + Minus)
    if (!appletMainLoop() || switch_input_is_exit_requested()) {
        printf("[VBlank] Solicitud de salida detectada en frame %llu. Saliendo...\n",
               (unsigned long long)s_frame_count);
        if (s_sd_log) fflush(s_sd_log);
        memcard_flush_all();
        exit(0);
    }

    // 3. Si la aplicación pierde el foco (menú HOME o suspensión), ceder CPU y no intentar presentar a la GPU
    if (appletGetFocusState() != AppletFocusState_InFocus) {
        svcSleepThread(16000000ULL); // ~16 ms
        return;
    }

    // 4. Detección de salida de FMV y latido de sincronización post-FMV
    GpuDisplayInfo cur_di{};
    gpu_get_display_info(&cur_di);
    static int s_prev_depth24 = -1;
    if (s_prev_depth24 == 1 && cur_di.depth24 == 0) {
        printf("[PostFMV] Fin de FMV detectado en frame %llu! Activando diagnostico sincronico fsync por 600 frames...\n",
               (unsigned long long)s_frame_count);
        g_post_fmv_sync_active = 600;
        fflush(stdout);
    }
    s_prev_depth24 = cur_di.depth24;

    if (g_post_fmv_sync_active > 0) {
        g_post_fmv_sync_active--;
        printf("[PostFMV_Tick] frame=%llu vtx=%u disp=(%u,%u %ux%u) cd_read=%d\n",
               (unsigned long long)s_frame_count,
               Deko3DRenderer::Instance().GetLastFrameVtx(),
               cur_di.display_x, cur_di.display_y, cur_di.width, cur_di.height,
               cdrom_is_reading());
        fflush(stdout);
    }

    // 5. Bombeo de audio hacia el subsistema nativo audren
    u64 audio_start = armGetSystemTick();
    switch_audio_update();
    u64 audio_end = armGetSystemTick();
    u64 audio_ns = armTicksToNs(audio_end - audio_start);
    s_win_audio_ns += audio_ns;

    // 6. Renderizar y presentar frame con Deko3D
    u64 render_start = armGetSystemTick();
    Deko3DRenderer::Instance().RenderDisplay(nullptr, 1280, 0, 0, 1280, 720);
    u64 render_end = armGetSystemTick();
    u64 render_ns = armTicksToNs(render_end - render_start);
    s_win_render_ns += render_ns;

    u64 frame_end_tick = armGetSystemTick();
    u64 total_cb_ns = armTicksToNs(frame_end_tick - frame_start_tick);
    s_win_loop_ns += total_cb_ns;

    u64 active_work_ns = input_ns + audio_ns + render_ns;
    u64 other_ns = (total_cb_ns > active_work_ns) ? (total_cb_ns - active_work_ns) : 0;
    s_win_other_ns += other_ns;
    s_win_samples++;
    s_last_cb_exit_tick = frame_end_tick;

    if (s_frame_count <= 5 || (s_frame_count % report_interval) == 0) {
        double denom = s_win_samples ? (double)s_win_samples : 1.0;
        auto rt = Deko3DRenderer::Instance().GetAndResetRenderTimings();
        double rt_denom = rt.samples ? (double)rt.samples : 1.0;
        printf("[PerfAvg] frame=%llu win%llu: loop=%.3fms (input=%.3fms audio=%.3fms render=%.3fms other=%.3fms) guest_between=%.3fms | render_detail(prep=%.3fms submit=%.3fms pres=%.3fms wait=%.3fms [gpu_fence=%.3fms acq_wait=%.3fms vram_fence=%.3fms vram_up=%.3fms %uKB])\n",
               (unsigned long long)s_frame_count,
               (unsigned long long)report_interval,
               (double)s_win_loop_ns / denom / 1e6,
               (double)s_win_input_ns / denom / 1e6,
               (double)s_win_audio_ns / denom / 1e6,
               (double)s_win_render_ns / denom / 1e6,
               (double)s_win_other_ns / denom / 1e6,
               (double)s_win_guest_ns / denom / 1e6,
               (double)rt.vram_prep_ns / rt_denom / 1e6,
               (double)rt.submit_ns / rt_denom / 1e6,
               (double)rt.present_ns / rt_denom / 1e6,
               (double)rt.fence_wait_ns / rt_denom / 1e6,
               (double)rt.gpu_fence_wait_ns / rt_denom / 1e6,
               (double)rt.acq_wait_ns / rt_denom / 1e6,
               (double)rt.vram_fence_wait_ns / rt_denom / 1e6,
               (double)rt.vram_upload_ns / rt_denom / 1e6,
               (unsigned)((double)rt.vram_upload_bytes / rt_denom / 1024.0));

        s_win_loop_ns = 0;
        s_win_input_ns = 0;
        s_win_audio_ns = 0;
        s_win_render_ns = 0;
        s_win_other_ns = 0;
        s_win_guest_ns = 0;
        s_win_samples = 0;
    }
    if ((s_frame_count % report_interval) == 0) {
        prof_report(s_frame_count);
        tek_block_prof_report_and_reset(s_frame_count);
    }
}

// ============================================================================
// Funciones de Configuración Gráfica Dinámica (config.txt)
// ============================================================================
static void trim_string(char* str) {
    if (!str) return;
    char* start = str;
    while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n') start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r' || str[len - 1] == '\n')) {
        str[--len] = '\0';
    }
}

static void to_lower_case(char* str) {
    for (; *str; ++str) *str = (char)tolower((unsigned char)*str);
}

static void load_or_create_config(int argc, char* argv[]) {
    char target_config_path[512] = {0};

    // Intentar deducir la ruta al lado del .nro desde argv[0]
    if (argc > 0 && argv && argv[0] && argv[0][0] != '\0') {
        strncpy(target_config_path, argv[0], sizeof(target_config_path) - 1);
        char* last_slash = strrchr(target_config_path, '/');
        char* last_backslash = strrchr(target_config_path, '\\');
        char* sep = (last_slash > last_backslash) ? last_slash : last_backslash;
        if (sep) {
            *(sep + 1) = '\0';
            strncat(target_config_path, "config.txt", sizeof(target_config_path) - strlen(target_config_path) - 1);
        } else {
            snprintf(target_config_path, sizeof(target_config_path), "config.txt");
        }
    } else {
        snprintf(target_config_path, sizeof(target_config_path), "sdmc:/switch/tekken3/config.txt");
    }

    const char* candidate_paths[] = {
        target_config_path,
        "sdmc:/switch/tekken3/config.txt",
        "config.txt"
    };

    FILE* f = nullptr;
    const char* found_path = nullptr;
    for (size_t i = 0; i < sizeof(candidate_paths) / sizeof(candidate_paths[0]); i++) {
        if (!candidate_paths[i] || candidate_paths[i][0] == '\0') continue;
        f = fopen(candidate_paths[i], "r");
        if (f) {
            found_path = candidate_paths[i];
            break;
        }
    }

    if (!f) {
        printf("[Config] No se encontro 'config.txt'. Creando archivo por defecto en '%s'...\n", target_config_path);
        FILE* create_f = fopen(target_config_path, "w");
        if (!create_f) {
            create_f = fopen("sdmc:/switch/tekken3/config.txt", "w");
            if (create_f) strncpy(target_config_path, "sdmc:/switch/tekken3/config.txt", sizeof(target_config_path) - 1);
        }
        if (create_f) {
            fprintf(create_f,
                "# ==============================================================================\n"
                "# Tekken 3 (USA) - Graphics Configuration for Nintendo Switch\n"
                "# ==============================================================================\n"
                "# Place this file in the same directory as 'tekken3_nx.nro' on your microSD card\n"
                "# (e.g. sdmc:/switch/tekken3/config.txt).\n"
                "# If this file is missing, the game will automatically create it with default values.\n"
                "#\n"
                "# ------------------------------------------------------------------------------\n"
                "# Internal Supersampling Anti-Aliasing (SSAA):\n"
                "#   1 = 1x PS1 Native (1024x512)\n"
                "#   2 = 2x HD (2048x1024) [RECOMMENDED]\n"
                "#       -> Sharp edges, optimal configuration\n"
                "#   3 = 3x Ultra HD (3072x1536)\n"
                "#       -> Maximum polygon edge definition for 4K / large TV screens (Switch has a 1080p limit, so it is overkill and may cause FPS drops to 55 FPS)\n"
                "# ------------------------------------------------------------------------------\n"
                "supersampling = 2\n\n"
                "# ------------------------------------------------------------------------------\n"
                "# Screen Aspect Ratio:\n"
                "#   OFF = 4:3 Original with black sidebars (Pillarbox) [RECOMMENDED]\n"
                "#         -> Preserves authentic retro proportions for characters and stages.\n"
                "#   ON  = 16:9 Fullscreen Widescreen\n"
                "#         -> Fills the entire Switch screen and 16:9 TV display without bars.\n"
                "# ------------------------------------------------------------------------------\n"
                "widescreen_16_9 = OFF\n"
            );
            fclose(create_f);
            printf("[Config] Archivo 'config.txt' creado exitosamente con valores por defecto (supersampling=2, widescreen=OFF).\n");
        } else {
            printf("[Config] Advertencia: No se pudo crear 'config.txt'. Usando valores predeterminados en memoria.\n");
        }
        g_config.supersampling = 2;
        g_config.widescreen_16_9 = false;
        return;
    }

    printf("[Config] Cargando configuracion grafica desde: '%s'\n", found_path);
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        trim_string(line);
        if (line[0] == '\0' || line[0] == '#' || line[0] == ';') continue;

        char* eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char* key = line;
        char* val = eq + 1;
        trim_string(key);
        trim_string(val);
        to_lower_case(key);
        to_lower_case(val);

        if (strstr(key, "supersampling") || strstr(key, "scale") || strstr(key, "ssaa")) {
            int s = atoi(val);
            if (s >= 1 && s <= 3) {
                g_config.supersampling = s;
            }
        } else if (strstr(key, "widescreen") || strstr(key, "16:9") || strstr(key, "aspect")) {
            if (strcmp(val, "on") == 0 || strcmp(val, "1") == 0 || strcmp(val, "true") == 0 ||
                strcmp(val, "yes") == 0 || strcmp(val, "wide") == 0 || strcmp(val, "16:9") == 0) {
                g_config.widescreen_16_9 = true;
            } else if (strcmp(val, "off") == 0 || strcmp(val, "0") == 0 || strcmp(val, "false") == 0 ||
                       strcmp(val, "no") == 0 || strcmp(val, "4:3") == 0) {
                g_config.widescreen_16_9 = false;
            }
        }
    }
    fclose(f);

    printf("[Config] Configuracion cargada con exito:\n");
    printf("   [+] supersampling   = %d (%s)\n", g_config.supersampling,
           g_config.supersampling == 3 ? "3x Ultra HD (3072x1536)" :
           (g_config.supersampling == 2 ? "2x HD (2048x1024)" : "1x Nativo (1024x512)"));
    printf("   [+] widescreen_16_9 = %s\n", g_config.widescreen_16_9 ? "ON (16:9 Panoramica)" : "OFF (4:3 Original Pillarbox)");
}

// ============================================================================
// Detección Universal de Disco (.cue, .bin, .iso, .chd)
// ============================================================================
static bool try_mount_disc_file(const std::string& path, std::string& out_mounted) {
    if (path.empty()) return false;
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return false;
    fclose(f);

    printf("[DiscDetect] Probando montaje: '%s'...\n", path.c_str());
    cdrom_init(path.c_str());
    cdrom_set_disc_scex("SCEA");
    cdrom_set_speed(4);
    cdrom_set_game_speed(1);

    if (cdrom_has_disc()) {
        out_mounted = path;
        printf("[DiscDetect] [+] ¡Montaje exitoso!\n");
        return true;
    }
    printf("[DiscDetect] [-] No se pudo montar como disco valido.\n");
    return false;
}

static std::string detect_and_mount_disc(int argc, char* argv[]) {
    std::string mounted;

    // Directorios donde buscar imágenes de disco
    std::vector<std::string> search_dirs;
    if (argc > 0 && argv && argv[0] && argv[0][0] != '\0') {
        std::string nro_dir = argv[0];
        size_t last_sep = nro_dir.find_last_of("/\\");
        if (last_sep != std::string::npos) {
            std::string dir = nro_dir.substr(0, last_sep);
            search_dirs.push_back(dir + "/disc");
            search_dirs.push_back(dir);
        }
    }
    search_dirs.push_back("sdmc:/switch/tekken3/disc");
    search_dirs.push_back("sdmc:/switch/tekken3");
    search_dirs.push_back("disc");
    search_dirs.push_back(".");

    // Paso 1: Probar primero nombres exactos estándar
    const char* exact_candidates[] = {
        "sdmc:/switch/tekken3/disc/Tekken 3 (USA).cue",
        "sdmc:/switch/tekken3/disc/Tekken 3.cue",
        "sdmc:/switch/tekken3/disc/tekken3.cue",
        "sdmc:/switch/tekken3/Tekken 3 (USA).cue",
        "sdmc:/switch/tekken3/Tekken 3.cue",
        "sdmc:/switch/tekken3/disc/Tekken 3 (USA) (Track 1).bin",
        "disc/Tekken 3 (USA).cue"
    };
    for (const char* exact : exact_candidates) {
        if (try_mount_disc_file(exact, mounted)) {
            printf("[DiscDetect] Disco encontrado por coincidencia conocida: '%s'\n", mounted.c_str());
            return mounted;
        }
    }

    // Paso 2: Escaneo dinámico en búsqueda de CUALQUIER archivo .cue
    for (const auto& dir_path : search_dirs) {
        DIR* d = opendir(dir_path.c_str());
        if (!d) continue;

        struct dirent* ent;
        while ((ent = readdir(d)) != nullptr) {
            const char* name = ent->d_name;
            if (name[0] == '.') continue;
            size_t len = strlen(name);
            if (len > 4 && strcasecmp(name + len - 4, ".cue") == 0) {
                std::string full_path = dir_path + "/" + name;
                if (try_mount_disc_file(full_path, mounted)) {
                    printf("[DiscDetect] ¡Disco detectado automaticamente (.cue): '%s'!\n", mounted.c_str());
                    closedir(d);
                    return mounted;
                }
            }
        }
        closedir(d);
    }

    // Paso 3: Escaneo dinámico en búsqueda de CUALQUIER imagen directa (.chd, .bin, .iso, .img)
    const char* image_exts[] = { ".chd", ".bin", ".iso", ".img" };
    for (const auto& dir_path : search_dirs) {
        DIR* d = opendir(dir_path.c_str());
        if (!d) continue;

        struct dirent* ent;
        while ((ent = readdir(d)) != nullptr) {
            const char* name = ent->d_name;
            if (name[0] == '.') continue;
            size_t len = strlen(name);
            for (const char* ext : image_exts) {
                size_t ext_len = strlen(ext);
                if (len > ext_len && strcasecmp(name + len - ext_len, ext) == 0) {
                    std::string full_path = dir_path + "/" + name;
                    if (try_mount_disc_file(full_path, mounted)) {
                        printf("[DiscDetect] ¡Disco detectado automaticamente (imagen): '%s'!\n", mounted.c_str());
                        closedir(d);
                        return mounted;
                    }
                }
            }
        }
        closedir(d);
    }

    return "";
}

int main(int argc, char* argv[]) {
    // 1. Asegurar directorios de log en la microSD y abrir archivo persistente
    mkdir("sdmc:/switch", 0777);
    mkdir("sdmc:/switch/tekken3", 0777);
    mkdir("sdmc:/switch/tekken3/disc", 0777);
    s_sd_log = fopen("sdmc:/switch/tekken3/tekken3.log", "w");
    if (s_sd_log) {
        static char s_sd_log_buf[64 * 1024];
        setvbuf(s_sd_log, s_sd_log_buf, _IOFBF, sizeof(s_sd_log_buf));
    }

    // 2. Intentar inicializar soporte de red y conectar con nxlink en la PC
    Result sock_rc = socketInitializeDefault();
    s_nxlink_sock = nxlinkConnectToHost(false, false);

    // 3. Redirigir stdout y stderr a nivel de devoptab para capturar TODOS los printf del ejecutable
    devoptab_list[STD_OUT] = &s_log_devoptab;
    devoptab_list[STD_ERR] = &s_log_devoptab;
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    printf("\n============================================\n");
    printf("   TEKKEN 3 (USA) - Nintendo Switch Port\n");
    printf("   Static Recompilation con Deko3D + audren\n");
    printf("   Build: %s %s (v27 - Clean Audio + Right Edge Inset Fix)\n", __DATE__, __TIME__);
    printf("============================================\n\n");
    printf("[Main] Red/Socket rc=0x%08X, nxlink_sock=%d\n", sock_rc, s_nxlink_sock);
    if (s_nxlink_sock >= 0) {
        printf("[Main] [+] nxlink conectado exitosamente a tu PC!\n");
    } else {
        printf("[Main] [!] nxlink no conectado (comprueba firewall en PC: puerto 28280/tcp).\n");
    }
    if (s_sd_log) {
        printf("[Main] [+] Log persistente activo en 'sdmc:/switch/tekken3/tekken3.log'\n");
    } else {
        printf("[Main] [-] Error: no se pudo abrir 'sdmc:/switch/tekken3/tekken3.log'\n");
    }

    // Cargar o crear archivo config.txt al lado del .nro
    load_or_create_config(argc, argv);

    // 0. Fijar afinidad del hilo principal de emulación a Core 0 (Paso 4a)
    // En Nintendo Switch los cores 0, 1 y 2 están disponibles para apps de usuario.
    // Fijar el bucle principal de ejecución de PS1 a Core 0 garantiza que nunca sea migrado por
    // el kernel entre núcleos (evitando invalidaciones de L1I/L1D y context switches costosos),
    // reservando Core 1 y Core 2 para los hilos de audio (audren), renderizado y E/S del sistema.
    s32 prev_core = -1;
    u64 prev_mask = 0;
    svcGetThreadCoreMask(&prev_core, &prev_mask, CUR_THREAD_HANDLE);
    Result rc_affinity = svcSetThreadCoreMask(CUR_THREAD_HANDLE, 0, (1u << 0));
    s32 cur_core = -1;
    u64 cur_mask = 0;
    svcGetThreadCoreMask(&cur_core, &cur_mask, CUR_THREAD_HANDLE);
    printf("[Main] [CPU Affinity] Core fijado a 0: prev(core=%d, mask=0x%llX) -> set(core=0, mask=0x1) rc=0x%08X -> cur(core=%d, mask=0x%llX)\n",
           prev_core, (unsigned long long)prev_mask, rc_affinity, cur_core, (unsigned long long)cur_mask);

    // 1. Montar RomFS (recursos y shaders empaquetados)
    printf("[Main] [1/7] Montando RomFS...\n");
    Result rc = romfsInit();
    if (R_FAILED(rc)) {
        printf("[Main] Advertencia: no se pudo montar romfs (codigo 0x%08X)\n", rc);
    } else {
        printf("[Main] RomFS montado correctamente en 'romfs:/'.\n");
    }

    // 2. Inicializar subsistema de mandos (2 jugadores)
    printf("[Main] [2/7] Inicializando mandos (2 jugadores)...\n");
    if (!switch_input_init()) {
        printf("[Main] Error inicializando mandos.\n");
    } else {
        printf("[Main] Mandos inicializados correctamente.\n");
    }

    // 3. Inicializar subsistema de audio nativo audren a 44.1 kHz
    printf("[Main] [3/7] Inicializando audio nativo audren (44100 Hz)...\n");
    if (!switch_audio_init(44100)) {
        printf("[Main] Error inicializando audio nativo audren.\n");
    } else {
        printf("[Main] Audio nativo audren listo.\n");
    }

    // 4. Inicializar backend gráfico de bajo nivel con Deko3D
    printf("[Main] [4/7] Inicializando Deko3D Renderer (SSAA=%ux, Relacion=%s)...\n",
           g_config.supersampling, g_config.widescreen_16_9 ? "16:9 Panoramica" : "4:3 Original");
    uint16_t* vram_ptr = gpu_get_vram_ptr();
    if (!Deko3DRenderer::Instance().Init(vram_ptr, g_config.supersampling, g_config.widescreen_16_9)) {
        printf("[Main] Error inicializando Deko3D.\n");
    } else {
        printf("[Main] Deko3D inicializado correctamente.\n");
    }

    // Registrar Deko3D como el renderer activo de psxrecomp
    gr_set_custom_backend(deko3d_backend_get());
    gr_init(vram_ptr);

    // 5. Inicializar subsistemas de hardware de PlayStation 1
    printf("[Main] [5/7] Inicializando perifericos de PS1 (DMA, Timers, MDEC, SIO, SPU, GPU)...\n");
    interrupts_init();
    dma_init();
    timers_init();
    mdec_init();
    sio_init();
    sio_set_pad_connected(0, 1);
    sio_set_pad_connected(1, 0);
    spu_init();
    gpu_init();

    // Configurar relación de aspecto y perspectiva según config.txt
    gpu_ws_set_gte_game_mode(0);
    if (g_config.widescreen_16_9) {
        gte_ws_set_dome_aspect(16, 9);
        gte_set_display_aspect(16, 9);
        gpu_ws_configure(16, 9, 0, 0, 1);
    } else {
        gte_ws_set_dome_aspect(4, 3);
        gte_set_display_aspect(4, 3);
        gpu_ws_configure(4, 3, 0, 0, 0);
    }
    gpu_texture_correction_set(0);
    printf("[Main] Aspecto: %s | SSAA: %ux (Resolucion interna: %ux%u)\n",
           g_config.widescreen_16_9 ? "16:9 Pantalla Completa" : "4:3 Original (Pillarbox)",
           g_config.supersampling,
           1024 * g_config.supersampling, 512 * g_config.supersampling);

    // Configurar región de overlays estáticos de Tekken 3 (load_address 0x00010000 + text_size 0x00121000)
    g_overlay_region_floor = 0x00131000u;

    // Habilitar elisión de bucles de espera (idle_skip = true en game.toml)
    g_idle_skip_enabled = 1;

    // Inicializar tarjetas de memoria en la microSD
    mkdir("sdmc:/switch/tekken3/saves", 0777);
    memcard_init("sdmc:/switch/tekken3/saves");
    std::atexit(memcard_flush_all);

    // Conectar el callback de VBlank de la GPU hacia Deko3D + audren
    gpu_set_vblank_callback(switch_vblank_callback);

    // 6. Cargar OpenBIOS desde RomFS
    const char* bios_path = "romfs:/bios/openbios.bin";
    printf("[Main] [6/7] Inicializando memoria con BIOS: %s...\n", bios_path);
    memory_init(bios_path);

    const PsxBiosBackend* bundled_bios = psx_bios_bundled();
    if (bundled_bios) {
        psx_bios_activate(bundled_bios);
        printf("[Main] Backend de OpenBIOS activado exitosamente.\n");
    } else {
        printf("[Main] Advertencia: no se encontro descriptor de BIOS empaquetada.\n");
    }

    // Registrar el entry_pc de Tekken 3 (SLUS-00402)
    const uint32_t tekken3_entry_pc = 0x80079C70u;
    fntrace_set_game_range(tekken3_entry_pc, 0);

    // Configurar HLE plan para arranque rápido y salto directo al juego
    PsxBiosHleRequest hle_req{};
    hle_req.bios_hle               = 1;
    hle_req.keep_intro             = 0;
    hle_req.fast_boot              = 1;
    hle_req.have_deliver_event_ret = (psx_bios_image.deliver_event_ret != 0);
    hle_req.have_shell_entry       = (psx_bios_image.shell_entry_phys != 0);
    hle_req.have_game_entry        = 1;
    const PsxBiosHlePlan plan = psx_bios_hle_plan(hle_req);
    psx_bios_hle_configure(plan.call_hle, plan.boot_skip);
    printf("[Main] Plan BIOS configurado: call_hle=%d, boot_skip=%d (entry=0x%08X)\n",
           plan.call_hle, plan.boot_skip, tekken3_entry_pc);
    printf("[Main] BootPlan detail: shell_entry=0x%08X deliver_event_ret=0x%08X keep_intro=%d fast_boot=%d game_started=%d\n",
           (unsigned)psx_bios_image.shell_entry_phys,
           (unsigned)psx_bios_image.deliver_event_ret,
           hle_req.keep_intro,
           hle_req.fast_boot,
           fntrace_is_game_started());

    // 7. Montar la imagen de disco de Tekken 3 (detección universal automática de cualquier .cue / .bin / .iso / .chd)
    printf("[Main] [7/7] Detectando y montando CD-ROM desde la microSD...\n");
    std::string chosen_disc_path = detect_and_mount_disc(argc, argv);
    if (!chosen_disc_path.empty() && cdrom_has_disc()) {
        printf("[Main] [+] Disco montado con exito en la unidad de CD-ROM: '%s' (region=SCEA).\n", chosen_disc_path.c_str());
    } else {
        printf("\n========================================================\n");
        printf("[FATAL ERROR] No se encontro ninguna imagen de Tekken 3!\n");
        printf("Por favor coloca los archivos (.cue + .bin, .iso o .chd) en:\n");
        printf("   sdmc:/switch/tekken3/disc/\n");
        printf("o directamente en:\n");
        printf("   sdmc:/switch/tekken3/\n");
        printf("========================================================\n\n");
        if (s_sd_log) {
            fflush(s_sd_log);
            fsync(fileno(s_sd_log));
            fclose(s_sd_log);
            s_sd_log = NULL;
        }
        svcSleepThread(3000000000ULL);
        return 1;
    }

    // Extraer imagen de texto de TEKKEN3/SLUS_004.02 del disco para dirty_ram_register_text_image
    {
        PS1::ISOReader iso;
        if (iso.Open(chosen_disc_path)) {
            const char* exe_iso_path = "TEKKEN3/SLUS_004.02";
            size_t exe_sz = iso.GetFileSize(exe_iso_path);
            if (exe_sz == 0) {
                exe_iso_path = "SLUS_004.02";
                exe_sz = iso.GetFileSize(exe_iso_path);
            }
            if (exe_sz > 2048u) {
                std::vector<uint8_t> exe_bytes(exe_sz);
                size_t got = iso.ReadFile(exe_iso_path, exe_bytes.data(), exe_sz);
                if (got > 2048u) {
                    uint32_t text_load_phys = 0x00010000u;
                    uint32_t text_size = (uint32_t)(got - 2048u);
                    if (text_size > 0x00121000u) text_size = 0x00121000u;
                    uint8_t* persistent_img = (uint8_t*)malloc(text_size);
                    if (persistent_img) {
                        memcpy(persistent_img, exe_bytes.data() + 2048u, text_size);
                        dirty_ram_register_text_image(text_load_phys, persistent_img, text_size);
                        printf("[Main] Imagen .text de %s registrada: base=0x%08X size=0x%08X\n",
                               exe_iso_path, text_load_phys, text_size);
                    }
                }
            } else {
                printf("[Main] Advertencia: no se pudo encontrar TEKKEN3/SLUS_004.02 en ISO (sz=%zu).\n", exe_sz);
            }
        } else {
            printf("[Main] Advertencia: ISOReader no pudo abrir '%s'.\n", chosen_disc_path.c_str());
        }
    }

    // 8. Inicializar estado del procesador R3000A de PS1
    printf("[Main] Configurando estado inicial del CPU R3000A...\n");
    CPUState cpu;
    memset(&cpu, 0, sizeof(cpu));
    cpu.ld_which_t = 0x20;
    psx_icache_reset();

    // Mapear punteros de acceso a memoria
    cpu.read_word  = psx_read_word;
    cpu.write_word = psx_write_word;
    cpu.read_half  = psx_read_half;
    cpu.write_half = psx_write_half;
    cpu.read_byte  = psx_read_byte;
    cpu.write_byte = psx_write_byte;

    // CRÍTICO: Vincular punteros de COP0 SR (para aislamiento de caché IsC=0x10000) y COP0 CAUSE (para IP2 de IRQ)
    memory_set_sr_ptr(&cpu.cop0[12]);
    psx_irq_set_cause_ptr(&cpu.cop0[13]);
    debug_cpu_ptr = &cpu;

    // Inicializar intérprete de apoyo
    interp_init(&cpu);

    // Vector de reset del CPU
    cpu.pc = 0xBFC00000u;        // Vector de reset de la BIOS de PS1
    cpu.cop0[12] = 0x00400000u;  // Registro SR: BEV=1 (Boot Exception Vectors)

    // Activar el planificador determinista TCB (sin fibras de host, óptimo para Switch)
    psx_hle_scheduler_set_default(1);

    // Armar registro continuo de dispatches en buffer circular para diagnostico
    extern void fntrace_arm(uint32_t target);
    fntrace_arm(0xFFFFFFFFu);

    printf("[Main] ================================================\n");
    printf("[Main] ¡Iniciando ejecucion nativa de Tekken 3!\n");
    printf("[Main] ================================================\n");

    // 9. Arrancar el planificador y la recompilación
    prof_start();
    u64 sched_start = armGetSystemTick();
    psx_scheduler_run(&cpu);
    u64 sched_end = armGetSystemTick();
    printf("[PerfGuest] psx_scheduler_run_ns=%llu\n",
           (unsigned long long)armTicksToNs(sched_end - sched_start));
    fflush(stdout);

    printf("\n[Main] === DIAGNOSTICO POST-SALIDA DE PSX_SCHEDULER_RUN ===\n");
    printf("[Main] escape=%d pc0_reason=%u pc=0x%08X ra=0x%08X sp=0x%08X gp=0x%08X fp=0x%08X\n",
           g_sched_escape.reason, g_pc0_reason, cpu.pc, cpu.gpr[31], cpu.gpr[29], cpu.gpr[28], cpu.gpr[30]);
    printf("[Main] COP0: SR(12)=0x%08X CAUSE(13)=0x%08X EPC(14)=0x%08X cycles=%llu frames=%llu\n",
           cpu.cop0[12], cpu.cop0[13], cpu.cop0[14],
           (unsigned long long)psx_cycle_count, (unsigned long long)s_frame_count);
    printf("[Main] Vectores RAM baja: [0x80]=0x%08X [0xA0]=0x%08X [0xB0]=0x%08X [0xC0]=0x%08X [0x200]=0x%08X\n",
           psx_read_word(0x80), psx_read_word(0xA0), psx_read_word(0xB0), psx_read_word(0xC0), psx_read_word(0x200));
    printf("[Main] DirtyRAM: blocks=%llu insns=%llu unsup=%llu entry=0x%08X (ra=0x%08X sp=0x%08X ran=%u) last_pc=0x%08X last_insn=0x%08X (%s)\n",
           (unsigned long long)g_dirty_ram_blocks_run,
           (unsigned long long)g_dirty_ram_insns_run,
           (unsigned long long)g_dirty_ram_unsupported_midblock,
           g_dirty_ram_last_unsupported_entry,
           g_dirty_ram_last_unsupported_entry_ra,
           g_dirty_ram_last_unsupported_entry_sp,
           g_dirty_ram_last_unsupported_insns,
           g_dirty_ram_last_unsupported_pc,
           g_dirty_ram_last_unsupported_insn,
           g_dirty_ram_last_unsupported_reason ? g_dirty_ram_last_unsupported_reason : "none");
    for (int r = 0; r < 32; r += 4) {
        printf("[Main] GPR[%02d..%02d]: %08X %08X %08X %08X\n",
               r, r + 3, cpu.gpr[r], cpu.gpr[r + 1], cpu.gpr[r + 2], cpu.gpr[r + 3]);
    }
    printf("[Main] Stack dump (sp=0x%08X):\n", cpu.gpr[29]);
    for (int i = 0; i < 16; i += 4) {
        uint32_t sp_addr = cpu.gpr[29] + i * 4;
        printf("  [0x%08X]: %08X %08X %08X %08X\n",
               sp_addr,
               psx_read_word(sp_addr),
               psx_read_word(sp_addr + 4),
               psx_read_word(sp_addr + 8),
               psx_read_word(sp_addr + 12));
    }
    printf("[Main] Total dispatches (fntrace_seq): %llu\n", (unsigned long long)g_fntrace_seq);
    if (g_fntrace_seq > 0) {
        printf("[Main] Ultimos dispatches registrados:\n");
        uint64_t count = g_fntrace_seq < 30u ? g_fntrace_seq : 30u;
        for (uint64_t i = g_fntrace_seq - count; i < g_fntrace_seq; i++) {
            const FntraceEntry* e = &g_fntrace_ring[i % FNTRACE_RING_CAP];
            printf("  [%llu] target=0x%08X ra=0x%08X a0=0x%08X a1=0x%08X sp=0x%08X\n",
                   (unsigned long long)i, e->target, e->ra, e->a0, e->a1, e->sp);
        }
    }
    if (g_dirty_ram_block_log_seq > 0) {
        printf("[Main] Ultimos DirtyRAM blocks registrados:\n");
        uint64_t bcount = g_dirty_ram_block_log_seq < 15u ? g_dirty_ram_block_log_seq : 15u;
        for (uint64_t i = g_dirty_ram_block_log_seq - bcount; i < g_dirty_ram_block_log_seq; i++) {
            const DirtyRamBlockLogEntry* be = &g_dirty_ram_block_log[i & (DIRTY_RAM_BLOCK_LOG_CAP - 1u)];
            printf("  [DirtyBlock %llu] target=0x%08X ra=0x%08X a0=0x%08X a1=0x%08X\n",
                   (unsigned long long)i, be->target, be->ra, be->a0, be->a1);
        }
    }
    fflush(stdout);

    // 10. Limpieza ordenada si el juego termina
    printf("[Main] Finalizando subsistemas...\n");
    cd_prefetch_shutdown();
    memcard_flush_all();
    switch_overlay_capture_write_json();
    if (s_sd_log) {
        fclose(s_sd_log);
        s_sd_log = NULL;
    }
    switch_audio_shutdown();
    Deko3DRenderer::Instance().Shutdown();
    romfsExit();
    socketExit();

    return 0;
}
