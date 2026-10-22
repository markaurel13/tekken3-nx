#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/iosupport.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <switch.h>

#include "renderer/deko3d_renderer.h"
#include "platform/switch_audio.h"
#include "platform/switch_input.h"
#include "iso_reader.h"

// Referencias a los componentes de psxrecomp
extern "C" {
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
extern const char* g_dirty_ram_last_unsupported_reason;
void dirty_ram_text_exact_mismatch_stats(uint64_t *count, uint32_t out[5]);

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
void overlay_capture_before_dma(uint32_t a, uint32_t b, int c) { (void)a; (void)b; (void)c; }
void overlay_capture_on_dma(uint32_t a, uint32_t b, int c) { (void)a; (void)b; (void)c; }
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

// Búfer simulado de VRAM de 1024x512 de 16-bit
static uint16_t s_vram[1024 * 512];

// Sistema de logging unificado (MicroSD + nxlink over Wi-Fi)
static FILE* s_sd_log = NULL;
static int s_nxlink_sock = -1;
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

static ssize_t log_devoptab_write(struct _reent *r, void *fd, const char *ptr, size_t len) {
    (void)r; (void)fd;
    if (s_sd_log) {
        fwrite(ptr, 1, len, s_sd_log);
        fflush(s_sd_log);
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

static void switch_vblank_callback(void) {
    s_frame_count++;
    u64 now_tick = armGetSystemTick();
    if (s_last_vblank_tick == 0) {
        s_last_vblank_tick = now_tick;
        s_last_vblank_frame = s_frame_count;
    }

    if (s_frame_count <= 5 || s_frame_count == 60 || (s_frame_count % 120) == 0) {
        u64 elapsed_ns = armTicksToNs(now_tick - s_last_vblank_tick);
        uint64_t frames_delta = s_frame_count - s_last_vblank_frame;
        double avg_ms = (frames_delta > 0) ? ((double)elapsed_ns / 1e6 / (double)frames_delta) : 0.0;
        double fps = (avg_ms > 0.0) ? (1000.0 / avg_ms) : 0.0;
        uint64_t d_insns = g_dirty_ram_insns_run - s_last_dirty_insns;
        uint64_t d_static = g_dispatch_static_hits - s_last_static_hits;
        uint64_t mism_cnt = 0;
        uint32_t mism_info[5] = {0};
        dirty_ram_text_exact_mismatch_stats(&mism_cnt, mism_info);

        printf("[VBlank] frame=%llu fps=%.1f (%.2f ms/f) static_d=%llu dirty_insns_d=%llu (tot=%llu) idle_skips=%llu mism=%llu\n",
               (unsigned long long)s_frame_count,
               fps, avg_ms,
               (unsigned long long)d_static,
               (unsigned long long)d_insns,
               (unsigned long long)g_dirty_ram_insns_run,
               (unsigned long long)g_idle_skip_count,
               (unsigned long long)mism_cnt);
        if (mism_cnt > 0) {
            printf("  [Mismatch] range=0x%08X+%u at=0x%08X live=0x%02X ref=0x%02X\n",
                   mism_info[0], mism_info[1], mism_info[2], mism_info[3], mism_info[4]);
        }
        if (d_insns > 100000) {
            // Mostrar top 3 PCs interpretados en dirty_ram_pc_table
            uint32_t top_pc[3] = {0};
            uint64_t top_in[3] = {0};
            for (uint32_t i = 0; i < DIRTY_RAM_PC_TABLE_SIZE; i++) {
                uint64_t ins = g_dirty_ram_pc_table[i].insns;
                uint32_t pc = g_dirty_ram_pc_table[i].pc;
                if (!pc || !ins) continue;
                for (int k = 0; k < 3; k++) {
                    if (ins > top_in[k]) {
                        for (int m = 2; m > k; m--) { top_in[m] = top_in[m - 1]; top_pc[m] = top_pc[m - 1]; }
                        top_in[k] = ins; top_pc[k] = pc;
                        break;
                    }
                }
            }
            printf("  [DirtyTop] #1 pc=0x%08X (%llu insns) | #2 pc=0x%08X (%llu) | #3 pc=0x%08X (%llu)\n",
                   top_pc[0], (unsigned long long)top_in[0],
                   top_pc[1], (unsigned long long)top_in[1],
                   top_pc[2], (unsigned long long)top_in[2]);
        }

        s_last_vblank_tick = now_tick;
        s_last_vblank_frame = s_frame_count;
        s_last_dirty_insns = g_dirty_ram_insns_run;
        s_last_static_hits = g_dispatch_static_hits;
    }

    // 1. Manejo de bucle principal y salida rápida con botones (Plus + Minus)
    if (!appletMainLoop() || switch_input_is_exit_requested()) {
        printf("[VBlank] Solicitud de salida detectada en frame %llu. Saliendo...\n",
               (unsigned long long)s_frame_count);
        exit(0);
    }

    // 2. Actualizar mandos y estados de botones para la PS1
    switch_input_update();

    // 3. Bombeo de audio hacia el subsistema nativo audren
    switch_audio_update();

    // 4. Renderizar y presentar frame con Deko3D
    Deko3DRenderer::Instance().RenderDisplay(nullptr, 1280, 0, 0, 1280, 720);
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // 1. Asegurar directorios de log en la microSD y abrir archivo persistente
    mkdir("sdmc:/switch", 0777);
    mkdir("sdmc:/switch/tekken3", 0777);
    s_sd_log = fopen("sdmc:/switch/tekken3/tekken3.log", "w");

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
    printf("   Build: %s %s (Game Dispatch Active)\n", __DATE__, __TIME__);
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
    printf("[Main] [4/7] Inicializando Deko3D Renderer...\n");
    memset(s_vram, 0, sizeof(s_vram));
    if (!Deko3DRenderer::Instance().Init(s_vram)) {
        printf("[Main] Error inicializando Deko3D.\n");
    } else {
        printf("[Main] Deko3D inicializado correctamente.\n");
    }

    // Registrar Deko3D como el renderer activo de psxrecomp
    gr_set_custom_backend(deko3d_backend_get());
    gr_init(s_vram);

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

    // Configurar región de overlays estáticos de Tekken 3 (load_address 0x00010000 + text_size 0x00121000)
    g_overlay_region_floor = 0x00131000u;

    // Habilitar elisión de bucles de espera (idle_skip = true en game.toml)
    g_idle_skip_enabled = 1;

    // Inicializar tarjetas de memoria en la microSD
    mkdir("sdmc:/switch/tekken3/saves", 0777);
    memcard_init("sdmc:/switch/tekken3/saves");

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

    // 7. Montar la imagen de disco de Tekken 3 desde la microSD
    const char* candidate_paths[] = {
        "sdmc:/switch/tekken3/disc/Tekken 3 (USA).cue",
        "sdmc:/switch/tekken3/disc/Tekken 3.cue",
        "sdmc:/switch/tekken3/disc/tekken3.cue",
        "sdmc:/switch/tekken3/Tekken 3 (USA).cue",
        "sdmc:/switch/tekken3/Tekken 3.cue",
        "sdmc:/switch/tekken3/disc/Tekken 3 (USA) (Track 1).bin",
        "disc/Tekken 3 (USA).cue"
    };

    const char* chosen_disc_path = candidate_paths[0];
    for (size_t i = 0; i < sizeof(candidate_paths) / sizeof(candidate_paths[0]); i++) {
        FILE* test_f = fopen(candidate_paths[i], "rb");
        if (test_f) {
            fclose(test_f);
            chosen_disc_path = candidate_paths[i];
            break;
        }
    }

    printf("[Main] [7/7] Montando CD-ROM desde: %s...\n", chosen_disc_path);
    cdrom_init(chosen_disc_path);
    cdrom_set_disc_scex("SCEA");

    if (cdrom_has_disc()) {
        printf("[Main] Disco montado con exito en la unidad de CD-ROM (region=SCEA).\n");
    } else {
        printf("[Main] Advertencia: No se pudo montar el disco en '%s'.\n", chosen_disc_path);
        printf("[Main] Comprobando fallback a bin directo...\n");
        const char* bin_fallback = "sdmc:/switch/tekken3/disc/Tekken 3 (USA) (Track 1).bin";
        FILE* bf = fopen(bin_fallback, "rb");
        if (bf) {
            fclose(bf);
            chosen_disc_path = bin_fallback;
            cdrom_init(bin_fallback);
            if (cdrom_has_disc()) {
                printf("[Main] ¡Disco montado con exito usando fallback BIN!\n");
            }
        }
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
            printf("[Main] Advertencia: ISOReader no pudo abrir '%s'.\n", chosen_disc_path);
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

    printf("[Main] ================================================\n");
    printf("[Main] ¡Iniciando ejecucion nativa de Tekken 3!\n");
    printf("[Main] ================================================\n");

    // 9. Arrancar el planificador y la recompilación
    psx_scheduler_run(&cpu);

    printf("\n[Main] === DIAGNOSTICO POST-SALIDA DE PSX_SCHEDULER_RUN ===\n");
    printf("[Main] escape=%d pc0_reason=%u pc=0x%08X ra=0x%08X sp=0x%08X gp=0x%08X fp=0x%08X\n",
           g_sched_escape.reason, g_pc0_reason, cpu.pc, cpu.gpr[31], cpu.gpr[29], cpu.gpr[28], cpu.gpr[30]);
    printf("[Main] COP0: SR(12)=0x%08X CAUSE(13)=0x%08X EPC(14)=0x%08X cycles=%llu frames=%llu\n",
           cpu.cop0[12], cpu.cop0[13], cpu.cop0[14],
           (unsigned long long)psx_cycle_count, (unsigned long long)s_frame_count);
    printf("[Main] Vectores RAM baja: [0x80]=0x%08X [0xA0]=0x%08X [0xB0]=0x%08X [0xC0]=0x%08X [0x200]=0x%08X\n",
           psx_read_word(0x80), psx_read_word(0xA0), psx_read_word(0xB0), psx_read_word(0xC0), psx_read_word(0x200));
    printf("[Main] DirtyRAM: blocks=%llu insns=%llu unsup=%llu last_pc=0x%08X last_insn=0x%08X (%s)\n",
           (unsigned long long)g_dirty_ram_blocks_run,
           (unsigned long long)g_dirty_ram_insns_run,
           (unsigned long long)g_dirty_ram_unsupported_midblock,
           g_dirty_ram_last_unsupported_pc,
           g_dirty_ram_last_unsupported_insn,
           g_dirty_ram_last_unsupported_reason ? g_dirty_ram_last_unsupported_reason : "none");
    for (int r = 0; r < 32; r += 4) {
        printf("[Main] GPR[%02d..%02d]: %08X %08X %08X %08X\n",
               r, r + 3, cpu.gpr[r], cpu.gpr[r + 1], cpu.gpr[r + 2], cpu.gpr[r + 3]);
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
    fflush(stdout);

    // 10. Limpieza ordenada si el juego termina
    printf("[Main] Finalizando subsistemas...\n");
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
