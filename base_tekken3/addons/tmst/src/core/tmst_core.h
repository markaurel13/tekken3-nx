/* TMST core: what every TMST mod shares. Not a mod of its own (no package,
 * no plugin): a library linked with the mods. */
#ifndef TMST_CORE_H
#define TMST_CORE_H
#ifdef __cplusplus
extern "C" {
#endif

/* "TMST: " + the message on stderr (the game's log). */
void tmst_log(const char *fmt, ...);

/* The Expanded this build was made from: `git describe` of its folder
 * ("v1.4.1", "v1.4.1-3-gabcdef0"), "unknown" without git. */
const char *tmst_upstream_rev(void);
/* Nonzero when that release is in addon.cmake's TMST_TESTED_WITH. */
int tmst_upstream_tested(void);

/* The version guard, called by a mod at its activation: logs the add-on and
 * the Expanded once, then whether `mod` can run. `needs` is a mask of
 * TMST_NEED_* (src/compat/tmst_compat.h); a need the Expanded cannot meet
 * switches the mod off (logged, never a crash). Returns 1 to go on. */
int tmst_guard(const char *mod, unsigned needs);

/* Tekken 2 Content (tmst.tekken2, src/tekken2/tmst_tekken2.c): nonzero when
 * its Music part is on (the package's switch too; their activations have run). */
int tmst_tekken2_music_active(void);
/* Its switch (tmst.tekken2), for its parts: nonzero when on. */
int tmst_tekken2_on(void);

/* Tournament Mode (src/tournament/tmst_tournament.c): the stage of the fight
 * in `mode` being loaded, when it is a tournament's match whose stage is the
 * CPU's (TMST_TSTAGE_T2 | _T3 | _TTT of src/tournament/tmst_tournament_stages.h,
 * its number in *stage); TMST_TSTAGE_NONE (0) for every other fight, the
 * Free Tournament's too. Asked from src/tekken2/tmst_tekken2_stages.c. */
int tmst_tournament_fight_stage(unsigned mode, int *stage);
/* ... and its music, when it is a tournament's match with one: the CPU's
 * key, the Jukebox category ("t2-console" ...) and piece ("t2-paul.bgm",
 * "xa-42"); 0 none. Asked from src/jukebox/tmst_jukebox.c. */
int tmst_tournament_fight_music(const char **key, const char **cat, const char **name);
/* src/tekken2/tmst_tekken2_stages.c: a VS or Practice fight on a Tekken 2
 * stage from the STAGE line, its own piece the same way (key "t2-stage",
 * "t2-console" or "t2-arcade" by OPTION's music mode); 0 none. Asked from
 * src/jukebox/tmst_jukebox.c. */
int tmst_tekken2_fight_music(const char **key, const char **cat, const char **name);
/* src/jukebox/tmst_jukebox.c: the coming fight's piece (the opponent's
 * Jukebox choice, a tournament's piece) started now, ahead of the game's,
 * at the player's music volume: Tournament Mode's boss intro plays under it.
 * The game's own start of the same piece later takes it over unbroken.
 * 1 when one plays (the Jukebox's player on, a piece there). */
int tmst_jukebox_lead_in(void);
/* src/jukebox/tmst_jukebox.c: the Jukebox's player (its music hook and
 * mixer) without the Jukebox (issue #9.2), for Tekken 2's stages' and
 * Tournament Mode's pieces: from their activation (who: for the log; once,
 * nothing more with the Jukebox on) and each VBlank (the fight's piece ends
 * with the fight's screens). */
void tmst_jukebox_player_on(const char *who);
void tmst_jukebox_player_tick(void);
/* src/tekken2/tmst_tekken2_stages.c: a tournament's Tekken 3 arena (or TTT
 * stage's arena) to hold until its fight, -1 none; the tournament's VBlank
 * writes it. */
int tmst_stage_forced_arena(void);

/* Defined by Unified Character Select (tmst.ui.unified-select) once it
 * exists: nonzero while its single grid is on. Other mods declare it weak and
 * read it only when it is linked (the Jukebox hides its L2 / R2 hint then). */
int tmst_unified_select_active(void) __attribute__((weak));
/* Defined by Unified Character Select too: while `allowed` is set, its grid
 * greys and refuses every fighter (character ID, costume) for which it
 * returns 0, as the Team Battle grid does a fighter already in the team
 * (Tournament Mode's pools). NULL lifts it. */
void tmst_unified_select_set_allowed(int (*allowed)(unsigned id, unsigned costume)) __attribute__((weak));

#ifdef __cplusplus
}
#endif
#endif
