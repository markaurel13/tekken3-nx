/* tmst.tournament: the bracket, as data (no game memory: built and tested alone).
 *
 * 16 fighters in single elimination, 8 on each side, and the boss apart, the
 * 17th, waiting in the middle for the Grand Final:
 *
 *   slots 0..7  (left)   R1 matches 0..3  -> QF 8, 9   -> SF 12 \
 *                                                                  FINAL 14 -> GRAND FINAL 15 (vs the boss)
 *   slots 8..15 (right)  R1 matches 4..7  -> QF 10, 11 -> SF 13 /
 *
 * Match m of the first round is slots 2m and 2m + 1; a later match takes the
 * winners of two earlier ones (tmst_bracket_feeds). 16 matches; the player's
 * way is always 5 of them. A fighter is an index into the caller's roster.
 * Only the player's matches are fought: once the player's match of a round
 * is decided, the round's other matches are drawn (an even chance each). */
#ifndef TMST_BRACKET_H
#define TMST_BRACKET_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum { TMST_BRACKET_SLOTS = 16, TMST_BRACKET_MATCHES = 16, TMST_BRACKET_ROUNDS = 5,
       TMST_GRAND_FINAL = 15, TMST_NOBODY = -1 };
enum { TMST_ROUND_R1, TMST_ROUND_QF, TMST_ROUND_SF, TMST_ROUND_FINAL, TMST_ROUND_GRAND };

typedef struct {
    int slot[TMST_BRACKET_SLOTS];          /* the fighters, slot by slot */
    int boss;                              /* the Grand Final's fighter */
    int player;                            /* the player's fighter */
    int winner[TMST_BRACKET_MATCHES];      /* TMST_NOBODY until decided */
    uint32_t rng;
} TmstBracket;

/* A new bracket: the player, the boss, and 15 fighters drawn from `pool`
 * (n roster indices; the player and the boss in it are left out, as is any
 * index twice), every place drawn. `seed` 0: a fixed seed. Returns 0 when the
 * pool has fewer than 15 others. */
int tmst_bracket_new(TmstBracket *b, int player, int boss, const int *pool, int n, uint32_t seed);

/* A number in 0..n-1 from the bracket's own generator (n > 0). */
unsigned tmst_bracket_draw(TmstBracket *b, unsigned n);

int tmst_bracket_round(int match);
/* The two matches feeding a later match (-1, -1 for the first round). */
void tmst_bracket_feeds(int match, int *a, int *b);
/* The match a match's winner goes on to, -1 after the Grand Final. */
int tmst_bracket_next(int match);
/* The two fighters of a match, TMST_NOBODY where not known yet. */
void tmst_bracket_fighters(const TmstBracket *b, int match, int *a, int *c);

/* The player's next match to fight, -1 when out or the tournament is over. */
int tmst_bracket_player_match(const TmstBracket *b);
/* The player's opponent there, TMST_NOBODY when none. */
int tmst_bracket_opponent(const TmstBracket *b);
/* The player's match decided; the other matches of its round then drawn.
 * Returns a mask of the matches decided now (bit m), the player's included. */
uint32_t tmst_bracket_player_result(TmstBracket *b, int won);
/* Every match left drawn (the player out: the standings to the end). Returns
 * the mask of the matches decided now. */
uint32_t tmst_bracket_finish(TmstBracket *b);

int tmst_bracket_player_out(const TmstBracket *b);
int tmst_bracket_champion(const TmstBracket *b);    /* the Grand Final's winner, TMST_NOBODY before */
/* The match a fighter lost, -1 when it has not lost (yet). */
int tmst_bracket_lost_at(const TmstBracket *b, int fighter);

#ifdef __cplusplus
}
#endif
#endif
