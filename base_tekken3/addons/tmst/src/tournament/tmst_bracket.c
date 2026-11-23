/* tmst.tournament: the bracket (tmst_bracket.h). */
#include "tmst_bracket.h"
#include <string.h>

unsigned tmst_bracket_draw(TmstBracket *b, unsigned n) {
    uint32_t x = b->rng ? b->rng : 0x9e3779b9u;           /* xorshift32 */
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    b->rng = x;
    return n ? (unsigned)((uint64_t)x * n >> 32) : 0;
}

int tmst_bracket_new(TmstBracket *b, int player, int boss, const int *pool, int n, uint32_t seed) {
    memset(b, 0, sizeof *b);
    b->rng = seed ? seed : 0x2545f491u;
    b->player = player; b->boss = boss;
    for (int m = 0; m < TMST_BRACKET_MATCHES; m++) b->winner[m] = TMST_NOBODY;
    int others[256], k = 0;
    for (int i = 0; i < n && k < 256; i++) {
        int f = pool[i], seen = f == player || f == boss || f < 0;
        for (int j = 0; j < k && !seen; j++) seen = others[j] == f;
        if (!seen) others[k++] = f;
    }
    if (k < TMST_BRACKET_SLOTS - 1) return 0;
    for (int i = 0; i < TMST_BRACKET_SLOTS - 1; i++) {    /* 15 of them, each drawn */
        int j = i + (int)tmst_bracket_draw(b, (unsigned)(k - i)), t = others[i];
        others[i] = others[j]; others[j] = t;
    }
    int at = (int)tmst_bracket_draw(b, TMST_BRACKET_SLOTS);  /* the player's place */
    for (int s = 0, i = 0; s < TMST_BRACKET_SLOTS; s++) b->slot[s] = s == at ? player : others[i++];
    return 1;
}

int tmst_bracket_round(int m) {
    return m < 8 ? TMST_ROUND_R1 : m < 12 ? TMST_ROUND_QF : m < 14 ? TMST_ROUND_SF : m < 15 ? TMST_ROUND_FINAL : TMST_ROUND_GRAND;
}
void tmst_bracket_feeds(int m, int *a, int *c) {
    if (m < 8) { *a = *c = -1; return; }
    if (m < 12) { *a = (m - 8) * 2; *c = *a + 1; return; }
    if (m < 14) { *a = 8 + (m - 12) * 2; *c = *a + 1; return; }
    if (m == 14) { *a = 12; *c = 13; return; }
    *a = 14; *c = -1;                                      /* the Grand Final: the final's winner and the boss */
}
int tmst_bracket_next(int m) {
    if (m < 8) return 8 + m / 2;
    if (m < 12) return 12 + (m - 8) / 2;
    if (m < 14) return 14;
    return m == 14 ? TMST_GRAND_FINAL : -1;
}
void tmst_bracket_fighters(const TmstBracket *b, int m, int *a, int *c) {
    if (m < 8) { *a = b->slot[2 * m]; *c = b->slot[2 * m + 1]; return; }
    int fa, fc; tmst_bracket_feeds(m, &fa, &fc);
    *a = b->winner[fa];
    *c = fc >= 0 ? b->winner[fc] : b->boss;
}

static int in_match(const TmstBracket *b, int m, int f) {
    int a, c; tmst_bracket_fighters(b, m, &a, &c);
    return f != TMST_NOBODY && (a == f || c == f);
}
int tmst_bracket_lost_at(const TmstBracket *b, int f) {
    for (int m = 0; m < TMST_BRACKET_MATCHES; m++)
        if (b->winner[m] != TMST_NOBODY && b->winner[m] != f && in_match(b, m, f)) return m;
    return -1;
}
int tmst_bracket_player_out(const TmstBracket *b) { return tmst_bracket_lost_at(b, b->player) >= 0; }
int tmst_bracket_champion(const TmstBracket *b) { return b->winner[TMST_GRAND_FINAL]; }

int tmst_bracket_player_match(const TmstBracket *b) {
    if (tmst_bracket_player_out(b)) return -1;
    for (int m = 0; m < TMST_BRACKET_MATCHES; m++)
        if (b->winner[m] == TMST_NOBODY && in_match(b, m, b->player)) return m;
    return -1;
}
int tmst_bracket_opponent(const TmstBracket *b) {
    int m = tmst_bracket_player_match(b), a, c;
    if (m < 0) return TMST_NOBODY;
    tmst_bracket_fighters(b, m, &a, &c);
    return a == b->player ? c : a;
}

/* The undecided matches of a round whose two fighters are known, drawn. */
static uint32_t draw_round(TmstBracket *b, int round) {
    uint32_t mask = 0;
    for (int m = 0; m < TMST_BRACKET_MATCHES; m++) {
        if (tmst_bracket_round(m) != round || b->winner[m] != TMST_NOBODY) continue;
        int a, c; tmst_bracket_fighters(b, m, &a, &c);
        if (a == TMST_NOBODY || c == TMST_NOBODY) continue;
        b->winner[m] = tmst_bracket_draw(b, 2) ? c : a;
        mask |= 1u << m;
    }
    return mask;
}

uint32_t tmst_bracket_player_result(TmstBracket *b, int won) {
    int m = tmst_bracket_player_match(b), a, c;
    if (m < 0) return 0;
    tmst_bracket_fighters(b, m, &a, &c);
    b->winner[m] = won ? b->player : (a == b->player ? c : a);
    return 1u << m | draw_round(b, tmst_bracket_round(m));
}

uint32_t tmst_bracket_finish(TmstBracket *b) {
    uint32_t mask = 0;
    for (int r = TMST_ROUND_R1; r <= TMST_ROUND_GRAND; r++) mask |= draw_round(b, r);
    return mask;
}
