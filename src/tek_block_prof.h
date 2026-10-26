#pragma once
#include <stdint.h>
#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEK_BLOCK_PROF 1

#ifdef TEK_BLOCK_PROF

enum TekBlockId {
    TEK_BLOCK_80036CAC = 0, // func_80036CAC (3D mesh renderer)
    TEK_BLOCK_8007E078 = 1, // func_8007E078 (GP0 word transfer loop)
    TEK_BLOCK_80037484 = 2, // func_80037484 (3D primitive emitter)
    TEK_BLOCK_80029700 = 3, // block_80029700 (GTE vertex transform setup)
    TEK_BLOCK_80029720 = 4, // block_80029720 (GTE matrix / transform dispatch)
    TEK_BLOCK_COUNT    = 5
};

typedef struct {
    uint64_t start_tick;
    uint32_t id;
} TekBlockProfScope;

void tek_block_prof_record_caller(uint32_t id, uint32_t caller_pc);
void tek_block_prof_record_time(uint32_t id, uint64_t ticks);
void tek_block_prof_report_and_reset(uint64_t frame);

static inline TekBlockProfScope tek_block_prof_enter(uint32_t id, uint32_t caller_pc) {
    tek_block_prof_record_caller(id, caller_pc);
    TekBlockProfScope scope;
    scope.start_tick = armGetSystemTick();
    scope.id = id;
    return scope;
}

static inline void tek_block_prof_exit(TekBlockProfScope* scope) {
    uint64_t end_tick = armGetSystemTick();
    tek_block_prof_record_time(scope->id, end_tick - scope->start_tick);
}

static inline void tek_block_prof_cleanup_helper(TekBlockProfScope* scope) {
    tek_block_prof_exit(scope);
}

#define TEK_BLOCK_PROF_ENTER(id, caller_pc) \
    TekBlockProfScope _prof_scope_##id = tek_block_prof_enter((id), (caller_pc))

#define TEK_BLOCK_PROF_EXIT(id) \
    tek_block_prof_exit(&_prof_scope_##id)

#define TEK_BLOCK_PROF_SCOPE(id, caller_pc) \
    TekBlockProfScope _prof_scope_##id __attribute__((cleanup(tek_block_prof_cleanup_helper))) = \
        tek_block_prof_enter((id), (caller_pc))

#else

#define TEK_BLOCK_PROF_ENTER(id, caller_pc) ((void)0)
#define TEK_BLOCK_PROF_EXIT(id) ((void)0)
#define TEK_BLOCK_PROF_SCOPE(id, caller_pc) ((void)0)
static inline void tek_block_prof_report_and_reset(uint64_t frame) { (void)frame; }

#endif

#ifdef __cplusplus
}
#endif
