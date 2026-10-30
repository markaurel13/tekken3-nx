#ifndef TEKKEN3_FMV_OVERLAY_H
#define TEKKEN3_FMV_OVERLAY_H

#include <stdint.h>
#include "psx_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

int psx_dispatch_fmv_overlay(CPUState* cpu, uint32_t addr);
int psx_dispatch_fmv_str_compiled(CPUState* cpu, uint32_t addr);
int psx_dispatch_fmv_vlc_compiled(CPUState* cpu, uint32_t addr);

#ifdef __cplusplus
}
#endif

#endif /* TEKKEN3_FMV_OVERLAY_H */
