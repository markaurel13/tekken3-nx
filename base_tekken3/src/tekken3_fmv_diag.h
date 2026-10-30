#ifndef TEKKEN3_FMV_DIAG_H
#define TEKKEN3_FMV_DIAG_H

#include <stdint.h>
#include "psx_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

void fmv_diag_pre_dispatch(CPUState* cpu, uint32_t addr);
void fmv_diag_post_dispatch(CPUState* cpu, uint32_t addr);
void fmv_diag_vblank(uint64_t frame_count);

#ifdef __cplusplus
}
#endif

#endif /* TEKKEN3_FMV_DIAG_H */
