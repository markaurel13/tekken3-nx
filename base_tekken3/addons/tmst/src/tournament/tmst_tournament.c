/* tmst.tournament: the King of Iron Fist Tournament, a mode of its own.
 *
 * TOURNAMENT MODE, the main menu's row under TEKKEN FORCE MODE
 * (src/core/tmst_menu.c), opens the Team Battle grid: the team size prompt is
 * answered for the player (1 against 1), and the grid's lower part shows the
 * five tournaments (src/tournament/tmst_tournament_pools.c): up / down choose,
 * CROSS takes an open one, CIRCLE leaves to the menu. Then the grid is the
 * tournament's: the fighters outside its pool, and its final boss, are grey
 * and refused as the grid refuses a fighter already in the team (the player
 * block's availability mask, ANDed at every pad read, on whichever page L2 /
 * R2 show), and the confirm buttons are swallowed on them; SELECT goes back to
 * the list. The fighter chosen, the bracket is drawn (src/tournament/
 * tmst_bracket.c): 16 fighters, the boss apart.
 *
 * The matches. Each is the Team Battle's 1 against 1, the CPU's member the
 * player's opponent in the bracket: written as the game hands the member to
 * the fighters (0x8004F0E4, chained), the CPU's team size held at 0 until the
 * fight so the Expanded's team draw leaves it alone. Its AI level rises with
 * the round (0x800AE211, within the menu's difficulty); the rounds to win
 * (two, the bosses their own: match_rounds) are written where the Arcade
 * puts GAME OPTION's; each round full life for both. A boss with a second
 * form is a CPU team of two (Kazuya, then Devil). The fight's HUD reads
 * TOURNAMENT MODE, its opening words the tournament's name, its team figures
 * are the Arcade's round lamps. The TEAM BATTLE RESULTS tell who won (never
 * shown: covered, skipped, their music silent); the way back to the grid is
 * driven under the tournament's screens (src/tournament/
 * tmst_tournament_screens.c: the bracket, the next match, CHAMPION or
 * TOURNAMENT OVER), the next fight starting on the player's CROSS.
 *
 * With Unified Character Select on (tmst_unified_select_active), its grid
 * gets the same rule through tmst_unified_select_set_allowed.
 *
 * Progress: tournament.txt beside the memory card
 * (src/tournament/tmst_tournament_progress.c). */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_ui_font.h"
#include "tmst_overlay.h"
#include "tmst_menu.h"
#include "tmst_music.h"
#include "tmst_input.h"
#include "tmst_spu.h"
#include "tmst_bracket.h"
#include "tmst_tournament_pools.h"
#include "tmst_tournament_progress.h"
#include "tmst_tournament_stages.h"
#include "tmst_tournament_art.h"
#include "tmst_tournament_screens.h"
#include "tmst_tournament_story.h"
#include "tmst_tournament_options.h"
#include "tmst_tournament_cine.h"
#include "tmst_files.h"
#include "cpu_state.h"
#include "mod_plugins.h"
#include "memcard.h"
#include "psx_sdl.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <strings.h>
#endif

/* The Expanded's TTT1 roster (weak: absent without it). */
extern const char *tekken3_guest_key(unsigned id) __attribute__((weak));
extern unsigned tekken3_ttt1_grid_character(unsigned cell) __attribute__((weak));
extern int tekken3_ttt1_selector_page(void) __attribute__((weak));
/* The Expanded's MOVESET card and the CPU's moves (weak too). */
extern int tekken3_native_moves_available(unsigned id) __attribute__((weak));
extern int tekken3_native_moves_assign(unsigned slot, unsigned id, int ttt1) __attribute__((weak));
/* A guest that switches movesets on a button (Unknown). */
extern int tekken3_guest_switches_on_button(unsigned player) __attribute__((weak));
/* A native drawn by its TTT1 model with a triangle (RP) model of its own: R1
 * gives its third costume (src/tekken3_native_moves.c turn_keys; issue #14). */
extern int tekken3_ttt1_native_rp_model(unsigned id) __attribute__((weak));
extern int tekken3_outfits_captures_input(int pad) __attribute__((weak));

enum { SCREEN = 0x800ae204, PHASE = 0x800ae224, MODE = 0x800afa88, MENU = 4, GRID = 10, TEAM_BATTLE = 2,
       TEKKEN_FORCE = 8, STOCK_GRID = 0x80022768, THIRD_COSTUMES = 0x80097ef4,
       GRID_BLOCK = 0x800b8d70, BLOCK_STRIDE = 172, B_X = 8, B_Y = 12, B_MASK = 36, B_SIZE = 40, B_COUNT = 44,
       B_CHOICES = 56, SIZE_PROMPT = 1, CHOOSING = 2, EMPTY = 88,
       STOCK = 21, TRUE_OGRE = 20, GUEST_FIRST = 23, GUEST_MAX = 32, KUMA = 11, EDDY = 8, MAX_FIGHTERS = 64, KEY = 24 };
enum { B_SELECT = TMST_PAD_SELECT, B_START = TMST_PAD_START, B_UP = TMST_PAD_UP, B_RIGHT = TMST_PAD_RIGHT,
       B_DOWN = TMST_PAD_DOWN, B_LEFT = TMST_PAD_LEFT, B_L2 = TMST_PAD_L2, B_R2 = TMST_PAD_R2,
       B_TRIANGLE = TMST_PAD_TRIANGLE, B_CIRCLE = TMST_PAD_CIRCLE, B_CROSS = TMST_PAD_CROSS, B_SQUARE = TMST_PAD_SQUARE,
       B_R1 = TMST_PAD_R1, B_FACE = B_TRIANGLE | B_CIRCLE | B_CROSS | B_SQUARE };
enum { VIEW_OFF, VIEW_LIST, VIEW_GRID, VIEW_PICKED };

static const char *const stock_keys[STOCK] = {"paul","law","lei","king","yoshimitsu","nina","hwoarang","xiaoyu",
    "eddy","jin","julia","kuma","bryan","heihachi","ogre","mokujin","gunjack","gon","anna","drb","trueogre"};
/* The bracket's names (DOCTOR B.: the user's, issue #14). */
static const char *const stock_names[STOCK] = {"PAUL PHOENIX","FOREST LAW","LEI WULONG","KING","YOSHIMITSU",
    "NINA WILLIAMS","HWOARANG","LING XIAOYU","EDDY GORDO","JIN KAZAMA","JULIA CHANG","KUMA","BRYAN FURY",
    "HEIHACHI MISHIMA","OGRE","MOKUJIN","GUN JACK","GON","ANNA WILLIAMS","DOCTOR B.","TRUE OGRE"};

static int on;                         /* the feature, at activation */

/* ---- the roster this install has ------------------------------------------- */
typedef struct { char key[KEY]; char name[40]; unsigned id, costume; } Fighter;
static Fighter roster[MAX_FIGHTERS];
static int roster_n;

/* The guests' full names (the user's list, 2026-11-02); others: their label. */
static const char *const full_names[][2] = {
    {"kazuya", "KAZUYA MISHIMA"}, {"jun", "JUN KAZAMA"}, {"michelle", "MICHELLE CHANG"}, {"baek", "BAEK DOO SAN"},
    {"armorking", "ARMOR KING"}, {"bruce", "BRUCE IRVIN"}, {"kunimitsu", "KUNIMITSU"}, {"jack2", "JACK-2"},
    {"lee", "LEE CHAOLAN"}, {"ganryu", "GANRYU"}, {"pjack", "PROTOTYPE JACK"}, {"devil", "DEVIL"}, {"angel", "ANGEL"},
    {"roger", "ROGER"}, {"alex", "ALEX"}, {"tetsujin", "TETSUJIN"}, {"wang", "WANG JINREI"}, {"unknown", "UNKNOWN"},
    {"tiger", "TIGER JACKSON"}};
static const char *full_name(const char *key) {
    for (size_t i = 0; i < sizeof full_names / sizeof full_names[0]; i++)
        if (!strcasecmp(full_names[i][0], key)) return full_names[i][1];
    return NULL;
}
/* A guest's name: its full name, else <game>/mods/ttt1/<Key>-T3-label.txt,
 * whatever the key's case. */
static void guest_label(const char *key, char *out, size_t size) {
    snprintf(out, size, "%s", key);
    if (full_name(key)) { snprintf(out, size, "%s", full_name(key)); return; }
    const char *base = SDL_GetBasePath();
    char dir[4200]; snprintf(dir, sizeof dir, "%smods/ttt1", base ? base : "./");
#if !defined(PSX_SDL3)
    if (base) SDL_free((void *)base);
#endif
    char path[4400] = "";
#ifdef _WIN32
    char pattern[4300]; snprintf(pattern, sizeof pattern, "%s\\%s-T3-label.txt", dir, key);
    WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA(pattern, &fd);       /* case-insensitive on Windows */
    if (h != INVALID_HANDLE_VALUE) { snprintf(path, sizeof path, "%s\\%s", dir, fd.cFileName); FindClose(h); }
#else
    DIR *d = opendir(dir); struct dirent *e;
    char want[96]; snprintf(want, sizeof want, "%s-T3-label.txt", key);
    while (d && (e = readdir(d))) if (!strcasecmp(e->d_name, want)) { snprintf(path, sizeof path, "%s/%s", dir, e->d_name); break; }
    if (d) closedir(d);
#endif
    FILE *f = path[0] ? fopen(path, "rb") : NULL;
    if (f) {
        char line[96];
        if (fgets(line, sizeof line, f)) {
            char *s = line; while (isspace((unsigned char)*s)) s++;
            size_t n = strlen(s); while (n && isspace((unsigned char)s[n - 1])) s[--n] = 0;
            if (*s) snprintf(out, size, "%s", s);
        }
        fclose(f);
    }
    for (char *p = out; *p; p++) *p = (char)toupper((unsigned char)*p);
}
static void add_fighter(const char *key, const char *name, unsigned id, unsigned costume) {
    if (roster_n >= MAX_FIGHTERS || strlen(key) >= KEY) return;
    Fighter *f = &roster[roster_n++];
    snprintf(f->key, KEY, "%s", key); snprintf(f->name, sizeof f->name, "%s", name); f->id = id; f->costume = costume;
}
/* The stock fighters, Panda (Kuma's second costume) and Tiger (Eddy's third),
 * the Expanded's guests: read once, when the grid first opens (the guests'
 * roster is made at the game's start). */
static void build_roster(void) {
    if (roster_n) return;
    for (unsigned id = 0; id < STOCK; id++) {
        add_fighter(stock_keys[id], stock_names[id], id, 0);
        art_load_stock_hd(stock_keys[id], id);       /* Expanded 1.5.1's, where made */
    }
    add_fighter("panda", "PANDA", KUMA, 1);
    add_fighter("tiger", full_name("tiger"), EDDY, 2);
    for (unsigned id = GUEST_FIRST; tekken3_guest_key && id < GUEST_FIRST + GUEST_MAX; id++) {
        const char *key = tekken3_guest_key(id);
        if (!key) break;
        char name[40]; guest_label(key, name, sizeof name);
        add_fighter(key, name, id, 0);
        art_load_guest(key);
    }
    art_load_guest("panda"); art_load_guest("tiger");
    tmst_log("tournament: %d fighters in the roster", roster_n);
}
static int roster_find(const char *key) {
    for (int i = 0; key && i < roster_n; i++) if (!strcmp(roster[i].key, key)) return i;
    return -1;
}
static const char *stock_key(unsigned id) {
    if (id < STOCK) return stock_keys[id];
    return id >= GUEST_FIRST && tekken3_guest_key ? tekken3_guest_key(id) : NULL;
}

/* ---- the tournaments ---------------------------------------------------------- */
static TmstProgress progress;
static char progress_file[4200];
static void progress_path(void) {
    const char *card = NULL;
    progress_file[0] = 0;
    const char *test = getenv("TMST_TOURNAMENT_PROGRESS");          /* a test's own file */
    if (test && *test) { snprintf(progress_file, sizeof progress_file, "%s", test); return; }
    if (memcard_debug_info(0, &card, NULL, NULL, NULL) || !card || !*card) return;
    const char *slash = strrchr(card, '/'), *back = strrchr(card, '\\');
    if (back && (!slash || back > slash)) slash = back;
    int n = slash ? (int)(slash - card) : 1;
    snprintf(progress_file, sizeof progress_file, "%.*s/tournament.txt", n, slash ? card : ".");
}

/* A tournament's fighters in this install (roster indices; the boss's second
 * form never in the bracket), its boss's index (-1 drawn at the start, -2
 * missing). Returns how many. */
static int pool_of(int t, int *pool, int *boss) {
    const TmstTourney *d = tmst_tourney(t);
    int n = 0;
    for (int i = 0; i < roster_n; i++)
        if (tmst_tourney_has(t, roster[i].key) && !(d->boss2 && !strcmp(d->boss2, roster[i].key))) pool[n++] = i;
    *boss = d->boss ? roster_find(d->boss) : -1;
    if (d->boss && *boss < 0) *boss = -2;
    return n;
}
/* Playable here: its boss there and 15 others besides the player. */
static int playable(int t) {
    int pool[MAX_FIGHTERS], boss, n = pool_of(t, pool, &boss);
    return boss != -2 && n - (boss >= 0) - 1 >= TMST_BRACKET_SLOTS - 1;
}
/* A key in tournament t that the player may take (the boss and its second
 * form are not). */
static int allowed_key(int t, const char *key) {
    const TmstTourney *d = tmst_tourney(t);
    return key && roster_find(key) >= 0 && tmst_tourney_has(t, key) && !(d->boss && !strcmp(d->boss, key)) &&
           !(d->boss2 && !strcmp(d->boss2, key));
}
/* A grid cell's fighter (any costume): allowed when one of its identities is. */
static int allowed_id(int t, unsigned id) {
    if (allowed_key(t, stock_key(id))) return 1;
    if (id == KUMA) return allowed_key(t, "panda");
    if (id == EDDY) return allowed_key(t, "tiger");
    return 0;
}
/* The identity a pick is in this tournament: Kuma's second costume is Panda,
 * Eddy's third Tiger, where the tournament has them apart. */
static const char *pick_key(int t, unsigned id, unsigned costume) {
    const char *key = stock_key(id);
    if (id == KUMA && (costume == 1 || !allowed_key(t, key)) && allowed_key(t, "panda")) return "panda";
    if (id == EDDY && (costume == 2 || !allowed_key(t, key)) && allowed_key(t, "tiger")) return "tiger";
    return key;
}

/* ---- the screen ---------------------------------------------------------------- */
static struct {
    int pending, active, view, exiting;
    int armed;                                /* the buttons released since the view changed */
    int sel;                                  /* the list's row */
    int tourney;                              /* the tournament being played */
    unsigned frames, left_at;
    uint16_t previous;
    unsigned version;
} tm;
static TmstBracket bracket;
static int menu_row = -1, release_wait;

static int team_grid(void) { return psx_mod_read_word(SCREEN) == GRID && psx_mod_read_word(MODE) == TEAM_BATTLE; }
static uint32_t block(int p) { return GRID_BLOCK + (uint32_t)p * BLOCK_STRIDE; }
static unsigned grid_character(unsigned cell) {
    if (tekken3_ttt1_grid_character) return tekken3_ttt1_grid_character(cell);
    return cell < 22 ? psx_mod_read_byte(STOCK_GRID + cell * 6 + 4) / 4 : 22;
}
static unsigned cursor_fighter(void) {
    unsigned x = psx_mod_read_word(block(0) + B_X), y = psx_mod_read_word(block(0) + B_Y);
    return x < 7 && y < 6 ? grid_character(y * 7 + x) : 0xffff;
}

static void begin(void) {
    tm.pending = 1; tm.active = 0; tm.exiting = 0;
    tmst_log("tournament: chosen in the main menu");
}
static void exit_to_menu(void) {
    tm.exiting = 1; tm.version++;
    if (menu_row >= 0) tmst_menu_select_next_time(menu_row);
    tmst_log("tournament: back to the main menu");
}
static int unified_allowed(unsigned id, unsigned costume) { (void)costume; return allowed_id(tm.tourney, id); }
static int unified(void) { return tmst_unified_select_active && tmst_unified_select_active(); }

/* The grid's availability, the tournament's way: the fighters of the page
 * on screen outside its pool lose their bit (the game greys and refuses
 * them). Bits of the other page are left as the Expanded set them. */
static void restrict_grid(int t) {
    if (unified()) return;
    uint32_t b = block(0), mask = psx_mod_read_word(b + B_MASK), keep = mask;
    int page = tekken3_ttt1_selector_page ? tekken3_ttt1_selector_page() : 0;
    if (page == 1) {
        for (unsigned id = GUEST_FIRST; tekken3_guest_key && id < GUEST_FIRST + GUEST_MAX && tekken3_guest_key(id); id++)
            if (!allowed_id(t, id)) mask &= ~(1u << (id & 31));
    } else {
        for (unsigned id = 0; id < STOCK; id++) if (!allowed_id(t, id)) mask &= ~(1u << id);
    }
    if (mask != keep) psx_mod_write_word(b + B_MASK, mask);
}

static int start_bracket(unsigned id, unsigned costume) {
    int t = tm.tourney, pool[MAX_FIGHTERS], boss, n = pool_of(t, pool, &boss);
    int player = roster_find(pick_key(t, id, costume));
    if (player < 0) { tmst_log("tournament: the pick (ID %u, costume %u) is not in the roster", id, costume); return 0; }
    uint32_t seed = (uint32_t)time(NULL) ^ (tm.frames * 2654435761u) ^ (uint32_t)player << 16;
    if (boss < 0) {                                       /* the Free Tournament: a boss drawn, never the player */
        TmstBracket draw = {.rng = seed | 1};
        do boss = pool[tmst_bracket_draw(&draw, (unsigned)n)]; while (boss == player);
        seed = draw.rng;
    }
    if (!tmst_bracket_new(&bracket, player, boss, pool, n, seed)) {
        tmst_log("tournament: %s has too few fighters here", tmst_tourney(t)->name); return 0;
    }
    tmst_log("tournament: %s, %s against the field, %s waits in the Grand Final", tmst_tourney(t)->name,
             roster[player].name, roster[boss].name);
    for (int s = 0; s < TMST_BRACKET_SLOTS; s += 2)
        tmst_log("tournament:   %-2d %s vs %s", s / 2 + 1, roster[bracket.slot[s]].name, roster[bracket.slot[s + 1]].name);
    return 1;
}

/* ---- the matches ------------------------------------------------------------------ */
/* The teams (0x800AFA88 + 89, 13 bytes a player: the members, +9 the members
 * lost, +12 the size), the fighters' IDs and costumes, the CPU's AI level
 * (the stage of the difficulty table, 0..9: Team Battle's own is 9), the
 * fighters' records (the rounds won at +0x44, life 16.16 at +0x3F4, its
 * maximum at +0x3F8), the fight's rounds to win less one. */
enum { TEAMS = 0x800afae1, TEAM_STRIDE = 13, T_LOST = 9, T_SIZE = 12, FIGHTERS = 0x800add5c, COSTUMES = 0x800add98,
       AI_LEVEL = 0x800ae211, ROUNDS = 0x800afaa8,
       RECORD = 0x800a9228, RECORD_STRIDE = 0x188c, ROUNDS_WON = 0x44, LIFE = 0x3f4, LIFE_MAX = 0x3f8,
       FIGHT = 8, LOADING = 11, RESULTS = 12, TITLE = 16, BOOT_LOADING = 1, AFTER_FIGHT = 18, ROUND_SETTLE = 60, ATTRACT = 6, RESULTS_READS = 2,
       CPU_MEMBER_RETURN = 0x800b0a7c };
enum { FLOW_NONE, FLOW_MATCH, FLOW_BETWEEN };             /* a match to fight; the way back to the next */
enum { NEXT_MATCH, ELIMINATED, CHAMPION, QUIT };
enum { STEP_SETTLE, STEP_PAGE, STEP_CURSOR, STEP_COSTUME, STEP_CARD };
/* The cover's screens: the story (T1, T2, T3), the opening draw, the round's winners going up, the
 * wait for START (and the question to leave), the zoom onto the next match,
 * the match view (the loading screen), the champion's or the end's. */
enum { SCR_NONE, SCR_DRAW, SCR_RESULT, SCR_WAIT, SCR_ASK, SCR_ZOOM, SCR_MATCH, SCR_END, SCR_STORY };
enum { STORY_SPEED_NUM = 3, STORY_SPEED_DEN = 10 };      /* the story's text, pixels a frame */
enum { DRAW_STEP = 6, RESULT_WAIT = 30, RESULT_MOVE = 48, ZOOM_FRAMES = 30, ZOOM_MAX = 190, MATCH_HOLD = 90 };
static const unsigned char round_level[TMST_BRACKET_ROUNDS] = {1, 3, 5, 7, 9};
static const char *const round_names[TMST_BRACKET_ROUNDS] = {"FIRST ROUND", "QUARTER FINAL", "SEMI FINAL", "FINAL",
                                                             "GRAND FINAL"};
static const uint16_t costume_button[4] = {B_SQUARE, B_CROSS, B_TRIANGLE, B_CIRCLE};
static struct {
    int flow, outcome;
    int match, opponent;                      /* the bracket's match, the opponent's roster index */
    int second, member;                       /* the boss's second form (-1 none), the CPU member placed */
    unsigned pick;                            /* the player's fighter, ID * 4 + costume */
    uint16_t key;                             /* the button that picked it (issue #14), 0 not known */
    int life_set, size_held, ai_saved, ai_was;    /* life_set: the fight begun (both lives full) */
    int fight_over;                           /* the fight's end seen (covered from there) */
    int cover, confirmed;                     /* the black cover up; START pressed on it */
    int stalled; unsigned watch_key, watch_since;   /* the cover lifted: nothing moved under it */
    int step, tries; unsigned step_at;        /* the grid, picked for the player */
    uint16_t hold, previous;                  /* the buttons pressed for the player; the last read */
    int scr; unsigned scr_at;                 /* the cover's screen, since when */
    int story_done;                           /* the story's text at its end (CROSS starts the draw) */
    uint32_t shown, fresh;                    /* matches drawn decided; decided this round (going up) */
    int loading_seen;                         /* this match's loading screen came */
    int loading_vs; unsigned loading_at;      /* its VS picture drawn (the grid's last lists before), since when */
    int loading_ok; unsigned loading_vs_at;   /* its portraits checked (vs_portraits_good) or given up on; since when drawn */
    unsigned rounds_won, round_end;           /* both fighters' rounds won; the frame a round ended, 0 none */
    int want, want_now, card_now;             /* the MOVESET card (moves_count's choices, 0 Tekken 3's): the player's
                                               * choice; where the Expanded's card stands; where it is turned to */
    int unlocked;                             /* the tournament the win opened, -1 none */
} mt = {.match = -1, .opponent = -1, .second = -1, .unlocked = -1};

static const char *fighter_name(int i) { return i >= 0 && i < roster_n ? roster[i].name : "?"; }
static const char *fighter_key(int i) { return i >= 0 && i < roster_n ? roster[i].key : ""; }
/* On the screens the player is the costume picked: Kuma's second is Panda,
 * Eddy's third Tiger, also in a tournament that has them as one fighter. */
static int player_alt(void) {
    unsigned id = mt.pick >> 2, costume = mt.pick & 3;
    return id == KUMA && costume == 1 ? 1 : id == EDDY && costume == 2 ? 2 : 0;
}
static const char *screen_key(int i) {
    if (i >= 0 && i == bracket.player && player_alt()) return player_alt() == 1 ? "panda" : "tiger";
    return fighter_key(i);
}
static const char *screen_name(int i) {             /* Tiger by his full name, as the roster's (issue #14) */
    if (i >= 0 && i == bracket.player && player_alt()) return player_alt() == 1 ? "PANDA" : full_name("tiger");
    return fighter_name(i);
}
static const ScreenRoster screen_roster = {screen_key, screen_name};
static void restore_ai_level(void) {
    if (mt.ai_saved) psx_mod_write_byte(AI_LEVEL, (uint8_t)mt.ai_was);
    mt.ai_saved = 0;
}
static void end_tournament(const char *why) {
    if (mt.flow != FLOW_NONE) tmst_log("tournament: over (%s)", why);
    restore_ai_level();
    mt.flow = FLOW_NONE; mt.cover = 0; mt.stalled = 0; mt.hold = 0; mt.match = mt.opponent = -1;
    tmst_overlay_backdrop(0, 0);
}
static void intro_new_match(void);
static void arm_match(void) {
    intro_new_match();
    mt.match = tmst_bracket_player_match(&bracket);
    mt.opponent = tmst_bracket_opponent(&bracket);
    if (mt.match < 0 || mt.opponent < 0) { end_tournament("no match left"); return; }
    mt.flow = FLOW_MATCH; mt.life_set = 0; mt.fight_over = 0; mt.stalled = 0; mt.loading_seen = 0;
    /* Left from the pause (RESET), the main menu comes back on our row, not
     * TEAM BATTLE's (issue #14). */
    if (menu_row >= 0) tmst_menu_select_next_time(menu_row);
    mt.loading_vs = 0; mt.loading_ok = 0; mt.loading_vs_at = 0; mt.loading_at = 0; mt.round_end = 0; mt.member = 0;
    const TmstTourney *d = tmst_tourney(tm.tourney);
    mt.second = mt.match == TMST_GRAND_FINAL && d->boss2 ? roster_find(d->boss2) : -1;
    int r = tmst_bracket_round(mt.match);
    tmst_log("tournament: match %d, %s: %s vs %s (AI level %u)", mt.match + 1, round_names[r],
             fighter_name(bracket.player), fighter_name(mt.opponent), round_level[r]);
}

/* The CPU's members: the opponent, and in the Grand Final of a boss with a
 * second form that form too (Kazuya, then Devil: a team of two, a round
 * each). */
static int team_size(void) { return mt.second >= 0 ? 2 : 1; }
/* The rounds to win, less one (the fight's 0x800AFAA8): two (the user's
 * choice, not GAME OPTION's FIGHT COUNT); a boss with a second form a round
 * each, Unknown (who heals) one, the Free Tournament's drawn boss three. */
static unsigned match_rounds(void) {
    if (team_size() > 1) return 0;
    if (mt.match == TMST_GRAND_FINAL && tm.tourney == TMST_TOURNEY_TTT) return 0;
    if (mt.match == TMST_GRAND_FINAL && tm.tourney == TMST_TOURNEY_FREE) return 2;
    return 1;
}
static const Fighter *member_fighter(int i) { return &roster[i && mt.second >= 0 ? mt.second : mt.opponent]; }
/* The member due: the first, or (a team of two) the next once one is lost. */
static int member_due(void) {
    int due = psx_mod_read_byte(TEAMS + TEAM_STRIDE + T_LOST);
    return due >= team_size() ? team_size() - 1 : due;
}
/* The fight's stage (tmst_core.h): the CPU's member due, its home in this
 * tournament (src/tournament/tmst_tournament_stages.c). School: the game's
 * own record for its ID and costume names the school arena (Xiaoyu's,
 * Jin's school outfit; 0x80097D40 + (ID * 4 + costume) * 4, arena at +10). */
enum { RECORDS = 0x80097d40, RECORD_ARENA = 10, SCHOOL_ARENA = 7, ARENA = 0x800adc84, ARENA_COPY = 0x800afaa2 };
static int due_stage(const Fighter *f, int *stage, int *school) {
    *school = 0;
    if (f->id < STOCK) {
        uint32_t record = psx_mod_read_word(RECORDS + (f->id * 4 + f->costume) * 4);
        *school = record >> 24 == 0x80 && psx_mod_read_byte(record + RECORD_ARENA) == SCHOOL_ARENA;
    }
    return tmst_tourney_stage(tm.tourney, f->key, *school, stage);
}
static int in_match(void) {
    return on && mt.flow == FLOW_MATCH && mt.opponent >= 0 && psx_mod_read_word(MODE) == TEAM_BATTLE;
}
/* Tekken 3's Arcade piece for its Console XA "xa-NN" (issue #9): the game's
 * music table (0x80024FD0, 32 entries of two bytes: Console, Arcade; 0x80 a
 * CD track), the entry whose Console byte it is, its Arcade byte; as the
 * Jukebox's find_default, a CD track stays in t3-console. NULL none. */
static const char *t3_arcade_piece(const char *console, const char **cat) {
    static char name[16];
    unsigned xa;
    if (!console || sscanf(console, "xa-%u", &xa) != 1) return NULL;
    for (unsigned n = 0; n < 32; n++) {
        if (psx_mod_read_byte(0x80024fd0u + n * 4) != xa) continue;
        unsigned b = psx_mod_read_byte(0x80024fd0u + n * 4 + 2);
        if (b & 0x80) { snprintf(name, sizeof name, "cd-track-%u", b & 0x7f); *cat = "t3-console"; }
        else { snprintf(name, sizeof name, "xa-%02u", b); *cat = "t3-arcade"; }
        return name;
    }
    return NULL;
}
/* The fight's music (tmst_core.h): the CPU's member due, its piece in this
 * tournament (src/tournament/tmst_tournament_stages.c) in the version of the
 * fight's music (issue #9: GAME OPTION's BGM SELECT, RANDOM's draw included;
 * SILENT: the game starts no music, the Jukebox none either); the Jukebox
 * plays it. */
int tmst_tournament_fight_music(const char **key, const char **cat, const char **name) {
    if (!in_match()) return 0;
    const Fighter *f = member_fighter(member_due());
    int stage, school, kind = due_stage(f, &stage, &school), arcade = tmst_music_version() == TMST_MUSIC_ARCADE;
    *key = f->key;
    *name = tmst_tourney_music(tm.tourney, f->key, kind, stage, arcade, cat);
    if (arcade && *name && *cat && !strcmp(*cat, "t3-console")) {
        const char *a = t3_arcade_piece(*name, cat);
        if (a) *name = a;
        else tmst_log("tournament: no Arcade piece for %s, the Console's plays", *name);
    }
    return *name != NULL;
}
int tmst_tournament_fight_stage(unsigned mode, int *stage) {
    *stage = -1;
    if (!in_match() || mode != TEAM_BATTLE) return TMST_TSTAGE_NONE;
    const Fighter *f = member_fighter(member_due());
    int school, kind = due_stage(f, stage, &school);
    static const char *const kinds[] = {"none", "Tekken 2", "Tekken 3", "Tekken Tag"};
    tmst_log("tournament: %s's stage in %s: %s %d%s", f->name, tmst_tourney(tm.tourney)->name, kinds[kind], *stage,
             school ? " (school)" : "");
    return kind;
}
/* Member i on: in the fighters' IDs and costumes, its moves (a native's,
 * the CPU MOVESET row's). */
static void member_on(int i) {
    const Fighter *f = member_fighter(i);
    psx_mod_write_half(FIGHTERS + 2, (uint16_t)f->id);
    psx_mod_write_half(COSTUMES + 2, (uint16_t)f->costume);
    if (f->id < STOCK && tekken3_native_moves_assign) tekken3_native_moves_assign(1, f->id, 0);
    mt.member = i;
}
/* The CPU's member handed to the fighters (0x8004F0E4 from 0x800B0A40, a0 the
 * player, the frame the grid is left): the opponent instead, in the team, the
 * fighters' IDs and costumes, before the copy. The team's size 0 until the
 * fight: the Expanded's team draw (in the loading screen's VBlanks) passes
 * an empty team by. */
static void place_opponent(void) {
    for (int i = 0; i < team_size(); i++) {
        const Fighter *f = member_fighter(i);
        psx_mod_write_byte(TEAMS + TEAM_STRIDE + (uint32_t)i, (uint8_t)(f->id * 4 + f->costume));
        tmst_log("tournament: the CPU's member %d is %s (ID %u, costume %u)", i + 1, f->name, f->id, f->costume);
    }
    member_on(0);
    psx_mod_write_byte(TEAMS + TEAM_STRIDE + T_SIZE, 0);
    mt.size_held = 1;
    if (!mt.ai_saved) { mt.ai_was = psx_mod_read_byte(AI_LEVEL); mt.ai_saved = 1; }
    psx_mod_write_byte(AI_LEVEL, round_level[tmst_bracket_round(mt.match)]);
}
extern void __tmst_chain_func_8004F0E4(CPUState *cpu);
#ifdef TMST_BRIDGE_func_8004F0E4
extern void __real_func_8004F0E4(CPUState *cpu);
void __tmst_chain_func_8004F0E4(CPUState *cpu) { __real_func_8004F0E4(cpu); }
#endif
void __wrap_func_8004F0E4(CPUState *cpu) {
    if (on && mt.flow == FLOW_MATCH && mt.opponent >= 0 && (cpu->pc == 0 || cpu->pc == 0x8004f0e4) &&
        cpu->gpr[4] == 1 && cpu->gpr[31] == CPU_MEMBER_RETURN && psx_mod_read_word(MODE) == TEAM_BATTLE)
        place_opponent();
    __tmst_chain_func_8004F0E4(cpu);
}

/* Unknown's regeneration, as the Expanded gives it to her as the Arcade's
 * final boss (src/tekken3_ttt1_roster.c, the Arcade only): here for the CPU's
 * Unknown in a tournament's fights. The damage routine (fighter_damage_apply,
 * 0x8004439C, once a played frame for each fighter, not in the pause): at
 * its entry her life (+0x3F4, 16.16) gains 195 / 6000 of a point a frame (1.95
 * points a second) up to a ceiling each hit lowers by 60 % of its damage (40 %
 * of what she took comes back), not while her move has guard bits (actor
 * +0x66 & 0x18). Whole points only: T3's K.O. is a life of exactly 0. */
enum { REGEN_RATE = (195 << 16) / 6000, REGEN_SHARE = 40, ACTOR_GUARD = 0x66 };
static struct { int track; int32_t last, ceiling, fraction; } regen;
static void regenerate(uint32_t actor) {
    int32_t life = (int32_t)psx_mod_read_word(actor + LIFE), max = (int32_t)psx_mod_read_word(actor + LIFE_MAX);
    if (!regen.track || life > regen.last) { regen.track = 1; regen.ceiling = life; regen.fraction = 0; }
    else if (life < regen.last) regen.ceiling -= (int32_t)((int64_t)(regen.last - life) * (100 - REGEN_SHARE) / 100);
    if (regen.ceiling < life) regen.ceiling = life;
    if (regen.ceiling > max) regen.ceiling = max;
    if (life > 0 && life + 0x10000 <= regen.ceiling && !(psx_mod_read_half(actor + ACTOR_GUARD) & 0x18)) {
        regen.fraction += REGEN_RATE;
        if (regen.fraction >= 0x10000) {
            regen.fraction -= 0x10000; life += 0x10000;
            psx_mod_write_word(actor + LIFE, (uint32_t)life);
        }
    }
    regen.last = life;
}
extern void __tmst_chain_func_8004439C(CPUState *cpu);
#ifdef TMST_BRIDGE_func_8004439C
extern void __real_func_8004439C(CPUState *cpu);
void __tmst_chain_func_8004439C(CPUState *cpu) { __real_func_8004439C(cpu); }
#endif
void __wrap_func_8004439C(CPUState *cpu) {
    if (on && mt.flow == FLOW_MATCH && (cpu->pc == 0 || cpu->pc == 0x8004439c) &&
        cpu->gpr[4] == RECORD + RECORD_STRIDE && psx_mod_read_word(MODE) == TEAM_BATTLE &&
        tekken3_guest_switches_on_button && tekken3_guest_switches_on_button(1))
        regenerate(cpu->gpr[4]);
    __tmst_chain_func_8004439C(cpu);
}

/* ---- the boss intro ------------------------------------------------------------ */
/* Before the boss's second form (the CPU's team of two, its second member's
 * round: Kazuya, then Devil), once that round is loaded (the fight's screen,
 * phase INTRO_PHASE: Team Battle's next member has no intro of its own, the
 * round starts 0.3 s later); or before a Grand Final boss alone that has an
 * entry (Heihachi, Unknown), once the fight is loaded (phase ALONE_PHASE,
 * after the VS screen, a few frames before the round's title), a short
 * cinematic (src/tournament/
 * tmst_tournament_cine.c) in the Expanded's cinematic engine
 * (src/tekken3_cine.c): its .cine written from the boss's own TTT1 combat
 * pack (the player's install, mods/ttt1) into addons/tmst/workspace/
 * boss-intro, played with no one to take over, so the fight goes on where
 * it was (the engine puts back the phase, the fighters' places, the camera).
 * START skips it: at the start of one of the engine's frames (0x80050450 at
 * the fight frame's own stack: the engine is not halfway through a call of
 * the game's), the engine let go (tekken3_cine_abandon) and what it would
 * have put back put back here (the same: saved as it starts). The boss's
 * music leads in with it (tmst_jukebox_lead_in, the user's choice).
 * The parts' own sounds play with them (intro_mix). Bench:
 * TMST_BOSS_INTRO_PHASE, another phase to start in; TMST_BOSS_INTRO_CINE,
 * another .cine played instead. */
extern int tekken3_cine_play(const char *path, int pre, void (*on_done)(void)) __attribute__((weak));
extern int tekken3_cine_engine_active(void) __attribute__((weak));
extern void tekken3_cine_abandon(void) __attribute__((weak));
extern const char *tekken3_ttt1_asset_root(void) __attribute__((weak));
extern const char *tekken3_guest_moves_prefix_for(unsigned player) __attribute__((weak));
enum { INTRO_PHASE = 4, ALONE_PHASE = 7, FRAME_PIECE = 0x80050450, ACTORS = 0x800a9228, ACTOR_SIZE = 0x188c, FILL = 0x8009e73c,
       DARK = 0x800afaa0, CAM_MODE = 0x800a0400, CAM_MODE_SIZE = 0x80, CAM_OBJECT = 0x800a8c40, CAM_OBJECT_SIZE = 0xc0 };
static struct {
    int played_match, playing, skip;
    int swap;                                 /* the intro's character change (its frame), 0 none: no skip before it */
    int native_swap;                          /* a native boss it changes into (its ID), its moves put back after; -1 */
    uint16_t previous;                        /* the player's last read (a START held from before is no press) */
    uint32_t frame_sp;                        /* the fight frame's stack at 0x80050450 (none playing) */
    unsigned phase; uint32_t actor[2][12], fill[4], mode[CAM_MODE_SIZE / 4], object[CAM_OBJECT_SIZE / 4]; uint8_t dark;
    int cine_used;                            /* an intro took the engine's prepared cinematic (embu_restore) */
} intro = {.played_match = -2, .native_swap = -1};
/* The engine keeps one prepared cinematic (src/tekken3_cine.c `cine`), and
 * the attract's Embu rotation prepares its TTT Embu ahead (the turn chosen
 * as the attract is left for the title, src/tekken3_cine_trigger.c
 * `planned`), then plays whatever is prepared once the attract comes. A
 * player who leaves the title before it, plays a tournament with an intro
 * and comes back left that turn pending: the attract played the intro as
 * its Embu (1.5.0 and 1.5.1 alike, seen 2026-11-14). On the title or the
 * main menu after an intro, the rotation's .cine (triggers.txt's "attract
 * rotation=") prepared again, as it was. */
extern int tekken3_cine_prepare(const char *path) __attribute__((weak));
static void embu_restore(void) {
    if (!intro.cine_used || !tekken3_cine_prepare || !tekken3_ttt1_asset_root ||
        (tekken3_cine_engine_active && tekken3_cine_engine_active()))
        return;
    intro.cine_used = 0;
    const char *root = tekken3_ttt1_asset_root();
    if (!root) return;
    char dir[4200], path[4400], line[1200], name[1024] = "";
    snprintf(dir, sizeof dir, "%s/../ttt-cinematics", root);
    snprintf(path, sizeof path, "%s/triggers.txt", dir);
    FILE *f = fopen(path, "r");
    if (!f) return;
    while (!name[0] && fgets(line, sizeof line, f)) {
        const char *c = strstr(line, "cine=");
        if (!strncmp(line, "attract", 7) && strstr(line, "rotation=") && c) sscanf(c + 5, "%1023s", name);
    }
    fclose(f);
    if (!name[0]) return;
    snprintf(path, sizeof path, "%s/%s", dir, name);
    tmst_log("tournament: the attract's Embu (%s) %s after the intro", name,
             tekken3_cine_prepare(path) ? "prepared again" : "could not be prepared");
}
/* A match armed: its intro not played yet. The Grand Final is match 16 of
 * every tournament, so a mark kept from an earlier one left the next Grand
 * Finals of the same run without their intro (1.5.1 test: Heihachi's, then
 * neither Devil's nor True Ogre's). */
static void intro_new_match(void) { intro.played_match = -2; }
static int intro_phase(int alone) {
    const char *e = getenv("TMST_BOSS_INTRO_PHASE");
    return e && *e ? atoi(e) : alone ? ALONE_PHASE : INTRO_PHASE;
}
/* The packs an intro's parts read (tmst_tournament_cine.h's TmstPackOf):
 * the boss's own (NULL: its moves' prefix, P2's: a guest's; a native's is
 * its entry's `own`, a stem) or another by its stem
 * ("Kazuya-TTT1"), mods/ttt1/<stem>-combat.jmv, read whole once while the
 * .cine is written. */
enum { PACKS_MAX = 4 };
static struct { char stem[64]; uint8_t *data; size_t size; } packs[PACKS_MAX];
static const uint8_t *intro_pack(const char *stem, size_t *size, void *ctx) {
    (void)ctx;
    const char *root = tekken3_ttt1_asset_root ? tekken3_ttt1_asset_root() : NULL;
    if (!stem) stem = tekken3_guest_moves_prefix_for ? tekken3_guest_moves_prefix_for(1) : NULL;
    if (!root || !stem || !*stem || strlen(stem) >= sizeof packs[0].stem) return NULL;
    int i = 0;
    for (; i < PACKS_MAX && packs[i].stem[0]; i++)
        if (!strcmp(packs[i].stem, stem)) { *size = packs[i].size; return packs[i].data; }
    if (i == PACKS_MAX) return NULL;
    snprintf(packs[i].stem, sizeof packs[i].stem, "%s", stem);   /* tried, found or not */
    char path[4200];
    FILE *f = snprintf(path, sizeof path, "%s/%s-combat.jmv", root, stem) < (int)sizeof path ? fopen(path, "rb") : NULL;
    if (!f) { tmst_log("tournament: no %s for the boss intro", path); return NULL; }
    long n = fseek(f, 0, SEEK_END) == 0 ? ftell(f) : -1;
    uint8_t *data = NULL;
    if (n > 32 && n < 64L * 1024 * 1024 && fseek(f, 0, SEEK_SET) == 0 && (data = malloc((size_t)n)) &&
        fread(data, 1, (size_t)n, f) != (size_t)n) { free(data); data = NULL; }
    fclose(f);
    packs[i].data = data; packs[i].size = data ? (size_t)n : 0;
    *size = packs[i].size;
    return data;
}
static void intro_packs_free(void) {
    for (int i = 0; i < PACKS_MAX; i++) { free(packs[i].data); packs[i].data = NULL; packs[i].size = 0; packs[i].stem[0] = 0; }
}
static void intro_save(void) {
    intro.phase = psx_mod_read_half(PHASE);
    for (unsigned p = 0; p < 2; p++)
        for (unsigned k = 0; k < 12; k++) intro.actor[p][k] = psx_mod_read_word(ACTORS + p * ACTOR_SIZE + k * 4);
    for (unsigned i = 0; i < 4; i++) intro.fill[i] = psx_mod_read_word(FILL + i * 16);
    intro.dark = psx_mod_read_byte(DARK);
    for (unsigned k = 0; k < CAM_MODE_SIZE / 4; k++) intro.mode[k] = psx_mod_read_word(CAM_MODE + k * 4);
    for (unsigned k = 0; k < CAM_OBJECT_SIZE / 4; k++) intro.object[k] = psx_mod_read_word(CAM_OBJECT + k * 4);
}
static void intro_restore(void) {
    psx_mod_write_half(PHASE, (uint16_t)intro.phase);
    for (unsigned p = 0; p < 2; p++)
        for (unsigned k = 0; k < 12; k++) psx_mod_write_word(ACTORS + p * ACTOR_SIZE + k * 4, intro.actor[p][k]);
    for (unsigned i = 0; i < 4; i++) psx_mod_write_word(FILL + i * 16, intro.fill[i]);
    psx_mod_write_byte(DARK, intro.dark);
    for (unsigned k = 0; k < CAM_MODE_SIZE / 4; k++) psx_mod_write_word(CAM_MODE + k * 4, intro.mode[k]);
    for (unsigned k = 0; k < CAM_OBJECT_SIZE / 4; k++) psx_mod_write_word(CAM_OBJECT + k * 4, intro.object[k]);
}
/* The intro's sounds (tmst_boss_intro_sounds: the parts' records' own): each
 * from its fighter's TTT1 sound pack (mods/ttt1/<stem>-sfx.jus, the
 * Expanded's tools/ttt1/sfx.py: "JUS1", 1, size, count, 44 100, then per
 * sound index, offset, frames, driver ID; mono 16-bit), read as the intro
 * starts, played by our mixer (src/core/tmst_spu.h) from its frame, at the
 * player's sound-effects volume, at the level the Expanded gives the TTT1
 * sounds (its 200 %, against the SPU's main volume about half). The
 * engine plays none of them: its records have no sounds. */
enum { CUES_MAX = 8, JUS_MAGIC = 0x3153554a };
static struct { int frame; int16_t *pcm; uint32_t frames; volatile int32_t pos; } cues[CUES_MAX];
static volatile int cue_count;
static uint32_t le32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }
/* Sound `index` of <stem>-sfx.jus (malloc'ed), NULL without it. */
static int16_t *jus_sound(const char *stem, unsigned index, uint32_t *frames) {
    const char *root = tekken3_ttt1_asset_root ? tekken3_ttt1_asset_root() : NULL;
    char path[4200];
    if (!root || !stem || snprintf(path, sizeof path, "%s/%s-sfx.jus", root, stem) >= (int)sizeof path) return NULL;
    FILE *f = fopen(path, "rb");
    uint8_t head[32];
    int16_t *pcm = NULL;
    if (!f) return NULL;
    if (fread(head, 1, sizeof head, f) == sizeof head && le32(head) == JUS_MAGIC && le32(head + 4) == 1 &&
        le32(head + 16) == 44100) {
        uint32_t count = le32(head + 12), size = le32(head + 8);
        for (uint32_t i = 0; i < count && i < 0x800; i++) {
            uint8_t e[16];
            if (fseek(f, 32 + (long)i * 16, SEEK_SET) || fread(e, 1, 16, f) != 16) break;
            if (le32(e) != index) continue;
            uint32_t offset = le32(e + 4), n = le32(e + 8);
            if (!n || offset > size || n > (size - offset) / 2 || fseek(f, (long)offset, SEEK_SET)) break;
            uint8_t *raw = malloc((size_t)n * 2);
            if (raw && fread(raw, 2, n, f) == n) {
                pcm = (int16_t *)raw;
                for (uint32_t k = 0; k < n; k++) pcm[k] = (int16_t)(raw[k * 2] | raw[k * 2 + 1] << 8);
                *frames = n;
            } else free(raw);
            break;
        }
    }
    fclose(f);
    return pcm;
}
static void intro_sounds_free(void) {
    cue_count = 0;
    for (int i = 0; i < CUES_MAX; i++) { free(cues[i].pcm); cues[i].pcm = NULL; cues[i].frames = 0; cues[i].pos = -1; }
}
static void intro_sounds_load(const TmstBossIntro *e) {
    TmstCineSound s[CUES_MAX];
    intro_sounds_free();
    int n = tmst_boss_intro_sounds(e, intro_pack, NULL, s, CUES_MAX), k = 0;
    const char *own = e->own ? e->own : tekken3_guest_moves_prefix_for ? tekken3_guest_moves_prefix_for(1) : NULL;
    for (int i = 0; i < n; i++) {
        uint32_t frames = 0;
        int16_t *pcm = jus_sound(s[i].stem ? s[i].stem : own, s[i].index, &frames);
        if (!pcm) { tmst_log("tournament: the intro's sound %u of %s not there", s[i].index, s[i].stem ? s[i].stem : own); continue; }
        cues[k].frame = s[i].frame; cues[k].pcm = pcm; cues[k].frames = frames; cues[k].pos = -1;
        tmst_log("tournament: the intro's sound %u of %s at its frame %d", s[i].index, s[i].stem ? s[i].stem : own, s[i].frame);
        k++;
    }
    cue_count = k;
}
extern int tekken3_cine_engine_frame(void) __attribute__((weak));
static void intro_mix(int16_t *out, int frames, int audible, int music_gain) {
    (void)music_gain;
    int f = intro.playing && tekken3_cine_engine_frame ? tekken3_cine_engine_frame() : -1, gain = tmst_spu_sfx_gain();
    for (int c = 0; c < cue_count; c++) {
        if (!cues[c].pcm) continue;
        if (cues[c].pos < 0) {
            if (f < cues[c].frame || f > cues[c].frame + 2) continue;   /* not yet, or skipped past */
            cues[c].pos = 0;
            tmst_log("tournament: the intro's sound at its frame %d", f);
        }
        if (!intro.playing) continue;                                    /* left: silent from here */
        for (int i = 0; i < frames && (uint32_t)cues[c].pos < cues[c].frames; i++, cues[c].pos++) {
            int32_t v = audible ? cues[c].pcm[cues[c].pos] * gain / 100 : 0;
            for (int k = 0; k < 2; k++) {
                int32_t m = out[i * 2 + k] + v;
                out[i * 2 + k] = (int16_t)(m > 32767 ? 32767 : m < -32768 ? -32768 : m);
            }
        }
    }
}
/* A native boss the intro changed into (the engine's `swap`: True Ogre)
 * comes back on its TTT1 moves (the engine's load_begin forces them,
 * tekken3_native_moves_force, so the poses play) and would fight on them:
 * the moves the tournament picked for it (member_on, the CPU MOVESET row)
 * again, as the game's own reload chooses them (tekken3_native_moves_reload;
 * an ID no native's in between, or it takes the forced choice for made).
 * Mid-fight the Expanded follows the change itself (its TTT1 combat's
 * follow_identity), before the round starts. */
extern void tekken3_native_moves_reload(unsigned p, unsigned id) __attribute__((weak));
enum { NO_NATIVE_ID = 0xffff };
static void intro_moves_back(void) {
    if (intro.native_swap < 0 || !tekken3_native_moves_reload) return;
    tekken3_native_moves_reload(1, NO_NATIVE_ID);
    tekken3_native_moves_reload(1, (unsigned)intro.native_swap);
    tmst_log("tournament: the boss (ID %d) on the moves picked for it again", intro.native_swap);
    intro.native_swap = -1;
}
static void boss_intro_tick(unsigned screen, int due) {
    if (intro.playing && !(tekken3_cine_engine_active && tekken3_cine_engine_active())) {
        intro.playing = 0; intro.skip = 0;
        tmst_log("tournament: the boss intro over");
        intro_moves_back();
    }
    /* The Grand Final's last member: the second form, or the boss alone
     * (only with an entry of its own: the Free Tournament's drawn one not). */
    int alone = team_size() == 1;
    if (!tekken3_cine_play || !tekken3_cine_abandon || mt.match != TMST_GRAND_FINAL || due != team_size() - 1 ||
        intro.played_match == mt.match || screen != FIGHT || psx_mod_read_half(PHASE) != (unsigned)intro_phase(alone))
        return;
    const Fighter *boss = member_fighter(due);
    const TmstBossIntro *e = tmst_boss_intro(boss->key);
    if (alone && !e->key) return;
    intro.played_match = mt.match;
    size_t size = 0;
    /* A native's pack is its entry's (its moves in the fight are not TTT1's). */
    if ((boss->id < STOCK && !e->own) || !intro_pack(e->own, &size, NULL)) {
        tmst_log("tournament: no TTT1 moves for %s, no intro", boss->name);
        intro_packs_free();
        return;
    }
    char dir[4200], path[4300];
    snprintf(dir, sizeof dir, "%s/addons/tmst/workspace/boss-intro", tmst_files_root());
    snprintf(path, sizeof path, "%s/%s.cine", dir, boss->key);
    FILE *f = tmst_files_make_dirs(dir) ? fopen(path, "w") : NULL;
    int ok = f && tmst_boss_intro_write(f, e, intro_pack, NULL, psx_mod_read_half(FIGHTERS), psx_mod_read_half(COSTUMES),
                                        boss->id, boss->costume);
    if (f && fclose(f)) ok = 0;
    if (ok) intro_sounds_load(e);
    intro_packs_free();
    if (!ok) { tmst_log("tournament: %s not written, no intro", path); return; }
    const char *bench = getenv("TMST_BOSS_INTRO_CINE");      /* bench: another .cine played instead */
    if (bench && *bench) snprintf(path, sizeof path, "%s", bench);
    intro_save();
    if (!tekken3_cine_play(path, 0, NULL)) { tmst_log("tournament: %s not played", path); return; }
    intro.cine_used = 1;
    intro.playing = 1; intro.skip = 0; intro.previous = 0; intro.swap = e->start ? e->swap : 0;
    intro.native_swap = e->start && boss->id < STOCK ? (int)boss->id : -1;
    tmst_log("tournament: %s's intro (%s) at phase %d, %d frames", boss->name, e->key ? e->key : "the fallback",
             intro_phase(alone), e->frames);
    /* The boss's piece from the intro's first frame on (the Jukebox; the
     * game starts it only with the round). */
    /* BGM SELECT's SILENT: no music, the intro's either (issue #9). */
    if (tmst_music_version() == TMST_MUSIC_SILENT) tmst_log("tournament: the intro without music (BGM SELECT: SILENT)");
    else if (!tmst_jukebox_lead_in()) tmst_log("tournament: the intro without music (no Jukebox piece)");
}
/* The fight frame's piece after its pause check (src/tekken3_cine.c runs its
 * frames there): a skip at a frame's start. */
extern void __tmst_chain_func_80050450(CPUState *cpu);
#ifdef TMST_BRIDGE_func_80050450
extern void __real_func_80050450(CPUState *cpu);
void __tmst_chain_func_80050450(CPUState *cpu) { __real_func_80050450(cpu); }
#endif
void __wrap_func_80050450(CPUState *cpu) {
    if (cpu->pc == 0 || cpu->pc == FRAME_PIECE) {
        if (!intro.playing) intro.frame_sp = cpu->gpr[29];
        /* Not before the character change: the boss would stay the other
         * character's model (the START is kept till just after it). */
        else if (intro.skip && cpu->gpr[29] == intro.frame_sp && !cpu->gpr[2] &&
                 (!intro.swap || !tekken3_cine_engine_frame || tekken3_cine_engine_frame() > intro.swap + 1)) {
            tekken3_cine_abandon();
            intro_restore();
            intro.playing = 0; intro.skip = 0;
            tmst_log("tournament: the boss intro left, the fight goes on");
            intro_moves_back();
        }
    }
    __tmst_chain_func_80050450(cpu);
}

/* A match under way: the opponent kept through the loading screen, the
 * result at the results. */
static void match_tick(unsigned screen) {
    /* The CPU's Tekken 3 arena (TTT stage's arena), every VBlank until the
     * fight: the game's Team Battle loading writes its chooser's again. */
    int arena = tmst_stage_forced_arena();
    if (arena >= 0 && psx_mod_read_half(ARENA) != (uint16_t)arena) {
        psx_mod_write_half(ARENA, (uint16_t)arena);
        psx_mod_write_byte(ARENA_COPY, (uint8_t)arena);
    }
    int due = member_due();
    const Fighter *f = member_fighter(due);
    boss_intro_tick(screen, due);
    if (screen == LOADING && (psx_mod_read_half(FIGHTERS + 2) != f->id || mt.member != due)) {
        if (mt.member == due) tmst_log("tournament: the CPU's member was changed (ID %u), %s again", psx_mod_read_half(FIGHTERS + 2), f->name);
        else tmst_log("tournament: the CPU's next member, %s", f->name);
        psx_mod_write_byte(TEAMS + TEAM_STRIDE + (uint32_t)due, (uint8_t)(f->id * 4 + f->costume));
        member_on(due);
    }
    /* The fight over (the winner's pose played to its end), its screen back
     * to phase 0 for a frame (or screen 18), the game loads the results
     * (screen 1): covered from there. Screen 18 has no display list: the
     * fight's last frame stands still (redrawn by the GL renderer as the
     * load changes VRAM) and nothing of ours is drawn on it. */
    if (mt.life_set && !mt.cover && (screen == BOOT_LOADING || screen == AFTER_FIGHT ||
                                     (screen == FIGHT && psx_mod_read_half(PHASE) == 0))) {
        mt.cover = 1; mt.fight_over = 1; mt.scr = SCR_NONE;
        tmst_log("tournament: the fight over (screen %u), covered", screen);
    }
    if (screen != LOADING && mt.size_held) { psx_mod_write_byte(TEAMS + TEAM_STRIDE + T_SIZE, (uint8_t)team_size()); mt.size_held = 0; }
    if (screen != FIGHT || psx_mod_read_half(PHASE) < 6) regen.track = 0;      /* between rounds: a new ceiling */
    /* The game sets its own level as the fight loads, and reads it again in
     * the fight: held. */
    /* The rounds (match_rounds), where the Arcade copies GAME OPTION's FIGHT
     * COUNT for the fight (0x800AFAA8; Team Battle's own is 0). */
    unsigned rounds = match_rounds();
    if ((screen == LOADING || screen == FIGHT) && psx_mod_read_byte(ROUNDS) != rounds) psx_mod_write_byte(ROUNDS, (uint8_t)rounds);
    unsigned level = round_level[tmst_bracket_round(mt.match)];
    if ((screen == LOADING || screen == FIGHT) && mt.ai_saved && psx_mod_read_byte(AI_LEVEL) != level)
        psx_mod_write_byte(AI_LEVEL, (uint8_t)level);
    if (screen == FIGHT) {
        uint32_t max = psx_mod_read_word(RECORD + LIFE_MAX), max2 = psx_mod_read_word(RECORD + RECORD_STRIDE + LIFE_MAX);
        unsigned won = psx_mod_read_byte(RECORD + ROUNDS_WON) + psx_mod_read_byte(RECORD + RECORD_STRIDE + ROUNDS_WON);
        if (!mt.life_set && max && max2 && psx_mod_read_word(RECORD + LIFE) == max) { mt.life_set = 1; mt.rounds_won = won; }
        /* A round won (counted at the KO): Team Battle's winner goes on with
         * the life left (and a little back), the fallen one full. Each round
         * full for both, as in the other modes: once both stand again (the
         * next round; a second after the time ran out). */
        /* Not for the boss's second form: the player goes on to it with the
         * life the first left (the user's choice). */
        if (mt.life_set && won != mt.rounds_won) { mt.rounds_won = won; if (team_size() == 1) mt.round_end = tm.frames | 1; }
        if (mt.round_end && tm.frames - mt.round_end >= ROUND_SETTLE && psx_mod_read_word(RECORD + LIFE) &&
            psx_mod_read_word(RECORD + RECORD_STRIDE + LIFE)) {
            psx_mod_write_word(RECORD + LIFE, max); psx_mod_write_word(RECORD + RECORD_STRIDE + LIFE, max2);
            mt.round_end = 0;
            tmst_log("tournament: a new round, both lives full");
        }
    }
    if (screen == RESULTS) {
        int p1_lost = psx_mod_read_byte(TEAMS + T_LOST) != 0, p2_lost = psx_mod_read_byte(TEAMS + TEAM_STRIDE + T_LOST) != 0;
        int won = p2_lost && !p1_lost;
        uint32_t decided = tmst_bracket_player_result(&bracket, won);
        /* Lost to the boss's second form (the first one down): it is the winner. */
        if (!won && mt.second >= 0 && p2_lost) bracket.winner[mt.match] = mt.second;
        mt.fresh = decided; mt.shown |= decided;
        for (int m = 0; m < TMST_BRACKET_MATCHES; m++)
            if (decided >> m & 1) {
                int a, b; tmst_bracket_fighters(&bracket, m, &a, &b);
                tmst_log("tournament:   match %d: %s beats %s", m + 1, fighter_name(bracket.winner[m]),
                         fighter_name(bracket.winner[m] == a ? b : a));
            }
        mt.outcome = !won ? ELIMINATED : tmst_bracket_champion(&bracket) >= 0 ? CHAMPION : NEXT_MATCH;
        if (!won) {
            mt.shown |= tmst_bracket_finish(&bracket);
            tmst_log("tournament: %s is out, %s wins the tournament", fighter_name(bracket.player),
                     fighter_name(tmst_bracket_champion(&bracket)));
        } else if (mt.outcome == CHAMPION) {
            tmst_log("tournament: %s wins %s", fighter_name(bracket.player), tmst_tourney(tm.tourney)->name);
            mt.unlocked = tmst_progress_win(&progress, tm.tourney);
            if (!tmst_progress_save(&progress, progress_file)) tmst_log("tournament: %s not written", progress_file);
            else tmst_log("tournament: progress written (won 0x%x)", progress.won);
        } else {
            mt.match = tmst_bracket_player_match(&bracket); mt.opponent = tmst_bracket_opponent(&bracket);
        }
        /* The cover from this very frame: the results are never seen. */
        mt.flow = FLOW_BETWEEN; mt.confirmed = 0; mt.cover = 1; mt.previous = 0; mt.scr = SCR_RESULT; mt.scr_at = tm.frames;
        if (menu_row >= 0) tmst_menu_select_next_time(menu_row);
        return;
    }
    /* Left without results (the pause's RESET): the tournament is dropped. */
    if (screen == TITLE || screen == MENU) end_tournament("left the match");
}

/* Between the matches: the cover from the results on (they are not shown:
 * CROSS on them, and the game goes straight back to the grid), the game
 * driven to the grid (else START in the attract loop: NAMCO PRESENTS, the
 * records, the demo; the menu's row), where the player's START is waited
 * for. The tournament over, the main menu is where it ends. */
static void between_tick(unsigned screen) {
    if (!mt.cover && !mt.stalled) {
        mt.cover = 1; mt.previous = 0;                   /* a button held from before is no press */
        mt.scr = SCR_RESULT; mt.scr_at = tm.frames;
    }
    mt.hold = 0;
    /* The results are not shown: START on them (from their phase 2, read at
     * 0x800EEEF8) continues, straight back to the grid (12, 8, 10). */
    if (screen == RESULTS && psx_mod_read_half(PHASE) >= RESULTS_READS) mt.hold = tm.frames / 4 & 1 ? 0 : B_START;
    if (psx_mod_read_word(MODE) == ATTRACT && screen != MENU && screen != BOOT_LOADING)
        mt.hold = tm.frames / 4 & 1 ? 0 : B_START;
    if (screen == MENU && mt.outcome != NEXT_MATCH) {
        if (mt.confirmed) end_tournament("back to the main menu");
        return;
    }
    if (screen == MENU && tmst_menu_highlighted(menu_row)) mt.hold = tm.frames / 4 & 1 ? 0 : B_CROSS;
}

/* What happens under the cover, logged as it changes; nothing changing for
 * ten seconds (but the wait for START) lifts the cover and lets the player's
 * buttons through with the mod's, to see where the way back stopped. */
static void between_watch(unsigned screen) {
    unsigned state = team_grid() ? psx_mod_read_word(block(0)) & 0xff : 0xff, phase = psx_mod_read_half(PHASE);
    unsigned key = screen | phase << 8 | state << 16 | (unsigned)(tm.view * 8 + mt.step) << 24;
    if (key != mt.watch_key) {
        mt.watch_key = key; mt.watch_since = tm.frames;
        tmst_log("tournament: between: screen %u phase %u mode %u grid %u view %d step %d", screen, phase,
                 psx_mod_read_word(MODE), state, tm.view, mt.step);
        return;
    }
    if (!mt.confirmed) mt.watch_since = tm.frames;      /* the player looking at the screens: no stop */
    if (mt.cover && screen != RESULTS && tm.frames - mt.watch_since > 600) {
        mt.cover = 0; mt.stalled = 1;
        unsigned id = mt.pick >> 2;
        unsigned cell = 0;
        while (cell < 21 && grid_character(cell) != id) cell++;
        tmst_log("tournament: the way back stopped (screen %u phase %u mode %u grid %u count %u view %d step %d "
                 "page %d cursor %u,%u pick 0x%x cell %u, menu row %d highlighted %d), the cover lifted", screen, phase,
                 psx_mod_read_word(MODE), state, psx_mod_read_word(block(0) + B_COUNT), tm.view, mt.step,
                 tekken3_ttt1_selector_page ? tekken3_ttt1_selector_page() : -1, psx_mod_read_word(block(0) + B_X),
                 psx_mod_read_word(block(0) + B_Y), mt.pick, cell, menu_row, tmst_menu_highlighted(menu_row));
    }
}

/* The MOVESET card's choices for a native (L1, 2026-11-12): Tekken 3's
 * moves, then each line of the Expanded's natives.txt for it, read as
 * src/tekken3_native_moves.c reads them (it has no call that says them): the
 * optional fourth column the source ("ttt1" without), "dr" only with the
 * Tekken 5 DR Content mod on, a line only with its <Name>-<SOURCE>-combat.jmv,
 * at most 4. Its card turns through them in that order, round. A source
 * we do not know shows as its own name. */
extern int tekken3_dr_content_enabled(void) __attribute__((weak));
enum { MOVES_MAX = 5, MOVES_NAME = 24 };
static struct { int read; unsigned char n[STOCK]; char name[STOCK][MOVES_MAX][MOVES_NAME]; } moves;
static void moves_read(void) {
    if (moves.read) return;
    moves.read = 1;
    for (unsigned id = 0; id < STOCK; id++) { moves.n[id] = 1; snprintf(moves.name[id][0], MOVES_NAME, "TEKKEN 3"); }
    const char *root = tekken3_ttt1_asset_root ? tekken3_ttt1_asset_root() : NULL;
    char path[1024], line[96];
    if (!root || snprintf(path, sizeof path, "%s/natives.txt", root) >= (int)sizeof path) return;
    FILE *f = fopen(path, "rb");
    if (!f) return;
    while (fgets(line, sizeof line, f)) {
        unsigned id, moveset; char key[32], source[24] = "ttt1";
        if (sscanf(line, "%u %31s %u %23s", &id, key, &moveset, source) < 3 || id >= STOCK || moves.n[id] >= MOVES_MAX) continue;
        if (!strcmp(source, "dr") && !(tekken3_dr_content_enabled && tekken3_dr_content_enabled())) continue;
        char tag[24], combat[1024];
        for (int i = 0; i < 24; i++) tag[i] = (char)toupper((unsigned char)source[i]);
        key[0] = (char)toupper((unsigned char)key[0]);
        if (snprintf(combat, sizeof combat, "%s/%s-%s-combat.jmv", root, key, tag) >= (int)sizeof combat) continue;
        FILE *c = fopen(combat, "rb");
        if (!c) continue;
        fclose(c);
        char *name = moves.name[id][moves.n[id]++];
        snprintf(name, MOVES_NAME, "%s", !strcmp(source, "ttt1") ? "TEKKEN TAG TOURNAMENT" : !strcmp(source, "dr") ? "TEKKEN 5 DR" : tag);
        tmst_log("tournament: %s's moveset %u: %s", key, (unsigned)moves.n[id] - 1, name);
    }
    fclose(f);
}
static int moves_count(unsigned id) { moves_read(); return id < STOCK ? moves.n[id] : 1; }
static const char *moves_name(unsigned id, int k) {
    return k > 0 && k < moves_count(id) ? moves.name[id][k] : "TEKKEN 3";
}

/* The player's fighter picked again on the grid: its page, the cursor on its
 * cell, its costume's button, the MOVESET card confirmed where it shows. */
static void pick_tick(void) {
    unsigned id = mt.pick >> 2, costume = mt.pick & 3, t = tm.frames - mt.step_at;
    int page = tekken3_ttt1_selector_page ? tekken3_ttt1_selector_page() : -1;
    mt.hold = 0;
    if (!mt.confirmed || psx_mod_read_word(block(0)) != CHOOSING || psx_mod_read_word(block(0) + B_COUNT)) return;
    if (mt.cover && (mt.scr == SCR_ZOOM || (mt.scr == SCR_MATCH && tm.frames - mt.scr_at < MATCH_HOLD))) return;
    switch (mt.step) {
    case STEP_SETTLE:
        if (t >= 20) { mt.step = STEP_PAGE; mt.step_at = tm.frames; }
        break;
    case STEP_PAGE:
        if (page >= 0 && page != (id >= GUEST_FIRST)) {
            if (t < 4) mt.hold = B_R2;
            else if (t >= 24) mt.step_at = tm.frames;
        } else { mt.step = STEP_CURSOR; mt.step_at = tm.frames; }
        break;
    case STEP_CURSOR: {
        unsigned cell = 0;
        while (cell < 21 && grid_character(cell) != id) cell++;
        if (cell == 21 || ++mt.tries > 3) {
            tmst_log("tournament: %s not picked on the grid", fighter_name(bracket.player));
            tm.view = VIEW_LIST; tm.version++; tm.armed = 0; end_tournament("the fighter not picked");
            break;
        }
        psx_mod_write_word(block(0) + B_X, cell % 7); psx_mod_write_word(block(0) + B_Y, cell / 7);
        mt.step = STEP_COSTUME; mt.step_at = tm.frames;
        break;
    }
    case STEP_COSTUME:
        /* The player's own button (issue #14): the Expanded's model follows
         * the key, not the costume alone (square / triangle both costume 0,
         * triangle the RP model; R1 the third costume, triangle to the game). */
        if (t >= 6 && t < 10) mt.hold = mt.key ? mt.key : costume_button[costume];
        if (t >= 30) { mt.step = STEP_CARD; mt.step_at = tm.frames; }
        break;
    case STEP_CARD:                           /* the pick not taken yet: the card is up, on the player's moves */
        if (id < STOCK && tekken3_native_moves_available && tekken3_native_moves_available(id)) {
            /* RIGHT as often as the card is from the choice (4 frames down,
             * 4 up each), then CROSS. The card opens where the Expanded's
             * stands, within this native's choices. */
            int n = moves_count(id), now = mt.want_now < n ? mt.want_now : n - 1;
            int turns = ((mt.want - now) % n + n) % n;            if ((int)t < turns * 8 && t % 8 < 4) mt.hold = B_RIGHT;
            if ((int)t >= turns * 8 + 6 && (int)t < turns * 8 + 10) { mt.hold = B_CROSS; mt.want_now = mt.want; }
        }
        if (t >= 60) { mt.step = STEP_CURSOR; mt.step_at = tm.frames; }
        break;
    }
}

static int card_open;
static unsigned card_id;                      /* the native whose card is open */
static uint16_t grid_key;                     /* the last pick's button on the grid (issue #14), 0 none */
static uint16_t screen_input(uint16_t buttons) {
    uint16_t pressed = (uint16_t)~buttons, edges = pressed & tm.previous;    /* previous: last read's released bits */
    tm.previous = buttons;
    const uint16_t none = 0xffff;
    if (tm.exiting) return (uint16_t)~(B_START | B_SELECT);                 /* the game's own way out */
    /* The team size prompt, answered for the player: 1 against 1, pressed
     * for 4 frames, released for 4 (the pad may be read more than once a frame). */
    if (psx_mod_read_word(block(0)) == SIZE_PROMPT) {
        /* The cover waits for START, and for its release: Team Battle reads
         * START from the pad itself (0x80053968, not through the filters)
         * and on the grid it draws a random team. */
        if (mt.flow == FLOW_BETWEEN && (!mt.confirmed || !(mt.previous & B_START))) return none;
        psx_mod_write_word(block(0) + B_SIZE, 1); psx_mod_write_word(block(1) + B_SIZE, 1);
        return tm.frames / 4 & 1 ? none : (uint16_t)~B_CROSS;
    }
    if (mt.flow == FLOW_BETWEEN) return mt.stalled ? (uint16_t)(buttons & ~mt.hold) : (uint16_t)~mt.hold;   /* the player's fighter again */
    /* A view just changed: nothing of the button that changed it goes on. */
    if (!tm.armed) {
        if (pressed & (B_FACE | B_SELECT | B_START)) return none;
        tm.armed = 1;
    }
    if (tm.view == VIEW_LIST) {
        int moved = tm.sel;
        if (edges & B_UP) moved = (tm.sel + TMST_TOURNEYS - 1) % TMST_TOURNEYS;
        if (edges & B_DOWN) moved = (tm.sel + 1) % TMST_TOURNEYS;
        if (moved != tm.sel) { tm.sel = moved; tm.version++; }
        if ((edges & B_CROSS) && tmst_progress_open(&progress, tm.sel) && playable(tm.sel)) {
            tm.tourney = tm.sel; tm.view = VIEW_GRID; tm.version++; tm.armed = 0; grid_key = 0;
            if (unified() && tmst_unified_select_set_allowed) tmst_unified_select_set_allowed(unified_allowed);
            tmst_log("tournament: %s chosen", tmst_tourney(tm.tourney)->name);
        }
        if (edges & B_CIRCLE) exit_to_menu();
        restrict_grid(tm.sel);                /* the grid above shows the highlighted tournament's fighters */
        return none;
    }
    if (tm.view == VIEW_GRID) {
        if (!card_open && tekken3_outfits_captures_input && tekken3_outfits_captures_input(0)) {
            restrict_grid(tm.tourney);
            return buttons;
        }
        if (edges & B_SELECT && !card_open) {
            tm.view = VIEW_LIST; tm.version++; tm.armed = 0;
            if (unified() && tmst_unified_select_set_allowed) tmst_unified_select_set_allowed(NULL);
            return none;
        }
        restrict_grid(tm.tourney);
        uint16_t out = buttons | B_SELECT | B_START;
        unsigned under = cursor_fighter();
        /* R1, as the Expanded turns it (turn_keys): a native whose third
         * costume is unlocked (0x80097EF4) and that has a TTT1 RP model. */
        int r1 = under < STOCK && (psx_mod_read_word(THIRD_COSTUMES) >> under & 1) && tekken3_ttt1_native_rp_model &&
                 tekken3_ttt1_native_rp_model(under);
        uint16_t picks = (uint16_t)(edges & (B_FACE | (r1 ? B_R1 : 0)));
        if (pressed & B_SQUARE) picks &= (uint16_t)~B_TRIANGLE;     /* square with triangle: Devil Jin's */
        if (!unified() && (pressed & (B_FACE | B_R1)) && !allowed_id(tm.tourney, under)) out |= B_FACE | B_R1;   /* refused */
        /* The MOVESET card (natives with TTT1 moves): opened by the pick's
         * button, LEFT / RIGHT turn it, CROSS or START takes it, CIRCLE or
         * SELECT closes it, as the Expanded's (issue #14). The Expanded draws it over the team panel, under ours:
         * shown in ours (tournament_prims). */
        else if (card_open) {                 /* as the Expanded's card turns: round, kept when closed */
            int n = moves_count(card_id);
            out = buttons;                    /* START and SELECT go on to the Expanded's card */
            if (edges & B_RIGHT) { mt.card_now = (mt.card_now + 1) % n; tm.version++; }
            if (edges & B_LEFT) { mt.card_now = (mt.card_now + n - 1) % n; tm.version++; }
            if (edges & (B_CROSS | B_START)) { card_open = 0; tm.version++; mt.want = mt.want_now = mt.card_now; }
            else if (edges & (B_CIRCLE | B_SELECT)) { card_open = 0; tm.version++; mt.want_now = mt.card_now; grid_key = 0; }
        } else if (picks) {
            /* The pick's button, kept for the picks before each match
             * (pick_tick; issue #14): R1 first, else the lowest face bit, as
             * the Expanded's record_key. */
            grid_key = picks & B_R1 ? B_R1 : (uint16_t)(picks & -picks);
            if (under < STOCK && tekken3_native_moves_available && tekken3_native_moves_available(under)) {
                card_open = 1; card_id = under; tm.version++;
                mt.card_now = mt.want_now < moves_count(under) ? mt.want_now : moves_count(under) - 1;
            }
        }
        return out;
    }
    return buttons;                           /* picked: the MOVESET card and the game's own way on */
}

/* The player's buttons on the cover: START (or CROSS) skips an animation,
 * then goes on; CIRCLE asks to leave the tournament (CROSS yes, CIRCLE no).
 * Under the cover the game has only the buttons pressed for it. */
static void cover_input(uint16_t buttons) {
    uint16_t edges = (uint16_t)~buttons & mt.previous;
    mt.previous = buttons;
    if (!mt.cover) return;
    int go = (edges & (B_START | B_CROSS)) != 0;
    if (mt.scr == SCR_STORY) {               /* START skips it; at its end CROSS (or START) starts the draw */
        if ((edges & B_START) || (mt.story_done && (edges & B_CROSS))) {
            tmst_log("tournament: the story %s", mt.story_done ? "read" : "skipped");
            mt.scr = SCR_DRAW; mt.scr_at = tm.frames;
        }
        return;
    }
    if ((mt.scr == SCR_DRAW || mt.scr == SCR_RESULT) && go) mt.scr_at = 0;     /* to its end */
    else if (mt.scr == SCR_WAIT && go) { mt.confirmed = 1; mt.scr = SCR_ZOOM; mt.scr_at = tm.frames; }
    else if (mt.scr == SCR_WAIT && (edges & B_CIRCLE)) { mt.scr = SCR_ASK; tm.version++; }
    else if (mt.scr == SCR_ASK && (edges & B_CROSS)) {
        mt.outcome = QUIT; mt.confirmed = 1; mt.scr = SCR_END;
        tmst_log("tournament: left by the player");
    } else if (mt.scr == SCR_ASK && (edges & B_CIRCLE)) mt.scr = SCR_WAIT;
    else if (mt.scr == SCR_END && go) mt.confirmed = 1;
}
static uint16_t input(int player, uint16_t buttons) {
    if (!on) return buttons;
    /* The boss intro: START skips it; nothing reaches the game meanwhile
     * (its pause would hold the cinematic). */
    if (intro.playing) {
        if (player == 0) {
            uint16_t edges = (uint16_t)~buttons & intro.previous;
            intro.previous = buttons;
            if ((edges & B_START) && !intro.skip) { intro.skip = 1; tmst_log("tournament: the boss intro skipped"); }
        }
        return 0xffff;
    }
    if (mt.flow == FLOW_BETWEEN && player == 0) cover_input(buttons);
    if (mt.flow == FLOW_BETWEEN && (mt.cover || mt.stalled) && !tm.active)
        return player != 0 ? 0xffff : mt.stalled ? (uint16_t)(buttons & ~mt.hold) : (uint16_t)~mt.hold;
    if (tm.active && tm.exiting && !team_grid()) release_wait = 1;
    if (tm.active && release_wait) return buttons | B_START | B_SELECT | B_CROSS | B_CIRCLE;
    if (!tm.active) {
        if (!release_wait) return buttons;
        const uint16_t keys = B_START | B_SELECT | B_CROSS | B_CIRCLE;
        if (player == 0 && (buttons & keys) == keys) release_wait = 0;
        return buttons | keys;
    }
    return player == 0 ? screen_input(buttons) : 0xffff;
}

/* ---- the drawing ------------------------------------------------------------------ */
/* As the Jukebox: the screen's name in the selector's own title font on a red
 * band over the Team Battle title, a half see-through dark panel over the
 * teams under the grid (the game's primitives there moved out of sight), and
 * a picture of 1024 x 392 texels on the screen's rows 284..480 in the
 * UI font (Fira Sans). The title font: page (832,0) 4 bpp, palette (336,505), cells
 * of 14 x 24 from u 172, six to a row: 1..9, A..Z, !. */
enum { CENTRE_X = 184, FONT_PAGE = 832 / 64, FONT_CLUT = (505 << 6) | (336 / 16), FONT_U = 172,
       GLYPH_W = 14, GLYPH_H = 24, ADVANCE = 13, PANEL_TOP = 256, BANNER_TOP = 24, BANNER_H = 32,
       PANEL = 0x140c10, BANNER = 0x1018a8, HIDE_FROM_Y = 256, BACKDROP_W = 300,
       WHITE_CLUT = (505 << 6) | (352 / 16), FIGHT_X = 28, FIGHT_Y = 42 };
static int font_glyph(char c) {
    if (c >= '1' && c <= '9') return c - '1';
    if (c >= 'A' && c <= 'Z') return 9 + c - 'A';
    return c == '!' ? 35 : -1;
}
static uint32_t font_uv(int i, int du, int dv) {
    int u = FONT_U + i % 6 * GLYPH_W + du;
    return (uint32_t)(u > 255 ? 255 : u) | (uint32_t)(i / 6 * GLYPH_H + dv) << 8;
}
/* A line of the title font from x (align 0), centred on x (1) or ending at
 * x (2), scaled by num/den, tinted (0x808080 as it is). Characters it lacks
 * (0, punctuation) leave their room. */
static int font_width(const char *s, int num, int den) {
    int n = (int)strlen(s);
    return n ? (n * ADVANCE + (GLYPH_W - ADVANCE)) * num / den : 0;
}
static void font_line_in(TmstPrims *k, const char *s, int x, int y, int align, int num, int den, uint32_t tint,
                         uint32_t clut) {
    int advance = ADVANCE * num / den, w = GLYPH_W * num / den, h = GLYPH_H * num / den;
    x -= align == 1 ? font_width(s, num, den) / 2 : align == 2 ? font_width(s, num, den) : 0;
    for (; *s; s++, x += advance) {
        int i = font_glyph(*s);
        if (i < 0) continue;
        tmst_prim_room(k, 9);
        tmst_prim_word(k, 0x2c000000u | tint);
        tmst_prim_word(k, tmst_xy(x, y)); tmst_prim_word(k, font_uv(i, 0, 0) | clut << 16);
        tmst_prim_word(k, tmst_xy(x + w, y)); tmst_prim_word(k, font_uv(i, GLYPH_W, 0) | (uint32_t)FONT_PAGE << 16);
        tmst_prim_word(k, tmst_xy(x, y + h)); tmst_prim_word(k, font_uv(i, 0, GLYPH_H));
        tmst_prim_word(k, tmst_xy(x + w, y + h)); tmst_prim_word(k, font_uv(i, GLYPH_W, GLYPH_H));
    }
}
static void font_line(TmstPrims *k, const char *s, int x, int y, int align, int num, int den, uint32_t tint) {
    font_line_in(k, s, x, y, align, num, den, tint, FONT_CLUT);
}
static void font_text(TmstPrims *k, const char *s, int cx, int y, uint32_t tint) { font_line(k, s, cx, y, 1, 1, 1, tint); }
static void flat(TmstPrims *k, uint32_t colour, int x, int y, int w, int h) {
    tmst_prim_room(k, 3);
    tmst_prim_word(k, 0x60000000u | colour); tmst_prim_word(k, tmst_xy(x, y)); tmst_prim_word(k, tmst_xy(w, h));
}
static void shaded_band(TmstPrims *k, int x0, int x1, int y, int h, uint32_t a, uint32_t b) {
    tmst_prim_room(k, 8);
    tmst_prim_word(k, 0x38000000u | a); tmst_prim_word(k, tmst_xy(x0, y));
    tmst_prim_word(k, b);               tmst_prim_word(k, tmst_xy(x1, y));
    tmst_prim_word(k, a);               tmst_prim_word(k, tmst_xy(x0, y + h));
    tmst_prim_word(k, b);               tmst_prim_word(k, tmst_xy(x1, y + h));
}
static int showing(void) { return tm.active && (tm.view == VIEW_LIST || tm.view == VIEW_GRID || tm.exiting); }
static const char *boss_name(int t);
static void hud_prims(TmstPrims *k);
/* Under the cover's picture: black, and the match in the UI font (Fira
 * Sans; scale 2: capitals 24 tall, 1: 12), seen only where the picture is
 * not (a screen without a list to draw on, the picture given up to the
 * stage engine). y: the line's top, the capitals 2 (1) below it, where the
 * old font's were centred. */
enum { BIG = 2, SMALL = 1, GOLD = 0x30a0c0, WHITE = 0x808080, GREY = 0x505050 };
static void cover_line(TmstPrims *k, int y, const char *s, int scale, uint32_t tint) {
    tmst_ui_prim_text(k, CENTRE_X - tmst_ui_prim_width(s, scale) / 2, y + scale, s, scale, tint);
}
static void cover_prims(TmstPrims *k) {
    flat(k, 0x000000, -128, 0, 624, 480);
    if (mt.flow == FLOW_MATCH || mt.outcome != NEXT_MATCH || mt.match < 0) return;   /* the fight just over: black */
    cover_line(k, 100, round_names[tmst_bracket_round(mt.match)], BIG, GOLD);
    cover_line(k, 170, fighter_name(bracket.player), BIG, WHITE);
    cover_line(k, 215, "VS", SMALL, GREY);
    cover_line(k, 245, fighter_name(mt.opponent), BIG, WHITE);
}
static void tournament_prims(TmstPrims *k) {
    /* The loading screen: for its first frames the grid goes on drawing its
     * way out, black over it until a list holds the VS portraits (the
     * visitor walks a list before this builds our part of it). Then the
     * banner: the round, where the game writes FIGHT 1 (its letters gone,
     * fight_letters), in the font's white palette. */
    if (mt.flow == FLOW_MATCH && mt.match >= 0 && psx_mod_read_word(SCREEN) == LOADING) {
        if (!mt.loading_vs) { flat(k, 0x000000, -128, 0, 624, 480); return; }     /* still the grid's lists */
        font_line_in(k, round_names[tmst_bracket_round(mt.match)], FIGHT_X, FIGHT_Y, 0, 1, 1, 0x808080, WHITE_CLUT);
        return;
    }
    if (mt.cover) { cover_prims(k); return; }
    /* The fight's last list: built in the fight, handed to the GPU once the
     * game has left it (screen 18, then 1: no list more for a while, this
     * one standing still): black already. */
    unsigned now = psx_mod_read_word(SCREEN);
    if (mt.flow == FLOW_MATCH && mt.life_set && (now == AFTER_FIGHT || now == BOOT_LOADING)) {
        flat(k, 0x000000, -128, 0, 624, 480);
        return;
    }
    if (mt.flow == FLOW_MATCH && now == FIGHT) { hud_prims(k); return; }
    if (!showing()) return;                       /* another mod's picture */
    tmst_prim_room(k, 1);
    tmst_prim_word(k, 0xe1000600u | FONT_PAGE);
    tmst_prim_room(k, 3);
    tmst_prim_word(k, 0x62000000u | PANEL); tmst_prim_word(k, tmst_xy(-128, PANEL_TOP)); tmst_prim_word(k, tmst_xy(624, 480 - PANEL_TOP));
    int inner = 112;
    flat(k, BANNER, -128, BANNER_TOP, 128, BANNER_H);
    shaded_band(k, 0, CENTRE_X - inner, BANNER_TOP, BANNER_H, BANNER, 0x000000);
    flat(k, 0x000000, CENTRE_X - inner, BANNER_TOP, 2 * inner, BANNER_H);
    shaded_band(k, CENTRE_X + inner, 368, BANNER_TOP, BANNER_H, 0x000000, BANNER);
    flat(k, BANNER, 368, BANNER_TOP, 128, BANNER_H);
    font_text(k, "TOURNAMENT MODE", CENTRE_X, 28, 0x808080);
    char line[96];
    if (tm.view == VIEW_LIST || (tm.exiting && tm.view != VIEW_GRID)) {
        font_line(k, "CHOOSE A TOURNAMENT", CENTRE_X, 262, 1, 3, 5, 0x808080);
        for (int t = 0; t < TMST_TOURNEYS; t++) {
            int y = 288 + t * 32, open = tmst_progress_open(&progress, t), ok = playable(t), chosen = t == tm.sel;
            if (chosen) {                              /* the highlight, see-through red */
                tmst_prim_room(k, 3);
                tmst_prim_word(k, 0x62000000u | (open && ok ? 0x2020a0u : 0x302830u));
                tmst_prim_word(k, tmst_xy(12, y - 3)); tmst_prim_word(k, tmst_xy(344, 31));
            }
            uint32_t ink = !open ? 0x303030 : !ok ? 0x484848 : chosen ? 0x808080 : 0x686868;
            font_line(k, tmst_tourney(t)->name, 22, y, 0, 3, 5, ink);
            snprintf(line, sizeof line, "FINAL BOSS %s", open ? boss_name(t) : "");
            font_line(k, line, 30, y + 16, 0, 2, 5, open ? 0x585858 : 0x282828);
            const char *state = !open ? "LOCKED" : !ok ? "NOT AVAILABLE" : (progress.won >> t & 1) ? "WON" : "";
            if (*state) font_line(k, state, 346, y + 2, 2, 1, 2, !open ? 0x2020a0 : !ok ? 0x484848 : 0x30a0c0);
        }
    } else if (tm.view == VIEW_GRID && card_open) {      /* the MOVESET card, as the Expanded's */
        int sway = (int)(tm.frames % 60 < 30 ? tm.frames % 60 : 60 - tm.frames % 60) / 8;
        font_line(k, "MOVESET", CENTRE_X, 266, 1, 3, 5, 0x30a0c0);
        font_line(k, moves_name(card_id, mt.card_now), CENTRE_X, 300, 1, 1, 1, 0x808080);
        tmst_ui_prim_text(k, 14 - sway, 300, "<", 2, 0x3040f0);        /* the UI font's, as tall as the title's line */
        tmst_ui_prim_text(k, 342 + sway, 300, ">", 2, 0x3040f0);
    } else if (tm.view == VIEW_GRID) {
        font_line(k, tmst_tourney(tm.tourney)->name, CENTRE_X, 266, 1, 3, 5, 0x30a0c0);
        snprintf(line, sizeof line, "FINAL BOSS %s", boss_name(tm.tourney));
        font_line(k, line, CENTRE_X, 300, 1, 1, 2, 0x808080);
        font_line(k, "ONLY THE HIGHLIGHTED PLAYERS PARTICIPATE IN THIS TOURNAMENT", CENTRE_X, 330, 1, 2, 5, 0x606060);
    }
}

/* The game's primitives lying wholly below the grid go off screen, but the
 * full-width backdrop (as the Jukebox). */
static int vertex_x(uint32_t w) { return (int16_t)(w & 0xffff); }
static int vertex_y(uint32_t w) { return (int16_t)(w >> 16); }
/* Each drawing primitive of a packet: its command, its corners' extent
 * (a rectangle's width from its size), moved off screen where fn says so. */
typedef int (*PrimFn)(unsigned cmd, int minx, int miny, int maxx, int maxy, int width);
static void each_prim(uint32_t node, PrimFn fn) {
    unsigned words = psx_mod_read_word(node) >> 24;
    for (unsigned i = 1; i <= words;) {
        uint32_t a = node + i * 4, cmd = psx_mod_read_word(a) >> 24;
        unsigned size = 1, pos[16], count = 0;
        int width = 0;
        if (cmd >= 0x20 && cmd < 0x40) {
            unsigned verts = cmd & 8 ? 4 : 3, gouraud = (cmd & 0x10) != 0, tex = (cmd & 4) != 0, w = 1;
            for (unsigned v = 0; v < verts; v++) {
                if (v && gouraud) w++;
                pos[count++] = w++;
                if (tex) w++;
            }
            size = w;
        } else if (cmd >= 0x60 && cmd < 0x80) {
            pos[count++] = 1;
            size = 2 + ((cmd & 4) ? 1 : 0) + ((cmd & 0x18) == 0 ? 1 : 0);
            if ((cmd & 0x18) == 0 && i + size - 1 <= words) width = (int)(psx_mod_read_word(a + (size - 1) * 4) & 0x3ff);
        } else if (cmd >= 0x40 && cmd < 0x60) {
            unsigned gouraud = (cmd & 0x10) != 0, w = 1;
            for (unsigned v = 0; i + w <= words && count < 16; v++) {
                if (v && gouraud) w++;
                if (i + w > words) break;
                uint32_t word = psx_mod_read_word(a + w * 4);
                if (v >= 2 && (cmd & 8) && (word & 0xf000f000u) == 0x50005000u) break;
                pos[count++] = w++;
                if (v == 1 && !(cmd & 8)) break;
            }
            size = w + ((cmd & 8) ? 1 : 0);
        }
        if (i + size - 1 > words) break;
        int minx = 4096, maxx = -4096, miny = 4096, maxy = -4096;
        for (unsigned v = 0; v < count; v++) {
            uint32_t word = psx_mod_read_word(a + pos[v] * 4);
            if (vertex_y(word) < miny) miny = vertex_y(word);
            if (vertex_y(word) > maxy) maxy = vertex_y(word);
            if (vertex_x(word) < minx) minx = vertex_x(word);
            if (vertex_x(word) > maxx) maxx = vertex_x(word);
        }
        if (count > 1) width = maxx - minx;
        if (count && fn(cmd, minx, miny, maxx, maxy, width))
            for (unsigned v = 0; v < count; v++) psx_mod_write_word(a + pos[v] * 4, tmst_xy(-512, -512));
        i += size;
    }
}
static int below_grid(unsigned cmd, int minx, int miny, int maxx, int maxy, int width) {
    (void)cmd; (void)minx; (void)maxx; (void)maxy;
    return miny >= HIDE_FROM_Y && width < BACKDROP_W;
}
static void hide_below_grid(uint32_t node) {
    if (showing()) each_prim(node, below_grid);
}
/* The loading screen's see-through white bands behind the names (Team
 * Battle's VS screen: untextured, semi-transparent quads, shaded, wide, below
 * the top banner): gone. */
static int vs_band(unsigned cmd, int minx, int miny, int maxx, int maxy, int width) {
    (void)minx; (void)maxx; (void)maxy;
    return cmd >= 0x28 && cmd < 0x40 && !(cmd & 4) && (cmd & 2) && miny >= FIGHT_Y + 38 && width >= 150;
}
/* The loading screen's FIGHT 1: sprites of the title font on the red band
 * at its top left (x 28.., y 42, the band 36..72 and its fade to x 216). */
static int fight_letters(unsigned cmd, int minx, int miny, int maxx, int maxy, int width) {
    (void)maxx; (void)maxy; (void)width;
    return (cmd & 0xfc) == 0x64 && miny >= FIGHT_Y - 6 && miny < FIGHT_Y + 30 && minx < 216;
}

/* The game greys only the cell under the cursor when its fighter is not
 * available (the icon routine's flag 0x20: the palette swapped for the
 * stock grey ramp at (256, 501), whose order the guests' palettes follow
 * too). Every cell outside the tournament greys the same way here: the
 * textured primitives of the grid's icons get the ramp's palette, for that
 * drawing only. Each textured primitive: its palette word's address, its
 * top-left corner and size. */
static int prim_log;                      /* TMST_TOURNAMENT_PRIMLOG: this frame's walk is logged */
typedef void (*TexturedFn)(uint32_t clut_at, int x, int y, int w, int h, unsigned op);
static uint32_t walk_tpage;               /* the texture page the walk has reached (E1, or a polygon's) */
static void each_textured(uint32_t node, TexturedFn fn) {
    unsigned words = psx_mod_read_word(node) >> 24;
    for (unsigned i = 1; i <= words;) {
        uint32_t a = node + i * 4, cmd = psx_mod_read_word(a) >> 24;
        unsigned size = 1;
        if (cmd == 0xe1) walk_tpage = psx_mod_read_word(a) & 0x9ff;
        if (cmd >= 0x20 && cmd < 0x40) {
            unsigned verts = cmd & 8 ? 4 : 3, gouraud = (cmd & 0x10) != 0, tex = (cmd & 4) != 0, w = 1;
            int minx = 4096, miny = 4096, maxx = -4096, maxy = -4096;
            for (unsigned v = 0; v < verts; v++) {
                if (v && gouraud) w++;
                if (i + w <= words) {
                    uint32_t xy = psx_mod_read_word(a + w * 4);
                    int x = vertex_x(xy), y = vertex_y(xy);
                    if (x < minx) minx = x; if (x > maxx) maxx = x; if (y < miny) miny = y; if (y > maxy) maxy = y;
                }
                w++;
                if (tex) w++;
            }
            size = w;
            if (tex && i + size - 1 <= words) {
                walk_tpage = psx_mod_read_word(a + ((cmd & 0x10) ? 20 : 16)) >> 16 & 0x9ff;
                fn(a + 8, minx, miny, maxx - minx, maxy - miny, cmd);
            }
        } else if (cmd >= 0x60 && cmd < 0x80) {
            size = 2 + ((cmd & 4) ? 1 : 0) + ((cmd & 0x18) == 0 ? 1 : 0);
            if ((cmd & 4) && i + size - 1 <= words) {
                uint32_t xy = psx_mod_read_word(a + 4);
                int w = (cmd & 0x18) == 0x08 ? 1 : (cmd & 0x18) == 0x10 ? 8 : (cmd & 0x18) == 0x18 ? 16 : (int)(psx_mod_read_word(a + 12) & 0x3ff);
                int h = (cmd & 0x18) == 0 ? (int)(psx_mod_read_word(a + 12) >> 16 & 0x1ff) : w;
                fn(a + 8, vertex_x(xy), vertex_y(xy), w, h, cmd);
            }
        } else if (cmd >= 0x40 && cmd < 0x60) {
            unsigned gouraud = (cmd & 0x10) != 0, w = 1;
            for (unsigned v = 0; i + w <= words; v++) {
                if (v && gouraud) w++;
                if (i + w > words) break;
                uint32_t word = psx_mod_read_word(a + w * 4);
                if (v >= 2 && (cmd & 8) && (word & 0xf000f000u) == 0x50005000u) break;
                w++;
                if (v == 1 && !(cmd & 8)) break;
            }
            size = w + ((cmd & 8) ? 1 : 0);
        }
        i += size;
    }
}
static void log_textured(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    if (y < HIDE_FROM_Y)
        tmst_log("tournament: prim %02x at %d,%d %dx%d clut %04x page %04x rank %d", op, x, y, w, h,
                 psx_mod_read_word(clut_at) >> 16, op < 0x40 ? psx_mod_read_word(clut_at + 8) >> 16 : 0, tmst_overlay_rank());
}
/* The grid's table as the game reads it: the stock one (0x80022768), or
 * the Expanded's copy whose characters follow its page, which its LUI at
 * 0x80052E24 points at. 6 bytes a cell: x, y on the screen, ID * 4. */
static uint32_t grid_table(void) {
    uint32_t w = psx_mod_read_word(0x80052e24u);
    return (w >> 26) == 0x0f ? (w & 0xffffu) << 16 : STOCK_GRID;
}
enum { CELL_W = 36, CELL_H = 63, ICON_W = 32, ICON_H = 58, GREY_RAMP = 0x7d50, CELLS = 22, LOADING_SCREEN = 11,
       FIGHTERS_AT = 0x800add5c, COSTUMES_AT = 0x800add98, HELD_PORTRAIT = 0x80098106, NO_PORTRAIT = 0xff };
static int grey_tourney;
/* A stock fighter's icon read from VRAM the first time the grid draws it
 * (not greyed: the game's own grey ramp is not its palette). */
static void capture_icon(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    if (op < 0x64 || op > 0x67 || w != ICON_W || h < 50 || h > ICON_H || y >= HIDE_FROM_Y) return;
    /* The Tag page draws its guests in the stock cells (the table keeps the
     * stock IDs): read on the Tekken 3 page only. True Ogre has no cell of
     * his own (a guest's face was kept for him): Ogre's stands in. */
    if (tekken3_ttt1_selector_page && tekken3_ttt1_selector_page() > 0) return;
    uint32_t table = grid_table(), uv = psx_mod_read_word(clut_at);
    if ((uv >> 16) == GREY_RAMP) return;
    for (unsigned c = 0; c < CELLS; c++) {
        int cx = (int16_t)psx_mod_read_half(table + c * 6), cy = (int16_t)psx_mod_read_half(table + c * 6 + 2);
        if (x < cx || x >= cx + CELL_W || y < cy || y >= cy + CELL_H) continue;
        unsigned id = psx_mod_read_half(table + c * 6 + 4) / 4;
        ArtImage im;
        if (id < STOCK && id != TRUE_OGRE && !art_icon(stock_keys[id]) &&
            art_read_texture(walk_tpage, uv >> 16, (int)(uv & 255), (int)(uv >> 8 & 255), w, h, &im)) {
            art_set_icon(stock_keys[id], &im);
            tmst_log("tournament: %s's icon read from the grid", stock_keys[id]);
        }
        return;
    }
}
static void grey_icon(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    if (op < 0x64 || op > 0x67 || w != ICON_W || h > ICON_H || y >= HIDE_FROM_Y) return;
    uint32_t table = grid_table();
    for (unsigned c = 0; c < CELLS; c++) {
        int cx = (int16_t)psx_mod_read_half(table + c * 6), cy = (int16_t)psx_mod_read_half(table + c * 6 + 2);
        if (x < cx || x >= cx + CELL_W || y < cy || y >= cy + CELL_H) continue;
        unsigned id = psx_mod_read_half(table + c * 6 + 4) / 4;
        if (!allowed_id(grey_tourney, id))
            tmst_overlay_patch_word(clut_at, (psx_mod_read_word(clut_at) & 0xffffu) | (uint32_t)GREY_RAMP << 16);
        return;
    }
}
/* The loading screen's VS portraits: 126 x 252, 8 bpp, drawn as four bands
 * of 64 rows (the last 60), each with a palette of its own (P1's in one
 * texture page, P2's in another). Every band seen, the picture is read back
 * from VRAM: a stock fighter's VS portrait for the screens, kept in the
 * add-on's workspace for the next time. */
enum { VS_W = 126, VS_H = 252, VS_BAND = 64 };
static struct { uint32_t tpage; uint16_t clut[4]; int u, seen, done; } vs[2];
static void capture_vs(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    (void)y;
    if (op < 0x2c || op > 0x2f || w != VS_W || h > VS_BAND || (walk_tpage >> 7 & 3) != 1) return;
    uint32_t uv = psx_mod_read_word(clut_at);
    int band = (int)(uv >> 8 & 255) / VS_BAND, side = x + w / 2 < CENTRE_X ? 0 : 1;
    if (vs[side].seen && vs[side].tpage != walk_tpage) vs[side].seen = 0;
    vs[side].tpage = walk_tpage; vs[side].clut[band] = (uint16_t)(uv >> 16); vs[side].u = (int)(uv & 255);
    vs[side].seen |= 1 << band;
    if (vs[side].seen == 15) mt.loading_vs = 1;      /* a whole portrait: the VS screen (not the grid's PLEASE WAIT) */
}
/* A stock fighter's portrait read back from VRAM (NULL px: not readable). */
static int vs_read(int p, ArtImage *im) {
    *im = (ArtImage){0};
    im->px = calloc((size_t)VS_W * VS_H, sizeof(uint32_t)); im->w = VS_W; im->h = VS_H;
    int ok = im->px != NULL;
    for (int b = 0; b < 4 && ok; b++) {
        ArtImage band;
        int rows = b == 3 ? VS_H - 3 * VS_BAND : VS_BAND;
        ok = art_read_texture(vs[p].tpage, vs[p].clut[b], vs[p].u, b * VS_BAND, VS_W, rows, &band);
        if (ok) { memcpy(im->px + (size_t)b * VS_BAND * VS_W, band.px, sizeof(uint32_t) * VS_W * (size_t)rows); free(band.px); }
    }
    if (!ok) { free(im->px); im->px = NULL; }
    return ok;
}
/* The stock fighter whose own portrait the loading screen shows for p (-1:
 * a guest's, Panda's, Tiger's, which come from their .jui). */
static int vs_stock(int p) {
    unsigned id = psx_mod_read_half(FIGHTERS_AT + 2 * p), costume = psx_mod_read_half(COSTUMES_AT + 2 * p);
    return id >= STOCK || (id == KUMA && costume == 1) || (id == EDDY && costume == 2) ? -1 : (int)id;
}
/* Pixels (RGB, see-through as one) that differ, of the portrait's. */
static int vs_differ(const ArtImage *a, const ArtImage *b) {
    if (!a->px || !b->px || a->w != b->w || a->h != b->h) return a->w * a->h;
    int n = 0;
    for (int i = 0; i < a->w * a->h; i++) {
        uint32_t x = a->px[i] >> 24 ? a->px[i] & 0xffffff : 0x1000000u, y = b->px[i] >> 24 ? b->px[i] & 0xffffff : 0x1000000u;
        n += x != y;
    }
    return n;
}
/* Each loading-screen frame, both portraits read back once all their bands
 * were drawn: a portrait is good when it is the cached one (the game drew
 * its bands before the texels were in place, or over others' pixels, is
 * what the player saw broken), or, none cached, when it has held still for
 * VS_STILL frames: only then cached (never a broken one kept). */
/* VS_STILL: the loading screen lasts about 15 frames, a portrait reads the
 * same from its first whole read on (measured 2026-11-11): at 10 it was never
 * kept, the stock fighters had their grid icons on the screens (issue #3). */
enum { VS_STILL = 4, VS_SLACK = 200 };          /* pixels allowed to differ (of 31752) */
static struct { uint32_t sum; int still, good; } vs_check[2];
static int vs_portraits_good(void) {
    int good = 1;
    for (int p = 0; p < 2; p++) {
        if (vs_check[p].good) continue;
        if (vs[p].seen != 15) { good = 0; continue; }
        int id = vs_stock(p);
        if (id < 0) { vs_check[p].good = 1; continue; }
        ArtImage im;
        if (!vs_read(p, &im)) { good = 0; continue; }
        /* Another costume's (or an outfit mod's) portrait is not the cached
         * one: held still, it is good too, but not cached. */
        const ArtImage *cached = art_portrait_cached(stock_keys[id]);
        int same = cached && vs_differ(&im, cached) <= VS_SLACK;
        uint32_t sum = 2166136261u;
        for (int i = 0; i < VS_W * VS_H; i++) sum = (sum ^ im.px[i]) * 16777619u;
        vs_check[p].still = sum == vs_check[p].sum ? vs_check[p].still + 1 : 0;
        vs_check[p].sum = sum;
        vs_check[p].good = same || vs_check[p].still >= VS_STILL;
        if (vs_check[p].good && !cached) art_set_portrait(stock_keys[id], &im, 1);
        else free(im.px);
        if (vs_check[p].good) tmst_log("tournament: P%d's loading portrait (%s) %s", p + 1, stock_keys[id],
                                       same ? "is the cached one" : cached ? "held still (not the cached one)" : "held still, kept");
        good &= vs_check[p].good;
    }
    return good;
}
static void log_loading(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    if (w >= 48 && h >= 48)
        tmst_log("tournament: loading prim %02x at %d,%d %dx%d uv %08x tpage %03x", op, x, y, w, h,
                 psx_mod_read_word(clut_at), walk_tpage);
}
/* The loading screen before a boss's second form (Devil, True Ogre): Team
 * Battle's lost member under its name, sliding in with it (y 369..433): its
 * icon greyed (one 32 x 58 sprite, Ogre's; a guest's in three, 32 x 10,
 * 24, 24, Kazuya's) and a mark over it (two 32 x 32 sprites, palette
 * 0x7F1D), in primitive lists of their own. Every 32-wide sprite there goes
 * (issue #14, measured: i14d.py; the names' letters are 24 wide, the
 * portraits 126). */
enum { LOST_W = 32, LOST_FROM_Y = 360 };
static void lost_member(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    (void)x;
    if ((op & 0xfc) == 0x64 && w == LOST_W && y >= LOST_FROM_Y && (h == 10 || h == 24 || h == 32 || h == 58))
        psx_mod_write_word(clut_at - 4, tmst_xy(-512, -512));
}
/* The fight's HUD (Team Battle's): over P1's bar TEAM BATTLE, sprites of
 * the HUD's letters (10 x 16, every 9 units from x 37, y 24, the white
 * palette; CREDIT on the right in the yellow one). Its letters go, the
 * tournament's are drawn there (tournament_prims), on the page and palette
 * found. The letters' cells: A..J from u 0x9A, v 0xE0, K..Z from u 0x04,
 * v 0xF0, every 10. Under the bars each team's members, a figure each
 * (textured quads 10 x 24 at x 156 and 202, y 64, palette 0x7ED0; a lost
 * one's 0x7ED1, with a mark over it, a 10 x 21 sprite): they go
 * too, the rounds' lamps come instead, as the Arcade draws them (sprites
 * 15 x 18 at y 70 on page (832, 0), u 0xF0 v 0xA8, dark palette 0x7F19,
 * lit 0x7F18; P1's from x 151 leftwards, P2's from 202 rightwards, every
 * 14, the first won the innermost). */
enum { HUD_Y = 24, HUD_W = 10, HUD_H = 16, HUD_ADVANCE = 9, HUD_WHITE = 0x7e16,
       FIGURE_W = 10, FIGURE_H = 24, FIGURE_CLUT = 0x7ed0, FIGURE_LOST = 0x7ed1, MARK_H = 21,
       LAMP_DX = -5, LAMP_DY = 6, LAMP_W = 15, LAMP_H = 18, LAMP_STEP = 14, LAMP_UV = 0xa8f0, LAMP_TPAGE = 0x00d,
       LAMP_DARK = 0x7f19, LAMP_LIT = 0x7f18, LAMPS_MAX = 5,
       FIGHT_FRAMES = 0x800afa4c, BLINK_BIT = 16 };
static struct {
    int seen; unsigned frame; int x0, x1; uint32_t tpage, clut;   /* the title; frame: of the list seen */
    int figure[2]; unsigned figure_frame; int figure_x[2], figure_y, figure_rank;
    int won[2], blink[2]; uint32_t ko_life[2];    /* issue #4: a side's last lamp blinking; the loser's life at the KO */
} hud;
static void lamp_prims(TmstPrims *k);
static void hud_figure(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    unsigned clut = psx_mod_read_word(clut_at) >> 16;
    if ((op & 0xfc) == 0x64 && w == FIGURE_W && h == MARK_H && clut == FIGURE_LOST) {     /* a lost member's mark */
        psx_mod_write_word(clut_at - 4, tmst_xy(-512, -512));
        return;
    }
    if ((op & 0xfc) != 0x2c || w != FIGURE_W || h != FIGURE_H || (clut != FIGURE_CLUT && clut != FIGURE_LOST)) return;
    int side = x + w / 2 >= CENTRE_X;
    if (hud.figure_frame != tm.frames) { hud.figure_frame = tm.frames; hud.figure[0] = hud.figure[1] = 0; }
    if (!hud.figure[side] || (side ? x < hud.figure_x[1] : x > hud.figure_x[0])) hud.figure_x[side] = x;
    hud.figure[side] = 1; hud.figure_y = y;
    int rank = tmst_overlay_rank();
    if (rank >= 0 && rank != hud.figure_rank) { hud.figure_rank = rank; tmst_overlay_set_rank_prims(rank, lamp_prims); }
    for (int v = 0; v < 4; v++) psx_mod_write_word(clut_at - 4 + (uint32_t)v * 8, tmst_xy(-512, -512));
}
static void big_letter(uint32_t clut_at, int x, int y, int w, int h, unsigned op);
static void hud_title(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    hud_figure(clut_at, x, y, w, h, op);
    big_letter(clut_at, x, y, w, h, op);
    if ((op & 0xfc) != 0x64 || w != HUD_W || h != HUD_H || y != HUD_Y || x >= CENTRE_X ||
        (psx_mod_read_word(clut_at) >> 16) != HUD_WHITE)
        return;
    if (!hud.seen || hud.frame != tm.frames) { hud.seen = 1; hud.frame = tm.frames; hud.x0 = x; hud.x1 = x + w; }
    if (x < hud.x0) hud.x0 = x;
    if (x + w > hud.x1) hud.x1 = x + w;
    hud.tpage = walk_tpage; hud.clut = HUD_WHITE;
    psx_mod_write_word(clut_at - 4, tmst_xy(-512, -512));
}
static uint32_t hud_uv(char c) {
    return c <= 'J' ? (uint32_t)(0x9a + (c - 'A') * 10) | 0xe0u << 8 : (uint32_t)(0x04 + (c - 'K') * 10) | 0xf0u << 8;
}
/* The fight's opening words, TEAM BATTLE: letters of a big font, 24 x 40 on
 * page (896, 0) 4 bpp, eight to a row (A..H, I..P, Q..X, Y Z ! ? . ' 1 2,
 * 3..9 -), sprites every 22 at y 112 (palette 0x7E98) once in place, quads
 * twice that size as they zoom in (0x7E9A). When a list's letters say
 * TEAMBATTLE they go, and the tournament's name comes instead, in two lines
 * (TEKKEN 1 / TOURNAMENT), at their size and centre (hud_prims). */
enum { BIG_W = 24, BIG_H = 40, BIG_ADVANCE = 22, BIG_LINE = 44, BIG_MAX = 24 };
static const char big_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!?.'1234567890-";
static struct {
    unsigned frame; int n;
    struct { int x, y, w, h; char c; uint32_t at, tpage, clut; int quad; } l[BIG_MAX];
} big;
static char big_char(uint32_t uv) {
    int col = (int)(uv & 255) / BIG_W, row = (int)(uv >> 8 & 255) / BIG_H, i = row * 8 + col;
    if (col > 7 || i >= 39) return '?';
    return i < 30 ? big_chars[i] : i < 32 ? "12"[i - 30] : "3456789-"[i - 32];
}
static int big_index(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= '1' && c <= '2') return 30 + c - '1';
    if (c >= '3' && c <= '9') return 32 + c - '3';
    return -1;
}
static void big_letter(uint32_t clut_at, int x, int y, int w, int h, unsigned op) {
    if ((walk_tpage & 0x19f) != 0x00e || h <= 0 || abs(w * BIG_H - h * BIG_W) > BIG_W + BIG_H) return;   /* a pixel either way */
    int quad = (op & 0xfc) == 0x2c;
    if (!quad && (op & 0xfc) != 0x64) return;
    if (big.frame != tm.frames) { big.frame = tm.frames; big.n = 0; }
    if (big.n >= BIG_MAX) return;
    uint32_t uv = psx_mod_read_word(clut_at);
    big.l[big.n].x = x; big.l[big.n].y = y; big.l[big.n].w = w; big.l[big.n].h = h;
    big.l[big.n].c = big_char(uv); big.l[big.n].at = clut_at - 4; big.l[big.n].quad = quad;
    big.l[big.n].tpage = walk_tpage & 0x9ff; big.l[big.n].clut = uv >> 16;
    big.n++;
}
/* This list's letters on one line (the same y and size), left to right,
 * TEAMBATTLE (READY? may share the list): gone, the name drawn. */
static void big_prims(TmstPrims *k) {
    if (big.frame != tm.frames) return;
    char word[BIG_MAX + 1]; int order[BIG_MAX], n = 0, first = -1;
    for (int a = 0; a < big.n && first < 0; a++) {
        n = 0;
        for (int i = 0; i < big.n; i++)
            if (big.l[i].y == big.l[a].y && big.l[i].h == big.l[a].h && big.l[i].quad == big.l[a].quad) order[n++] = i;
        for (int i = 1; i < n; i++)
            for (int j = i; j > 0 && big.l[order[j]].x < big.l[order[j - 1]].x; j--) { int t = order[j]; order[j] = order[j - 1]; order[j - 1] = t; }
        for (int i = 0; i < n; i++) word[i] = big.l[order[i]].c;
        word[n] = 0;
        if (!strcmp(word, "TEAMBATTLE")) first = a;
    }
    if (first < 0) return;
    int x0 = 4096, x1 = -4096, y0 = 4096, y1 = -4096, w = big.l[first].w, h = big.l[first].h;
    uint32_t tpage = big.l[first].tpage, clut = big.l[first].clut;
    for (int i = 0; i < n; i++) {
        const typeof(big.l[0]) *l = &big.l[order[i]];
        if (l->x < x0) x0 = l->x; if (l->x + l->w > x1) x1 = l->x + l->w;
        if (l->y < y0) y0 = l->y; if (l->y + l->h > y1) y1 = l->y + l->h;
        if (l->quad) for (int v = 0; v < 4; v++) psx_mod_write_word(l->at + (uint32_t)v * 8, tmst_xy(-512, -512));
        else psx_mod_write_word(l->at, tmst_xy(-512, -512));
    }
    /* The name in two lines: what comes before TOURNAMENT, then TOURNAMENT. */
    char head[48]; const char *name = tmst_tourney(tm.tourney)->name, *t = strstr(name, " TOURNAMENT");
    snprintf(head, sizeof head, "%.*s", t ? (int)(t - name) : (int)strlen(name), name);
    const char *lines[2] = {head, t ? "TOURNAMENT" : ""};
    int adv = BIG_ADVANCE * w / BIG_W, gap = BIG_LINE * h / BIG_H, cx = (x0 + x1) / 2, cy = (y0 + y1) / 2;
    tmst_prim_room(k, 1);
    tmst_prim_word(k, 0xe1000000u | tpage);
    for (int ln = 0; ln < 2; ln++) {
        int n = (int)strlen(lines[ln]), x = cx - ((n - 1) * adv + w) / 2, y = cy - gap / 2 - h / 2 + ln * gap;
        if (!*lines[1]) y = cy - h / 2;
        for (int i = 0; i < n; i++, x += adv) {
            int g = big_index(lines[ln][i]);
            if (g < 0) continue;
            uint32_t u = (uint32_t)(g % 8 * BIG_W), v = (uint32_t)(g / 8 * BIG_H);
            tmst_prim_room(k, 9);
            tmst_prim_word(k, 0x2d808080u);
            tmst_prim_word(k, tmst_xy(x, y));         tmst_prim_word(k, u | v << 8 | clut << 16);
            tmst_prim_word(k, tmst_xy(x + w, y));     tmst_prim_word(k, (u + BIG_W) | v << 8 | tpage << 16);
            tmst_prim_word(k, tmst_xy(x, y + h));     tmst_prim_word(k, u | (v + BIG_H) << 8);
            tmst_prim_word(k, tmst_xy(x + w, y + h)); tmst_prim_word(k, (u + BIG_W) | (v + BIG_H) << 8);
        }
        if (!*lines[1]) break;
    }
}

static void lamp(TmstPrims *k, int x, int y, int lit) {
    tmst_prim_room(k, 4);
    tmst_prim_word(k, 0x65808080u);
    tmst_prim_word(k, tmst_xy(x, y));
    tmst_prim_word(k, (uint32_t)LAMP_UV | (uint32_t)(lit ? LAMP_LIT : LAMP_DARK) << 16);
    tmst_prim_word(k, tmst_xy(LAMP_W, LAMP_H));
}
/* Issue #4: the Arcade's newly won lamp blinks (lit / dark, bit 4 of the
 * fight's frame counter 0x800AFA4C: 16 frames each) from the KO until the
 * fallen one's life comes back (the next round), at the match's last KO
 * till the fight's end (measured: i4a, i4b). The same here: a side's last
 * lamp blinks from its round won (the record's +0x44 up) until the other's
 * life is above what it was then. */
static void lamp_blink(void) {
    for (int side = 0; side < 2; side++) {
        int won = psx_mod_read_byte(RECORD + (uint32_t)side * RECORD_STRIDE + ROUNDS_WON);
        uint32_t loser = psx_mod_read_word(RECORD + (uint32_t)!side * RECORD_STRIDE + LIFE);
        if (!mt.life_set || won < hud.won[side]) hud.blink[side] = 0;          /* a new fight (or member) */
        else if (won > hud.won[side]) {
            hud.blink[side] = 1; hud.ko_life[side] = loser;
            tmst_log("tournament: P%d won a round, its lamp %d blinks", side + 1, won);
        }
        else if (hud.blink[side] && loser > hud.ko_life[side]) hud.blink[side] = 0;
        hud.won[side] = won;
    }
}
static void hud_title_prims(TmstPrims *k);
/* The lamps where the figures were, in the ordering table (a builder of
 * the figures' rank, seen in the frame before): behind the fighters as
 * they were, not over everything. The HUD's title too (issue #4: it was
 * drawn over the fighters; the game's TEAM BATTLE letters are at the
 * figures' rank, 1006, measured). */
static void lamp_prims(TmstPrims *k) {
    if (mt.flow != FLOW_MATCH || psx_mod_read_word(SCREEN) != FIGHT) return;
    hud_title_prims(k);
    lamp_blink();
    if (hud.figure_frame == tm.frames && (hud.figure[0] || hud.figure[1])) {
        int lamps = psx_mod_read_byte(ROUNDS) + 1, dark = !(psx_mod_read_half(FIGHT_FRAMES) & BLINK_BIT);
        if (lamps > LAMPS_MAX) lamps = LAMPS_MAX;
        tmst_prim_room(k, 1);
        tmst_prim_word(k, 0xe1000000u | LAMP_TPAGE);
        for (int side = 0; side < 2; side++) {
            if (!hud.figure[side]) continue;
            int won = hud.won[side];
            int x = side ? hud.figure_x[1] : hud.figure_x[0] + LAMP_DX, step = side ? LAMP_STEP : -LAMP_STEP;
            for (int i = 0; i < lamps; i++, x += step)
                lamp(k, x, hud.figure_y + LAMP_DY, i < won && !(i == won - 1 && hud.blink[side] && dark));
        }
    }
}
static void hud_prims(TmstPrims *k) { big_prims(k); }
static void hud_title_prims(TmstPrims *k) {
    static const char title[] = "TOURNAMENT MODE";
    if (!hud.seen || hud.frame != tm.frames) return;   /* the HUD not drawn in this list */
    int n = (int)sizeof title - 1, width = (n - 1) * HUD_ADVANCE + HUD_W, x = (hud.x0 + hud.x1) / 2 - width / 2;
    tmst_prim_room(k, 1);
    tmst_prim_word(k, 0xe1000000u | hud.tpage);
    for (int i = 0; i < n; i++, x += HUD_ADVANCE) {
        if (title[i] < 'A' || title[i] > 'Z') continue;
        tmst_prim_room(k, 4);
        tmst_prim_word(k, 0x65808080u);
        tmst_prim_word(k, tmst_xy(x, HUD_Y));
        tmst_prim_word(k, hud_uv(title[i]) | hud.clut << 16);
        tmst_prim_word(k, tmst_xy(HUD_W, HUD_H));
    }
}

static void grid_visitor(uint32_t node) {
    tmst_tournament_options_visit(node);                         /* GAME OPTION's mode box (its own screen) */
    if (psx_mod_read_word(SCREEN) == MENU) return;               /* its colour: the core's (tmst_menu_set_colour) */
    if (mt.flow == FLOW_MATCH && psx_mod_read_word(SCREEN) == FIGHT) { each_textured(node, hud_title); return; }
    if (psx_mod_read_word(SCREEN) == LOADING_SCREEN) {
        if (prim_log < 0) each_textured(node, log_loading);
        if (mt.flow == FLOW_MATCH && mt.match >= 0) {
            each_prim(node, fight_letters); each_prim(node, vs_band);
            if (mt.second >= 0) each_textured(node, lost_member);
        }
        each_textured(node, capture_vs);
    }
    hide_below_grid(node);
    if (!showing() || !team_grid()) return;
    if (prim_log > 0) each_textured(node, log_textured);
    each_textured(node, capture_icon);
    if (unified() || (tm.view != VIEW_LIST && tm.view != VIEW_GRID)) return;
    grey_tourney = tm.view == VIEW_LIST ? tm.sel : tm.tourney;
    each_textured(node, grey_icon);
}

/* The panel's picture: 1024 x 392 texels for the screen's 368 x 196 (576
 * wide in the upper half where the grid has the Expanded's HD faces:
 * TMST_OVERLAY_ROOM_FOR_HD). */
enum { PIC_X = 0, PIC_Y = 284, PIC_W = 368, PIC_H = 196, PW = 1024, PH = 392, HINT_UP = 50,   /* the hint clear of the screen's edge */
       MATTE = 0xff805858u };      /* about the red-brown backdrop under the hint (measured headless, 128 88 88) */
static uint32_t pic[PW * PH];
static unsigned pic_version = ~0u;
static int pic_on;
static void fill(int x, int y, int w, int h, uint32_t c) { tmst_fill_argb(pic, PW, PH, x, y, w, h, c); }
/* The UI font (Fira Sans), y the capitals' top. */
static void txt(int x, int y, const char *s, uint32_t c) { tmst_ui_text_argb(pic, PW, PH, x, y, &tmst_ui_face_list, s, c, MATTE); }
static int text_w(const char *s) { return tmst_ui_width(&tmst_ui_face_list, s); }
static void centred(int y, const char *s, uint32_t c) { txt((PW - text_w(s)) / 2, y, s, c); }
/* A hint with the pad's buttons (ART_X, ART_O), centred: the symbol alone
 * in the PlayStation's colour, 16 x 16 texels with 2-texel strokes, as the
 * Jukebox's and Photo Mode's hints draw it. */
enum { KEY_W = 18, KEY_GAP = 6 };
static void key_px(int x, int y, uint32_t c) { if (x >= 0 && x < PW && y >= 0 && y < PH) pic[y * PW + x] = c; }
static void key_line(int x0, int y0, int x1, int y1, uint32_t c) {
    int n = abs(x1 - x0) > abs(y1 - y0) ? abs(x1 - x0) : abs(y1 - y0);
    for (int i = 0; i <= n; i++)
        for (int dy = 0; dy < 2; dy++) for (int dx = 0; dx < 2; dx++)
            key_px(x0 + (x1 - x0) * i / (n ? n : 1) - 1 + dx, y0 + (y1 - y0) * i / (n ? n : 1) - 1 + dy, c);
}
static void pad_key(char b, int x, int y) {
    if (b == ART_CROSS) { key_line(x + 2, y + 2, x + 14, y + 14, 0xff7aa7ffu); key_line(x + 14, y + 2, x + 2, y + 14, 0xff7aa7ffu); return; }
    for (int dy = -7; dy <= 7; dy++)                    /* a ring of radius 7, 2 thick */
        for (int dx = -7; dx <= 7; dx++) { int d = dx * dx + dy * dy; if (d <= 49 && d >= 25) key_px(x + 8 + dx, y + 8 + dy, 0xffff6b6bu); }
}
static void hint(int y, const char *s, uint32_t c) {
    int width = 0;
    for (const char *p = s; *p;) {
        if (*p == ART_CROSS || *p == ART_CIRCLE) { width += KEY_W + KEY_GAP; p++; continue; }
        char part[128]; int n = 0;
        while (p[n] && p[n] != ART_CROSS && p[n] != ART_CIRCLE && n < 127) { part[n] = p[n]; n++; }
        part[n] = 0; width += text_w(part); p += n;
    }
    int x = (PW - width) / 2;
    for (const char *p = s; *p;) {
        if (*p == ART_CROSS || *p == ART_CIRCLE) { pad_key(*p, x, y); x += KEY_W + KEY_GAP; p++; continue; }
        char part[128]; int n = 0;
        while (p[n] && p[n] != ART_CROSS && p[n] != ART_CIRCLE && n < 127) { part[n] = p[n]; n++; }
        part[n] = 0; txt(x, y, part, c); x += text_w(part); p += n;
    }
}
static const char *boss_name(int t) {
    const TmstTourney *d = tmst_tourney(t);
    if (!d->boss) return "DRAWN AT THE START";
    int b = roster_find(d->boss);
    return b >= 0 ? roster[b].name : "NOT IN THIS INSTALL";
}
static void draw(void) {
    fill(0, 0, PW, PH, 0);
    if (tm.view == VIEW_LIST || (tm.exiting && tm.view != VIEW_GRID)) {
        hint(PH - HINT_UP, "UP / DOWN  Choose     " ART_X "Confirm     " ART_O "Exit", 0xffd8d0c8u);
    } else {
        int pages = tekken3_ttt1_selector_page && tekken3_ttt1_selector_page() >= 0 && !unified();
        if (card_open) hint(PH - HINT_UP, "LEFT/RIGHT  Change moveset     " ART_X "Take     " ART_O "Back", 0xffd8d0c8u);
        else hint(PH - HINT_UP, pages ? "L2/R2  Switch character panel     " ART_X "Select     SELECT  Back"
                                      : ART_X "Select     SELECT  Back", 0xffd8d0c8u);
    }
}
static void show_picture(void) {
    if (pic_version != tm.version || !pic_on) {
        if (pic_version != tm.version) { draw(); pic_version = tm.version; }
        tmst_overlay_show_scaled(pic, PW, PH, PIC_X, PIC_Y, PIC_W, PIC_H, TMST_OVERLAY_BORROW | TMST_OVERLAY_ROOM_FOR_HD);
        pic_on = 1;
    }
}

/* ---- the cover's screens ------------------------------------------------------------ */
/* The canvas over the 16:9 screen (490 units across, the 4:3 screen's 368 in
 * its middle), in upper VRAM borrowed: no stage loads on the menu, the title,
 * the grid nor the loading screen (the stage engine uploads once the fight
 * has begun, measured); where one does (the attract's demo) the picture
 * yields to it, and the black and the text under it show, till the next
 * screen. */
enum { CANVAS_X = -61, CANVAS_W = 490 };
static int canvas_borrowed = -1;                 /* the screen it was shown on */
static void show_canvas(unsigned screen, int x, int y, int w, int h) {
    if ((int)screen != canvas_borrowed) { tmst_overlay_hide_picture(); canvas_borrowed = (int)screen; }
    tmst_overlay_show_scaled(art_canvas, ART_W, ART_H, x, y, w, h, TMST_OVERLAY_BORROW | TMST_OVERLAY_YIELD);
    pic_on = 1;
}
/* The tournament's story (data/tournament/story/<id>.txt), read at its
 * start: 1 when it has one (the T1, T2 and T3 tournaments). */
static TmstStory story;
static int story_load(int tourney) {
    char path[4200], rel[200];
    snprintf(rel, sizeof rel, "data/tournament/story/%s.txt", tmst_tourney(tourney)->id);
    tmst_files_addon(path, sizeof path, rel);                  /* or the copy in mods/tmst-data */
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    static char text[TMST_STORY_TEXT];
    size_t n = fread(text, 1, sizeof text - 1, f);
    fclose(f);
    text[n] = 0;
    int ok = tmst_story_parse(&story, text);
    tmst_log("tournament: the story %s %s", path, ok ? "told" : "empty");
    return ok;
}
int tmst_tournament_covered(void) { return on && mt.cover; }

static void cover_screen(unsigned screen) {
    static int c_scr = -1, c_drawn = -1, c_blink = -1, c_conf = -1, c_match = -1, c_mt = -1, c_out = -1, c_tourn = -1;
    static unsigned c_shown = ~0u;
    unsigned t = tm.frames - mt.scr_at;
    const char *tourney = tmst_tourney(tm.tourney)->name;
    ScreenProgress s = {&bracket, &screen_roster, tourney, mt.shown, 0, 256, mt.match, (tm.frames / 20 & 1) != 0,
                        TMST_BRACKET_SLOTS, (int)tm.frames, 0, NULL};
    int x = CANVAS_X, y = 0, w = CANVAS_W, h = ART_H;
    int can_cache = (mt.scr == SCR_DRAW || mt.scr == SCR_WAIT || mt.scr == SCR_ASK ||
                     mt.scr == SCR_MATCH || mt.scr == SCR_END || mt.scr == SCR_RESULT);
    if (mt.scr == SCR_DRAW) {
        s.drawn = (int)(t / DRAW_STEP);
        if (s.drawn >= TMST_BRACKET_SLOTS) { s.drawn = TMST_BRACKET_SLOTS; mt.scr = SCR_WAIT; mt.scr_at = tm.frames; }
    } else if (mt.scr == SCR_RESULT) {
        if (!(tm.active && team_grid()) && screen != MENU) { mt.scr_at = tm.frames; t = 0; }
        int k = (int)t - RESULT_WAIT;
        s.moving = (int)mt.fresh;
        s.moving_t = k <= 0 ? 0 : k >= RESULT_MOVE ? 256 : k * 256 / RESULT_MOVE;
        if (t > RESULT_WAIT + RESULT_MOVE + 20) {
            mt.scr = mt.outcome == NEXT_MATCH ? SCR_WAIT : SCR_END; mt.scr_at = tm.frames; mt.fresh = 0;
        }
    }
    if (can_cache && pic_on && (int)screen == canvas_borrowed &&
        c_scr == mt.scr && c_drawn == s.drawn && c_blink == s.blink &&
        c_conf == mt.confirmed && c_match == mt.match && c_mt == s.moving_t &&
        c_out == mt.outcome && c_tourn == tm.tourney && c_shown == mt.shown) {
        return;
    }
    switch (mt.scr) {
    case SCR_STORY:
        s.next = -1; s.undrawn = 1;
        s.prompt = mt.story_done ? ART_X "  START THE DRAW" : "START  SKIP";
        if (screen_story(&s, &story, (int)(t * STORY_SPEED_NUM / STORY_SPEED_DEN))) mt.story_done = 1;
        break;
    case SCR_DRAW:
        s.next = -1;
        screen_progress(&s);
        break;
    case SCR_RESULT:
        s.next = -1;
        screen_progress(&s);
        break;
    case SCR_WAIT:
        s.prompt = ART_X "  NEXT MATCH     " ART_O "  QUIT";
        screen_progress(&s);
        break;
    case SCR_ASK:
        s.prompt = "QUIT THE TOURNAMENT   " ART_X "  YES   " ART_O "  NO";
        screen_progress(&s);
        break;
    case SCR_ZOOM: {
        int fx, fy, z = 100 + (ZOOM_MAX - 100) * (int)(t > ZOOM_FRAMES ? ZOOM_FRAMES : t) / ZOOM_FRAMES;
        s.blink = 1;
        if (!(pic_on && c_scr == SCR_ZOOM && c_tourn == tm.tourney && c_shown == mt.shown)) {
            screen_progress(&s);
        }
        screen_match_point(mt.match, &fx, &fy);
        int ux = CANVAS_X + fx * CANVAS_W / ART_W, p = (int)(t > ZOOM_FRAMES ? ZOOM_FRAMES : t);
        int cx = ux + (CENTRE_X - ux) * p / ZOOM_FRAMES, cy = fy + (ART_H / 2 - fy) * p / ZOOM_FRAMES;
        w = CANVAS_W * z / 100; h = ART_H * z / 100;
        x = cx - fx * w / ART_W; y = cy - fy * h / ART_H;
        if (t >= ZOOM_FRAMES) { mt.scr = SCR_MATCH; mt.scr_at = tm.frames; }
        break;
    }
    case SCR_MATCH:
        screen_match(&s, mt.match);
        break;
    case SCR_END:
        s.next = -1; s.prompt = mt.confirmed ? NULL : ART_X "  CONTINUE";
        if (mt.outcome == CHAMPION)
            screen_champion(&s, mt.unlocked >= 0 && mt.unlocked != tm.tourney ? tmst_tourney(mt.unlocked)->name : NULL);
        else screen_over(&s);
        break;
    default:
        return;
    }
    c_scr = mt.scr; c_drawn = s.drawn; c_blink = s.blink;
    c_conf = mt.confirmed; c_match = mt.match; c_mt = s.moving_t;
    c_out = mt.outcome; c_tourn = tm.tourney; c_shown = mt.shown;
    show_canvas(screen, x, y, w, h);
}

/* TMST_TOURNAMENT_GUIDE=<folder>: the screens' layouts over their
 * backgrounds as PNGs there (once the font is read), for drawing new ones. */
static void write_guides(void) {
    const char *dir = getenv("TMST_TOURNAMENT_GUIDE");
    static const char *const names[4] = {"progress", "match", "champion", "over"};
    if (!dir || !*dir) return;
    for (int i = 0; i < 4; i++) {
        char path[4200];
        snprintf(path, sizeof path, "%s/%s_guide.png", dir, names[i]);
        screen_guide(i);
        tmst_log("tournament: guide %s %s", path, art_write_png(path) ? "written" : "not written");
    }
}

/* ---- the frame -------------------------------------------------------------------- */
static void close_screen(void) {
    tm.left_at = 0; tm.active = 0; tm.view = VIEW_OFF; tmst_overlay_hide(); pic_on = 0;
    if (unified() && tmst_unified_select_set_allowed) tmst_unified_select_set_allowed(NULL);
    release_wait = tm.exiting;
    tmst_log("tournament: grid closed");
}
static void tick(void) {
    if (!on) return;
    tmst_jukebox_player_tick();                      /* the tournament's piece without the Jukebox (issue #9.2) */
    tmst_tournament_options_tick();
    tmst_menu_vblank();
    tmst_overlay_vblank();
    tm.frames++;
    unsigned screen = psx_mod_read_word(SCREEN);
    /* The Expanded's card starts on Tekken 3's moves again on every main
     * menu (tekken3_native_moves.c, screen MENU), also after a pause's RESET
     * or the list's way out: ours too (issue #14). */
    if (screen == MENU) mt.want_now = 0;
    if (screen == MENU || screen == TITLE) embu_restore();   /* before any attract */
    /* The grid opened from our row, or (between the matches) again straight
     * from the results, as the game does after a button there. */
    if ((tm.pending || (mt.flow == FLOW_BETWEEN && !tm.active)) && team_grid()) {
        tm.pending = 0; tm.active = 1; tm.view = VIEW_LIST; tm.exiting = 0; card_open = 0; tm.previous = 0xffff; tm.version++; tm.armed = 0;
        build_roster(); progress_path(); tmst_progress_load(&progress, progress_file);
        while (tm.sel > 0 && !(tmst_progress_open(&progress, tm.sel) && playable(tm.sel))) tm.sel--;
        tmst_log("tournament: open (progress %s, won 0x%x)", progress_file[0] ? progress_file : "-", progress.won);
        if (mt.flow == FLOW_BETWEEN && mt.outcome == NEXT_MATCH) {
            tm.view = VIEW_GRID; tm.sel = tm.tourney; mt.step = STEP_SETTLE; mt.step_at = tm.frames; mt.tries = 0;
        }
    }
    /* Between the matches, on the grid once the size is given: the player's
     * fighter, or (the tournament over) the list. */
    if (mt.flow == FLOW_BETWEEN && tm.active && team_grid() && psx_mod_read_word(block(0)) == CHOOSING) {
        if (mt.outcome == NEXT_MATCH) pick_tick();
        else if (!tm.exiting) { tmst_log("tournament: done (%s), to the main menu", mt.outcome == QUIT ? "left" : "over"); exit_to_menu(); }
    }
    /* The grid's availability again after the Expanded's VBlank may have
     * written its own (both run each frame, in an order we do not know). */
    if (tm.active && (tm.view == VIEW_LIST || tm.view == VIEW_GRID) && team_grid() &&
        psx_mod_read_word(block(0)) != SIZE_PROMPT)
        restrict_grid(tm.view == VIEW_LIST ? tm.sel : tm.tourney);
    /* The fighter chosen (its choice stored, not the empty 88). */
    unsigned choice = psx_mod_read_word(block(0) + B_CHOICES);
    if (tm.active && tm.view == VIEW_GRID && psx_mod_read_word(block(0) + B_COUNT) == 1 && choice != EMPTY &&
        (choice >> 2) < GUEST_FIRST + GUEST_MAX) {
        if (mt.flow == FLOW_BETWEEN) {
            tm.view = VIEW_PICKED; tm.version++;
            if (choice != mt.pick) tmst_log("tournament: picked 0x%x again, not 0x%x", choice, mt.pick);
            arm_match();
        } else if (start_bracket(choice >> 2, choice & 3)) {
            /* The first pick undone at once (the grid leaves two frames
             * later), back to the size prompt: the grid waits under the
             * draw, and the player's fighter is picked again after START. */
            psx_mod_write_word(block(0) + B_COUNT, 0); psx_mod_write_word(block(0) + B_CHOICES, EMPTY);
            psx_mod_write_word(block(0), SIZE_PROMPT);       /* where START does nothing (above) */
            mt.pick = choice; mt.shown = mt.fresh = 0; mt.unlocked = -1;
            mt.key = grid_key; grid_key = 0;
            tmst_log("tournament: the pick's button 0x%04x (issue #14)", mt.key);
            mt.want_now = mt.want;
            mt.match = tmst_bracket_player_match(&bracket); mt.opponent = tmst_bracket_opponent(&bracket);
            mt.flow = FLOW_BETWEEN; mt.outcome = NEXT_MATCH; mt.cover = 1; mt.confirmed = 0; mt.previous = 0;
            mt.story_done = 0; mt.scr = story_load(tm.tourney) ? SCR_STORY : SCR_DRAW;
            mt.scr_at = tm.frames; mt.step = STEP_SETTLE; mt.step_at = tm.frames; mt.tries = 0;
            tmst_log("tournament: the pick held for the draw (0x%x, %s moves)", choice, moves_name(choice >> 2, mt.want));
        } else {
            tm.view = VIEW_PICKED; tm.version++; pic_on = 0; tmst_overlay_hide();
        }
    }
    if (mt.flow == FLOW_MATCH) match_tick(screen);
    else if (mt.flow == FLOW_BETWEEN && !tm.active) between_tick(screen);
    if (mt.flow == FLOW_BETWEEN) between_watch(screen);
    if (mt.flow == FLOW_MATCH && screen == LOADING) {
        mt.loading_seen = 1;
        if (!mt.loading_at) mt.loading_at = tm.frames | 1;
        else if (!mt.loading_vs && tm.frames - mt.loading_at > 120) {     /* no VS portraits seen: shown anyway */
            mt.loading_vs = mt.loading_ok = 1; tmst_log("tournament: the loading screen's portraits not seen");
        }
    }
    /* The loading screen loads a player's portrait only when the one asked
     * for differs from the one held (0x80098106 + player): from the second
     * match on, the player's native fighter was never loaded again, and
     * what the fights had left in its place showed. Held as none from the
     * pick to the loading screen (not on it, where it would load again each
     * frame), as the Expanded does for its guests. */
    if (mt.flow == FLOW_MATCH && !mt.loading_seen && (screen == GRID || screen == FIGHT))
        for (unsigned p = 0; p < 2; p++)
            if (psx_mod_read_byte(HELD_PORTRAIT + p) != NO_PORTRAIT) psx_mod_write_byte(HELD_PORTRAIT + p, NO_PORTRAIT);
    if (screen == LOADING) {
        int good = vs_portraits_good();                   /* (outside the tournament it only fills the cache) */
        if (mt.flow == FLOW_MATCH && mt.loading_vs && !mt.loading_ok) {
            if (!mt.loading_vs_at) mt.loading_vs_at = tm.frames | 1;
            if (good) mt.loading_ok = 1;
            else if (tm.frames - mt.loading_vs_at > 90) {
                mt.loading_ok = 1; tmst_log("tournament: the loading screen's portraits not checked, shown anyway");
            }
        }
    } else { memset(vs, 0, sizeof vs); memset(vs_check, 0, sizeof vs_check); }
    static unsigned room_screen, room_gen;              /* TMST_TOURNAMENT_ROOM: the upper half's free room logged */
    if (getenv("TMST_TOURNAMENT_ROOM") && (screen != room_screen || tmst_compat_high_vram_generation() != room_gen)) {
        room_screen = screen; room_gen = tmst_compat_high_vram_generation();
        static const int sizes[4][2] = {{854, 480}, {512, 480}, {427, 240}, {256, 256}};
        for (int k = 0; k < 4; k++) {
            int free_at = -1;
            for (int y = 512; y + sizes[k][1] <= 1024 && free_at < 0; y += 256)
                for (int x = 0; x + sizes[k][0] <= 1024; x += 64)
                    if (!tmst_compat_high_vram_taken(x, y, sizes[k][0], sizes[k][1])) { free_at = x + (y << 16); break; }
            tmst_log("tournament: room: screen %u gen %u %dx%d %s (%d,%d)", screen, room_gen, sizes[k][0], sizes[k][1],
                     free_at >= 0 ? "free" : "taken", free_at & 0xffff, free_at >> 16);
        }
    }
    /* The loading screen is the game's own VS screen: in it other things
     * than the stage engine upload into the upper VRAM half (TTT Hitspark's
     * images), not a place for the picture. Black until its VS portraits are
     * drawn (the grid draws its way out in its first frames) and their
     * texels are right (vs_portraits_good). The fight lifts it whatever
     * the loading screen got to (a short one ends before its portraits are
     * checked), until its end is seen: not by life_set, which match_tick
     * sets in the fight's first frame before this (the whole fight stayed
     * black and silent, issue #5). */
    if (mt.cover && mt.flow == FLOW_MATCH && ((screen == LOADING && mt.loading_ok) ||
                                              (screen == FIGHT && mt.loading_seen && !mt.fight_over))) {   /* the grid leaves through 8 for a frame */
        if (screen == FIGHT && !mt.loading_ok) tmst_log("tournament: the fight lifts the cover (the loading screen's portraits unchecked)");
        mt.cover = 0; mt.scr = SCR_NONE;
    }
    /* What the screens need from VRAM, while the grid shows it. */
    if (tm.active && team_grid() && !art_font_ready() && art_font_capture()) write_guides();
    /* On the way out the game goes on drawing the grid until the menu has
     * loaded: covered until then (3 seconds at most). */
    if (tm.active && tm.exiting && !team_grid() && screen != MENU && !mt.cover) {
        if (!tm.left_at) tm.left_at = tm.frames;
        if (tm.frames - tm.left_at < 180) { show_picture(); return; }
    }
    if (tm.active && !team_grid()) close_screen();
    static int covered;
    /* The cover is the whole screen: nothing linked after our chain over it
     * (the Expanded's CHARACTER SELECT header showed on the progress screen). */
    tmst_overlay_cut_tail(mt.cover);
    if (mt.cover) {
        if (!covered) { tmst_overlay_hide_picture(); pic_on = 0; }
        tmst_overlay_backdrop(1, 0); tmst_overlay_show_prims(); covered = 1;
        /* The loading screen's first frames: black only (no picture there). */
        if (mt.flow == FLOW_MATCH && screen == LOADING) {
            if (pic_on) { tmst_overlay_hide_picture(); pic_on = 0; canvas_borrowed = -1; }
        } else cover_screen(screen);
    } else {
        if (covered) {                        /* on the loading screen the primitives go on: its banner */
            covered = 0; canvas_borrowed = -1; tmst_overlay_backdrop(0, 0); pic_on = 0;
            if (mt.flow == FLOW_MATCH && screen == LOADING) tmst_overlay_hide_picture(); else tmst_overlay_hide();
        }
        if (showing()) show_picture();
        /* The loading screen's VS portraits and the fight's HUD are read in
         * the visitor, which runs while something of ours is drawn. */
        static int watching_vs;
        if (mt.flow == FLOW_MATCH && (screen == LOADING || screen == FIGHT)) { tmst_overlay_show_prims(); watching_vs = 1; }
        else if (watching_vs) { watching_vs = 0; if (!showing()) tmst_overlay_hide(); }
    }
    static int bench = -1;                    /* the grid's primitives, logged every n-th frame */
    if (bench < 0) { const char *e = getenv("TMST_TOURNAMENT_PRIMLOG"); bench = e && *e ? atoi(e) : 0; }
    prim_log = bench > 0 && showing() && tm.frames % (unsigned)bench == 0;
    if (bench > 0 && screen == LOADING && tm.frames % (unsigned)bench == 0) prim_log = -1;
    if (tm.pending && screen == MENU && psx_mod_read_half(PHASE) == 1) tm.pending = 0;   /* the menu again, never left */
}

/* The music the game starts: none from the fight's end to the grid (the
 * results' piece, heard under the cover while they are skipped). */
static int music(int id) {
    if (!on || mt.flow == FLOW_NONE) return id;
    unsigned screen = psx_mod_read_word(SCREEN);
    int quiet = mt.cover && (screen == RESULTS || screen == BOOT_LOADING || screen == FIGHT);
    tmst_log("tournament: music %d on screen %u%s", id, screen, quiet ? ", not played" : "");
    return quiet ? -1 : id;
}

static void activate(void) {
    on = tmst_guard("tmst.tournament", TMST_NEED_ACTIVATION | TMST_NEED_VBLANK | TMST_NEED_GUEST_MEMORY | TMST_NEED_HIGH_VRAM);
    if (!on) return;
    menu_row = tmst_menu_add_row_after("TOURNAMENT MODE", TEAM_BATTLE, TEKKEN_FORCE, begin);
    if (menu_row < 0) tmst_log("tournament: no main-menu row (no room)");
    tmst_menu_set_colour(menu_row, 0xE8C030);                  /* old gold */
    tmst_input_add_filter(input);
    tmst_spu_add_mixer(intro_mix);
    tmst_music_add_hook(music);
    tmst_jukebox_player_on("Tournament Mode");       /* its pieces with the Jukebox off too (issue #9.2) */
    tmst_overlay_set_prims(tournament_prims);
    tmst_overlay_prepare();
    tmst_overlay_set_visitor(grid_visitor);
}

PSX_MOD_CONSTRUCTOR(tmst_register_tournament) {
    (void)psx_mod_register_activation_plugin("tmst.tournament", activate);
    (void)psx_mod_register_vblank_plugin("tmst.tournament", tick);
}
