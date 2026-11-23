/* tmst.tekken2.stages: Tekken 2 Content's Stages, Tekken 2's 23 stages drawn
 * by the Expanded's stage engine (src/tekken3_arcade_stages.c) on its Tekken 3
 * path, as Tekken 3 arcade's are (its floor: the near cells in halves, lit
 * round the fighters where a stage wants; the user's choice, 2026-11-03).
 *
 * The packs: tools/tekken2_stages.py makes them at setup from the player's
 * Tekken 2 disc (format 3, numbers 20 + Tekken 2's stage) into
 * workspace/t2-stages/classic; the build stages them beside the game as
 * mods/t2-stages/classic (addon.cmake's TMST_STAGED).
 *
 * The engine reads arena n's pack from <its folder>/stageNN.ast (n 0..12),
 * once: it keeps it while the arena stays n, and an arena whose pack it could
 * not read stays the PS1's until another arena is tried. A Tekken 2 fight is
 * always in arena 9 (Nina's: none of the engine's per-stage rules, its far
 * background's or its objects', applies to it), written until the fight
 * (with the guests' arena, tekken3_stage_select_arena, and the STAGES row's
 * decision, tekken3_stage_select_choice, saying the same). Our fopen wrapper
 * hands the engine, for stage09.ast in that fight, a copy of our pack made
 * arena 9's (the number in its header; loaded.ast beside the packs). Which
 * pack the engine holds is followed through the opens it makes; at a fight's
 * start, when the one it holds for this arena is not the one wanted (the last
 * fight was another Tekken 2 stage in arena 9, or Tekken 2's, and this one is
 * the game's own), or the arena was refused before, the arena reads 8 (or 7)
 * as the PS1's stage routine next starts (tmst_tekken2_stage_entry), until the
 * engine opens that one: it gets an empty pack (blank.ast: nothing drawn, one
 * frame), and the arena is given back, which the engine then loads again.
 *
 * Without tekken3.zip: the engine's Tekken 3 path comes up only when the
 * Arcade Stages feature (on by default) finds one of its packs at its
 * activation (packs_present: fopen of stage00..12). Until the game starts,
 * none there and ours made, the fopen wrapper answers that look with one of
 * ours (whether this part is on or not: that package activates first, ids in
 * order). The engine is then up, and its other arenas stay the PS1's (no
 * pack: refused).
 *
 * Which fights: VS and Practice, from the STAGE line (F18,
 * src/tekken3_stage_select.c), where TEKKEN 2 is a game after the last of
 * theirs (before DEFAULT), its stages RANDOM then the 23. Their line keeps
 * DEFAULT while TEKKEN 2 shows, and all is done through their own keys: our
 * input wrapper keeps some presses from them (until let go) and passes the
 * others, so that their machine moves as it would (LEFT from TEKKEN 2 to
 * their last game, their take) and only shows ours (the line wrapper). A take
 * on TEKKEN 2 arms the line; the fight that loads next in its mode (their
 * fight hook) is ours: the stage source says Tekken 3's (the TTT music and
 * stage stay out); the music is the stage's own (its owner's theme, the
 * Jukebox plays it: tmst_tekken2_fight_music), unless the player chose one
 * for the opponent. Their GAME RANDOM (any
 * stage of their games) draws from Tekken 2's 23 too, as many lots as stages:
 * the line seen on it (GAME, RANDOM) the last time it showed, their draw
 * made, one of ours taken in its place.
 *
 * The pack's own sections (the engine reads only its own): T2FX, the
 * fighters' light (SetBackColor, 0x800826E8, dimmed on the dark stages, as the
 * remake's FDIM); T2CH, Hong Kong's running lights (palettes uploaded in turn,
 * stepped as Tekken 2 steps them: a clock adding "add" a frame, a step each
 * "period"); T2BL, Hong Kong's floor fading from one palette to another and
 * back, as Tekken 2's own palette animation does it (issue #12);
 * T2MR, Mirror Darkness's mirror (src/tekken2/tmst_tekken2_mirror.c); T2AN,
 * the Remake's animations whose frames are not in the VRAM (the engine's ANIM
 * keeps its frames there; tools/t2_remake_stages.py frames_out()): each
 * period, as the PS1's stage routine starts and not in the pause, its next
 * frame uploaded to its place, as the engine's ANIM does.
 *
 * True Ogre's fight (P1's or the CPU's: the game's dark arena, 0x800AFAA0):
 * no far background, the black, and only the Classic's floor, whether
 * CLASSIC or REMAKE is chosen, lit round the fighters, its far edge into the
 * black (the user's rule, 2026-11-06). The engine keeps the PS1's floor in
 * the dark arena; our wrapper of the floor routine (0x80049848) clears that
 * byte while the engine's runs, and the engine is handed the Classic's pack
 * with its floor's kind made 1 (NIGHT, followed as a version). None of the
 * stage's own there (drawn() says no): the game's light on the fighters.
 *
 * Unknown in the fight (the loading screen's fighters; tmst_compat): a
 * "wolf_cut" stage's Remake pack for her, stageNNw.ast (its background's
 * top cut, off her wolf), the whole stageNN.ast otherwise (the user's,
 * 2026-11-08). The pack held follows her: Team Battle's next member, or
 * Survival's next opponent on the same arena, has the engine read the pack
 * again (fight_start(): the switch, as for another stage).
 *
 * GAME OPTION's TEKKEN 2 STAGES row: CLASSIC, REMAKE. REMAKE draws the
 * Classic's packs while mods/t2-stages/remake has none (the user's choice,
 * 2026-11-03). The choices are kept beside the memory card in t2-stages.txt
 * ("vs on 13", "practice off random", "version classic").
 *
 * Tournament Mode (src/tournament/tmst_tournament.c, its matches only): the
 * CPU's stage (tmst_tournament_fight_stage). Tekken 1's and 2's tournaments
 * a Tekken 2 stage, drawn as the STAGE line's (this part on, else the
 * system's settings); Tekken 3's an arena, Tekken Tag's a TTT stage (Tekken
 * 3's arena of that number without TTT's packs), whether this part is on or
 * not: these wrappers are the only ones, so the decision is said here, its
 * arena held until the fight by the tournament's VBlank
 * (tmst_stage_forced_arena; ours runs only with this part on).
 *
 * Benches: TMST_T2_STAGE=N gives every fight Tekken 2's stage N (0..22)
 * (with TMST_T2_LINE as the STAGE line's: its music ours, issue #9.2),
 * TMST_T2_STAGES_DIR the folder holding classic/ and remake/. */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_tournament_stages.h"
#include "tmst_music.h"
#include "cpu_state.h"
#include "mod_plugins.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef TMST_HAS_STAGE_ENGINE
#include "tekken3_stage_source.h"
#include "memcard.h"
#include "psx_sdl.h"
#ifdef TMST_HAS_GAME_OPTIONS
#include "tekken3_game_options.h"
#endif

enum { STAGES = 23, NUMBER0 = 20 };
static const char *const names[STAGES] = {
    "GRAND CANYON", "SZECHWAN FIELDS", "MORNING FIELDS", "NEW YORK CITY", "CHURCH SANCTUARY",
    "HONG KONG ROOFTOP", "PAGODA TEMPLE", "SERENE FOREST", "AKIHABARA", "THE GREAT WALL",
    "CAMBODIAN WETLANDS", "ATHENS", "NOWHERE DESERT", "LAUNCH PAD", "HILLS", "KYOTO AT SUNSET",
    "TAJ MAHAL", "BEACH", "LONDON RUINS", "ARIZONA DESERT", "KOREAN TEMPLE", "ETERNAL DARKNESS",
    "MIRROR DARKNESS" };

enum { SCREEN = 0x800ae204, MENU = 4, OPTIONS = 5, FIGHT = 8, MODE = 0x800afa88, ARENA = 0x800adc84,
       ARENA_COPY = 0x800afaa2, NO_STAGE = 0x800adca4, LAST_ARENA = 12, T2_ARENA = 9,
       PAUSED = 0x80095494,     /* 1 while the pause menu is open (the engine's anim_tick reads it) */
       DARK_ARENA = 0x800afaa0, /* True Ogre's fight (0x80050F5C): the dark arena */
       NIGHT_KIND = 1,          /* the dark arena's floor: FLOR kind 1, lit round each fighter */
       VS = 1, PRACTICE = 5,
       /* TEKKEN 2 STAGES' lamps: Arcade to Practice, where the STAGES row
        * (TEKKEN 2, RANDOM) brings Tekken 2's stages too (issue #11) */
       ROW_MODES = 0x3f };
/* Pad bits, active low. */
enum { PAD_SELECT = 0x0001, PAD_START = 0x0008, PAD_UP = 0x0010, PAD_RIGHT = 0x0020, PAD_DOWN = 0x0040,
       PAD_LEFT = 0x0080, PAD_CIRCLE = 0x2000, PAD_CROSS = 0x4000 };
enum { LINE_VS, LINE_PRACTICE, LINES };
static const char *const line_words[LINES] = { "vs", "practice" };
/* NIGHT: not a choice, what the engine holds in the dark arena (the Classic's
 * pack, its floor lit round the fighters) */
enum { CLASSIC, REMAKE, VERSIONS, NIGHT = VERSIONS };
static const char *const version_words[VERSIONS] = { "classic", "remake" };
static const char *const held_words[VERSIONS + 1] = { "classic", "remake", "night" };
static const char *const version_labels[VERSIONS] = { "CLASSIC", "REMAKE" };
/* What the engine holds for an arena: the game's own pack, the empty one, or
 * Tekken 2's stage n (0..22). */
enum { HELD_NONE = -3, HELD_BLANK = -2, HELD_REAL = -1 };
enum { NO_DIM = 0x80, CHASE_MAX = 8, CHASE_FRAMES = 8, BLEND_MAX = 64, ANIMS_MAX = 32 };   /* ANIMS_MAX: Eternal Darkness 15 (its lights in short runs) */
static const double TAU = 6.283185307179586;

int __real_tekken3_stage_source(void);
int __real_tekken3_stage_ttt_stage(void);
void __real_tekken3_stage_select_fight(unsigned mode);
int __real_tekken3_stage_select_choice(unsigned mode, int *source, int *ttt_stage, int *arena);
int __real_tekken3_stage_select_arena(void);
int __real_tekken3_stage_select_music(void);
uint16_t __real_tekken3_stage_select_input(int player, uint16_t buttons);
int __real_tekken3_stage_select_line(const char **label, const char **name, int *arrows);
/* fopen is chained (addon.cmake): the Expanded's Android runtime wraps it
 * (android_apk_file.c, the files kept in the APK); elsewhere the bridge is the
 * C library's. */
FILE *__tmst_chain_fopen(const char *path, const char *mode);
#ifdef TMST_BRIDGE_fopen
FILE *__real_fopen(const char *path, const char *mode);
FILE *__tmst_chain_fopen(const char *path, const char *mode) { return __real_fopen(path, mode); }
#endif

static int stages;                       /* the feature, at activation */
static int t2[LINES];                    /* TEKKEN 2 on the line (theirs on DEFAULT) */
static int chosen[LINES] = { -1, -1 };   /* its stage, -1 RANDOM */
static int on_stage[LINES];              /* its STAGE step */
static int armed[LINES];                 /* taken on TEKKEN 2: the next fight of that mode */
static int their_random[LINES];          /* their GAME RANDOM, when the line last showed */
/* Their stages, for RANDOM's lots: Tekken 3's 24, TTT's 15 with its packs
 * (src/tekken3_stage_select.c's catalogue). */
enum { THEIR_T3 = 24, THEIR_TTT = 15 };
static int version = CLASSIC;
static int fight = -1, fight_mode = -1, moving;   /* the fight's stage, its mode; arena 9 written */
static int line_fight;                             /* the fight's stage the STAGE line's (its music ours) */
static uint64_t draw = 0x9e3779b97f4a7c15u;
/* the engine's pack: the arena it holds one for, what it is (of Tekken 2's,
 * which version: CLASSIC or REMAKE, switched in GAME OPTION between two
 * fights on the same stage); the arena it last tried; a switch under way
 * (the arena read meanwhile, the fight's) */
static int held_arena = -1, held = HELD_NONE, held_version = CLASSIC, tried = -1;
/* The Remake pack held is a "wolf_cut" stage's for Unknown (stageNNw.ast);
 * fight_extras() read that one. */
static int held_wolf, extras_wolf;
static void fight_extras(void);
static int switch_want = -1, switch_to = -1, switch_back = -1, switch_frames, checked;
static int dark_call;                    /* in the engine's floor, the dark arena's byte cleared meanwhile */
/* src/tekken2/tmst_tekken2_extras.c: the Remake's extras (ANI2..4, DUST,
 * FFLY, FTHR, RAIN, DIYA) */
void tmst_tekken2_extras_load(const uint8_t *pack, long size);
void tmst_tekken2_extras_clear(void);
void tmst_tekken2_extras_tick(void);
int tmst_tekken2_extras_on(void);
int tmst_tekken2_extras_light(uint32_t *rgb, int *left, int *frames);
/* the fight's pack's own: the fighters' light (FDIM's tint, flicker), the
 * running lights */
static unsigned dim = NO_DIM;
static uint8_t tint[3], flicker;
/* T2LT (the Remake's): the fighters' light of a Tekken 3 arena instead of
 * arena 9's. Tekken 3 lights its fighters from 28 bytes an arena
 * (0x80096C78 + 28 * arena; the Expanded's B76, src/tekken3_arcade_stages.c):
 * the back colour at +0 (SetBackColor, each frame), the light's colour and
 * angles at +8..+15 (0x80039960, each round's set-up), colours at +4, +16,
 * +20 (the stage's load). Our fights are arena 9's, whose light is dim
 * (back 0x640, the others' 0x898) and orange: on a stage in daylight its
 * fighters looked dark (the user's report, 2026-11-05). From the fight's
 * choice (before its load) to its end, arena 9's first 24 bytes are those
 * of the arena T2LT names, put back after, as the Expanded does for a TTT
 * stage's LGHT. */
enum { LIGHT_TABLE = 0x80096c78, LIGHT_ENTRY = 28, LIGHT_BYTES = 24 };
static int light_from = -1, light_stood;
static uint8_t light_saved[LIGHT_BYTES];
/* FARB's bottom colour (its 12 colour bytes after 6 halfwords: cell rgb, top
 * rgb, top on, bottom rgb, bottom on): the engine's Tekken 3 path draws it
 * under the panorama only to 75 % of the screen (the TTT path has its
 * below_band), and Tekken 3 never clears the screen. Where the floor's far
 * edge lies lower (the camera's angle, a wide screen), a band of nothing
 * showed: an older frame in the fight, black in the pause (the user's
 * screenshot, Arizona, Practice, 2026-11-05). Filled with it under all of
 * the stage (src/tekken2/tmst_tekken2_mirror.c ground_fill). */
static uint8_t ground[3], ground_on;
/* FIXW (the Remake's Church): its scenery fixed to the floor, the floor's
 * period (src/tekken2/tmst_tekken2_anchor.c); 0: it follows. */
static int32_t fixed_period;
/* COLL (the Church's): its scenery as walls; the shapes in a copy
 * (src/tekken2/tmst_tekken2_anchor.c collide). */
static int16_t *coll_data;
static int coll_n, coll_radius;
static int32_t mirror_at[6];             /* T2MR: radius, angle, width, top, bottom, panels (0: none) */
static uint8_t mirror_style[20];         /* the Remake's MIRR after its place: the frame, the glow (0: none) */
static int mirror_styled;
static struct {
    unsigned period, add, frames, count, frame, clock;
    uint16_t at[CHASE_MAX][2];
    uint16_t colours[CHASE_FRAMES][CHASE_MAX][16];
} chase;
/* T2BL (issue #12): Tekken 2's palette blend (its program's 0x80082424, the
 * table at 0x800A3010: Hong Kong's 0x7B04 towards 0x7B09): each frame
 * f = ((cos(clock) * 4096 + 4096) / 2) / 4 (0..1024, the clock 4096 a turn),
 * then clock += add; the colours from..to of each palette A + (((B - A) * f
 * + 512) >> 10), 5 bits a channel. */
static struct {
    unsigned count, from, to, add, clock;
    uint16_t at[BLEND_MAX][2];
    uint16_t a[BLEND_MAX][16], b[BLEND_MAX][16];
} blend;
/* T2AN: the place (halfwords), the period, the entries (each a frame), the
 * frames' pixels; in the section's copy (anim_data) */
static uint8_t *anim_data;
static unsigned anims;
static struct {
    uint16_t x, y, w, h, period, entries, frames;
    const uint16_t *order, *pixels;
    unsigned clock, at;
} anim[ANIMS_MAX];

/* ---- the files ---- */

static const char *base_dir(void) {
    static char dir[1024];
    if (!dir[0]) {
        const char *base = SDL_GetBasePath();
        snprintf(dir, sizeof dir, "%s", base ? base : "");
#if !defined(PSX_SDL3)
        SDL_free((void *)base);
#endif
    }
    return dir;
}
/* The engine's folder of Tekken 3's packs, as it makes it (pack_dir_of). */
static const char *t3_dir(void) {
    static char dir[1024];
    if (!dir[0]) {
        const char *e = getenv("TEKKEN3_ARCADE_STAGES_DIR");
        if (e && *e) snprintf(dir, sizeof dir, "%s", e);
        else snprintf(dir, sizeof dir, "%smods/arcade-stages", base_dir());
    }
    return dir;
}
/* Our folder (classic/, remake/, the copies handed over). */
static void our_path(const char *name, char *out, size_t size) {
    const char *e = getenv("TMST_T2_STAGES_DIR");
    if (e && *e) snprintf(out, size, "%s/%s", e, name);
    else snprintf(out, size, "%smods/t2-stages/%s", base_dir(), name);
}
static void pack_path(int version_dir, int stage, char *out, size_t size) {
    char name[64];
    snprintf(name, sizeof name, "%s/stage%02d.ast", version_words[version_dir], stage);
    our_path(name, out, size);
}
#ifdef __SWITCH__
extern const unsigned char *switch_cached_file_get(const char *path, size_t *out_size);
#endif
static int there(const char *path) {
#ifdef __SWITCH__
    size_t sz = 0;
    return switch_cached_file_get(path, &sz) != NULL;
#else
    FILE *f = __tmst_chain_fopen(path, "rb");
    if (f) fclose(f);
    return f != NULL;
#endif
}
/* Any pack of that version (cached). */
static int version_made(int v) {
    static signed char made[VERSIONS];
    if (!made[v]) {
        char path[1100];
        made[v] = -1;
        for (int s = 0; s < STAGES && made[v] < 0; s++) { pack_path(v, s, path, sizeof path); if (there(path)) made[v] = 1; }
    }
    return made[v] > 0;
}
/* The version drawn: REMAKE's packs only once they exist. */
static int drawn_version(void) { return version == REMAKE && version_made(REMAKE) ? REMAKE : CLASSIC; }
/* Tekken 3's own packs (tekken3.zip), any of the 13 (cached). */
static int t3_real(void) {
    static signed char real;
    if (!real) {
        char path[1100];
        real = -1;
        for (int s = 0; s <= LAST_ARENA && real < 0; s++) {
            snprintf(path, sizeof path, "%s/stage%02d.ast", t3_dir(), s);
            if (there(path)) real = 1;
        }
    }
    return real > 0;
}

/* A pack's file, read whole (NULL: none); its size in *size. */
static uint8_t *read_all(const char *path, long *size) {
#ifdef __SWITCH__
    size_t sz = 0;
    const unsigned char *cached = switch_cached_file_get(path, &sz);
    if (!cached || sz <= 16) return NULL;
    uint8_t *b = malloc(sz);
    if (!b) return NULL;
    memcpy(b, cached, sz);
    *size = (long)sz;
    return b;
#else
    FILE *f = __tmst_chain_fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = *size > 16 ? malloc((size_t)*size) : NULL;
    if (b && fread(b, 1, (size_t)*size, f) != (size_t)*size) { free(b); b = NULL; }
    fclose(f);
    return b;
#endif
}
/* A format 3 pack's section (NULL: none). */
static const uint8_t *section(const uint8_t *b, long size, const char *id, uint32_t *len) {
    uint32_t n; memcpy(&n, b + 8, 4);
    for (uint32_t i = 0; i < n && 16 + 12 * (long)i + 12 <= size; i++) {
        if (memcmp(b + 16 + 12 * i, id, 4)) continue;
        uint32_t o, l; memcpy(&o, b + 20 + 12 * i, 4); memcpy(&l, b + 24 + 12 * i, 4);
        if ((long)o + (long)l > size) return NULL;
        *len = l;
        return b + o;
    }
    return NULL;
}

/* A "wolf_cut" stage's Remake pack for Unknown's fights (stageNNw.ast: its
 * background's top cut, off her wolf; tools/t2_remake_stages.py wolf_name()),
 * when she is one of the fight's fighters and the stage has one: 1, its path
 * into out (out may be NULL). The whole one otherwise. */
static int wolf_pack(int stage, char *out, size_t size) {
    if (stage < 0 || !tmst_compat_unknown_fights()) return 0;
    char name[64], path[1100];
    snprintf(name, sizeof name, "%s/stage%02dw.ast", version_words[REMAKE], stage);
    our_path(name, path, sizeof path);
    if (!there(path)) return 0;
    if (out) snprintf(out, size, "%s", path);
    return 1;
}

/* Our pack of Tekken 2's stage as arena n's (loaded.ast): the engine checks
 * that the header's number is the arena it asked for. NIGHT: the Classic's,
 * its floor (FLOR's first halfword, the kind) lit round each fighter, as the
 * PS1's dark arena lights its own; nothing else of it is drawn there (the
 * PS1's stage routine, after which the engine draws the far background,
 * never runs in the dark arena: its black TILE stays). */
static FILE *hand_over(int stage, int n, int held_as, const char *mode) {
    char from[1100], to[1100];
    long size = 0;
    pack_path(held_as == NIGHT ? CLASSIC : held_as, stage, from, sizeof from);
    int wolf = held_as == REMAKE && wolf_pack(stage, from, sizeof from);
    uint8_t *b = read_all(from, &size);
    if (!b) return NULL;
    uint16_t number = (uint16_t)n;
    memcpy(b + 6, &number, 2);
    uint32_t len = 0;
    uint8_t *flor = held_as == NIGHT ? (uint8_t *)section(b, size, "FLOR", &len) : NULL;
    if (flor && len >= 8) { int16_t kind = NIGHT_KIND; memcpy(flor, &kind, 2); }
    our_path("loaded.ast", to, sizeof to);
    FILE *f = __tmst_chain_fopen(to, "wb");
    int ok = f && fwrite(b, 1, (size_t)size, f) == (size_t)size;
    if (f) fclose(f);
    free(b);
    if (!ok) return NULL;
    held_wolf = wolf;
    if (stage == fight && wolf != extras_wolf) fight_extras();      /* Unknown come in (or gone) since */
    if (wolf) tmst_log("tekken2: Unknown in the fight: %s's pack for her (its background's top cut)", names[stage]);
    return __tmst_chain_fopen(to, mode);
}

/* An empty pack for arena n (blank.ast): nothing to draw (no far background
 * cells or bands, no floor quads), its tables computed. */
static FILE *blank(int n, const char *mode) {
    enum { VRAM_B = 4, TILE_B = 8, FARB_B = 24, FLOR_B = 8 + 800 + 3200 + 100 + 4 + 100 + 2 * 4 * 4 * (1 + 2 + 4),
           TRIG_B = 2 * (4096 + 4096 + 192) + 1024 + 2 * 192, HEAD = 16 + 5 * 12 };
    static const char ids[5][5] = { "VRAM", "TILE", "FARB", "FLOR", "TRIG" };
    static const uint32_t sizes[5] = { VRAM_B, TILE_B, FARB_B, FLOR_B, TRIG_B };
    static uint8_t b[HEAD + VRAM_B + TILE_B + FARB_B + FLOR_B + TRIG_B];
    uint32_t total = sizeof b, count = 5, at = HEAD, tile = 800;
    uint16_t version3 = 3, number = (uint16_t)n;
    memset(b, 0, sizeof b);
    memcpy(b, "ASTG", 4); memcpy(b + 4, &version3, 2); memcpy(b + 6, &number, 2);
    memcpy(b + 8, &count, 4); memcpy(b + 12, &total, 4);
    for (int i = 0; i < 5; i++) {
        memcpy(b + 16 + 12 * i, ids[i], 4); memcpy(b + 20 + 12 * i, &at, 4); memcpy(b + 24 + 12 * i, &sizes[i], 4);
        if (i == 1) memcpy(b + at, &tile, 4);                     /* TILE: its size, no tiles */
        if (i == 2) {                                             /* FARB: 1 column, no rows, no bands */
            const int16_t h[6] = { 1, 0, 1, 0, 45, 0 };
            memcpy(b + at, h, sizeof h);
        }
        if (i == 4) {                                             /* TRIG */
            int16_t *t = (int16_t *)(b + at);
            for (int a = 0; a < 4096; a++) {
                t[a] = (int16_t)lround(4096 * sin(TAU * a / 4096));
                t[4096 + a] = (int16_t)lround(4096 * cos(TAU * a / 4096));
            }
            for (int k = 0; k < 192; k++) t[8192 + k] = (int16_t)(sqrt(64.0 + k) * 512);
            uint8_t *fog = b + at + 2 * (8192 + 192);
            for (int k = 0; k < 1024; k++) fog[k] = (uint8_t)lround(127 * pow(k / 1024.0, 1.5));
            memcpy(fog + 1024, t + 8192, 2 * 192);
        }
        at += sizes[i];
    }
    char path[1100];
    our_path("blank.ast", path, sizeof path);
    FILE *f = __tmst_chain_fopen(path, "wb");
    int ok = f && fwrite(b, 1, sizeof b, f) == sizeof b;
    if (f) fclose(f);
    return ok ? __tmst_chain_fopen(path, mode) : NULL;
}

/* The number of a Tekken 3 pack the engine opens (<its folder>/stageNN.ast),
 * -1 for any other file. */
static int t3_pack(const char *path) {
    size_t n = strlen(path);
    if (n < 12) return -1;
    const char *b = path + n - 11;                   /* stageNN.ast */
    if (b[-1] != '/' && b[-1] != '\\') return -1;
    if (memcmp(b, "stage", 5) || b[5] < '0' || b[5] > '9' || b[6] < '0' || b[6] > '9' || strcmp(b + 7, ".ast"))
        return -1;
    const char *dir = t3_dir();
    size_t d = strlen(dir);
    if ((size_t)(b - 1 - path) != d || memcmp(path, dir, d)) return -1;
    return (b[5] - '0') * 10 + (b[6] - '0');
}

static int our_fight(void);
static int held_wanted(void);

int tmst_tekken2_stage_fopen_needed(int source, int stage) {
    (void)source;
    if (!stages) return 0;
    if (stage == switch_to) return 1;
    if (stage == T2_ARENA && (our_fight() || held_arena == T2_ARENA)) return 1;
    return 0;
}

FILE *__wrap_fopen(const char *path, const char *mode) {
    int n = path && mode && mode[0] == 'r' ? t3_pack(path) : -1;
    if (n < 0) return __tmst_chain_fopen(path, mode);
    char ours[1100];
    if (!psx_mod_game_started()) {
        /* Arcade Stages' activation looking for its packs: one of ours, when it has none */
        FILE *f = __tmst_chain_fopen(path, mode);
        if (f || n > LAST_ARENA || t3_real()) return f;
        for (int s = 0; s < STAGES && !f; s++) { pack_path(CLASSIC, s, ours, sizeof ours); f = __tmst_chain_fopen(ours, mode); }
        if (f) tmst_log("tekken2: no Tekken 3 arcade stage packs, ours bring up the stage engine");
        return f;
    }
    FILE *f;
    int what;
    tried = n;
    if (n == switch_to) {                            /* the switch: the empty pack, the arena given back */
        f = blank(n, mode); what = HELD_BLANK;
        psx_mod_write_half(ARENA, (uint16_t)switch_back);
        switch_to = -1;
    } else if (n == T2_ARENA && our_fight()) {
        int as = held_wanted();
        f = hand_over(fight, n, as, mode); what = fight;
        if (f) held_version = as;
        if (f && as == NIGHT) tmst_log("tekken2: True Ogre's dark arena: %s's Classic floor at night", names[fight]);
    } else {
        f = __tmst_chain_fopen(path, mode); what = HELD_REAL;
    }
    if (f) { held_arena = n; held = what; }
    return f;
}

/* Kept beside the memory card, as the Expanded's stage-select.txt. */
static void file_path(const char *file, char *out, size_t size) {
    const char *card = NULL; out[0] = 0;
    if (memcard_debug_info(0, &card, NULL, NULL, NULL) || !card || !*card) return;
    const char *slash = strrchr(card, '/'), *back = strrchr(card, '\\');
    if (back && (!slash || back > slash)) slash = back;
    int n = slash ? (int)(slash - card) : 1;
    snprintf(out, size, "%.*s/%s", n, slash ? card : ".", file);
}
static void setting_path(char *out, size_t size) { file_path("t2-stages.txt", out, size); }
/* GAME OPTION's STAGES = TEKKEN 2 (issue #11): ours, or theirs (the Expanded's
 * value, kept in its ttt-stages.txt) */
enum { OPT_THEIRS, OPT_TEKKEN2 };
static const char *const option_words[2] = { "theirs", "tekken2" };
static int option = OPT_THEIRS, option_forced;     /* option_forced: TMST_STAGES */
static void save(void) {
    char path[4096]; setting_path(path, sizeof path);
    FILE *f = path[0] ? fopen(path, "wb") : NULL;
    if (!f) return;
    for (int i = 0; i < LINES; i++) {
        char stage[8] = "random";
        if (chosen[i] >= 0) snprintf(stage, sizeof stage, "%d", chosen[i]);
        fprintf(f, "%s %s %s\n", line_words[i], t2[i] ? "on" : "off", stage);
    }
    fprintf(f, "version %s\n", version_words[version]);
    fprintf(f, "stages %s\n", option_words[option]);
    fclose(f);
}
static void load(void) {
    static int done;
    if (done) return;
    done = 1;
    draw ^= (uint64_t)time(NULL) * 1000003u + (uint64_t)clock();
    const char *e = getenv("TMST_STAGES");
    if (e && *e) { option = !strcmp(e, "tekken2") ? OPT_TEKKEN2 : OPT_THEIRS; option_forced = 1; }
    char path[4096]; setting_path(path, sizeof path);
    FILE *f = path[0] ? fopen(path, "rb") : NULL;
    if (!f) return;
    char key[16], a[16], b[16], line[64];
    while (fgets(line, sizeof line, f)) {
        int n = sscanf(line, "%15s %15s %15s", key, a, b);
        if (n == 2 && !strcmp(key, "version")) version = !strcmp(a, "remake") ? REMAKE : CLASSIC;
        if (n == 2 && !strcmp(key, "stages") && !option_forced) option = !strcmp(a, "tekken2") ? OPT_TEKKEN2 : OPT_THEIRS;
        for (int i = 0; n == 3 && i < LINES; i++) {
            if (strcmp(key, line_words[i])) continue;
            t2[i] = !strcmp(a, "on");
            int s = atoi(b);
            chosen[i] = !strcmp(b, "random") || s < 0 || s >= STAGES ? -1 : s;
        }
    }
    fclose(f);
}

/* This part can draw: on, its packs there, the engine's Tekken 3 path up. */
static int ready(void) {
    if (!stages || !tmst_tekken2_on() || !tekken3_arcade_stages_source_ready(TEKKEN3_STAGE_T3)) return 0;
    if (!version_made(CLASSIC)) return 0;
    load();
    return 1;
}

static int pick(int count) {
    draw ^= draw << 13; draw ^= draw >> 7; draw ^= draw << 17;
    return (int)(draw % (uint64_t)count);
}
static const char *stage_name(int s) { return s < 0 ? "RANDOM" : names[s]; }

/* The fight's pack's own sections: the fighters' light, the running lights. */
static void fight_extras(void) {
    char path[1100];
    long size = 0;
    dim = NO_DIM;
    memset(tint, 0, sizeof tint);
    flicker = 0;
    light_from = -1;
    ground_on = 0;
    fixed_period = 0;
    free(coll_data);
    coll_data = NULL;
    coll_n = coll_radius = 0;
    memset(&chase, 0, sizeof chase);
    memset(&blend, 0, sizeof blend);
    memset(mirror_at, 0, sizeof mirror_at);
    free(anim_data);
    anim_data = NULL;
    anims = 0;
    tmst_tekken2_extras_clear();
    pack_path(drawn_version(), fight, path, sizeof path);
    /* the pack the engine gets (hand_over()): its animations' places are its own */
    extras_wolf = drawn_version() == REMAKE && wolf_pack(fight, path, sizeof path);
    uint8_t *b = read_all(path, &size);
    if (!b) return;
    uint32_t len = 0;
    const uint8_t *o = section(b, size, "T2FX", &len);
    if (o && len >= 2) { uint16_t v; memcpy(&v, o, 2); dim = v ? v : NO_DIM; }
    /* FDIM (the Remake's): after its dim (T2FX's), r, g, b / 0x80 and a flicker's depth */
    o = section(b, size, "FDIM", &len);
    if (o && len >= 8) { memcpy(tint, o + 4, 3); flicker = o[7]; }
    o = section(b, size, "FARB", &len);
    if (o && len >= 24 && o[22]) { memcpy(ground, o + 19, 3); ground_on = 1; }
    o = section(b, size, "FIXW", &len);
    if (o && len >= 4) { memcpy(&fixed_period, o, 4); if (fixed_period < 0) fixed_period = 0; }
    o = section(b, size, "COLL", &len);
    if (o && len >= 4) {
        int16_t c[2]; memcpy(c, o, 4);
        if (c[0] > 0 && len >= 4u + 12u * (unsigned)c[0] && (coll_data = malloc(12u * (unsigned)c[0])) != NULL) {
            memcpy(coll_data, o + 4, 12u * (unsigned)c[0]);
            coll_n = c[0]; coll_radius = c[1];
        }
    }
    o = section(b, size, "T2LT", &len);
    if (o && len >= 2) {
        uint16_t a; memcpy(&a, o, 2);
        if (a <= 17 && a != T2_ARENA) light_from = a;
    }
    tmst_tekken2_extras_load(b, size);
    o = section(b, size, "T2MR", &len);
    if (o && len >= sizeof mirror_at) memcpy(mirror_at, o, sizeof mirror_at);
    /* MIRR (the old fork's, the Remake's): its style after the place, 20 bytes */
    memset(mirror_style, 0, sizeof mirror_style);
    mirror_styled = 0;
    o = section(b, size, "MIRR", &len);
    if (o && len >= 24 + 17) { memcpy(mirror_style, o + 24, len - 24 < 20 ? len - 24 : 20); mirror_styled = 1; }
    o = section(b, size, "T2CH", &len);
    if (o && len >= 8) {
        uint16_t h[4]; memcpy(h, o, 8);
        if (h[1] && h[1] <= CHASE_FRAMES && h[2] && h[2] <= CHASE_MAX && len >= 8 + 4u * h[2] + 32u * h[1] * h[2]) {
            /* h[3]: the clock's step (0, the packs before issue #12: 1) */
            chase.period = h[0] ? h[0] : 1; chase.add = h[3] ? h[3] : 1; chase.frames = h[1]; chase.count = h[2];
            memcpy(chase.at, o + 8, 4u * h[2]);
            for (unsigned f = 0; f < h[1]; f++)
                for (unsigned p = 0; p < h[2]; p++)
                    memcpy(chase.colours[f][p], o + 8 + 4u * h[2] + 32u * (f * h[2] + p), 32);
        }
    }
    /* T2BL: u16 palettes, from, to, add; each palette (u16 x, y), A[16], B[16] */
    o = section(b, size, "T2BL", &len);
    if (o && len >= 8) {
        uint16_t h[4]; memcpy(h, o, 8);
        if (h[0] && h[0] <= BLEND_MAX && h[1] <= h[2] && h[2] < 16 && len >= 8 + 68u * h[0]) {
            blend.count = h[0]; blend.from = h[1]; blend.to = h[2]; blend.add = h[3];
            for (unsigned p = 0; p < h[0]; p++) {
                memcpy(blend.at[p], o + 8 + 68u * p, 4);
                memcpy(blend.a[p], o + 12 + 68u * p, 32);
                memcpy(blend.b[p], o + 44 + 68u * p, 32);
            }
        }
    }
    o = section(b, size, "T2AN", &len);
    if (o && len >= 4 && (anim_data = malloc(len)) != NULL) {
        memcpy(anim_data, o, len);
        uint32_t n, at = 4; memcpy(&n, anim_data, 4);
        for (uint32_t i = 0; i < n && anims < ANIMS_MAX && at + 20 <= len; i++) {
            uint16_t h[8]; memcpy(h, anim_data + at + 4, 16);
            uint32_t order = at + 20, pixels = order + ((2u * h[5] + 3u) & ~3u);
            uint32_t end = pixels + 2u * h[2] * h[3] * h[6];
            if (!h[2] || !h[3] || !h[5] || !h[6] || end > len) break;
            int good = 1;
            for (unsigned e = 0; e < h[5]; e++) {
                uint16_t f; memcpy(&f, anim_data + order + 2 * e, 2);
                if (f >= h[6]) good = 0;
            }
            if (good) {
                anim[anims].x = h[0]; anim[anims].y = h[1]; anim[anims].w = h[2]; anim[anims].h = h[3];
                anim[anims].period = h[4] ? h[4] : 1; anim[anims].entries = h[5]; anim[anims].frames = h[6];
                anim[anims].order = (const uint16_t *)(anim_data + order);
                anim[anims].pixels = (const uint16_t *)(anim_data + pixels);
                anim[anims].clock = anim[anims].at = 0;
                anims++;
            }
            at = end;
        }
    }
    free(b);
}

/* ---- the engine's questions ---- */

/* Our fight: its mode still, in arena 9. */
static int our_fight(void) {
    return fight >= 0 && psx_mod_read_word(MODE) == (uint32_t)fight_mode && psx_mod_read_half(ARENA) == T2_ARENA;
}
/* True Ogre's fight (P1's or the CPU's, 0x80050F5C): the dark arena, its
 * byte cleared only while the engine draws our floor there (dark_floor). */
static int dark(void) { return dark_call || psx_mod_read_byte(DARK_ARENA); }
/* What the engine is to hold for our fight: NIGHT in the dark arena (the
 * Classic's floor whichever version is chosen, the user's rule, 2026-11-06),
 * else the version drawn. */
static int held_wanted(void) { return dark() ? NIGHT : drawn_version(); }
/* Our pack drawn now: our fight, the engine holding it. Not in the dark
 * arena: there only its floor, none of the stage's own (the fighters' light,
 * the mirror, the extras, the running lights, the walls). */
static int drawn(void) {
    return our_fight() && !dark() && held_arena == T2_ARENA && held == fight && held_version == drawn_version() &&
           !psx_mod_read_byte(NO_STAGE) &&
           psx_mod_read_word(SCREEN) == FIGHT;
}
/* For src/tekken2/tmst_tekken2_mirror.c: the mirror's place while our pack
 * with one is drawn. */
int tmst_tekken2_mirror(int32_t at[6]) {
    if (mirror_at[0] <= 0 || !drawn()) return 0;
    memcpy(at, mirror_at, sizeof mirror_at);
    return 1;
}
/* ... and its style (the Remake's MIRR: frame, rails, glow), 0: none. */
int tmst_tekken2_mirror_style(uint8_t style[20]) {
    if (!mirror_styled) return 0;
    memcpy(style, mirror_style, sizeof mirror_style);
    return 1;
}
/* ... and whether its extras are to be drawn this frame. */
int tmst_tekken2_extras_wanted(void) { return tmst_tekken2_extras_on() && drawn(); }
/* ... whether our pack is drawn now (src/tekken2/tmst_tekken2_trace.c). */
int tmst_tekken2_drawn(void) { return drawn(); }
/* ... a Remake pack, drawn now (src/tekken2/tmst_tekken2_anchor.c). */
int tmst_tekken2_remake_drawn(void) { return drawn() && held_version == REMAKE; }
int32_t tmst_tekken2_fixed_period(void) { return fixed_period; }
/* ... and its walls: the shapes (6 halfwords each), their count, the fighters' radius (cm). */
const int16_t *tmst_tekken2_walls(int *n, int *radius) {
    if (dark()) { *n = *radius = 0; return NULL; }
    *n = coll_n; *radius = coll_radius; return coll_data;
}
/* ... and the colour under all of its stage (FARB's bottom colour), 0: none. */
int tmst_tekken2_ground(uint8_t rgb[3]) {
    if (!ground_on || !drawn()) return 0;
    memcpy(rgb, ground, 3);
    return 1;
}
int __wrap_tekken3_stage_source(void) { return our_fight() ? TEKKEN3_STAGE_T3 : __real_tekken3_stage_source(); }
int __wrap_tekken3_stage_ttt_stage(void) { return our_fight() ? -1 : __real_tekken3_stage_ttt_stage(); }

/* A tournament's Tekken 3 arena or TTT stage for this fight (its mode, -1
 * none): the source, the TTT stage (-1), the arena loaded; held until the
 * fight. */
enum { TTT_UNKNOWN = 14, TTT_EDDY_A = 13, TTT_LAST_ARENA = 12, OGRE_ARENA = 6, EDDY_ARENA = 10 };
static int forced_mode = -1, forced_source, forced_ttt, forced_arena, forced_held;
static int option_mode = -1;             /* the fight's stage GAME OPTION's STAGES row's (TEKKEN 2, RANDOM's lot): its mode */
static void force(unsigned mode, int kind, int stage, const char *whose) {
    int ttt = kind == TMST_TSTAGE_TTT && tekken3_arcade_stages_source_ready(TEKKEN3_STAGE_TTT);
    forced_mode = (int)mode; forced_held = 1;
    forced_source = ttt ? TEKKEN3_STAGE_TTT : TEKKEN3_STAGE_T3;
    forced_ttt = ttt ? stage : -1;
    /* TTT's Unknown stage is drawn in Ogre's arena, Eddy-A in Eddy's (the
     * Expanded's arena_of_stage) */
    forced_arena = ttt && stage > TTT_LAST_ARENA
        ? (stage == TTT_UNKNOWN ? OGRE_ARENA : stage == TTT_EDDY_A ? EDDY_ARENA : stage) : stage;
    tmst_log("tekken2: %s fight on %s %d (arena %d)%s", whose, ttt ? "TTT's stage" : "Tekken 3's arena",
             ttt ? stage : forced_arena, forced_arena,
             kind == TMST_TSTAGE_TTT && !ttt ? ", no TTT stage packs: Tekken 3's" : "");
}
/* For the tournament's VBlank: the arena to hold until the fight, -1 none. */
int tmst_stage_forced_arena(void) {
    unsigned screen = psx_mod_read_word(SCREEN);
    if (screen == FIGHT || screen == MENU) forced_held = 0;
    return forced_mode >= 0 && forced_held ? forced_arena : -1;
}

/* From the routine that picks the fight's arena (0x8004F3B0), each fight. */
void __wrap_tekken3_stage_select_fight(unsigned mode) {
    __real_tekken3_stage_select_fight(mode);
    fight = fight_mode = -1; moving = 0; line_fight = 0; forced_mode = -1; forced_held = 0; option_mode = -1;
    int tournament_stage, tournament = tmst_tournament_fight_stage(mode, &tournament_stage);
    if (tournament == TMST_TSTAGE_T3 || tournament == TMST_TSTAGE_TTT) { force(mode, tournament, tournament_stage, "tournament"); return; }
    if (!ready()) {
        if (tournament == TMST_TSTAGE_T2) tmst_log("tekken2: tournament fight, Tekken 2's stages off: the system's stage");
        return;
    }
    int line = mode == VS ? LINE_VS : mode == PRACTICE ? LINE_PRACTICE : -1, stage = -1, theirs = 0;
    const char *e = getenv("TMST_T2_STAGE");
    if (e && *e) { stage = atoi(e) % STAGES; line_fight = getenv("TMST_T2_LINE") != NULL; }
    else if (tournament == TMST_TSTAGE_T2) stage = tournament_stage;
    else if (line >= 0 && armed[line]) { stage = chosen[line] >= 0 ? chosen[line] : pick(STAGES); line_fight = 1; }
    else if (line >= 0 && their_random[line]) {      /* their GAME RANDOM: ours among the lots */
        int source, ttt, arena;
        if (__real_tekken3_stage_select_choice(mode, &source, &ttt, &arena)) {
            const char *t = getenv("TEKKEN3_STAGES_TTT");
            int n = THEIR_T3 + (tekken3_arcade_stages_source_ready(TEKKEN3_STAGE_TTT) || (t && *t == '1') ? THEIR_TTT : 0);
            int lot = pick(n + STAGES);
            if (lot < STAGES) { stage = lot; theirs = line_fight = 1; }
        }
    }
    if (line >= 0) armed[line] = 0;
    if (stage < 0) return;
    fight = stage; fight_mode = (int)mode; moving = 1;
    fight_extras();
    tmst_log("tekken2: mode %u fight on Tekken 2's %s (stage %d, %s, arena %d)%s%s", mode, names[stage], stage,
             version_words[drawn_version()], T2_ARENA,
             theirs ? ", their GAME RANDOM" : line >= 0 && chosen[line] < 0 && !(e && *e) ? ", RANDOM" : "",
             tournament == TMST_TSTAGE_T2 ? ", the tournament's" : "");
}
/* The same arena for the guests' correction (src/tekken3_ttt1_roster.c) and
 * the STAGES row's decision (Tekken 3's stage there: nothing moved). */
int __wrap_tekken3_stage_select_arena(void) {
    if (forced_mode >= 0 && forced_held) return forced_arena;
    return fight >= 0 && moving ? T2_ARENA : __real_tekken3_stage_select_arena();
}
/* ... and none of their drawn stage's music (their GAME RANDOM's lot ours):
 * the fighter's. */
int __wrap_tekken3_stage_select_music(void) {
    return fight >= 0 && moving ? -1 : __real_tekken3_stage_select_music();
}

/* The stage's own piece, as Tekken 3's and TTT's on the STAGE line (the
 * user's, 2026-11-08): its owner's theme (tmst_tournament_stages.c's), in
 * the version of the fight's music (issue #9: GAME OPTION's BGM SELECT,
 * RANDOM's draw included, tmst_music_version; SILENT: the game starts no
 * music, so neither does the Jukebox). The Jukebox plays it unless the player
 * chose a piece for the opponent; without Tekken 2 Content's Music its
 * categories are off and the fighter's piece plays. */
int tmst_tekken2_fight_music(const char **key, const char **cat, const char **name) {
    if (!line_fight || fight < 0 || fight >= STAGES || psx_mod_read_word(MODE) != (uint32_t)fight_mode) return 0;
    int arcade = tmst_music_version() == TMST_MUSIC_ARCADE;
    *key = "t2-stage";
    *cat = arcade ? "t2-arcade" : "t2-console";
    *name = tmst_t2_stage_music(fight, arcade);
    return 1;
}
int __wrap_tekken3_stage_select_choice(unsigned mode, int *source, int *ttt_stage, int *arena) {
    if (forced_mode >= 0 && (unsigned)forced_mode == mode) {
        *source = forced_source; *ttt_stage = forced_ttt; *arena = forced_arena;
        return 1;
    }
    if (fight < 0 || (unsigned)fight_mode != mode) return __real_tekken3_stage_select_choice(mode, source, ttt_stage, arena);
    *source = TEKKEN3_STAGE_T3; *ttt_stage = -1; *arena = T2_ARENA;
    return 1;
}

/* ---- GAME OPTION's STAGES row: TEKKEN 2, and RANDOM's Tekken 2 lot (issue #11) ----
 *
 * The row is the Expanded's (src/tekken3_stage_source.c): it is added with
 * TEKKEN 2 after TEKKEN TAG TOURNAMENT (the user's order, 2026-11-13), and
 * the Expanded reads its own values only: on TEKKEN 2 the one it had, so it
 * keeps (and saves) that. Ours is kept in t2-stages.txt ("stages tekken2").
 * Offered with this part on and the row there (it is, with TTT's packs).
 *
 * TEKKEN 2: the fighter who decides the fight's stage (the Expanded's: the
 * CPU in Arcade, the player who chose last in VS, the member on the field in
 * Team Battle) gets its stage of the user's list (tmst_option_t2_stage): a
 * Tekken 2 fighter its Tekken 2 stage, the others their Tekken 3 arena or
 * TTT stage. RANDOM: a Tekken 2 fighter's pool is T3, TTT and its Tekken 2
 * stage, one chance in three each; the Expanded draws the two others. Tekken
 * 3's newcomers have a Tekken 2 lot too (the user's list), not in the school
 * outfit (tmst_random_t2_stage). The
 * STAGE line of VS and Practice and the tournaments decide first. A Continue
 * keeps the stage. A Tekken 2 stage plays its own music, as from the STAGE
 * line (tmst_tekken2_fight_music). */
#ifdef TMST_HAS_STAGE_OPTION
int __real_tekken3_game_options_add(const char *name, const char *const *labels, unsigned count,
                                    unsigned value, unsigned default_value, unsigned modes);
int __real_tekken3_game_options_value(int row);
void __real_tekken3_stage_source_decide(unsigned mode, int player, const char *guest, int arena);
int __real_tekken3_stage_source_load_arena(void);

enum { DEMO_MODE = 6, LOADING_SCREEN = 11, FIGHTERS = 0x800add5c, COSTUMES = 0x800add98, HUMANS = 0x800ae1f8,
       KUMA_ID = 11, EDDY_ID = 8, TIGER_ID = 22, STOCK_IDS = 21, SCHOOL_ARENA = 7, THEIR_MAX = 8 };
static const char *const stock_keys[STOCK_IDS] = {"paul","law","lei","king","yoshimitsu","nina","hwoarang","xiaoyu",
    "eddy","jin","julia","kuma","bryan","heihachi","ogre","mokujin","gunjack","gon","anna","drb","trueogre"};
static int stages_row = -1, t2_at = -1, their_count, their_last = -1;
static const char *their_labels[THEIR_MAX];
static int opt_last_mode = -1, opt_kind, opt_stage, opt_continuing;
static uint16_t opt_was_human[2];

static int ttt_ready(void) {
    const char *t = getenv("TEKKEN3_STAGES_TTT");
    return tekken3_arcade_stages_source_ready(TEKKEN3_STAGE_TTT) || (t && *t == '1');
}
static int option_on(void) { return ready() && ttt_ready() && option == OPT_TEKKEN2; }
/* Theirs on RANDOM: the row's label once it is there, their file before. */
static int their_on_random(void) {
    static int read = -1;
    if (!ready() || !ttt_ready()) return 0;
    if (stages_row >= 0 && their_last >= 0 && their_last < their_count) return !strcmp(their_labels[their_last], "RANDOM");
    if (read < 0) {
        char word[32] = "";
        const char *e = getenv("TEKKEN3_STAGES");
        if (e && *e) snprintf(word, sizeof word, "%s", e);
        else {
            char path[4096]; file_path("ttt-stages.txt", path, sizeof path);
            FILE *f = path[0] ? fopen(path, "rb") : NULL;
            if (f) { if (fscanf(f, "%31s", word) != 1) word[0] = 0; fclose(f); }
        }
        read = !strcmp(word, "random");
    }
    return read;
}
static void set_option(int o) {
    option = o;
    if (!option_forced) save();
    tmst_log("tekken2: STAGES set to %s", o == OPT_TEKKEN2 ? "TEKKEN 2" : "theirs");
}

int __wrap_tekken3_game_options_add(const char *name, const char *const *labels, unsigned count,
                                    unsigned value, unsigned default_value, unsigned modes) {
    if (strcmp(name, "STAGES") || stages_row >= 0 || count + 1 > THEIR_MAX || value >= count || !ready())
        return __real_tekken3_game_options_add(name, labels, count, value, default_value, modes);
    const char *list[THEIR_MAX];
    int at = -1;
    for (unsigned i = 0; i < count; i++) {
        their_labels[i] = labels[i];
        if (!strcmp(labels[i], "TEKKEN TAG TOURNAMENT")) at = (int)i + 1;
    }
    for (unsigned i = 0; at < 0 && i < count; i++) if (!strcmp(labels[i], "TEKKEN 3")) at = (int)i + 1;
    if (at < 0) at = (int)count;
    for (int i = 0, j = 0; i <= (int)count; i++) list[i] = i == at ? "TEKKEN 2" : labels[j++];
    unsigned shown = option == OPT_TEKKEN2 ? (unsigned)at : value + (value >= (unsigned)at);
    unsigned fallback = default_value + (default_value >= (unsigned)at);
    int row = __real_tekken3_game_options_add(name, list, count + 1, shown, fallback, modes);
    if (row < 0) return row;
    stages_row = row; t2_at = at; their_count = (int)count; their_last = (int)value;
    tmst_log("tekken2: TEKKEN 2 on the STAGES row (value %d of %u), shows %s", at, count + 1, list[shown]);
    return row;
}
int __wrap_tekken3_game_options_value(int row) {
    int v = __real_tekken3_game_options_value(row);
    if (row < 0 || row != stages_row || v < 0) return v;
    if (v == t2_at) {
        if (option != OPT_TEKKEN2) set_option(OPT_TEKKEN2);
        return their_last;                           /* theirs unchanged: nothing saved there */
    }
    if (option != OPT_THEIRS) set_option(OPT_THEIRS);
    return their_last = v > t2_at ? v - 1 : v;
}

static const char *native_key(int p) {
    unsigned id = psx_mod_read_half(FIGHTERS + 2u * (unsigned)p), costume = psx_mod_read_half(COSTUMES + 2u * (unsigned)p) & 3;
    if (id == KUMA_ID && costume == 1) return "panda";
    if (id == EDDY_ID && costume == 2) return "tiger";
    if (id == TIGER_ID) return "tiger";
    return id < STOCK_IDS ? stock_keys[id] : NULL;
}
static const char *const kind_words[] = { "theirs", "Tekken 2's stage", "Tekken 3's arena", "TTT's stage" };

void __wrap_tekken3_stage_source_decide(unsigned mode, int player, const char *guest, int arena) {
    /* decided again (Team Battle's member swapped in): ours undone first */
    if (option_mode >= 0) { fight = fight_mode = -1; moving = line_fight = 0; forced_mode = -1; forced_held = 0; option_mode = -1; }
    int t2 = option_on(), lot = !t2 && their_on_random(), s, t, a;
    if ((!t2 && !lot) || !(mode <= PRACTICE || mode == DEMO_MODE) || player < 0 || player > 1 ||
        __wrap_tekken3_stage_select_choice(mode, &s, &t, &a)) {
        opt_continuing = 0;
        __real_tekken3_stage_source_decide(mode, player, guest, arena);
        return;
    }
    const char *key = guest ? guest : native_key(player);
    int kind = TMST_TSTAGE_NONE, stage = -1, again = opt_continuing && opt_last_mode == (int)mode;
    opt_continuing = 0;
    if (again) { kind = opt_kind; stage = opt_stage; }
    else if (t2) kind = tmst_option_t2_stage(key, !guest && arena == SCHOOL_ARENA, &stage);
    else if (tmst_random_t2_stage(key, !guest && arena == SCHOOL_ARENA, &stage) && pick(3) == 0) kind = TMST_TSTAGE_T2;
    else stage = -1;
    opt_last_mode = (int)mode; opt_kind = kind; opt_stage = stage;
    tmst_log("tekken2: mode %u, P%d's %s decides, STAGES %s -> %s %d%s", mode, player + 1, key ? key : "-",
             t2 ? "TEKKEN 2" : "RANDOM", kind_words[kind], stage, again ? " (Continue: the same)" : "");
    if (kind == TMST_TSTAGE_T2) {
        fight = stage; fight_mode = (int)mode; moving = 1; line_fight = 1; option_mode = (int)mode;
        fight_extras();
    } else if (kind == TMST_TSTAGE_T3 || kind == TMST_TSTAGE_TTT) {
        force(mode, kind, stage, "STAGES row"); option_mode = (int)mode;
    }
    __real_tekken3_stage_source_decide(mode, player, guest, arena);
}
/* The arena the roster loads for it (and gives a guest). */
int __wrap_tekken3_stage_source_load_arena(void) {
    if (option_mode >= 0 && forced_mode >= 0 && forced_held) return forced_arena;
    if (option_mode >= 0 && fight >= 0 && moving) return T2_ARENA;
    return __real_tekken3_stage_source_load_arena();
}
/* Every VBlank: a Continue (a human side handed to the CPU in the fight, as
 * the Expanded sees it), and the forced arena held until the fight. */
static void option_tick(unsigned screen) {
    if (screen == MENU) opt_last_mode = -1;
    if (screen != FIGHT && screen != LOADING_SCREEN && screen != 9 && screen != 1) {
        opt_continuing = 0; opt_was_human[0] = opt_was_human[1] = 0;
    }
    if (screen == FIGHT)
        for (unsigned p = 0; p < 2; p++) {
            uint16_t human = psx_mod_read_half(HUMANS + p * 2);
            if (opt_was_human[p] && !human) opt_continuing = 1;
            opt_was_human[p] = human;
        }
    int a = tmst_stage_forced_arena();
    if (option_mode >= 0 && a >= 0 && psx_mod_read_half(ARENA) != (uint16_t)a) {
        psx_mod_write_half(ARENA, (uint16_t)a);
        psx_mod_write_byte(ARENA_COPY, (uint8_t)a);
    }
}
#else
static void option_tick(unsigned screen) { (void)screen; }
#endif

/* ---- the STAGE line ---- */

static uint16_t previous[2] = { 0xffff, 0xffff }, kept[2];

static void set_t2(int line, int on, unsigned player) {
    t2[line] = on; on_stage[line] = 0;
    tmst_log("tekken2: P%u sets the %s game to %s", player + 1, line ? "Practice" : "VS", on ? "TEKKEN 2" : "theirs");
    save();
}

/* The Practice's STAGE line follows the pad that picks the dummy, as the
 * Expanded's (src/tekken3_stage_select.c practice_dummy / quick_dummy):
 * player 1's, or player 2's when player 2 started the Practice (1.5.1, B170,
 * GH#218: the dummy in player 1's block). 0 when no dummy waits (1.5.0:
 * always player 1's). */
static unsigned practice_pad(void) {
    enum { SEL_SCREEN = 0x800ae204, SEL_PHASE = 0x800ae224, SEL_MODE = 0x800afa88, ARCADE_GRID = 9, VS_GRID = 10,
           GRID_BLOCKS = 0x8011864c, GRID_STRIDE = 0x7c, GRID_CHOOSING = 1, GRID_CHOOSING_CPU = 3,
           VS_BLOCKS = 0x800b8d70, VS_STRIDE = 172, QUICK_CPU = 7, QUICK_DONE = 11 };
    unsigned screen = psx_mod_read_word(SEL_SCREEN);
    if (psx_mod_read_word(SEL_MODE) != PRACTICE) return 0;
    for (unsigned d = 0; d < 2; d++) {
        if (screen == ARCADE_GRID && psx_mod_read_word(0x8010ff4c) == 0x27bdffd0 && psx_mod_read_half(SEL_PHASE) <= 1 &&
            psx_mod_read_word(GRID_BLOCKS + d * GRID_STRIDE) == GRID_CHOOSING_CPU &&
            psx_mod_read_word(GRID_BLOCKS + (d ^ 1) * GRID_STRIDE) != GRID_CHOOSING)
            return d ^ 1;
        if (screen == VS_GRID && psx_mod_read_word(VS_BLOCKS + d * VS_STRIDE) == QUICK_CPU &&
            psx_mod_read_word(VS_BLOCKS + (d ^ 1) * VS_STRIDE) == QUICK_DONE)
            return d ^ 1;
    }
    return 0;
}

uint16_t __wrap_tekken3_stage_select_input(int player, uint16_t buttons) {
    if (player < 0 || player > 1 || !psx_mod_game_started() || !ready())
        return __real_tekken3_stage_select_input(player, buttons);
    unsigned p = (unsigned)player;
    uint16_t edges = previous[p] & (uint16_t)~buttons, keep = 0;
    previous[p] = buttons;
    kept[p] &= (uint16_t)~buttons;                   /* kept keys let go */
    const char *label, *name; int arrows;
    int kind = __real_tekken3_stage_select_line(&label, &name, &arrows), line = kind - 1;
    if (!kind) on_stage[LINE_VS] = on_stage[LINE_PRACTICE] = 0;
    else their_random[line] = !strcmp(label, "GAME") && !strcmp(name, "RANDOM");
    if (!kind || (line == LINE_PRACTICE && p != practice_pad()))
        return __real_tekken3_stage_select_input(player, buttons | kept[p]);
    int game_step = !strcmp(label, "GAME"), at_default = game_step && !strcmp(name, "DEFAULT");
    if (t2[line] && !at_default) { t2[line] = on_stage[line] = 0; save(); }   /* theirs moved on */
    armed[line] = 0;
    if (t2[line] && !on_stage[line]) {               /* GAME: TEKKEN 2 */
        if (edges & PAD_RIGHT) { set_t2(line, 0, p); keep = PAD_RIGHT; }        /* to their DEFAULT */
        else if (edges & PAD_LEFT) set_t2(line, 0, p);                          /* passed: their last game */
        else if ((edges & PAD_DOWN) || (line == LINE_PRACTICE && (edges & (PAD_CROSS | PAD_START)))) {
            on_stage[line] = 1; keep = edges & (PAD_DOWN | PAD_CROSS | PAD_START);
        } else if (edges & (PAD_CROSS | PAD_START)) armed[line] = 1;           /* VS: their take */
    } else if (t2[line]) {                           /* STAGE: RANDOM, then the 23 */
        if (edges & (PAD_LEFT | PAD_RIGHT)) {
            int s = chosen[line] + 1 + (edges & PAD_LEFT ? -1 : 1);
            chosen[line] = (s + STAGES + 1) % (STAGES + 1) - 1;
            keep = edges & (PAD_LEFT | PAD_RIGHT);
            tmst_log("tekken2: P%u sets the %s stage to %s", p + 1, line ? "Practice" : "VS", stage_name(chosen[line]));
            save();
        } else if (edges & (PAD_UP | PAD_CIRCLE | PAD_SELECT)) {
            on_stage[line] = 0; keep = edges & (PAD_UP | PAD_CIRCLE | PAD_SELECT);
        } else if (edges & (PAD_CROSS | PAD_START)) armed[line] = 1;           /* their take, on DEFAULT */
        else keep = edges & PAD_DOWN;
    } else if (at_default && (edges & PAD_LEFT)) {   /* their DEFAULT back to TEKKEN 2 */
        set_t2(line, 1, p); keep = PAD_LEFT;
    }
    kept[p] |= keep;
    uint16_t out = __real_tekken3_stage_select_input(player, buttons | kept[p]);
    /* RIGHT from their last game: their DEFAULT, TEKKEN 2 shows between */
    if (!t2[line] && game_step && !at_default && (edges & PAD_RIGHT) &&
        __real_tekken3_stage_select_line(&label, &name, &arrows) == kind && !strcmp(label, "GAME") &&
        !strcmp(name, "DEFAULT"))
        set_t2(line, 1, p);
    if (armed[line]) tmst_log("tekken2: P%u takes Tekken 2's %s for %s", p + 1, stage_name(chosen[line]),
                              line ? "Practice" : "VS");
    return out;
}

int __wrap_tekken3_stage_select_line(const char **label, const char **name, int *arrows) {
    int kind = __real_tekken3_stage_select_line(label, name, arrows), line = kind - 1;
    if (!kind || !ready() || !t2[line] || strcmp(*label, "GAME") || strcmp(*name, "DEFAULT")) return kind;
    if (!on_stage[line]) *name = "TEKKEN 2";
    else { *label = "TEKKEN 2"; *name = stage_name(chosen[line]); }
    return kind;
}

/* ---- every VBlank ---- */

/* GAME OPTION's row, added at the first visit of the Options screen. */
static void menu(void) {
#ifdef TMST_HAS_GAME_OPTIONS
    static int row = -2, shown;
    if (row == -2) {
        row = tekken3_game_options_add("TEKKEN 2 STAGES", version_labels, VERSIONS, (unsigned)version, CLASSIC,
                                       ROW_MODES);
        shown = version;
    }
    if (row < 0) return;
    int v = tekken3_game_options_value(row);
    if (v >= 0 && v < VERSIONS && v != shown) {
        shown = version = v;
        save();
        tmst_log("tekken2: TEKKEN 2 STAGES set to %s%s", version_labels[v],
                 v == REMAKE && !version_made(REMAKE) ? " (no Remake packs yet: Classic drawn)" : "");
    }
#endif
}

/* At a fight's start: the engine about to draw from a pack it holds for this
 * arena that is not this fight's, or not to try the arena again (refused
 * before) when Tekken 2's is wanted there: another arena read until it opens
 * that one (the empty pack; fopen gives the arena back). */
static void fight_start(void) {
    int arena = psx_mod_read_half(ARENA);
    if (arena < 0 || arena > LAST_ARENA || psx_mod_read_byte(NO_STAGE)) return;
    int want = our_fight() ? fight : HELD_REAL, as = held_wanted();
    /* (a "wolf_cut" stage's: the whole pack held and Unknown come in, or
     * hers and she gone: Team Battle's next member, Survival's next
     * opponent, on the same arena; wolf_pack()) */
    int stale = held_arena == arena && (held != want || (want >= 0 && held_version != as) ||
                                        (want >= 0 && as == REMAKE && held_wolf != wolf_pack(fight, NULL, 0)));
    int refused = want >= 0 && tried == arena && held_arena != arena;
    if (!stale && !refused) return;
    /* an arena the engine will load: not this one, not the one it holds, not
     * the one it tried last */
    static const int others[4] = { 8, 7, 10, 1 };
    int to = -1;
    for (int i = 0; i < 4 && to < 0; i++)
        if (others[i] != arena && others[i] != held_arena && others[i] != tried) to = others[i];
    if (to < 0) return;
    switch_back = arena;
    switch_want = to;
    int wolf = want >= 0 && as == REMAKE && wolf_pack(fight, NULL, 0);
    tmst_log("tekken2: the engine holds %s (%s%s) for arena %d, %s (%s%s) wanted: arena %d until it loads that one",
             held == HELD_REAL ? "the game's pack" : held == HELD_BLANK ? "the empty pack" : held >= 0 ? names[held]
             : "nothing", held >= 0 ? held_words[held_version] : "-", held >= 0 && held_wolf ? ", Unknown's" : "", arena,
             want >= 0 ? names[want] : "the game's", want >= 0 ? held_words[as] : "-", wolf ? ", Unknown's" : "",
             switch_want);
}

/* The switch's arena, read by the engine this very frame: as the PS1's
 * stage routine starts, or in the dark arena (where it never runs) as its
 * floor routine does. */
static void switch_now(void) {
    /* the dark arena's byte may change after the fight's first frame (its
     * check, fight_start): our pack held as the other kind (NIGHT or not),
     * checked again, once a kind a fight */
    static int again_for = -1;
    if (psx_mod_read_word(SCREEN) != FIGHT) again_for = -1;
    else if (switch_want < 0 && switch_to < 0 && our_fight() && held_arena == T2_ARENA && held == fight &&
             (held_version == NIGHT) != (held_wanted() == NIGHT) && again_for != held_wanted()) {
        again_for = held_wanted();
        fight_start();
    }
    if (switch_want < 0 || psx_mod_read_word(SCREEN) != FIGHT) return;
    switch_to = switch_want; switch_want = -1; switch_frames = 0;
    psx_mod_write_half(ARENA, (uint16_t)switch_to);
}

/* From src/tekken2/tmst_tekken2_mirror.c, as the PS1's stage routine
 * (0x8006D014) starts, before the engine's wrapper asks for its pack: the
 * switch's arena, read by the engine this very frame. */
void tmst_tekken2_stage_entry(void) {
    /* the T2AN animations: once a frame of the stage, not in the pause */
    if (anims && drawn() && !psx_mod_read_word(PAUSED))
        for (unsigned i = 0; i < anims; i++) {
            if (++anim[i].clock < anim[i].period) continue;
            anim[i].clock = 0;
            unsigned f = anim[i].order[anim[i].at++ % anim[i].entries];
            (void)psx_mod_gpu_high_vram_upload(anim[i].x, anim[i].y, anim[i].w, anim[i].h,
                                               anim[i].pixels + (size_t)f * anim[i].w * anim[i].h);
        }
    /* the extras' palettes and rounds (not in the pause: theirs) */
    if (tmst_tekken2_extras_on() && drawn()) tmst_tekken2_extras_tick();
    switch_now();
}

/* T2LT: arena 9's light entry the named arena's while our fight is on (its
 * mode still), its own bytes back after. */
static void light_tick(void) {
    int want = light_from >= 0 && fight >= 0 && psx_mod_read_word(MODE) == (uint32_t)fight_mode && !dark();
    uint32_t at = LIGHT_TABLE + LIGHT_ENTRY * T2_ARENA;
    if (want) {
        uint8_t e[LIGHT_BYTES];
        for (int i = 0; i < LIGHT_BYTES; i++) e[i] = psx_mod_read_byte(LIGHT_TABLE + LIGHT_ENTRY * (uint32_t)light_from + i);
        if (!light_stood) {
            for (int i = 0; i < LIGHT_BYTES; i++) light_saved[i] = psx_mod_read_byte(at + i);
            light_stood = 1;
            tmst_log("tekken2: arena %d's light on the fighters of %s", light_from, names[fight]);
        }
        for (int i = 0; i < LIGHT_BYTES; i++)
            if (psx_mod_read_byte(at + i) != e[i]) psx_mod_write_byte(at + i, e[i]);
    } else if (light_stood) {
        for (int i = 0; i < LIGHT_BYTES; i++) psx_mod_write_byte(at + i, light_saved[i]);
        light_stood = 0;
    }
}

/* T2BL's frame (issue #12): Tekken 2's 0x80082424 and 0x80087164 */
static void blend_tick(void) {
    long c = lround(cos(TAU * (double)(blend.clock & 0xFFF) / 4096.0) * 4096.0);
    int f = (int)((c + 4096) / 2 / 4);
    blend.clock += blend.add;
    for (unsigned p = 0; p < blend.count; p++) {
        uint16_t out[16];
        memcpy(out, blend.a[p], sizeof out);
        for (unsigned i = blend.from; i <= blend.to; i++) {
            uint16_t col = 0;
            for (int ch = 0; ch < 15; ch += 5) {
                int a = blend.a[p][i] >> ch & 31, d = (blend.b[p][i] >> ch & 31) - a;
                col |= (uint16_t)(((a + ((d * f + 512) >> 10)) & 31) << ch);
            }
            out[i] = col ? col : 0x0421;            /* never the see-through black (the packs' rule) */
        }
        (void)psx_mod_gpu_high_vram_upload(blend.at[p][0], blend.at[p][1], 16, 1, out);
    }
}

static void tick(void) {
    tmst_jukebox_player_tick();                      /* the stage's piece without the Jukebox (issue #9.2) */
    if (!psx_mod_game_started() || !ready()) return;
    light_tick();
    unsigned screen = psx_mod_read_word(SCREEN);
    if (screen == OPTIONS) menu();
    option_tick(screen);
    if (screen == MENU) fight = -1;
    if (moving && screen == FIGHT) moving = 0;
    /* arena 9 until the fight */
    if (moving && fight >= 0 && psx_mod_read_half(ARENA) != T2_ARENA) {
        psx_mod_write_half(ARENA, T2_ARENA);
        psx_mod_write_byte(ARENA_COPY, T2_ARENA);
    }
    if (screen != FIGHT) { checked = 0; switch_want = -1; return; }
    if (switch_to >= 0 && ++switch_frames > 8) {     /* the engine never asked: the arena back */
        psx_mod_write_half(ARENA, (uint16_t)switch_back);
        switch_to = -1;
    }
    if (!checked) { checked = 1; fight_start(); }
    /* Hong Kong's running lights: the next frame's palettes */
    if (chase.count && drawn() && (chase.clock += chase.add) >= chase.period) {
        chase.clock -= chase.period;
        chase.frame = (chase.frame + 1) % chase.frames;
        for (unsigned p = 0; p < chase.count; p++)
            (void)psx_mod_gpu_high_vram_upload(chase.at[p][0], chase.at[p][1], 16, 1, chase.colours[chase.frame][p]);
    }
    if (blend.count && drawn()) blend_tick();
}

static void activate(void) {
    stages = tmst_guard("tmst.tekken2.stages", TMST_NEED_ACTIVATION | TMST_NEED_VBLANK | TMST_NEED_HIGH_VRAM);
    if (!stages) return;
    if (!version_made(CLASSIC)) tmst_log("tekken2: no Tekken 2 stage packs (Setup TMST.cmd with the disc), stages off");
    else if (!tekken3_arcade_stages_source_ready(TEKKEN3_STAGE_T3))
        tmst_log("tekken2: the stage engine is off (Arcade Stages off, or no upper VRAM), stages off");
    else {
        tmst_log("tekken2: Tekken 2 stages on (TEKKEN 2 on the STAGE line), %s Tekken 3's packs", t3_real() ? "with" : "without");
        tmst_jukebox_player_on("Tekken 2's stages");    /* their pieces with the Jukebox off too (issue #9.2) */
    }
}

PSX_MOD_CONSTRUCTOR(tmst_register_tekken2_stages) {
    (void)psx_mod_register_activation_plugin("tmst.tekken2.stages", activate);
    (void)psx_mod_register_vblank_plugin("tmst.tekken2.stages", tick);
}

#else   /* an Expanded without the stage engine: the part says so */

int tmst_tekken2_mirror(int32_t at[6]) { (void)at; return 0; }
int tmst_tekken2_mirror_style(uint8_t style[20]) { (void)style; return 0; }
int tmst_tekken2_extras_wanted(void) { return 0; }
int tmst_tekken2_ground(uint8_t rgb[3]) { (void)rgb; return 0; }
int tmst_tekken2_drawn(void) { return 0; }
int tmst_tekken2_remake_drawn(void) { return 0; }
int32_t tmst_tekken2_fixed_period(void) { return 0; }
const int16_t *tmst_tekken2_walls(int *n, int *radius) { *n = *radius = 0; return NULL; }
void tmst_tekken2_stage_entry(void) {}
int tmst_stage_forced_arena(void) { return -1; }
int tmst_tekken2_fight_music(const char **key, const char **cat, const char **name) {
    (void)key; (void)cat; (void)name; return 0;
}

static void activate(void) {
    if (tmst_guard("tmst.tekken2.stages", TMST_NEED_ACTIVATION))
        tmst_log("tekken2: this Expanded has no stage engine to draw them, Tekken 2 stages off");
}

PSX_MOD_CONSTRUCTOR(tmst_register_tekken2_stages) {
    (void)psx_mod_register_activation_plugin("tmst.tekken2.stages", activate);
}
#endif

/* 0x800826E8 SetBackColor(r, g, b): the fighters' ambient light (its callers
 * 0x80039CC0, 0x8003A164, 0x8003A2C8), dimmed on a dark Tekken 2 stage
 * (T2FX), as the remake's FDIM. Chained: the Expanded may wrap it one day. */
extern void __tmst_chain_func_800826E8(CPUState *cpu);
#ifdef TMST_BRIDGE_func_800826E8
extern void __real_func_800826E8(CPUState *cpu);
void __tmst_chain_func_800826E8(CPUState *cpu) { __real_func_800826E8(cpu); }
#endif
void __wrap_func_800826E8(CPUState *cpu) {
#ifdef TMST_HAS_STAGE_ENGINE
    if (dim != NO_DIM && (cpu->pc == 0 || cpu->pc == 0x800826e8u) && cpu->gpr[31] >= 0x80039c00u &&
        cpu->gpr[31] < 0x8003a400u && drawn())
        for (int k = 0; k < 3; k++) cpu->gpr[4 + k] = (uint32_t)((int)cpu->gpr[4 + k] * (int)dim / NO_DIM);
    /* FDIM's candlelight: tinted, its level wavering (three slow waves of
     * the vsync count), as the old fork's */
    if (tint[0] && (cpu->pc == 0 || cpu->pc == 0x800826e8u) && cpu->gpr[31] >= 0x80039c00u &&
        cpu->gpr[31] < 0x8003a400u && drawn()) {
        float t = (float)psx_mod_read_word(0x800a08f0u);
        float w = 0.5f * sinf(t * 0.11f) + 0.3f * sinf(t * 0.27f + 1.3f) + 0.2f * sinf(t * 0.61f + 2.1f);
        float lv = 1.0f + w * (float)flicker / 128.0f;
        for (int k = 0; k < 3; k++) {
            int v = (int)((float)(int)cpu->gpr[4 + k] * (float)tint[k] / 128.0f * lv);
            cpu->gpr[4 + k] = (uint32_t)(v < 0 ? 0 : v > 255 ? 255 : v);
        }
    }
    /* a lightning flash adds its light, fading over its frames (the Remake's LGHT) */
    uint32_t rgb; int left, frames;
    if ((cpu->pc == 0 || cpu->pc == 0x800826e8u) && cpu->gpr[31] >= 0x80039c00u && cpu->gpr[31] < 0x8003a400u &&
        tmst_tekken2_extras_light(&rgb, &left, &frames) && drawn())
        for (int k = 0; k < 3; k++) {
            int v = (int)cpu->gpr[4 + k] + (int)((rgb >> (8 * k)) & 0xFF) * left / frames;
            cpu->gpr[4 + k] = (uint32_t)(v > 255 ? 255 : v);
        }
#endif
    __tmst_chain_func_800826E8(cpu);
}

/* 0x80049848, the PS1's floor of kinds 1-3, the dark arena's (True Ogre's
 * fight): the PS1 there calls only its black TILE and this, never the stage
 * routine 0x8006D014, and the engine, wrapping it, keeps the PS1's floor in
 * the dark arena unless a TTT stage is wanted (its active()). In our fight
 * its byte reads 0 while the engine's wrapper runs inside ours: the engine
 * draws our pack's floor (NIGHT: the Classic's, lit round the fighters, its
 * far edge into the black), the PS1's TILE the rest. A switch of the pack
 * (fight_start) is made here, as the PS1's stage routine would. Chained:
 * the Expanded wraps it. */
extern void __tmst_chain_func_80049848(CPUState *cpu);
#ifdef TMST_BRIDGE_func_80049848
extern void __real_func_80049848(CPUState *cpu);
void __tmst_chain_func_80049848(CPUState *cpu) { __real_func_80049848(cpu); }
#endif
void __wrap_func_80049848(CPUState *cpu) {
#ifdef TMST_HAS_STAGE_ENGINE
    uint8_t was = 0;
    if ((cpu->pc == 0 || cpu->pc == 0x80049848u) && stages && (was = psx_mod_read_byte(DARK_ARENA)) != 0 &&
        our_fight() && ready()) {
        switch_now();
        dark_call = 1;
        psx_mod_write_byte(DARK_ARENA, 0);
        __tmst_chain_func_80049848(cpu);
        psx_mod_write_byte(DARK_ARENA, was);
        dark_call = 0;
        return;
    }
#endif
    __tmst_chain_func_80049848(cpu);
}
