/* tmst.tournament: the five tournaments' pools and bosses (tmst_tournament_pools.h).
 * Each tournament's fighters, as the King of Iron Fist Tournament of its game
 * had them; edit here only. */
#include "tmst_tournament_pools.h"
#include <string.h>

static const char *const tekken1[] = {
    "law", "king", "heihachi", "kazuya", "kunimitsu", "yoshimitsu", "jack2", "pjack", "lee", "wang", "kuma",
    "michelle", "anna", "nina", "paul", "armorking", "ganryu", NULL};
static const char *const tekken2[] = {
    "law", "king", "heihachi", "kazuya", "kunimitsu", "yoshimitsu", "jack2", "pjack", "lee", "wang", "kuma",
    "michelle", "anna", "nina", "paul", "armorking", "ganryu",
    "angel", "devil", "alex", "roger", "baek", "jun", "lei", "bruce", NULL};
static const char *const tekken3[] = {
    "jin", "xiaoyu", "paul", "law", "nina", "king", "lei", "hwoarang", "eddy", "tiger", "panda", "kuma", "bryan",
    "gunjack", "mokujin", "drb", "gon", "anna", "ogre", "trueogre", "yoshimitsu", "julia", "heihachi", NULL};

static const char *const not_in_tag[] = {"gon", "drb", NULL};       /* the user's choice, 2026-11-02 */

static const TmstTourney tourneys[TMST_TOURNEYS] = {
    {"t1", "TEKKEN 1 TOURNAMENT", tekken1, NULL, "heihachi", NULL},
    {"t2", "TEKKEN 2 TOURNAMENT", tekken2, NULL, "kazuya", "devil"},
    {"t3", "TEKKEN 3 TOURNAMENT", tekken3, NULL, "ogre", "trueogre"},
    {"ttt", "TEKKEN TAG TOURNAMENT", NULL, not_in_tag, "unknown", NULL},
    {"free", "FREE TOURNAMENT", NULL, NULL, NULL, NULL},
};

const TmstTourney *tmst_tourney(int t) { return t >= 0 && t < TMST_TOURNEYS ? &tourneys[t] : NULL; }

int tmst_tourney_has(int t, const char *key) {
    const TmstTourney *d = tmst_tourney(t);
    if (!d || !key) return 0;
    if (!d->keys) {
        for (const char *const *k = d->out; k && *k; k++) if (!strcmp(*k, key)) return 0;
        return 1;
    }
    for (const char *const *k = d->keys; *k; k++) if (!strcmp(*k, key)) return 1;
    return 0;
}
