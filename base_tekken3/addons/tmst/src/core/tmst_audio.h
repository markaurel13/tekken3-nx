/* TMST core: sound of the add-on's own, mixed into the game's output after
 * everything the game and the Expanded mix (their volumes included).
 *
 * The runtime opens its output with psx_sdl_audio_open(); our wrapper puts
 * its own callback in front of the game's: the game's fills the buffer, then
 * every registered mixer adds to it. Signed 16-bit interleaved stereo at
 * tmst_audio_rate(). In the runtime's legacy (queue) audio mode there is no
 * callback, and mixers are never called (logged once). Mixers run on the
 * audio thread: keep their state lock-free or guarded. */
#ifndef TMST_AUDIO_H
#define TMST_AUDIO_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TmstAudioMixer)(int16_t *stereo, int frames, int rate);

/* Up to 4 mixers; returns 0 when full. Adding the same one twice is a no-op. */
int tmst_audio_add_mixer(TmstAudioMixer mixer);
/* The output rate, 0 before the output opened or without a callback. */
int tmst_audio_rate(void);

#ifdef __cplusplus
}
#endif
#endif
