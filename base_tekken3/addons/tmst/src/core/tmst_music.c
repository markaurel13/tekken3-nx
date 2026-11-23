/* TMST core: the music call, chained (tmst_music.h). addon.cmake lists
 * func_8006AE14 in TMST_CHAINS: the build renames the Expanded's
 * __wrap_func_8006AE14 to __tmst_chain_func_8006AE14 (dev/tmst_chain.py),
 * or, on a build that does not wrap it, TMST_BRIDGE_ makes that name call
 * the game's own routine. Either way ours runs first and theirs after. */
#include "tmst_music.h"
#include "cpu_state.h"
#include "mod_plugins.h"
#include <stdint.h>
#ifdef TMST_HAS_BGM_SELECT
#include "tekken3_bgm_select.h"
#endif

int tmst_music_version(void) {
#ifdef TMST_HAS_BGM_SELECT
    int v = tekken3_bgm_select_version();
#else
    int v = psx_mod_read_byte(0x80097f05u);
#endif
    return v == TMST_MUSIC_ARCADE ? TMST_MUSIC_ARCADE : v >= TMST_MUSIC_SILENT ? TMST_MUSIC_SILENT : TMST_MUSIC_CONSOLE;
}

enum { MAX_HOOKS = 4 };
static TmstMusicHook hooks[MAX_HOOKS];
static int count;

int tmst_music_add_hook(TmstMusicHook h) {
    for (int i = 0; i < count; i++) if (hooks[i] == h) return 1;
    if (!h || count >= MAX_HOOKS) return 0;
    hooks[count++] = h;
    return 1;
}

extern void __tmst_chain_func_8006AE14(CPUState *cpu);
#ifdef TMST_BRIDGE_func_8006AE14
extern void __real_func_8006AE14(CPUState *cpu);
void __tmst_chain_func_8006AE14(CPUState *cpu) { __real_func_8006AE14(cpu); }
#endif

void __wrap_func_8006AE14(CPUState *cpu) {
    /* The routine is entered by its call (pc 0) or by the recompiled code at
     * its own address; re-entries at inner pcs go straight on. */
    if (count && (cpu->pc == 0 || cpu->pc == 0x8006ae14u)) {
        int id = (int)(int16_t)cpu->gpr[4];
        for (int i = 0; i < count; i++) {
            int to = hooks[i](id);
            if (to < 0) { cpu->pc = cpu->gpr[31]; return; }
            id = to;
        }
        cpu->gpr[4] = (uint32_t)id;
    }
    __tmst_chain_func_8006AE14(cpu);
}
