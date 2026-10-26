/* bios_hle.c — opt-in High-Level Emulation tier for PSX BIOS kernel services.
 *
 * See bios_hle.h for the design (LLE default + oracle, per-game opt-in,
 * unimplemented services fall through to the recompiled BIOS, boot shell-skip,
 * always-on observability ring).
 *
 * v1 service coverage — the B0 event family, ground-truthed instruction by
 * instruction against the SCPH1001 kernel (ROM 0xBFC11644..0xBFC11A84, kernel
 * RAM 0x1B44..0x1F8C; cross-checked with docs/psx_bios_disasm.txt B0 table @
 * 0x874 and PSX-SPX "Kernel: Events"):
 *
 *   B0:0x07 DeliverEvent(class, spec)
 *   B0:0x08 OpenEvent(class, spec, mode, func)
 *   B0:0x09 CloseEvent(event)
 *   B0:0x0B TestEvent(event)
 *   B0:0x0C EnableEvent(event)
 *   B0:0x0D DisableEvent(event)
 *
 * EvCB table: base pointer at kernel [0x120], size in bytes at [0x124], one
 * 0x1C-byte entry per event: +0x00 class, +0x04 status, +0x08 spec, +0x0C
 * mode, +0x10 func. Status values: 0 free, 0x1000 disabled, 0x2000 enabled,
 * 0x4000 ready/delivered. Mode values: 0x1000 = call callback on delivery,
 *0x2000 = mark ready (polled via TestEvent/WaitEvent).
 *
 * Everything else (WaitEvent — it blocks; threads; pads; the card/CD stack;
 * all of A0/C0) falls through to LLE in v1, exactly like gbarecomp's
 * machine-state SWIs. The recompiled BIOS stays fully load-bearing for them.
 *
 * TIMING NOTE: cycle costs charged here are approximations of the kernel
 * routine's dynamic instruction count (vector stub + table handler + body).
 * LLE accrues the exact count by executing; cycle-exact HLE is a documented
 * limitation of the opt-in tier. LLE remains the timing oracle.
 */

#include "bios_hle.h"
#include "memcard.h"

#include "cpu_state.h"
#include "psx_cycles.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* fntrace.c — game-entry handoff latch (drives the boot turbo window). */
extern int fntrace_is_game_started(void);

/* ── hook slot (referenced by the generated dispatch trampoline) ─────────── */

int (*g_psx_bios_hle_hook)(struct CPUState* cpu, uint32_t phys) = NULL;

/* ── mode state ──────────────────────────────────────────────────────────── */

static int s_call_hle_on     = 0;   /* service kernel calls in HLE */
static int s_boot_skip_on    = 0;   /* one-shot shell intercept */
static int s_shell_skipped   = 0;   /* boot skip already fired */

/* Per-image anchors come from the LINKED recompiled BIOS itself
 * (psx_bios_image, emitted into <stem>_dispatch.c from the BIOS profile's
 * [recompiler.runtime_exports]). For SCPH1001 the shell is unpacked to RAM
 * 0x30000 by LoadRunShell (ROM 0xBFC06FF0: memcpy(0x80030000, 0xBFC18000,
 * 0x67FF0)) and entered via an indirect call, which always routes through
 * the dispatcher. A profile that omits an anchor exports 0 = the feature is
 * STRUCTURALLY UNAVAILABLE on that BIOS (never "address 0"): the boot skip
 * then simply never fires, and DeliverEvent callbacks stay LLE. */
#include "psx_bios_image.h"

/* Kernel EvCB table anchors (docs/psx_bios_disasm.txt; kseg1 like the kernel
 * itself uses so reads/writes are plain RAM traffic). */
#define KADDR_EVCB_BASE 0xA0000120u
#define KADDR_EVCB_SIZE 0xA0000124u

/* EvCB field offsets / values (SCPH1001 kernel + PSX-SPX). */
#define EVCB_STRIDE   0x1Cu
#define EV_CLASS      0x00u
#define EV_STATUS     0x04u
#define EV_SPEC       0x08u
#define EV_MODE       0x0Cu
#define EV_FUNC       0x10u
#define EVST_FREE     0x0000u
#define EVST_DISABLED 0x1000u
#define EVST_ENABLED  0x2000u
#define EVST_READY    0x4000u
#define EVMD_CALLBACK 0x1000u
#define EVMD_MARK     0x2000u

/* DeliverEvent's callback return site (psx_bios_image.deliver_event_ret):
 * the instruction after the kernel loop's `jalr v0` — the exact $ra an LLE
 * delivery gives the callback, reused here so the dispatch return contract
 * matches. 0 = unknown for this BIOS: callback delivery stays LLE. */

/* ── always-on ring ──────────────────────────────────────────────────────── */

static PsxHleCallEntry s_ring[PSX_HLE_RING_CAP];
static uint64_t        s_ring_seq = 0;

static void hle_record(uint32_t vector, uint32_t fn, const CPUState* cpu,
                       uint32_t v0, uint8_t route)
{
    /* Thread attribution: current TCB via the kernel's process block chain
     * (0x108 -> PCB, PCB[0] -> running TCB; SCPH1001 layout, stable across
     * kernel versions). Distinguishes which guest thread issued each call —
     * load-bearing for event-consumption races (MMX6 card check). */
    uint32_t pcb = cpu->read_word(0xA0000108u);
    uint32_t pcb_phys = pcb & 0x1FFFFFFFu;
    uint32_t tcb = (pcb != 0u && pcb_phys < 0x200000u)
                       ? cpu->read_word(0xA0000000u | pcb_phys) : 0u;
    extern int psx_get_in_exception(void);
    uint8_t in_exc = (uint8_t)(psx_get_in_exception() ? 1u : 0u);

    /* Collapse tight polling cycles: guest code that spins on a handful of
     * calls (e.g. MMX6's 4-way TestEvent card poll at ~175K calls/s) would
     * otherwise flush the whole ring in ~0.1s. If an identical call
     * (vector/fn/args/ra/result/route/thread) exists among the last 8
     * entries, count it there. The first and last occurrence cycles plus the
     * exact repeat count survive; any change in args, result, or issuing
     * thread appends a fresh entry, so every TRANSITION is ring-ordered. */
    uint64_t back = s_ring_seq < 8 ? s_ring_seq : 8;
    for (uint64_t k = 1; k <= back; k++) {
        PsxHleCallEntry* p = &s_ring[(s_ring_seq - k) & (PSX_HLE_RING_CAP - 1)];
        if (p->vector == vector && p->fn == fn && p->route == route &&
            p->a0 == cpu->gpr[4] && p->a1 == cpu->gpr[5] &&
            p->a2 == cpu->gpr[6] && p->a3 == cpu->gpr[7] &&
            p->ra == cpu->gpr[31] && p->v0 == v0 &&
            p->tcb == tcb && p->in_exc == in_exc) {
            p->repeat++;
            p->cycle_last = psx_get_cycle_count();
            return;
        }
    }
    PsxHleCallEntry* e = &s_ring[s_ring_seq & (PSX_HLE_RING_CAP - 1)];
    e->seq    = s_ring_seq;
    e->cycle  = psx_get_cycle_count();
    e->cycle_last = e->cycle;
    e->vector = vector;
    e->fn     = fn;
    e->a0     = cpu->gpr[4];
    e->a1     = cpu->gpr[5];
    e->a2     = cpu->gpr[6];
    e->a3     = cpu->gpr[7];
    e->ra     = cpu->gpr[31];
    e->v0     = v0;
    e->repeat = 1;
    e->tcb    = tcb;
    e->route  = route;
    e->in_exc = in_exc;
    s_ring_seq++;
}

uint64_t psx_hle_ring_seq(void) { return s_ring_seq; }

const PsxHleCallEntry* psx_hle_ring_entry(uint64_t seq_index)
{
    if (seq_index >= s_ring_seq) return NULL;
    if (s_ring_seq - seq_index > PSX_HLE_RING_CAP) return NULL; /* evicted */
    return &s_ring[seq_index & (PSX_HLE_RING_CAP - 1)];
}

/* ── B0 event services (SCPH1001 kernel semantics, on real guest EvCBs) ──── */

/* get_free_EvCB_slot (kernel 0x1D00): first status==0 slot index, else -1. */
static uint32_t ev_free_slot(const CPUState* cpu, uint32_t base, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        if (cpu->read_word(base + i * EVCB_STRIDE + EV_STATUS) == EVST_FREE)
            return i;
    }
    return 0xFFFFFFFFu;
}

/* OpenEvent (kernel 0x1D8C): claim a free EvCB, fill it, status=disabled,
 * return 0xF1000000|slot; no free slot => 0xFFFFFFFF. */
static void hle_open_event(CPUState* cpu, uint32_t base, uint32_t n)
{
    uint32_t slot = ev_free_slot(cpu, base, n);
    if (slot == 0xFFFFFFFFu) {
        cpu->gpr[2] = 0xFFFFFFFFu;
    } else {
        uint32_t ev = base + slot * EVCB_STRIDE;
        cpu->write_word(ev + EV_CLASS,  cpu->gpr[4]);
        cpu->write_word(ev + EV_SPEC,   cpu->gpr[5]);
        cpu->write_word(ev + EV_MODE,   cpu->gpr[6]);
        cpu->write_word(ev + EV_FUNC,   cpu->gpr[7]);
        cpu->write_word(ev + EV_STATUS, EVST_DISABLED);
        cpu->gpr[2] = 0xF1000000u | slot;
    }
    psx_advance_cycles(40u + 6u * (slot == 0xFFFFFFFFu ? n : slot + 1u));
}

/* CloseEvent (kernel 0x1E1C): status=free, return 1. */
static void hle_close_event(CPUState* cpu, uint32_t base)
{
    uint32_t ev = base + (cpu->gpr[4] & 0xFFFFu) * EVCB_STRIDE;
    cpu->write_word(ev + EV_STATUS, EVST_FREE);
    cpu->gpr[2] = 1u;
    psx_advance_cycles(36u);
}

/* TestEvent (kernel 0x1EC8): ready => re-arm to enabled and return 1, else 0. */
static void hle_test_event(CPUState* cpu, uint32_t base)
{
    uint32_t ev = base + (cpu->gpr[4] & 0xFFFFu) * EVCB_STRIDE;
    if (cpu->read_word(ev + EV_STATUS) == EVST_READY) {
        cpu->write_word(ev + EV_STATUS, EVST_ENABLED);
        cpu->gpr[2] = 1u;
    } else {
        cpu->gpr[2] = 0u;
    }
    psx_advance_cycles(40u);
}

/* EnableEvent (kernel 0x1F10) / DisableEvent (kernel 0x1F4C): a non-free
 * event's status is set to enabled/disabled; return 1 either way. NOTE the
 * kernel really does discard a pending READY on EnableEvent — faithful. */
static void hle_set_event_status(CPUState* cpu, uint32_t base, uint32_t status)
{
    uint32_t ev = base + (cpu->gpr[4] & 0xFFFFu) * EVCB_STRIDE;
    if (cpu->read_word(ev + EV_STATUS) != EVST_FREE)
        cpu->write_word(ev + EV_STATUS, status);
    cpu->gpr[2] = 1u;
    psx_advance_cycles(40u);
}

/* DeliverEvent (kernel 0x1B44): for every ENABLED EvCB matching class+spec,
 * mode MARK => status=READY; mode CALLBACK => call the handler exactly as the
 * kernel's `jalr v0` does (same $ra, args untouched). */
static void hle_deliver_event(CPUState* cpu, uint32_t base, uint32_t n)
{
    uint32_t cls  = cpu->gpr[4];
    uint32_t spec = cpu->gpr[5];
    for (uint32_t i = 0; i < n; i++) {
        uint32_t ev = base + i * EVCB_STRIDE;
        if (cpu->read_word(ev + EV_STATUS) != EVST_ENABLED) continue;
        if (cpu->read_word(ev + EV_CLASS) != cls)  continue;
        if (cpu->read_word(ev + EV_SPEC)  != spec) continue;
        uint32_t mode = cpu->read_word(ev + EV_MODE);
        if (mode == EVMD_MARK) {
            cpu->write_word(ev + EV_STATUS, EVST_READY);
        } else if (mode == EVMD_CALLBACK) {
            uint32_t func = cpu->read_word(ev + EV_FUNC);
            /* deliver_event_ret == 0 never reaches here: configure() refuses
             * call-HLE when the linked BIOS exports no anchor (dropping a
             * callback silently wedges Ape LOAD card EvCBs). */
            if (func != 0 && psx_bios_image.deliver_event_ret != 0) {
                /* Nested guest call, same shape as any compiled call site.
                 * $ra is the kernel's real post-jalr address so the callback
                 * returns through the standard dispatch contract; the caller's
                 * $ra is restored before the hook returns. */
                uint32_t saved_ra = cpu->gpr[31];
                cpu->gpr[31] = psx_bios_image.deliver_event_ret;
                extern int g_exec_phase;   /* wall-time sampler (dirty_ram_interp.c) */
                int prev_phase = g_exec_phase;
                g_exec_phase = 3;   /* compiled route; dirty/native callees re-tag inside */
                psx_dispatch_call(cpu, func, psx_bios_image.deliver_event_ret);
                g_exec_phase = prev_phase;
                cpu->gpr[31] = saved_ra;
            }
        }
        /* mode==anything else: the kernel loop skips it — so do we. */
    }
    /* Return value: the kernel returns with v0 = last comparison scratch; no
     * caller may depend on it (libapi types DeliverEvent void). Set 0 for a
     * stable, observable value. */
    cpu->gpr[2] = 0u;
    psx_advance_cycles(36u + 12u * n);
}

/* B0 vector service. Returns 1 iff serviced in HLE. */
static int hle_service_b0(CPUState* cpu, uint32_t fn)
{
    /* Kernel executive not initialized yet (InitEvents hasn't run) => the
     * EvCB table doesn't exist; everything stays LLE. */
    uint32_t base = cpu->read_word(KADDR_EVCB_BASE);
    if (base == 0) return 0;
    uint32_t n = cpu->read_word(KADDR_EVCB_SIZE) / EVCB_STRIDE;
    if (n == 0 || n > 0x1000u) return 0; /* implausible table => LLE */

    switch (fn) {
    case 0x07u: hle_deliver_event(cpu, base, n);                  return 1;
    case 0x08u: hle_open_event(cpu, base, n);                     return 1;
    case 0x09u: hle_close_event(cpu, base);                       return 1;
    case 0x0Bu: hle_test_event(cpu, base);                        return 1;
    case 0x0Cu: hle_set_event_status(cpu, base, EVST_ENABLED);    return 1;
    case 0x0Du: hle_set_event_status(cpu, base, EVST_DISABLED);   return 1;
    default:    return 0; /* WaitEvent, threads, pads, card, ... => LLE */
    }
}

/* ── boot shell-skip ─────────────────────────────────────────────────────── */

static int hle_boot_shell_skip(CPUState* cpu)
{
    if (!s_boot_skip_on || s_shell_skipped || fntrace_is_game_started())
        return 0;
    s_shell_skipped = 1;
    printf("[BiosHLE] Shell skip activado (phase=boot, shell_entry=0x%08X, resume_ra=0x%08X, game_started=%d) -> cargando SYSTEM.CNF / SLUS_004.02...\n",
           psx_bios_image.shell_entry_phys, cpu->gpr[31], fntrace_is_game_started());
    fflush(stdout);
    /* Return straight to LoadRunShell's call site: BIOS Main() proceeds to
     * step 8 (SYSTEM.CNF + game EXE load) exactly as if the shell had exited
     * immediately with no disc-menu interaction. Kernel state at this point
     * was built entirely by the real recompiled kernel init. */
    cpu->gpr[2] = 0u;
    hle_record(psx_bios_image.shell_entry_phys, 0u, cpu, 0u, PSX_HLE_ROUTE_BOOT);
    return 1;
}

int psx_bios_hle_boot_turbo_active(void)
{
    return s_boot_skip_on && !fntrace_is_game_started();
}

static const char* b0_fn_name(uint32_t fn) {
    switch (fn) {
        case 0x07: return "DeliverEvent";
        case 0x08: return "OpenEvent";
        case 0x09: return "CloseEvent";
        case 0x0A: return "WaitEvent";
        case 0x0B: return "TestEvent";
        case 0x0C: return "EnableEvent";
        case 0x0D: return "DisableEvent";
        case 0x11: return "InitCARD";
        case 0x12: return "InitCARD/Pad";
        case 0x13: return "StartCARD/Pad";
        case 0x14: return "StopCARD/Pad";
        case 0x15: return "_card_load";
        case 0x16: return "_card_auto";
        case 0x32: return "open";
        case 0x33: return "lseek";
        case 0x34: return "read";
        case 0x35: return "write";
        case 0x36: return "close";
        case 0x37: return "ioctl";
        case 0x41: return "format";
        case 0x42: return "firstfile";
        case 0x43: return "nextfile";
        case 0x44: return "rename";
        case 0x45: return "erase";
        case 0x47: return "addDevice";
        case 0x49: return "_bu_init/printDev";
        case 0x4A: return "InitCARD2/_card_clear";
        case 0x4B: return "StartCARD2";
        case 0x4C: return "StopCARD2";
        case 0x4D: return "_card_write";
        case 0x4E: return "_card_read";
        case 0x4F: return "_new_card";
        case 0x50: return "_card_status";
        case 0x56: return "GetB0Table";
        case 0x5B: return "_card_info/ChangeClearPad";
        default: return "unknown";
    }
}

static void try_print_guest_str(CPUState* cpu, uint32_t addr, char* out, size_t max_len) {
    out[0] = '\0';
    uint32_t phys = addr & 0x1FFFFFFFu;
    if (phys < 0x200000u) {
        size_t i = 0;
        while (i < max_len - 1 && phys + i < 0x200000u) {
            uint8_t c = cpu->read_byte(phys + i);
            if (c == 0) break;
            if (c < 32 || c > 126) { out[0] = '\0'; return; }
            out[i++] = (char)c;
        }
        out[i] = '\0';
    }
}

/* ── Memory Card HLE (Vía A) ──────────────────────────────────────────────── */

static void hle_deliver_card_event(CPUState* cpu, uint32_t spec)
{
    uint32_t base = cpu->read_word(KADDR_EVCB_BASE);
    if (base == 0) base = cpu->read_word(0x80000120u);
    if (base == 0) base = cpu->read_word(0x00000120u);
    if (base == 0) return;
    uint32_t size_bytes = cpu->read_word(KADDR_EVCB_SIZE);
    if (size_bytes == 0) size_bytes = cpu->read_word(0x80000124u);
    if (size_bytes == 0) size_bytes = cpu->read_word(0x00000124u);
    uint32_t n = size_bytes / EVCB_STRIDE;
    if (n == 0 || n > 0x1000u) return;

    for (uint32_t i = 0; i < n; i++) {
        uint32_t ev = base + i * EVCB_STRIDE;
        if (cpu->read_word(ev + EV_STATUS) != EVST_ENABLED) continue;
        uint32_t cls = cpu->read_word(ev + EV_CLASS);
        if (cls != 0xF4000001u && cls != 0xF0000011u) continue;
        if (cpu->read_word(ev + EV_SPEC) != spec) continue;

        uint32_t mode = cpu->read_word(ev + EV_MODE);
        if (mode == EVMD_MARK) {
            cpu->write_word(ev + EV_STATUS, EVST_READY);
        } else if (mode == EVMD_CALLBACK) {
            uint32_t func = cpu->read_word(ev + EV_FUNC);
            if (func != 0 && psx_bios_image.deliver_event_ret != 0) {
                uint32_t saved_ra = cpu->gpr[31];
                cpu->gpr[31] = psx_bios_image.deliver_event_ret;
                extern int g_exec_phase;
                int prev_phase = g_exec_phase;
                g_exec_phase = 3;
                psx_dispatch_call(cpu, func, psx_bios_image.deliver_event_ret);
                g_exec_phase = prev_phase;
                cpu->gpr[31] = saved_ra;
            }
        }
    }
}

#define MAX_MC_HANDLES 8
#define MC_FD_FIRST    0x40

typedef struct {
    int      in_use;
    int      slot;        /* 0 o 1 */
    int      block;       /* 1 a 15 */
    uint32_t offset;      /* offset actual dentro del archivo */
    uint32_t size;        /* tamaño actual del archivo */
    uint32_t mode;        /* flags de apertura */
    char     filename[32];
} McFileHandle;

static McFileHandle s_mc_handles[MAX_MC_HANDLES];

static int mc_alloc_handle(void) {
    for (int i = 0; i < MAX_MC_HANDLES; i++) {
        if (!s_mc_handles[i].in_use) {
            s_mc_handles[i].in_use = 1;
            return MC_FD_FIRST + i;
        }
    }
    return -1;
}

static McFileHandle* mc_get_handle(int fd) {
    if (fd < MC_FD_FIRST || fd >= MC_FD_FIRST + MAX_MC_HANDLES)
        return NULL;
    if (!s_mc_handles[fd - MC_FD_FIRST].in_use)
        return NULL;
    return &s_mc_handles[fd - MC_FD_FIRST];
}

static void mc_free_handle(int fd) {
    if (fd >= MC_FD_FIRST && fd < MC_FD_FIRST + MAX_MC_HANDLES) {
        memset(&s_mc_handles[fd - MC_FD_FIRST], 0, sizeof(McFileHandle));
    }
}

static int mc_open(CPUState* cpu, const char* path, uint32_t mode)
{
    int slot = -1;
    const char* fname = NULL;
    if (strncmp(path, "bu00:", 5) == 0) {
        slot = 0;
        fname = path + 5;
    } else if (strncmp(path, "bu10:", 5) == 0) {
        slot = 1;
        fname = path + 5;
    } else {
        return -2; /* Not a memory card device -> fall through to LLE */
    }

    if (!memcard_is_present(slot)) {
        hle_deliver_card_event(cpu, 0x8000u);
        return -1;
    }

    uint8_t* data = memcard_get_data(slot);
    if (!data) {
        hle_deliver_card_event(cpu, 0x8000u);
        return -1;
    }

    /* Buscar archivo existente en los directory frames 1..15 */
    int found_block = -1;
    uint32_t file_size = 0;
    for (int s = 1; s <= 15; s++) {
        int off = s * 128;
        if (data[off] == 0x51) { /* Primer bloque en uso */
            char name[21] = {0};
            memcpy(name, &data[off + 0x0A], 20);
            name[20] = '\0';
            if (strncmp(name, fname, 20) == 0) {
                found_block = s;
                file_size = *(uint32_t*)&data[off + 0x04];
                break;
            }
        }
    }

    if (found_block != -1) {
        int fd = mc_alloc_handle();
        if (fd < 0) {
            hle_deliver_card_event(cpu, 0x8000u);
            return -1;
        }
        McFileHandle* h = mc_get_handle(fd);
        h->slot = slot;
        h->block = found_block;
        h->offset = 0;
        h->size = file_size;
        h->mode = mode;
        strncpy(h->filename, fname, sizeof(h->filename) - 1);
        hle_deliver_card_event(cpu, 0x0004u);
        printf("[MemcardHLE] open(\"%s\", mode=0x%X) existente en bloque %d (size=%u) -> fd=0x%X\n",
               path, mode, found_block, file_size, fd);
        fflush(stdout);
        return fd;
    }

    /* No existe. ¿O_CREAT? (mode & 0x0200) */
    if (mode & 0x0200u) {
        /* Buscar primer bloque libre */
        int free_block = -1;
        for (int s = 1; s <= 15; s++) {
            int off = s * 128;
            if (data[off] == 0xA0) {
                free_block = s;
                break;
            }
        }
        if (free_block == -1) {
            hle_deliver_card_event(cpu, 0x8000u);
            printf("[MemcardHLE] open(\"%s\", mode=0x%X) ERROR: Tarjeta llena\n", path, mode);
            fflush(stdout);
            return -1;
        }

        /* Configurar directory frame */
        int off = free_block * 128;
        memset(&data[off], 0, 128);
        data[off + 0] = 0x51; /* primer bloque en uso */
        *(uint32_t*)&data[off + 0x04] = 0; /* tamaño inicial 0 */
        data[off + 8] = 0xFF; /* siguiente bloque = ninguno */
        data[off + 9] = 0xFF;
        strncpy((char*)&data[off + 0x0A], fname, 20);
        data[off + 0x7F] = memcard_calc_checksum(&data[off]);

        memcard_mark_dirty(slot);

        int fd = mc_alloc_handle();
        if (fd < 0) {
            hle_deliver_card_event(cpu, 0x8000u);
            return -1;
        }
        McFileHandle* h = mc_get_handle(fd);
        h->slot = slot;
        h->block = free_block;
        h->offset = 0;
        h->size = 0;
        h->mode = mode;
        strncpy(h->filename, fname, sizeof(h->filename) - 1);
        hle_deliver_card_event(cpu, 0x0004u);
        printf("[MemcardHLE] open(\"%s\", mode=0x%X) CREADO en bloque %d -> fd=0x%X\n",
               path, mode, free_block, fd);
        fflush(stdout);
        return fd;
    }

    /* No existe y no es O_CREAT */
    hle_deliver_card_event(cpu, 0x8000u);
    printf("[MemcardHLE] open(\"%s\", mode=0x%X) archivo no encontrado -> -1\n", path, mode);
    fflush(stdout);
    return -1;
}

static int mc_read(CPUState* cpu, int fd, uint32_t guest_buf, uint32_t count)
{
    McFileHandle* h = mc_get_handle(fd);
    if (!h) return -2; /* Not our handle -> LLE */

    uint8_t* data = memcard_get_data(h->slot);
    if (!data) {
        hle_deliver_card_event(cpu, 0x8000u);
        return -1;
    }

    if (h->offset >= h->size) {
        hle_deliver_card_event(cpu, 0x0004u);
        return 0; /* EOF */
    }

    if (h->offset + count > h->size) {
        count = h->size - h->offset;
    }

    uint32_t block_data_offset = (uint32_t)h->block * 8192u + h->offset;
    for (uint32_t i = 0; i < count; i++) {
        cpu->write_byte(guest_buf + i, data[block_data_offset + i]);
    }
    h->offset += count;

    hle_deliver_card_event(cpu, 0x0004u);
    printf("[MemcardHLE] read(fd=0x%X, count=%u) -> leidos=%u (offset=%u/%u)\n",
           fd, count, count, h->offset, h->size);
    fflush(stdout);
    return (int)count;
}

static int mc_write(CPUState* cpu, int fd, uint32_t guest_buf, uint32_t count)
{
    McFileHandle* h = mc_get_handle(fd);
    if (!h) return -2; /* Not our handle -> LLE (e.g. stdout fd 1) */

    uint8_t* data = memcard_get_data(h->slot);
    if (!data) {
        hle_deliver_card_event(cpu, 0x8000u);
        return -1;
    }

    if (h->offset + count > 8192u) {
        if (h->offset >= 8192u) count = 0;
        else count = 8192u - h->offset;
    }

    uint32_t block_data_offset = (uint32_t)h->block * 8192u + h->offset;
    for (uint32_t i = 0; i < count; i++) {
        data[block_data_offset + i] = cpu->read_byte(guest_buf + i);
    }
    h->offset += count;

    if (h->offset > h->size) {
        h->size = h->offset;
        int off = h->block * 128;
        *(uint32_t*)&data[off + 0x04] = h->size;
        data[off + 0x7F] = memcard_calc_checksum(&data[off]);
    }

    memcard_mark_dirty(h->slot);
    memcard_flush(h->slot);

    hle_deliver_card_event(cpu, 0x0004u);
    printf("[MemcardHLE] write(fd=0x%X, count=%u) -> escritos=%u (nuevo tamano=%u)\n",
           fd, count, count, h->size);
    fflush(stdout);
    return (int)count;
}

static int mc_close(CPUState* cpu, int fd)
{
    McFileHandle* h = mc_get_handle(fd);
    if (!h) return -2; /* Not our handle -> LLE */

    memcard_flush(h->slot);
    printf("[MemcardHLE] close(fd=0x%X, archivo=%s)\n", fd, h->filename);
    fflush(stdout);
    mc_free_handle(fd);

    hle_deliver_card_event(cpu, 0x0004u);
    return 0;
}

static int mc_lseek(CPUState* cpu, int fd, int32_t offset, int whence)
{
    McFileHandle* h = mc_get_handle(fd);
    if (!h) return -2; /* Not our handle -> LLE */

    int32_t new_off = 0;
    switch (whence) {
        case 0: /* SEEK_SET */ new_off = offset; break;
        case 1: /* SEEK_CUR */ new_off = (int32_t)h->offset + offset; break;
        case 2: /* SEEK_END */ new_off = (int32_t)h->size + offset; break;
        default: return -1;
    }
    if (new_off < 0) new_off = 0;
    h->offset = (uint32_t)new_off;
    return (int)h->offset;
}

static int mc_format(CPUState* cpu, const char* dev)
{
    int slot = -1;
    if (strncmp(dev, "bu00:", 5) == 0) slot = 0;
    else if (strncmp(dev, "bu10:", 5) == 0) slot = 1;
    else return -2;

    printf("[MemcardHLE] format(\"%s\") -> Slot %d\n", dev, slot + 1);
    fflush(stdout);
    memcard_format_card(slot);
    memcard_flush(slot);
    hle_deliver_card_event(cpu, 0x0004u);
    return 1;
}

static int s_firstfile_slot = -1;
static int s_firstfile_idx = 1;
static char s_firstfile_pattern[32] = {0};

static int mc_match_pattern(const char* pat, const char* name)
{
    if (!pat || pat[0] == '\0' || strcmp(pat, "*") == 0) return 1;
    return (strncmp(pat, name, strlen(pat)) == 0);
}

static int mc_fill_direntry(CPUState* cpu, uint32_t dir_ptr, int slot, int block)
{
    uint8_t* data = memcard_get_data(slot);
    if (!data) return 0;
    int off = block * 128;
    char name[21] = {0};
    memcpy(name, &data[off + 0x0A], 20);
    name[20] = '\0';
    for (int i = 0; i < 20; i++) {
        cpu->write_byte(dir_ptr + i, (uint8_t)name[i]);
    }
    cpu->write_word(dir_ptr + 20, 0x10u); /* attr */
    uint32_t size = *(uint32_t*)&data[off + 0x04];
    cpu->write_word(dir_ptr + 24, size); /* size */
    cpu->write_word(dir_ptr + 28, 0u);   /* next */
    cpu->write_word(dir_ptr + 32, (uint32_t)block); /* head */
    for (int i = 0; i < 14; i++) {
        cpu->write_byte(dir_ptr + 36 + i, 0);
    }
    return 1;
}

static int mc_firstfile(CPUState* cpu, const char* pattern, uint32_t dir_ptr)
{
    int slot = -1;
    const char* pat = pattern;
    if (strncmp(pattern, "bu00:", 5) == 0) {
        slot = 0;
        pat = pattern + 5;
    } else if (strncmp(pattern, "bu10:", 5) == 0) {
        slot = 1;
        pat = pattern + 5;
    } else {
        return -2;
    }

    if (!memcard_is_present(slot)) {
        hle_deliver_card_event(cpu, 0x8000u);
        return 0;
    }

    s_firstfile_slot = slot;
    strncpy(s_firstfile_pattern, pat, sizeof(s_firstfile_pattern) - 1);

    uint8_t* data = memcard_get_data(slot);
    for (int s = 1; s <= 15; s++) {
        int off = s * 128;
        if (data[off] == 0x51) {
            char name[21] = {0};
            memcpy(name, &data[off + 0x0A], 20);
            name[20] = '\0';
            if (mc_match_pattern(s_firstfile_pattern, name)) {
                s_firstfile_idx = s + 1;
                mc_fill_direntry(cpu, dir_ptr, slot, s);
                hle_deliver_card_event(cpu, 0x0004u);
                printf("[MemcardHLE] firstfile(\"%s\") -> encontrado \"%s\" en bloque %d\n",
                       pattern, name, s);
                fflush(stdout);
                return (int)dir_ptr;
            }
        }
    }

    hle_deliver_card_event(cpu, 0x0004u);
    return 0;
}

static int mc_nextfile(CPUState* cpu, uint32_t dir_ptr)
{
    if (s_firstfile_slot < 0) return -2;
    int slot = s_firstfile_slot;
    if (!memcard_is_present(slot)) {
        hle_deliver_card_event(cpu, 0x8000u);
        return 0;
    }

    uint8_t* data = memcard_get_data(slot);
    for (int s = s_firstfile_idx; s <= 15; s++) {
        int off = s * 128;
        if (data[off] == 0x51) {
            char name[21] = {0};
            memcpy(name, &data[off + 0x0A], 20);
            name[20] = '\0';
            if (mc_match_pattern(s_firstfile_pattern, name)) {
                s_firstfile_idx = s + 1;
                mc_fill_direntry(cpu, dir_ptr, slot, s);
                hle_deliver_card_event(cpu, 0x0004u);
                printf("[MemcardHLE] nextfile() -> encontrado \"%s\" en bloque %d\n", name, s);
                fflush(stdout);
                return (int)dir_ptr;
            }
        }
    }

    hle_deliver_card_event(cpu, 0x0004u);
    return 0;
}

static int hle_service_memcard_b0(CPUState* cpu, uint32_t fn)
{
    if (!fntrace_is_game_started()) return 0;

    switch (fn) {
    case 0x15u: { /* _card_load(chan) */
        int chan = (int)cpu->gpr[4];
        int slot = (chan == 0x10 || chan == 1) ? 1 : 0;
        int pres = memcard_is_present(slot);
        static uint32_t s_log_load_b0 = 0;
        if (s_log_load_b0++ < 5) {
            printf("[MemcardHLE] B0:0x15 _card_load(chan=%d, slot=%d) -> pres=%d\n", chan, slot, pres);
            fflush(stdout);
        }
        if (pres) {
            hle_deliver_card_event(cpu, 0x0004u);
            cpu->gpr[2] = 1u;
        } else {
            hle_deliver_card_event(cpu, 0x8000u);
            cpu->gpr[2] = 0u;
        }
        return 1;
    }
    case 0x16u: { /* _card_auto */
        cpu->gpr[2] = 0u;
        return 1;
    }
    case 0x32u: { /* open */
        char path[128] = {0};
        try_print_guest_str(cpu, cpu->gpr[4], path, sizeof(path));
        int fd = mc_open(cpu, path, cpu->gpr[5]);
        if (fd == -2) return 0; /* Fall through to LLE */
        cpu->gpr[2] = (uint32_t)fd;
        return 1;
    }
    case 0x33u: { /* lseek */
        int res = mc_lseek(cpu, (int)cpu->gpr[4], (int32_t)cpu->gpr[5], (int)cpu->gpr[6]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x34u: { /* read */
        int res = mc_read(cpu, (int)cpu->gpr[4], cpu->gpr[5], cpu->gpr[6]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x35u: { /* write */
        int res = mc_write(cpu, (int)cpu->gpr[4], cpu->gpr[5], cpu->gpr[6]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x36u: { /* close */
        int res = mc_close(cpu, (int)cpu->gpr[4]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x41u: { /* format */
        char dev[64] = {0};
        try_print_guest_str(cpu, cpu->gpr[4], dev, sizeof(dev));
        int res = mc_format(cpu, dev);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x42u: { /* firstfile */
        char pat[64] = {0};
        try_print_guest_str(cpu, cpu->gpr[4], pat, sizeof(pat));
        int res = mc_firstfile(cpu, pat, cpu->gpr[5]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x43u: { /* nextfile */
        int res = mc_nextfile(cpu, cpu->gpr[4]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x49u: { /* _bu_init */
        cpu->gpr[2] = 0u;
        return 1;
    }
    case 0x4Fu: { /* _new_card */
        cpu->gpr[2] = 0u;
        return 1;
    }
    case 0x50u: { /* _card_status */
        int chan = (int)cpu->gpr[4];
        int slot = (chan == 0x10 || chan == 1) ? 1 : 0;
        cpu->gpr[2] = memcard_is_present(slot) ? 1u : 0u;
        return 1;
    }
    case 0x58u: { /* _card_chan */
        cpu->gpr[2] = 0u;
        return 1;
    }
    default:
        return 0;
    }
}

static int hle_service_memcard_a0(CPUState* cpu, uint32_t fn)
{
    if (!fntrace_is_game_started()) return 0;

    switch (fn) {
    case 0x32u: { /* open */
        char path[128] = {0};
        try_print_guest_str(cpu, cpu->gpr[4], path, sizeof(path));
        int fd = mc_open(cpu, path, cpu->gpr[5]);
        if (fd == -2) return 0;
        cpu->gpr[2] = (uint32_t)fd;
        return 1;
    }
    case 0x33u: { /* lseek */
        int res = mc_lseek(cpu, (int)cpu->gpr[4], (int32_t)cpu->gpr[5], (int)cpu->gpr[6]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x34u: { /* read */
        int res = mc_read(cpu, (int)cpu->gpr[4], cpu->gpr[5], cpu->gpr[6]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x35u: { /* write */
        int res = mc_write(cpu, (int)cpu->gpr[4], cpu->gpr[5], cpu->gpr[6]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x36u: { /* close */
        int res = mc_close(cpu, (int)cpu->gpr[4]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x41u: { /* format */
        char dev[64] = {0};
        try_print_guest_str(cpu, cpu->gpr[4], dev, sizeof(dev));
        int res = mc_format(cpu, dev);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x42u: { /* firstfile */
        char pat[64] = {0};
        try_print_guest_str(cpu, cpu->gpr[4], pat, sizeof(pat));
        int res = mc_firstfile(cpu, pat, cpu->gpr[5]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x43u: { /* nextfile */
        int res = mc_nextfile(cpu, cpu->gpr[4]);
        if (res == -2) return 0;
        cpu->gpr[2] = (uint32_t)res;
        return 1;
    }
    case 0x70u: { /* _bu_init */
        cpu->gpr[2] = 0u;
        return 1;
    }
    case 0xABu: { /* _card_info */
        int chan = (int)cpu->gpr[4];
        int slot = (chan == 0x10 || chan == 1) ? 1 : 0;
        int pres = memcard_is_present(slot);
        static uint32_t s_log_info_a0 = 0;
        if (s_log_info_a0++ < 5) {
            printf("[MemcardHLE] A0:0xAB _card_info(chan=%d, slot=%d) -> pres=%d\n", chan, slot, pres);
            fflush(stdout);
        }
        if (pres) {
            hle_deliver_card_event(cpu, 0x0004u);
            cpu->gpr[2] = 1u;
        } else {
            hle_deliver_card_event(cpu, 0x8000u);
            cpu->gpr[2] = 0u;
        }
        return 1;
    }
    case 0xACu: { /* _card_load(chan) */
        int chan = (int)cpu->gpr[4];
        int slot = (chan == 0x10 || chan == 1) ? 1 : 0;
        int pres = memcard_is_present(slot);
        static uint32_t s_log_load_a0 = 0;
        if (s_log_load_a0++ < 5) {
            printf("[MemcardHLE] A0:0xAC _card_load(chan=%d, slot=%d) -> pres=%d\n", chan, slot, pres);
            fflush(stdout);
        }
        if (pres) {
            hle_deliver_card_event(cpu, 0x0004u);
            cpu->gpr[2] = 1u;
        } else {
            hle_deliver_card_event(cpu, 0x8000u);
            cpu->gpr[2] = 0u;
        }
        return 1;
    }
    case 0xADu: { /* _card_auto */
        cpu->gpr[2] = 0u;
        return 1;
    }
    default:
        return 0;
    }
}

/* ── the hook ────────────────────────────────────────────────────────────── */

static int bios_hle_dispatch(struct CPUState* cpu, uint32_t phys)
{
    if (phys == 0xB0u) {
        uint32_t fn = cpu->gpr[9]; /* $t1 — the PSY-Q thunk's function number */

        int mc_handled = hle_service_memcard_b0(cpu, fn);
        if (mc_handled) {
            hle_record(0xB0u, fn, cpu, cpu->gpr[2], PSX_HLE_ROUTE_HLE);
            return 1;
        }

        if (fntrace_is_game_started()) {
            static uint32_t s_b0_log_count = 0;
            if (s_b0_log_count < 30) {
                char str_a0[64] = {0};
                try_print_guest_str(cpu, cpu->gpr[4], str_a0, sizeof(str_a0));
                int is_card_ev = (fn == 0x08 && (cpu->gpr[4] == 0xF4000001u || cpu->gpr[4] == 0xF0000011u));
                int is_card_fn = (fn >= 0x11 && fn <= 0x16) || (fn >= 0x32 && fn <= 0x5B);
                if (is_card_fn || is_card_ev || str_a0[0] != '\0' || s_b0_log_count < 10) {
                    s_b0_log_count++;
                    printf("[BIOS_B0] fn=0x%02X (%s) ra=0x%08X a0=0x%08X %s%s%s a1=0x%08X a2=0x%08X a3=0x%08X\n",
                           fn, b0_fn_name(fn), cpu->gpr[31], cpu->gpr[4],
                           str_a0[0] ? "(\"" : "", str_a0[0] ? str_a0 : "", str_a0[0] ? "\")" : "",
                           cpu->gpr[5], cpu->gpr[6], cpu->gpr[7]);
                    fflush(stdout);
                }
            }
        }

        int handled = 0;
        if (__builtin_expect(s_call_hle_on != 0, 0))
            handled = hle_service_b0(cpu, fn);
        hle_record(0xB0u, fn, cpu, handled ? cpu->gpr[2] : 0u,
                   handled ? PSX_HLE_ROUTE_HLE : PSX_HLE_ROUTE_LLE);
        return handled;
    }
    if (phys == 0xA0u || phys == 0xC0u) {
        if (phys == 0xA0u) {
            uint32_t fn = cpu->gpr[9];
            int mc_handled = hle_service_memcard_a0(cpu, fn);
            if (mc_handled) {
                hle_record(0xA0u, fn, cpu, cpu->gpr[2], PSX_HLE_ROUTE_HLE);
                return 1;
            }
        }

        if (fntrace_is_game_started()) {
            static uint32_t s_a0_log_count = 0;
            if (s_a0_log_count < 20) {
                uint32_t fn = cpu->gpr[9];
                if (fn == 0x19 || fn == 0xAC || fn == 0x70) {
                    s_a0_log_count++;
                    printf("[BIOS_%c0] fn=0x%02X ra=0x%08X a0=0x%08X a1=0x%08X\n",
                           phys == 0xA0u ? 'A' : 'C', fn, cpu->gpr[31], cpu->gpr[4], cpu->gpr[5]);
                    fflush(stdout);
                }
            }
        }
        /* No A0/C0 services implemented in v1 — observe + fall through. */
        hle_record(phys, cpu->gpr[9], cpu, 0u, PSX_HLE_ROUTE_LLE);
        return 0;
    }
    if (__builtin_expect(s_boot_skip_on != 0, 1) &&
        phys == psx_bios_image.shell_entry_phys)
        return hle_boot_shell_skip(cpu);
    return 0;
}

/* ── selection ───────────────────────────────────────────────────────────── */

void psx_bios_hle_configure(int call_hle, int boot_skip)
{
    /* IMPORTANT (Ape Escape + OpenBIOS): clamp call-HLE to deliver_event_ret.
     * OpenBIOS omits the anchor; servicing B0:07 without it silently skips
     * CALLBACK EvCBs (HwCARD/SwCARD flag handlers at 0x80022930/0x80022980
     * never run → LOAD phase waits never complete). Plan should already
     * refuse; this is the runtime backstop. Boot-skip remains independent. */
    s_call_hle_on  = (call_hle && psx_bios_image.deliver_event_ret != 0) ? 1 : 0;
    /* Clamp to what THIS image can actually do, so the state the banner and
     * `hle_dump` report can never claim a skip that bios_hle_dispatch below is
     * structurally unable to perform. Callers are expected to have gone
     * through psx_bios_hle_plan(); this is the backstop, not the policy. */
    s_boot_skip_on = (boot_skip && psx_bios_image.shell_entry_phys != 0) ? 1 : 0;
    /* Rematch / session_reboot re-enters bring-up without process exit; the
     * shell-skip latch must arm again or peers run the interactive shell and
     * appear hung after netplay lockstep. */
    s_shell_skipped = 0;
    g_psx_bios_hle_hook = &bios_hle_dispatch;
}

int psx_bios_hle_enabled(void)           { return s_call_hle_on; }
int psx_bios_hle_boot_skip_enabled(void) { return s_boot_skip_on; }

const char* psx_bios_hle_backend_name(void)
{
    return s_call_hle_on ? "HLE (LLE fallback)" : "LLE (recompiled BIOS)";
}
