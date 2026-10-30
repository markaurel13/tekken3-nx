#include "tekken3_fmv_overlay.h"

int g_native_bad_entry = 0;
void psx_native_bad_entry(CPUState *cpu, uint32_t owner, uint32_t pc) {
    (void)owner;
    cpu->pc = pc;
    g_native_bad_entry = 1;
}

int psx_dispatch_fmv_overlay(CPUState* cpu, uint32_t addr) {
    if (addr >= 0x800FDD58u && addr < 0x80100000u) {
        return psx_dispatch_fmv_str_compiled(cpu, addr);
    }
    if (addr >= 0x80104D58u && addr < 0x80106558u) {
        return psx_dispatch_fmv_vlc_compiled(cpu, addr);
    }
    return 0;
}
