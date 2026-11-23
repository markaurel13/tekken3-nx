/* TMST core: the game's "play music" call (0x8006AE14, a0 = the music id),
 * chained in front of the Expanded's own wrapper (its BGM SELECT), so that a
 * mod can see every piece the game starts and replace it. */
#ifndef TMST_MUSIC_H
#define TMST_MUSIC_H
#ifdef __cplusplus
extern "C" {
#endif

/* Called with the id the game asks for; returns the id to play instead,
 * -1 for silence (the call is skipped), or `id` itself to let it be. */
typedef int (*TmstMusicHook)(int id);

/* Up to 4 hooks, run in the order added; the first -1 skips the call. */
int tmst_music_add_hook(TmstMusicHook hook);

/* The version of the fight's music (issue #9): GAME OPTION's BGM SELECT, as
 * the Expanded plays Tekken 3's and TTT's stages: its RANDOM's draw for the
 * fight (tekken3_bgm_select_version) where the Expanded has it, else the
 * card's byte (0x80097F05). */
enum { TMST_MUSIC_CONSOLE = 0, TMST_MUSIC_ARCADE = 1, TMST_MUSIC_SILENT = 2 };
int tmst_music_version(void);

#ifdef __cplusplus
}
#endif
#endif
