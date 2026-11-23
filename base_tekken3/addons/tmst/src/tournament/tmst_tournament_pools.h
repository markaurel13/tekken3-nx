/* tmst.tournament: the five tournaments, as data.
 *
 * A fighter is named by its key (the Jukebox's keys: the stock fighters'
 * "paul" .. "trueogre", "panda" and "tiger" for Kuma's second and Eddy's
 * third costume, the Expanded's guests by their catalogue key, "kazuya",
 * "armorking", "jack2" ...). Which of them this install has is the game
 * side's business (src/tournament/tmst_tournament.c): this file knows only
 * names, so it builds and is tested alone. */
#ifndef TMST_TOURNAMENT_POOLS_H
#define TMST_TOURNAMENT_POOLS_H
#ifdef __cplusplus
extern "C" {
#endif

enum { TMST_TOURNEY_T1, TMST_TOURNEY_T2, TMST_TOURNEY_T3, TMST_TOURNEY_TTT, TMST_TOURNEY_FREE, TMST_TOURNEYS };

typedef struct {
    const char *id;                  /* "t1" .. "free": the progress file's word */
    const char *name;                /* upper case, the screens' font */
    const char *const *keys;         /* the pool, NULL-terminated; NULL = the whole roster */
    const char *const *out;          /* with the whole roster: those left out, NULL-terminated; NULL none */
    const char *boss;                /* the fixed Grand Final boss; NULL = drawn from the roster at each start */
    const char *boss2;               /* the boss's second form, its second round (Kazuya, then Devil); NULL none */
} TmstTourney;

const TmstTourney *tmst_tourney(int t);
/* Nonzero when `key` belongs to tournament t's pool (the whole roster:
 * always), the boss and its second form included. */
int tmst_tourney_has(int t, const char *key);

#ifdef __cplusplus
}
#endif
#endif
