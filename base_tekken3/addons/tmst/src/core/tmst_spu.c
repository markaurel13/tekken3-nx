/* TMST core: our mixers after the game's sound (tmst_spu.h). */
#include "tmst_spu.h"
#include "spu.h"
#include <stdint.h>

enum { MAX_MIXERS = 4, SPUCNT = 0x1f801daa };
static TmstSpuMixer mixers[MAX_MIXERS];
static int mixer_count;
static int sfx_gain = 100, music_gain = 100;
static int muted, applied_mute;

int tmst_spu_add_mixer(TmstSpuMixer m) {
    for (int i = 0; i < mixer_count; i++) if (mixers[i] == m) return 1;
    if (!m || mixer_count >= MAX_MIXERS) return 0;
    mixers[mixer_count++] = m;
    return 1;
}

int tmst_spu_music_gain(void) { return music_gain; }
int tmst_spu_sfx_gain(void) { return sfx_gain; }

/* The runtime's own setter, past every wrapper (ours and the Expanded's):
 * the player's volume stays what the wrappers recorded. */
extern void __real_spu_set_host_mix_volume(int sfx, int music);
void tmst_spu_mute_game_music(int on) {
    muted = on != 0;
    if (muted != applied_mute) {
        applied_mute = muted;
        __real_spu_set_host_mix_volume(sfx_gain, muted ? 0 : music_gain);
    }
}

extern void __tmst_chain_spu_set_host_mix_volume(int sfx, int music);
#ifdef TMST_BRIDGE_spu_set_host_mix_volume
void __tmst_chain_spu_set_host_mix_volume(int sfx, int music) { __real_spu_set_host_mix_volume(sfx, music); }
#endif
void __wrap_spu_set_host_mix_volume(int sfx, int music) {
    sfx_gain = sfx; music_gain = music;
    __tmst_chain_spu_set_host_mix_volume(sfx, music);
    if (muted) __real_spu_set_host_mix_volume(sfx, 0);
}

extern void __tmst_chain_spu_render(int16_t *out, int frames);
#ifdef TMST_BRIDGE_spu_render
extern void __real_spu_render(int16_t *out, int frames);
void __tmst_chain_spu_render(int16_t *out, int frames) { __real_spu_render(out, frames); }
#endif
void __wrap_spu_render(int16_t *out, int frames) {
    /* Muted again before each render: the Expanded sets the music volume
     * back itself when its own music ends (src/tekken3_ttt1_voices.c). */
    if (muted) __real_spu_set_host_mix_volume(sfx_gain, 0);
    __tmst_chain_spu_render(out, frames);
    if (!out || frames <= 0 || !mixer_count) return;
    SpuGlobalState g;
    spu_get_global_state(&g);
    int audible = (g.ctrl & 0xc000) == 0xc000;          /* SPU on and unmuted */
    for (int i = 0; i < mixer_count; i++) mixers[i](out, frames, audible, music_gain);
}
