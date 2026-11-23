/* tmst.tournament: which tournaments are won, kept beside the memory card
 * (tournament.txt, as the Jukebox's jukebox.txt and the Expanded's own
 * ttt1-guest-records.txt). Plain stdio, built and tested alone.
 *
 *   # Tournament Mode (TMST add-on): progress, written by the game.
 *   won = t1, t2
 *
 * The first tournament is always open; each of the others opens once the
 * one before it is won (the Free Tournament after Tekken Tag, for good). */
#ifndef TMST_TOURNAMENT_PROGRESS_H
#define TMST_TOURNAMENT_PROGRESS_H
#ifdef __cplusplus
extern "C" {
#endif

/* 1 opens every tournament (it was so while the mode was tested, the user's
 * choice 2026-11-01, locked again 2026-11-02); 0 ties them to the progress.
 * The environment's TMST_TOURNAMENT_ALL_OPEN=1 opens them too (the headless
 * tests). */
#ifndef TMST_TOURNAMENT_ALL_OPEN
#define TMST_TOURNAMENT_ALL_OPEN 0
#endif

typedef struct { unsigned won; } TmstProgress;   /* bit t: tournament t won */

/* Read from `path` (a missing or unreadable file: nothing won). */
void tmst_progress_load(TmstProgress *p, const char *path);
/* Written to `path`; returns 0 when it cannot be. */
int tmst_progress_save(const TmstProgress *p, const char *path);
int tmst_progress_open(const TmstProgress *p, int t);
/* Tournament t won: noted (the caller saves). Returns the tournament it
 * opens now, -1 none. */
int tmst_progress_win(TmstProgress *p, int t);

#ifdef __cplusplus
}
#endif
#endif
