/* TMST core: our mixers in front of the game's audio callback (tmst_audio.h). */
#include "tmst_audio.h"
#include "tmst_core.h"
#ifndef __SWITCH__
#include "psx_sdl_audio.h"
#endif
#include <stdatomic.h>

enum { MAX_MIXERS = 4 };
static _Atomic(TmstAudioMixer) mixers[MAX_MIXERS];
static atomic_int rate, stereo16;

#ifdef __SWITCH__
void tmst_audio_switch_mix(int16_t *samples, int frames, int r) {
    if (!samples || frames <= 0 || r <= 0) return;
    atomic_store(&rate, r);
    atomic_store(&stereo16, 1);
    for (int i = 0; i < MAX_MIXERS; i++) {
        TmstAudioMixer m = atomic_load(&mixers[i]);
        if (m) m(samples, frames, r);
    }
}
#else
static PsxSdlAudioCallback game_callback;
static void *game_userdata;
#endif

int tmst_audio_add_mixer(TmstAudioMixer mixer) {
    for (int i = 0; i < MAX_MIXERS; i++) if (atomic_load(&mixers[i]) == mixer) return 1;
    for (int i = 0; i < MAX_MIXERS; i++) {
        TmstAudioMixer empty = 0;
        if (atomic_compare_exchange_strong(&mixers[i], &empty, mixer)) return 1;
    }
    return 0;
}

int tmst_audio_rate(void) { return atomic_load(&rate); }

#ifndef __SWITCH__
static void SDLCALL callback(void *userdata, Uint8 *stream, int len) {
    (void)userdata;
    game_callback(game_userdata, stream, len);
    if (!atomic_load(&stereo16)) return;
    int frames = len / 4, r = atomic_load(&rate);
    for (int i = 0; i < MAX_MIXERS; i++) {
        TmstAudioMixer m = atomic_load(&mixers[i]);
        if (m) m((int16_t *)(void *)stream, frames, r);
    }
}

extern SDL_AudioDeviceID __real_psx_sdl_audio_open(const PsxSdlAudioSpec *want, PsxSdlAudioSpec *have);
SDL_AudioDeviceID __wrap_psx_sdl_audio_open(const PsxSdlAudioSpec *want, PsxSdlAudioSpec *have) {
    if (!want || !want->callback) {
        tmst_log("audio: the runtime's legacy audio mode, no mixing for the add-on");
        return __real_psx_sdl_audio_open(want, have);
    }
    PsxSdlAudioSpec mine = *want;
    PsxSdlAudioSpec got = {0};
    game_callback = want->callback;
    game_userdata = want->userdata;
    mine.callback = callback;
    mine.userdata = 0;
    SDL_AudioDeviceID device = __real_psx_sdl_audio_open(&mine, &got);
    if (have) *have = got;
    if (device) {
        /* The callback's buffer is in the requested format: SDL3 converts
         * the stream to the device (whose own format `got` reports), SDL2 is
         * opened without format changes. The rate is the one the game's
         * callback renders at (the runtime's host rate, `got.freq`). */
        int ok = want->channels == 2 && want->format == AUDIO_S16SYS;
        atomic_store(&rate, got.freq ? got.freq : want->freq);
        atomic_store(&stereo16, ok);
        if (ok) tmst_log("audio: add-on mixing at %d Hz", atomic_load(&rate));
        else tmst_log("audio: output is not 16-bit stereo, no mixing for the add-on");
    }
    return device;
}
#endif
