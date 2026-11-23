/* TMST core: music of our own beside the game's, at the game's own level.
 *
 * spu_render(out, frames) renders the game's sound (44 100 Hz stereo) and
 * spu_set_host_mix_volume(sfx, music) applies the player's volumes; the
 * Expanded wraps both (its TTT music and voices). Ours are chained in
 * front: our mixers add to the finished buffer, after the Expanded's, and the
 * game's music can be muted for a while without losing the player's volume. */
#ifndef TMST_SPU_H
#define TMST_SPU_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* out: 44 100 Hz stereo, the game's mix; `audible` is 0 while the SPU is
 * muted (pause, loading); `music_gain` the player's music volume, 0..100. */
typedef void (*TmstSpuMixer)(int16_t *out, int frames, int audible, int music_gain);
int tmst_spu_add_mixer(TmstSpuMixer mixer);

/* While on, the game's own music is silent (the player's volume kept). */
void tmst_spu_mute_game_music(int on);
/* The player's volumes, as last set (100 before the first setting). */
int tmst_spu_music_gain(void);
int tmst_spu_sfx_gain(void);

#ifdef __cplusplus
}
#endif
#endif
