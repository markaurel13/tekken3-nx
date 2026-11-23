/* tmst.tournament: the tournaments' stages (tmst_tournament_stages.h). The
 * user's lists (2026-11-06); edit here only. */
#include "tmst_tournament_stages.h"
#include "tmst_tournament_pools.h"
#include <string.h>

typedef struct { const char *key; int stage; } Home;

enum { SCHOOL = 7 };    /* T3's and TTT's school stage: Xiaoyu and Jin in the school outfit */

/* Tekken 2's stages: 0 Grand Canyon .. 22 Mirror Darkness. */
static const Home tekken1[] = {
    {"wang", 1}, {"paul", 3}, {"king", 4}, {"heihachi", 6}, {"yoshimitsu", 7}, {"jack2", 8}, {"law", 9},
    {"kunimitsu", 10}, {"nina", 11}, {"armorking", 12}, {"lee", 13}, {"kuma", 14}, {"ganryu", 15}, {"anna", 16},
    {"pjack", 18}, {"michelle", 19}, {"kazuya", 21}, {NULL, -1}};
static const Home tekken2[] = {
    {"bruce", 0}, {"wang", 1}, {"jun", 2}, {"paul", 3}, {"king", 4}, {"lei", 5}, {"heihachi", 6},
    {"yoshimitsu", 7}, {"jack2", 8}, {"law", 9}, {"kunimitsu", 10}, {"nina", 11}, {"armorking", 12}, {"lee", 13},
    {"kuma", 14}, {"ganryu", 15}, {"anna", 16}, {"roger", 17}, {"alex", 14}, {"pjack", 18}, {"michelle", 19},
    {"baek", 20}, {"kazuya", 21}, {"devil", 22}, {"angel", 22}, {NULL, -1}};
/* Tekken 3's arenas. */
static const Home tekken3[] = {
    {"paul", 0}, {"bryan", 0}, {"law", 1}, {"kuma", 1}, {"panda", 1}, {"yoshimitsu", 2}, {"mokujin", 2},
    {"xiaoyu", 3}, {"hwoarang", 4}, {"lei", 5}, {"ogre", 6}, {"trueogre", 6}, {"jin", 8}, {"nina", 9},
    {"anna", 9}, {"gunjack", 9}, {"eddy", 10}, {"tiger", 10}, {"king", 11}, {"julia", 12}, {"heihachi", 12},
    {"gon", 13}, {"drb", 14}, {NULL, -1}};
/* Tekken Tag's stages. */
static const Home tag[] = {
    {"paul", 0}, {"bryan", 0}, {"bruce", 0}, {"law", 1}, {"kuma", 1}, {"panda", 1}, {"lee", 1},
    {"yoshimitsu", 2}, {"mokujin", 2}, {"kunimitsu", 2}, {"tetsujin", 2}, {"xiaoyu", 3}, {"wang", 3},
    {"hwoarang", 4}, {"baek", 4}, {"lei", 5}, {"ogre", 6}, {"trueogre", 6}, {"devil", 6}, {"angel", 6},
    {"jin", 8}, {"jun", 8}, {"nina", 9}, {"anna", 9}, {"gunjack", 9}, {"pjack", 9}, {"jack2", 9},
    {"eddy", 10}, {"tiger", 10}, {"roger", 10}, {"alex", 10}, {"king", 11}, {"armorking", 11},
    {"julia", 12}, {"heihachi", 12}, {"michelle", 12}, {"ganryu", 12}, {"kazuya", 12}, {"unknown", 14},
    {NULL, -1}};

/* The Console pieces (addons/tmst/workspace/t2-music/console, ttt-music/console;
 * the Tekken 3 disc's XA). */
static const char *const t2_music[23] = {
    "bruce.bgm", "t2-wang.bgm", "t2-jun.bgm", "t2-paul.bgm", "t2-king.bgm", "t2-lei.bgm", "t2-heihachi.bgm",
    "t2-yoshimitsu.bgm", "jack2.bgm", "t2-law.bgm", "t2-kunimitsu.bgm", "t2-nina.bgm", "t2-armor-king.bgm", "lee.bgm",
    "t2-kuma.bgm", "t2-ganryu.bgm", "t2-anna.bgm", "t2-roger.bgm", "pjack.bgm", "michelle.bgm", "t2-baek.bgm",
    "t2-kazuya.bgm", "devil.bgm"};
static const char *const ttt_music[15] = {
    "ps2-paul.bgm", "ps2-law.bgm", "ps2-yoshimitsu.bgm", "ps2-xiaoyu.bgm", "ps2-hwoarang.bgm", "ps2-lei.bgm",
    "ps2-ogre.bgm", "ps2-school.bgm", "ps2-jin.bgm", "ps2-nina.bgm", "ps2-eddy.bgm", "ps2-king.bgm",
    "ps2-heihachi.bgm", NULL, "ps2-unknown.bgm"};
/* The Arcade pieces (issue #9: GAME OPTION's BGM SELECT; t2-music/arcade,
 * ttt-music/arcade). Tekken 3's come from the game's own table (the game side). */
static const char *const t2_arcade[23] = {
    "t2-bruce.bgm", "t2-wang.bgm", "t2-jun.bgm", "t2-paul.bgm", "t2-king.bgm", "t2-lei.bgm", "t2-heihachi.bgm",
    "t2-yoshimitsu.bgm", "t2-jack-2.bgm", "t2-law.bgm", "t2-kunimitsu.bgm", "t2-nina.bgm", "t2-armor-king.bgm",
    "t2-lee.bgm", "t2-kuma.bgm", "t2-ganryu.bgm", "t2-anna.bgm", "t2-roger.bgm", "t2-pjack.bgm", "t2-michelle.bgm",
    "t2-baek.bgm", "t2-kazuya.bgm", "t2-devil.bgm"};
static const char *const ttt_arcade[15] = {
    "arcade-paul.bgm", "arcade-law.bgm", "arcade-yoshimitsu.bgm", "arcade-xiaoyu.bgm", "arcade-hwoarang.bgm",
    "arcade-lei.bgm", "arcade-ogre.bgm", "arcade-school.bgm", "arcade-jin.bgm", "arcade-nina.bgm", "arcade-eddy.bgm",
    "arcade-king.bgm", "arcade-heihachi.bgm", NULL, "arcade-unknown.bgm"};
typedef struct { const char *key, *piece; } Theme;
static const Theme t3_music[] = {
    {"paul", "xa-42"}, {"law", "xa-35"}, {"yoshimitsu", "xa-46"}, {"xiaoyu", "xa-45"}, {"hwoarang", "xa-30"},
    {"lei", "xa-36"}, {"ogre", "xa-39"}, {"trueogre", "xa-41"}, {"jin", "xa-31"}, {"nina", "xa-38"},
    {"anna", "xa-21"}, {"gunjack", "xa-27"}, {"eddy", "xa-25"}, {"tiger", "xa-44"}, {"king", "xa-33"},
    {"julia", "xa-32"}, {"heihachi", "xa-29"}, {"gon", "xa-26"}, {"drb", "xa-22"}, {"bryan", "xa-23"},
    {"kuma", "xa-34"}, {"panda", "xa-34"}, {"mokujin", "xa-37"}, {NULL, NULL}};

const char *tmst_t2_stage_music(int stage, int arcade) {
    if (stage < 0 || stage >= 23) return NULL;
    return arcade ? t2_arcade[stage] : t2_music[stage];
}

const char *tmst_tourney_music(int t, const char *key, int kind, int stage, int arcade, const char **cat) {
    *cat = NULL;
    if (kind == TMST_TSTAGE_T2 && (t == TMST_TOURNEY_T1 || t == TMST_TOURNEY_T2) && stage >= 0 && stage < 23) {
        *cat = arcade ? "t2-arcade" : "t2-console";
        return tmst_t2_stage_music(stage, arcade);
    }
    if (kind == TMST_TSTAGE_TTT && t == TMST_TOURNEY_TTT && stage >= 0 && stage < 15 && ttt_music[stage]) {
        *cat = arcade ? "ttt-arcade" : "ttt-console";
        return arcade ? ttt_arcade[stage] : ttt_music[stage];
    }
    if (kind == TMST_TSTAGE_T3 && t == TMST_TOURNEY_T3 && key)
        for (const Theme *m = t3_music; m->key; m++)
            if (!strcmp(m->key, key)) { *cat = "t3-console"; return m->piece; }
    return NULL;
}

static int home(const Home *list, const char *key) {
    for (const Home *h = list; h->key; h++) if (!strcmp(h->key, key)) return h->stage;
    return -1;
}

int tmst_tourney_stage(int t, const char *key, int school, int *stage) {
    int kind = TMST_TSTAGE_NONE, s = -1;
    if (key) switch (t) {
    case TMST_TOURNEY_T1: kind = TMST_TSTAGE_T2; s = home(tekken1, key); break;
    case TMST_TOURNEY_T2: kind = TMST_TSTAGE_T2; s = home(tekken2, key); break;
    case TMST_TOURNEY_T3: kind = TMST_TSTAGE_T3; s = home(tekken3, key); break;
    case TMST_TOURNEY_TTT: kind = TMST_TSTAGE_TTT; s = home(tag, key); break;
    default: break;
    }
    if (s >= 0 && school && (kind == TMST_TSTAGE_T3 || kind == TMST_TSTAGE_TTT) &&
        (!strcmp(key, "xiaoyu") || !strcmp(key, "jin")))
        s = SCHOOL;
    if (s < 0) kind = TMST_TSTAGE_NONE;
    *stage = s;
    return kind;
}

/* GAME OPTION's STAGES = TEKKEN 2 (issue #11, the user's list, 2026-11-13):
 * Tekken 2's fighters their Tekken 2 stage (the T2 tournament's), the others
 * their Tekken 3 arena or, for TTT1's, their TTT stage. */
static const Home option_t3[] = {
    {"bryan", 0}, {"mokujin", 2}, {"xiaoyu", 3}, {"hwoarang", 4}, {"ogre", 6}, {"trueogre", 6}, {"jin", 8},
    {"gunjack", 9}, {"eddy", 10}, {"julia", 12}, {"gon", 13}, {"drb", 14}, {NULL, -1}};
static const Home option_ttt[] = {
    {"panda", 1}, {"tetsujin", 2}, {"tiger", 13}, {"unknown", 14}, {NULL, -1}};

int tmst_option_t2_stage(const char *key, int school, int *stage) {
    int s;
    *stage = -1;
    if (!key) return TMST_TSTAGE_NONE;
    if (school && (!strcmp(key, "xiaoyu") || !strcmp(key, "jin"))) { *stage = SCHOOL; return TMST_TSTAGE_T3; }
    if ((s = home(tekken2, key)) >= 0) { *stage = s; return TMST_TSTAGE_T2; }
    if ((s = home(option_t3, key)) >= 0) { *stage = s; return TMST_TSTAGE_T3; }
    if ((s = home(option_ttt, key)) >= 0) { *stage = s; return TMST_TSTAGE_TTT; }
    return TMST_TSTAGE_NONE;
}
/* RANDOM's Tekken 2 lot for Tekken 3's newcomers (issue #11, the user's list,
 * 2026-11-14): their T3 and TTT stages are the same place, so without a lot
 * RANDOM gave them their default only. Not for the TEKKEN 2 value. */
static const Home random_t2[] = {
    {"xiaoyu", 1}, {"panda", 1}, {"jin", 2}, {"bryan", 3}, {"mokujin", 7}, {"tetsujin", 7}, {"gunjack", 8},
    {"drb", 8}, {"gon", 17}, {"eddy", 17}, {"tiger", 17}, {"julia", 19}, {"hwoarang", 20}, {"ogre", 22},
    {"trueogre", 22}, {"unknown", 22}, {NULL, -1}};

int tmst_random_t2_stage(const char *key, int school, int *stage) {
    *stage = -1;
    if (!key) return 0;
    /* the school outfit: T3 or TTT only (the user, 2026-11-14) */
    if (school && (!strcmp(key, "xiaoyu") || !strcmp(key, "jin"))) return 0;
    if ((*stage = home(tekken2, key)) < 0) *stage = home(random_t2, key);
    return *stage >= 0;
}
