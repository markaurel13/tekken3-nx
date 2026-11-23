/* tmst.jukebox: each fighter's music, chosen in a screen of its own.
 *
 * The main menu's JUKEBOX row, always the list's last (src/core/tmst_menu.c),
 * opens the Team Battle grid: CROSS on a fighter opens the music list, where
 * L2 / R2 change the category, TRIANGLE plays or stops a preview, CROSS
 * assigns, SQUARE restores the default, CIRCLE goes back to the grid; on the
 * grid SQUARE restores every fighter's default (ARE YOU SURE?) and CIRCLE
 * leaves to the menu, asking SAVE? first when a choice is not saved yet.
 * Categories, each there when it has music now:
 *
 *   (TEKKEN 1 and 2 only while Tekken 2 Content's Music, tmst.tekken2, is on too)
 *   TEKKEN 1 CONSOLE / ARCADE  the Tekken 1 pieces the Tekken 2 disc plays
 *                      again: the T2 folders' files that the TEKKEN 1 list
 *                      names, only those
 *   TEKKEN 2 CONSOLE / ARCADE  addons/tmst/workspace/t2-music/{console,arcade}/*.bgm,
 *                      made by the add-on's setup from the player's T2 disc
 *                      (tools/tekken2_music.py)
 *   TEKKEN 3 CONSOLE / ARCADE  the Tekken 3 disc itself: TEKKEN3.XAS, the XA
 *                      the executable's music table (0x80024FD0) names per
 *                      music id and Options music mode (0 arranged, 1
 *                      arcade; bit 7 = a CD audio track), and the disc's CD
 *                      audio tracks (Console)
 *   TEKKEN TAG TOURNAMENT CONSOLE / ARCADE  addons/tmst/workspace/ttt-music/{console,
 *                      arcade}/*.bgm: Console the 14 PS2 stage pieces the
 *                      add-on's setup decodes from the Expanded's
 *                      mods/ttt-ps2-music (tools/ttt_ps2_music.py); Arcade
 *                      the 14 arcade stage pieces it renders from
 *                      mods/ttt-music's C352 writes (tools/ttt_arcade_music.py)
 *   TEKKEN 5 DR        addons/tmst/workspace/t5dr-music/*.bgm, the music of
 *                      the Dark Resurrection PSP ISO the Expanded's setup was
 *                      given, taken out by the add-on's setup (tools/tekken5dr_music.py)
 *
 * A .bgm is the frames themselves ('T3BGM001'), the game's own PS-ADPCM
 * ('T3ADP001': TTT CONSOLE, TEKKEN 5 DR) or a disc's XA audio ('T3XA0001',
 * 37 800 Hz: TEKKEN 1 and 2), both decoded as they play (src/jukebox/tmst_adpcm.c).
 *
 * Titles: addons/tmst/workspace/jukebox/<category>.txt ("file = title"; the setup
 * writes the defaults, the player may edit them; "--disable" in a title
 * leaves that piece out), else the file's name. Choices: jukebox.txt beside
 * the memory card ("key = category/file"). An opponent (P2) with a choice
 * whose music is there plays it in its fights instead of the game's piece:
 * when the game starts the fight's music (0x8006AE14, chained), it plays a
 * muted stand-in instead and ours streams as long as that one plays, at its
 * level (fades, the pause), mixed after the game's sound at the player's
 * music volume (src/core/tmst_spu.c). In a Tournament Mode match, an
 * opponent without the player's own choice plays the tournament's piece
 * (tmst_tournament_fight_music), on a Tekken 2 stage of the STAGE line the
 * stage's piece (tmst_tekken2_fight_music). While the Jukebox is open the game's
 * music is silent and the preview plays.
 *
 * The list is drawn by the GPU over the lower part of the grid screen
 * (src/core/tmst_overlay.c's primitives), in the UI font (Fira Sans); the pads
 * go through our filter (src/core/tmst_input.c) before the game and the
 * Expanded see them. Benches: TMST_JUKEBOX_ROOT (the Expanded's folder),
 * TMST_JUKEBOX_PCM=<raw> (the streamed frames). */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_ui_font.h"
#include "tmst_overlay.h"
#include "tmst_menu.h"
#include "tmst_input.h"
#include "tmst_music.h"
#include "tmst_spu.h"
#include "tmst_files.h"
#include "tmst_adpcm.h"
#include "mod_plugins.h"
#include "memcard.h"
#include "psx_sdl.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <strings.h>
#endif

/* The runtime's disc reader (psxrecomp/runtime/src/iso_reader_c.cpp). */
extern void *iso_open(const char *path);
extern int iso_read_sector(void *handle, uint32_t lba, uint8_t *buffer, int size);
extern int iso_read_raw_sector(void *handle, uint32_t lba, uint8_t *buffer, int size);
extern int iso_track_count(void *handle);
extern uint32_t iso_track_start_lba(void *handle, int track);
extern uint32_t iso_sector_count(void *handle);
extern int iso_track_is_audio(void *handle, int track);
/* The Expanded's TTT1 roster (weak: absent without it). */
extern const char *tekken3_guest_key(unsigned id) __attribute__((weak));
extern unsigned tekken3_ttt1_grid_character(unsigned cell) __attribute__((weak));
/* The Expanded's grid page (0 its stock one, 1 the Tag page), -1 where L2 /
 * R2 switch nothing. */
extern int tekken3_ttt1_selector_page(void) __attribute__((weak));

enum { SCREEN = 0x800ae204, PHASE = 0x800ae224, MODE = 0x800afa88, TEAM_BATTLE = 2, FIGHT = 8, LOADING = 11,
       GRID_BLOCK = 0x800b8d70, TEAM_SIZE = 1, STOCK_GRID = 0x80022768,
       XA_TABLE = 0x80024d78, XA_COUNT = 50, MUSIC_TABLE = 0x80024fd0, MUSIC_IDS = 32, MUSIC_OPTION = 0x80097f05,
       RECORDS = 0x80097d40, P1_ID = 0x800add5c, P2_ID = 0x800add5e, P1_COSTUME = 0x800add98, P2_COSTUME = 0x800add9a,
       STOCK = 21, GUEST_FIRST = 23, PANDA_ID = 41, TIGER_ID = 42, KUMA_ID = 11, EDDY_ID = 8,
       MAX_TRACKS = 192, NAME = 64, TITLE = 96, MAX_KEYS = 96, KEY = 32,
       FILE_LEVEL = 7600,      /* the .bgm packs, per 10 000 */
       /* The disc's XA decodes to RMS 5 900..10 800 (xa 2, 9, 21, 29, 35,
        * measured 2026-10-19); the game plays its music at RMS 3 912. */
       DISC_LEVEL = 5000 };
#define CATS 9
enum { B_SELECT = TMST_PAD_SELECT, B_START = TMST_PAD_START, B_UP = TMST_PAD_UP, B_RIGHT = TMST_PAD_RIGHT,
       B_DOWN = TMST_PAD_DOWN, B_LEFT = TMST_PAD_LEFT, B_L2 = TMST_PAD_L2, B_R2 = TMST_PAD_R2,
       B_TRIANGLE = TMST_PAD_TRIANGLE, B_CIRCLE = TMST_PAD_CIRCLE, B_CROSS = TMST_PAD_CROSS, B_SQUARE = TMST_PAD_SQUARE };
enum { VIEW_OFF, VIEW_GRID, VIEW_LIST, VIEW_DIALOG };
enum { ASK_SAVE, ASK_RESET };

static const char *const stock_keys[STOCK] = {"paul","law","lei","king","yoshimitsu","nina","hwoarang","xiaoyu",
    "eddy","jin","julia","kuma","bryan","heihachi","ogre","mokujin","gunjack","gon","anna","drb","trueogre"};
static const char *const stock_names[STOCK] = {"PAUL PHOENIX","FOREST LAW","LEI WULONG","KING","YOSHIMITSU",
    "NINA WILLIAMS","HWOARANG","LING XIAOYU","EDDY GORDO","JIN KAZAMA","JULIA CHANG","KUMA","BRYAN FURY",
    "HEIHACHI MISHIMA","OGRE","MOKUJIN","GUN JACK","GON","ANNA WILLIAMS","DR. B","TRUE OGRE"};

static int on;                         /* the feature, at activation */
static int hooked;                     /* the player (music hook, mixer): the feature's, or tmst_jukebox_player_on's */

/* ---- where things are ---------------------------------------------------- */
static char exe_dir[4096], root_dir[4096];
static const char *game_exe_dir(void) {
    if (!exe_dir[0]) {
        const char *base = SDL_GetBasePath();
        snprintf(exe_dir, sizeof exe_dir, "%s", base ? base : "./");
#if !defined(PSX_SDL3)
        if (base) SDL_free((void *)base);
#endif
    }
    return exe_dir;
}
/* The Expanded's folder: the first folder up from the game that holds
 * game.toml (build-release's parent for the player; deeper for a developer
 * build under addons/tmst/dev/build). */
static const char *game_root(void) {
    if (root_dir[0]) return root_dir;
#ifdef __SWITCH__
    snprintf(root_dir, sizeof root_dir, "sdmc:/switch/tekken3");
    return root_dir;
#else
    const char *e = getenv("TMST_JUKEBOX_ROOT");
    if (e && *e) { snprintf(root_dir, sizeof root_dir, "%s", e); return root_dir; }
    char at[4096]; snprintf(at, sizeof at, "%s", game_exe_dir());
    for (int up = 0; up < 8; up++) {
        char probe[4200]; snprintf(probe, sizeof probe, "%sgame.toml", at);
        FILE *f = fopen(probe, "rb");
        if (f) { fclose(f); snprintf(root_dir, sizeof root_dir, "%s", at); return root_dir; }
        size_t n = strlen(at);
        if (n + 3 >= sizeof at) break;
        memcpy(at + n, "../", 4);
    }
    snprintf(root_dir, sizeof root_dir, "%s..", game_exe_dir());
    return root_dir;
#endif
}
static void saves_path(char *out, size_t size) {
    const char *card = NULL; out[0] = 0;
    if (memcard_debug_info(0, &card, NULL, NULL, NULL) || !card || !*card) return;
    const char *slash = strrchr(card, '/'), *back = strrchr(card, '\\');
    if (back && (!slash || back > slash)) slash = back;
    int n = slash ? (int)(slash - card) : 1;
    snprintf(out, size, "%.*s/jukebox.txt", n, slash ? card : ".");
}

/* ---- categories ---------------------------------------------------------- */
enum { SRC_FILE, SRC_XA, SRC_CDDA };
typedef struct { char name[NAME]; char title[TITLE]; uint8_t src, dir; int16_t index; } Track;
typedef struct {
    const char *id, *name, *titles;
    const char *dirs[2];                     /* .bgm folders under the Expanded's folder */
    int disc_mode;                           /* the Tekken 3 disc's music of this mode, -1 none */
    int only_listed;                         /* only the files its list names (another's folders) */
    Track track[MAX_TRACKS]; unsigned count;
} Category;
#define T2_CONSOLE "addons/tmst/workspace/t2-music/console"
#define T2_ARCADE "addons/tmst/workspace/t2-music/arcade"
#define TTT_CONSOLE "addons/tmst/workspace/ttt-music/console"
#define TTT_ARCADE "addons/tmst/workspace/ttt-music/arcade"
#define T5DR "addons/tmst/workspace/t5dr-music"
static Category cats[CATS] = {
    {"t1-console", "TEKKEN 1 CONSOLE", "tekken1-console.txt", {T2_CONSOLE, NULL}, -1, 1},
    {"t1-arcade", "TEKKEN 1 ARCADE", "tekken1-arcade.txt", {T2_ARCADE, NULL}, -1, 1},
    {"t2-console", "TEKKEN 2 CONSOLE", "tekken2-console.txt", {T2_CONSOLE, NULL}, -1, 0},
    {"t2-arcade", "TEKKEN 2 ARCADE", "tekken2-arcade.txt", {T2_ARCADE, NULL}, -1, 0},
    {"t3-console", "TEKKEN 3 CONSOLE", "tekken3-console.txt", {NULL, NULL}, 0, 0},
    {"t3-arcade", "TEKKEN 3 ARCADE", "tekken3-arcade.txt", {NULL, NULL}, 1, 0},
    {"ttt-console", "TEKKEN TAG TOURNAMENT CONSOLE", "ttt-console.txt", {TTT_CONSOLE, NULL}, -1, 0},
    {"ttt-arcade", "TEKKEN TAG TOURNAMENT ARCADE", "ttt-arcade.txt", {TTT_ARCADE, NULL}, -1, 0},
    {"t5dr", "TEKKEN 5 DR", "tekken5-dr.txt", {T5DR, NULL}, -1, 0},
};
/* TEKKEN 1 and 2 are Tekken 2 Content's Music (tmst.tekken2): only while it is on. */
static int category_on(const Category *c) {
    return (strncmp(c->id, "t1-", 3) && strncmp(c->id, "t2-", 3)) || tmst_tekken2_music_active();
}
static int category_of(const char *id) {
    for (int c = 0; c < CATS; c++) if (!strcmp(cats[c].id, id)) return c;
    return -1;
}

typedef void (*FileFn)(void *ctx, const char *name);
static void list_bgm(const char *dir, FileFn fn, void *ctx) {
#ifdef _WIN32
    char pattern[4200]; snprintf(pattern, sizeof pattern, "%s\\*.bgm", dir);
    WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) fn(ctx, fd.cFileName); while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(dir); if (!d) return;
    struct dirent *e;
    while ((e = readdir(d))) {
        size_t n = strlen(e->d_name);
        if (n > 4 && !strcmp(e->d_name + n - 4, ".bgm")) fn(ctx, e->d_name);
    }
    closedir(d);
#endif
}
static int dir_path(const Category *c, unsigned d, char *out, size_t size) {
    return c->dirs[d] && snprintf(out, size, "%s/%s", game_root(), c->dirs[d]) < (int)size;
}
static int add_track(Category *c, const char *name, int src, int dir, int index) {
    if (c->count >= MAX_TRACKS || strlen(name) >= NAME) return 0;
    for (unsigned i = 0; i < c->count; i++) if (!strcmp(c->track[i].name, name)) return 0;
    Track *t = &c->track[c->count++];
    memset(t, 0, sizeof *t);
    snprintf(t->name, NAME, "%s", name); t->src = (uint8_t)src; t->dir = (uint8_t)dir; t->index = (int16_t)index;
    return 1;
}
typedef struct { Category *c; unsigned dir; } Scan;
static void scan_file(void *ctx, const char *name) {
    Scan *s = ctx;
    add_track(s->c, name, SRC_FILE, (int)s->dir, -1);
}

/* ---- the Tekken 3 disc --------------------------------------------------- */
static void *disc;
static int disc_tried;
static FILE *s_xa_fp;
static char s_xa_vbuf[256 * 1024];

static void open_xa_backing_file(const char *disc_path) {
    if (s_xa_fp || !disc_path || !*disc_path) return;
    char bin_path[4200];
    snprintf(bin_path, sizeof bin_path, "%s", disc_path);
    size_t len = strlen(bin_path);
    if (len >= 4 && (strcmp(bin_path + len - 4, ".cue") == 0 || strcmp(bin_path + len - 4, ".CUE") == 0)) {
        FILE *cf = fopen(bin_path, "r");
        if (cf) {
            char line[512];
            while (fgets(line, sizeof line, cf)) {
                char *q1 = strstr(line, "FILE \"");
                if (!q1) continue;
                q1 += 6;
                char *q2 = strchr(q1, '"');
                if (!q2) continue;
                *q2 = '\0';
                const char *slash = strrchr(disc_path, '/');
                if (slash) {
                    size_t dirlen = (size_t)(slash - disc_path + 1);
                    if (dirlen + strlen(q1) + 1 < sizeof bin_path) {
                        memcpy(bin_path, disc_path, dirlen);
                        strcpy(bin_path + dirlen, q1);
                    }
                } else {
                    snprintf(bin_path, sizeof bin_path, "%s", q1);
                }
                break;
            }
            fclose(cf);
        }
    }
    s_xa_fp = fopen(bin_path, "rb");
    if (s_xa_fp) setvbuf(s_xa_fp, s_xa_vbuf, _IOFBF, sizeof s_xa_vbuf);
}
static uint32_t xas_lba;                    /* TEKKEN3.XAS's first sector */
static uint32_t le32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }
static int iso_find(uint32_t lba, uint32_t size, const char *name, uint32_t *out_lba, uint32_t *out_size) {
    uint8_t s[2048]; size_t n = strlen(name);
    for (uint32_t at = 0; at < size && at < 64 * 2048; at += 2048) {
        if (!iso_read_sector(disc, lba + at / 2048, s, 2048)) return 0;
        for (unsigned i = 0; i < 2048 && s[i]; i += s[i])
            if (s[i + 32] >= n && !memcmp(s + i + 33, name, n) && (s[i + 32] == n || s[i + 33 + n] == ';')) {
                *out_lba = le32(s + i + 2); *out_size = le32(s + i + 10); return 1;
            }
    }
    return 0;
}
static int find_xas(void) {
    uint8_t s[2048]; uint32_t lba, size;
    if (!iso_read_sector(disc, 16, s, 2048) || memcmp(s + 1, "CD001", 5)) return 0;
    lba = le32(s + 156 + 2); size = le32(s + 156 + 10);
    return iso_find(lba, size, "TEKKEN3", &lba, &size) && iso_find(lba, size, "TEKKEN3.XAS", &xas_lba, &size);
}
/* The game's disc: the first line of disc.cfg beside it (the runtime caches
 * the disc picked there); else game.toml's [game] disc, relative to the folder
 * that holds game.toml (the Expanded's launcher passes --disc and writes no
 * disc.cfg). */
static int disc_cfg_path(char *out, size_t size) {
    char cfg[4200]; snprintf(cfg, sizeof cfg, "%sdisc.cfg", game_exe_dir());
    FILE *f = fopen(cfg, "rb");
    if (!f) return 0;
    int ok = fgets(out, (int)size, f) != NULL;
    fclose(f);
    if (!ok) return 0;
    size_t n = strlen(out);
    while (n && (out[n - 1] == '\n' || out[n - 1] == '\r' || out[n - 1] == ' ')) out[--n] = 0;
    return n > 0;
}
static char *trim(char *s);
static int game_toml_disc_path(char *out, size_t size) {
    const char *root = game_root();
    size_t r = strlen(root);
    const char *sep = r && (root[r - 1] == '/' || root[r - 1] == '\\') ? "" : "/";
    char toml[4200]; snprintf(toml, sizeof toml, "%s%sgame.toml", root, sep);
    FILE *f = fopen(toml, "rb");
    if (!f) return 0;
    char line[4200], value[4096] = "";
    int in_game = 0;
    while (!value[0] && fgets(line, sizeof line, f)) {
        char *s = trim(line);
        if (*s == '[') { in_game = !strncmp(s, "[game]", 6); continue; }
        if (!in_game || strncmp(s, "disc", 4)) continue;
        s = trim(s + 4);
        if (*s != '=') continue;
        s = trim(s + 1);
        char quote = *s;
        if (quote != '"' && quote != '\'') continue;
        char *end = strchr(s + 1, quote);
        if (!end || end == s + 1) continue;
        size_t o = 0;                        /* a basic string's \\ is one backslash */
        for (char *c = s + 1; c < end && o + 1 < sizeof value; c++)
            value[o++] = quote == '"' && c[0] == '\\' && c[1] == '\\' ? *c++ : *c;
        value[o] = 0;
    }
    fclose(f);
    if (!value[0]) return 0;
    int absolute = value[0] == '/' || value[0] == '\\' || (value[0] && value[1] == ':');
    if (absolute) return snprintf(out, size, "%s", value) < (int)size;
    return snprintf(out, size, "%s%s%s", root, sep, value) < (int)size;
}
#ifdef __SWITCH__
extern const char *switch_get_mounted_disc_path(void);
#endif

static int disc_ready(void) {
    if (disc_tried) return disc != NULL;
    disc_tried = 1;
    char path[4200];
    int got_path = disc_cfg_path(path, sizeof path) || game_toml_disc_path(path, sizeof path);
#ifdef __SWITCH__
    if (!got_path) {
        const char *mounted = switch_get_mounted_disc_path();
        if (mounted && *mounted && snprintf(path, sizeof path, "%s", mounted) < (int)sizeof path) {
            got_path = 1;
        }
    }
#endif
    if (!got_path) {
        tmst_log("jukebox: no disc.cfg beside the game and no [game] disc in game.toml");
        return 0;
    }
    disc = iso_open(path);
    if (disc && !find_xas()) { tmst_log("jukebox: no TEKKEN3.XAS on %s", path); disc = NULL; }
    if (disc) open_xa_backing_file(path);
    return disc != NULL;
}
typedef struct { uint32_t first, last, channel; } XaEntry;
static int xa_entry(unsigned i, XaEntry *e) {
    if (i >= XA_COUNT) return 0;
    e->first = psx_mod_read_word(XA_TABLE + i * 12); e->last = psx_mod_read_word(XA_TABLE + i * 12 + 4);
    e->channel = psx_mod_read_word(XA_TABLE + i * 12 + 8);
    return e->channel < 8 && e->first <= e->last && e->last < 300000;
}
static int cd_track_ok(int track) {
    return disc && track >= 2 && track <= iso_track_count(disc) && iso_track_is_audio(disc, track);
}
static void scan_disc(Category *c) {
    if (!disc_ready()) return;
    int used[XA_COUNT] = {0};
    for (unsigned id = 0; id < MUSIC_IDS; id++) {
        unsigned b = psx_mod_read_byte(MUSIC_TABLE + id * 4 + (unsigned)c->disc_mode * 2);
        if (!(b & 0x80) && b < XA_COUNT) used[b] = 1;
    }
    char name[NAME]; XaEntry e;
    for (unsigned i = 0; i < XA_COUNT; i++) if (used[i] && xa_entry(i, &e)) {
        snprintf(name, sizeof name, "xa-%02u", i); add_track(c, name, SRC_XA, 0, (int)i);
    }
    /* The disc's CD audio: the Console's (the one the table names plays in both modes). */
    for (int t = 2; c->disc_mode == 0 && t <= iso_track_count(disc) && t < 100; t++) if (cd_track_ok(t)) {
        snprintf(name, sizeof name, "cd-track-%d", t); add_track(c, name, SRC_CDDA, 0, t);
    }
}

/* ---- titles ---------------------------------------------------------------- */
static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1])) s[--n] = 0;
    return s;
}
static void sort_tracks(Category *c) {
    /* The list's order first, the rest by name. */
    Track *t = c->track; unsigned n = c->count;
    char path[4200];
    unsigned placed = 0;
    if (snprintf(path, sizeof path, "%s/addons/tmst/workspace/jukebox/%s", game_root(), c->titles) < (int)sizeof path) {
        FILE *f = fopen(path, "rb");
        if (!f) {               /* the copy the build staged beside the game (the Expanded's own APK) */
            char rel[300];
            snprintf(rel, sizeof rel, "workspace/jukebox/%s", c->titles);
            if (tmst_files_staged(path, sizeof path, rel)) f = fopen(path, "rb");
        }
        char line[1024];
        while (f && fgets(line, sizeof line, f)) {
            char *s = trim(line);
            if (!*s || *s == '#') continue;
            char *eq = strchr(s, '='); if (!eq) continue;
            *eq = 0; char *name = trim(s), *title = trim(eq + 1);
            for (unsigned i = placed; i < n; i++) if (!strcmp(t[i].name, name) ||
                   (!strchr(name, '.') && !strncmp(t[i].name, name, strlen(name)) && !strcmp(t[i].name + strlen(name), ".bgm"))) {
                if (strstr(title, "--disable")) {          /* left out of the list */
                    memmove(t + i, t + i + 1, (n - i - 1) * sizeof *t); n--;
                    break;
                }
                Track tmp = t[i]; memmove(t + placed + 1, t + placed, (i - placed) * sizeof *t); t[placed] = tmp;
                tmst_ui_copy(*title ? title : name, t[placed].title, TITLE);
                placed++; break;
            }
        }
        if (f) fclose(f);
    }
    if (c->only_listed) n = placed;            /* the rest are another category's */
    c->count = n;
    for (unsigned i = placed; i < n; i++) for (unsigned j = i + 1; j < n; j++)
        if (strcmp(t[j].name, t[i].name) < 0) { Track tmp = t[i]; t[i] = t[j]; t[j] = tmp; }
    for (unsigned i = placed; i < n; i++) {    /* no line: the file's name without .bgm */
        snprintf(t[i].title, TITLE, "%s", t[i].name);
        size_t k = strlen(t[i].title);
        if (k > 4 && !strcmp(t[i].title + k - 4, ".bgm")) t[i].title[k - 4] = 0;
    }
}
static void scan(void) {
    for (int ci = 0; ci < CATS; ci++) {
        Category *c = &cats[ci]; c->count = 0;
        if (!category_on(c)) continue;
        if (c->disc_mode >= 0) scan_disc(c);
        for (unsigned d = 0; d < 2; d++) {
            char dir[4200];
            if (!dir_path(c, d, dir, sizeof dir)) continue;
            Scan s = {c, d}; list_bgm(dir, scan_file, &s);
        }
        sort_tracks(c);
    }
}

/* ---- choices ----------------------------------------------------------------- */
typedef struct { char key[KEY]; int cat; char name[NAME]; } Choice;
static Choice choices[MAX_KEYS], saved[MAX_KEYS];
static unsigned choice_count, saved_count;
static int choices_loaded;
static unsigned read_choices(FILE *f, Choice *list) {
    unsigned n = 0;
    char line[512];
    while (f && fgets(line, sizeof line, f) && n < MAX_KEYS) {
        char *s = trim(line);
        if (!*s || *s == '#') continue;
        char *eq = strchr(s, '='); if (!eq) continue;
        *eq = 0; char *key = trim(s), *value = trim(eq + 1), *slash = strchr(value, '/');
        if (!slash || strlen(key) >= KEY) continue;
        *slash = 0; int cat = category_of(value);
        if (cat < 0 || strlen(slash + 1) >= NAME) continue;
        Choice *c = &list[n++];
        snprintf(c->key, KEY, "%s", key); c->cat = cat; snprintf(c->name, NAME, "%s", slash + 1);
    }
    return n;
}
/* The fighters' default choices, the add-on's setup's
 * (addons/tmst/workspace/jukebox-defaults.txt); a player's own choice comes first. */
static Choice defaults[MAX_KEYS];
static unsigned default_count;
static int defaults_loaded;
static Choice *default_of(const char *key) {
    if (!defaults_loaded) {
        defaults_loaded = 1;
        char path[4200];
        FILE *f = snprintf(path, sizeof path, "%s/addons/tmst/workspace/jukebox-defaults.txt", game_root()) < (int)sizeof path
                  ? fopen(path, "rb") : NULL;
        default_count = read_choices(f, defaults);
        if (f) fclose(f);
    }
    for (unsigned i = 0; i < default_count; i++) if (!strcmp(defaults[i].key, key)) return &defaults[i];
    return NULL;
}
static void load_choices(void) {
    choices_loaded = 1;
    char path[4200]; saves_path(path, sizeof path);
    FILE *f = path[0] ? fopen(path, "rb") : NULL;
    choice_count = read_choices(f, choices);
    if (f) fclose(f);
    memcpy(saved, choices, sizeof choices); saved_count = choice_count;
}
static void save_choices(void) {
    char path[4200]; saves_path(path, sizeof path);
    if (!path[0]) return;
    FILE *f = fopen(path, "wb");
    if (!f) { tmst_log("jukebox: cannot write %s", path); return; }
    fprintf(f, "# Jukebox (TMST add-on): each fighter's music (fighter = category/file), written by the game.\n");
    for (unsigned i = 0; i < choice_count; i++) fprintf(f, "%s = %s/%s\n", choices[i].key, cats[choices[i].cat].id, choices[i].name);
    fclose(f);
    memcpy(saved, choices, sizeof choices); saved_count = choice_count;
    tmst_log("jukebox: %u choices saved in %s", choice_count, path);
}
static Choice *choice_of(const char *key) {
    if (!choices_loaded) load_choices();
    for (unsigned i = 0; i < choice_count; i++) if (!strcmp(choices[i].key, key)) return &choices[i];
    return NULL;
}
static void set_choice(const char *key, int cat, const char *name) {
    Choice *c = choice_of(key);
    if (!c) {
        if (choice_count >= MAX_KEYS) return;
        c = &choices[choice_count++]; snprintf(c->key, KEY, "%s", key);
    }
    c->cat = cat; snprintf(c->name, NAME, "%s", name);
}
static void clear_choice(const char *key) {
    Choice *c = choice_of(key);
    if (!c) return;
    unsigned i = (unsigned)(c - choices);
    memmove(choices + i, choices + i + 1, (choice_count - i - 1) * sizeof *choices); choice_count--;
}

/* ---- streams ----------------------------------------------------------------- */
typedef struct {
    int kind;                                 /* 0 none, else the SRC_ + 1 */
    char what[NAME + 16];
    FILE *f; uint32_t frames, loop, pos;      /* a .bgm pack */
    int adpcm; TmstAdpcm adp; TmstXaPack xap; /* ... of ADPCM, 1, or XA, 2 (tmst_adpcm.h) */
    int16_t buf[2048 * 2]; unsigned buf_n, buf_at;
    XaEntry xa; uint32_t sector; int stereo, rate; int32_t hist[2][2];   /* XA */
    int16_t cur[2016 * 2]; int cur_n; int16_t prev[2]; uint32_t step, frac; int at;
    uint32_t cd_first, cd_end, cd_at;         /* CD audio */
    int level;                                /* per 10 000 */
} Stream;
static Stream preview, fight;

static void stream_close(Stream *s) {
    if (s->f) fclose(s->f);
    s->f = NULL; s->kind = 0; s->what[0] = 0; s->adpcm = 0;
}
static int file_path_of(const Category *c, const Track *t, char *out, size_t size) {
    char dir[4200];
    return dir_path(c, t->dir, dir, sizeof dir) && snprintf(out, size, "%s/%s", dir, t->name) < (int)size;
}
static int open_file(Stream *s, const char *path) {
    FILE *f = fopen(path, "rb");
    uint8_t head[TMST_ADPCM_HEAD];
    if (!f) return 0;
    int ok = fread(head, 1, 20, f) == 20;
    int adpcm = !ok ? 0 : !memcmp(head, TMST_ADPCM_MAGIC, 8) ? 1 : !memcmp(head, TMST_XA_MAGIC, 8) ? 2 : 0;
    if (adpcm) ok = fread(head + 20, 1, TMST_ADPCM_HEAD - 20, f) == TMST_ADPCM_HEAD - 20 &&
                    (adpcm == 1 ? tmst_adpcm_open(&s->adp, f, head) : tmst_xa_open(&s->xap, f, head));
    else ok = ok && !memcmp(head, "T3BGM001", 8) && le32(head + 8) == 44100 && le32(head + 12) &&
              le32(head + 16) < le32(head + 12);
    if (!ok) { fclose(f); tmst_log("jukebox: %s is not a music pack", path); return 0; }
    s->f = f; s->frames = le32(head + 12); s->loop = le32(head + 16); s->pos = 0; s->buf_n = s->buf_at = 0;
    s->adpcm = adpcm;
    s->level = FILE_LEVEL; s->kind = SRC_FILE + 1;
    return 1;
}
static void xa_restart(Stream *s) {
    s->sector = s->xa.first + s->xa.channel; s->cur_n = 0; s->at = 0; s->frac = 0;
    memset(s->hist, 0, sizeof s->hist); s->prev[0] = s->prev[1] = 0;
}
static int open_xa(Stream *s, unsigned index) {
    if (!disc_ready() || !xa_entry(index, &s->xa)) return 0;
    uint8_t raw[2352];
    if (!iso_read_raw_sector(disc, xas_lba + s->xa.first + s->xa.channel, raw, 2352) || !(raw[18] & 4)) {
        tmst_log("jukebox: XA %u is not audio", index); return 0;
    }
    s->stereo = raw[19] & 1; s->rate = raw[19] & 4 ? 18900 : 37800;
    s->step = (uint32_t)((uint64_t)s->rate * 65536 / 44100);
    xa_restart(s); s->level = DISC_LEVEL; s->kind = SRC_XA + 1;
    return 1;
}
static int open_cdda(Stream *s, int track) {
    if (!disc_ready() || !cd_track_ok(track)) return 0;
    s->cd_first = iso_track_start_lba(disc, track);
    s->cd_end = track < iso_track_count(disc) ? iso_track_start_lba(disc, track + 1) : iso_sector_count(disc);
    if (s->cd_end <= s->cd_first) return 0;
    s->cd_at = s->cd_first; s->buf_n = s->buf_at = 0; s->level = DISC_LEVEL; s->kind = SRC_CDDA + 1;
    return 1;
}
static int stream_open(Stream *s, int cat, const Track *t) {
    stream_close(s);
    int ok = 0;
    if (t->src == SRC_FILE) { char path[4400]; ok = file_path_of(&cats[cat], t, path, sizeof path) && open_file(s, path); }
    else if (t->src == SRC_XA) ok = open_xa(s, (unsigned)t->index);
    else ok = open_cdda(s, t->index);
    if (ok) snprintf(s->what, sizeof s->what, "%s/%s", cats[cat].id, t->name);
    return ok;
}
/* One XA sector of the stream (its channel's), decoded into cur. */
static const int K0[4] = {0, 60, 115, 98}, K1[4] = {0, 0, -52, -55};
static int xa_sector(Stream *s) {
    for (int tries = 0; tries < 2; tries++) {
        if (s->sector > s->xa.last + s->xa.channel) xa_restart(s);   /* the end: again from the start */
        uint8_t raw[2352];
        uint32_t at = s->sector; s->sector += 8;
        if (s_xa_fp) {
            if (fseeko(s_xa_fp, (off_t)(xas_lba + at) * 2352, SEEK_SET) != 0 ||
                fread(raw, 1, 2352, s_xa_fp) != 2352) return 0;
        } else {
            if (!iso_read_raw_sector(disc, xas_lba + at, raw, 2352)) return 0;
        }
        if (!(raw[18] & 4) || raw[17] != s->xa.channel) continue;     /* not this stream's audio */
        int n = 0;
        for (int g = 0; g < 18; g++) {
            const uint8_t *grp = raw + 24 + g * 128;
            for (int u = 0; u < 8; u++) {
                int ch = s->stereo ? (u & 1) : 0;
                int param = grp[4 + u], shift = param & 15, filter = (param >> 4) & 3;
                if (shift > 12) shift = 9;
                int base = s->stereo ? (g * 4 + u / 2) * 28 : (g * 8 + u) * 28;
                for (int i = 0; i < 28; i++) {
                    int nib = (grp[16 + i * 4 + u / 2] >> ((u & 1) * 4)) & 15;
                    int32_t v = (int32_t)((int16_t)(nib << 12)) >> shift;
                    v += (s->hist[ch][0] * K0[filter] + s->hist[ch][1] * K1[filter] + 32) >> 6;
                    if (v > 32767) v = 32767;
                    if (v < -32768) v = -32768;
                    s->hist[ch][1] = s->hist[ch][0]; s->hist[ch][0] = v;
                    if (s->stereo) s->cur[(base + i) * 2 + ch] = (int16_t)v;
                    else s->cur[(base + i) * 2] = s->cur[(base + i) * 2 + 1] = (int16_t)v;
                }
            }
            n = s->stereo ? (g + 1) * 112 : (g + 1) * 224;
        }
        s->cur_n = n;
        return 1;
    }
    return 0;
}
/* The next 44 100 Hz frame; 0 when the stream cannot go on. */
static int stream_frame(Stream *s, int16_t out[2]) {
    if (s->kind == SRC_FILE + 1 || s->kind == SRC_CDDA + 1) {
        if (s->buf_at >= s->buf_n) {
            s->buf_at = 0; s->buf_n = 0;
            if (s->kind == SRC_FILE + 1 && s->adpcm == 2) {
                s->buf_n = tmst_xa_fill(&s->xap, s->buf, 2048);
            } else if (s->kind == SRC_FILE + 1 && s->adpcm) {
                s->buf_n = tmst_adpcm_fill(&s->adp, s->buf, 2048, &s->buf_at);
            } else if (s->kind == SRC_FILE + 1) {
                if (s->pos >= s->frames) { s->pos = s->loop; fseek(s->f, 20 + (long)s->loop * 4, SEEK_SET); }
                uint32_t want = s->frames - s->pos; if (want > 2048) want = 2048;
                s->buf_n = (unsigned)fread(s->buf, 4, want, s->f); s->pos += s->buf_n;
            } else {
                for (int k = 0; k < 3; k++) {                 /* 3 sectors: 1764 frames */
                    if (s->cd_at >= s->cd_end) s->cd_at = s->cd_first;
                    uint8_t raw[2352];
                    if (!iso_read_raw_sector(disc, s->cd_at++, raw, 2352)) break;
                    memcpy(s->buf + s->buf_n * 2, raw, 2352); s->buf_n += 588;
                }
            }
            if (!s->buf_n) return 0;
        }
        out[0] = s->buf[s->buf_at * 2]; out[1] = s->buf[s->buf_at * 2 + 1]; s->buf_at++;
        return 1;
    }
    if (s->kind == SRC_XA + 1) {
        /* Linear between source frames at-1 (prev for -1) and at. */
        while (s->at >= s->cur_n) {
            if (s->cur_n) { s->prev[0] = s->cur[(s->cur_n - 1) * 2]; s->prev[1] = s->cur[(s->cur_n - 1) * 2 + 1]; }
            s->at -= s->cur_n;
            if (!xa_sector(s)) return 0;
        }
        for (int ch = 0; ch < 2; ch++) {
            int a = s->at > 0 ? s->cur[(s->at - 1) * 2 + ch] : s->prev[ch], b = s->cur[s->at * 2 + ch];
            out[ch] = (int16_t)(a + (int32_t)(b - a) * (int32_t)s->frac / 65536);
        }
        s->frac += s->step; s->at += (int)(s->frac >> 16); s->frac &= 65535;
        return 1;
    }
    return 0;
}
static FILE *pcm_dump;
static void stream_mix(Stream *s, int16_t *out, int frames, int audible, int gain) {
    static int asked;
    if (!asked) { asked = 1; const char *e = getenv("TMST_JUKEBOX_PCM"); if (e && *e) pcm_dump = fopen(e, "wb"); }
    for (int f = 0; f < frames && s->kind; f++) {
        int16_t v[2];
        if (!stream_frame(s, v)) { tmst_log("jukebox: %s stopped (read error)", s->what); stream_close(s); break; }
        if (pcm_dump) { fwrite(v, 2, 2, pcm_dump); if (!(f & 1023)) fflush(pcm_dump); }
        if (!audible) continue;
        for (int ch = 0; ch < 2; ch++) {
            int32_t mixed = out[f * 2 + ch] + v[ch] * gain / 100 * s->level / 10000;
            out[f * 2 + ch] = (int16_t)(mixed > 32767 ? 32767 : mixed < -32768 ? -32768 : mixed);
        }
    }
}

/* ---- a choice's music ------------------------------------------------------- */
/* The track a choice names, if its music is there now (without a full scan). */
static int resolve(const Choice *c, Track *out) {
    const Category *cat = &cats[c->cat];
    memset(out, 0, sizeof *out); snprintf(out->name, NAME, "%s", c->name);
    if (!category_on(cat)) return 0;
    if (cat->disc_mode >= 0) {
        unsigned i; int t;
        if (sscanf(c->name, "xa-%u", &i) == 1) { XaEntry e; out->src = SRC_XA; out->index = (int16_t)i; return disc_ready() && xa_entry(i, &e); }
        if (sscanf(c->name, "cd-track-%d", &t) == 1) { out->src = SRC_CDDA; out->index = (int16_t)t; return disc_ready() && cd_track_ok(t); }
        return 0;
    }
    for (unsigned d = 0; d < 2; d++) {
        if (!cat->dirs[d]) continue;
        char path[4400]; out->src = SRC_FILE; out->dir = (uint8_t)d;
        if (!file_path_of(cat, out, path, sizeof path)) continue;
        FILE *f = fopen(path, "rb");
        if (f) { fclose(f); return 1; }
    }
    return 0;
}
/* The player's choice whose music is there, else the default one. */
static Choice *heard_choice(const char *key) {
    Track t; Choice *c = choice_of(key);
    if (c && resolve(c, &t)) return c;
    c = default_of(key);
    return c && resolve(c, &t) ? c : NULL;
}

/* ---- fighters ------------------------------------------------------------- */
/* A guest's name: <mods/ttt1>/<Key>-T3-label.txt, whatever the key's case. */
static void guest_label(const char *key, char *out, size_t size) {
    snprintf(out, size, "%s", key);
    char dir[4200]; snprintf(dir, sizeof dir, "%smods/ttt1", game_exe_dir());
#ifdef _WIN32
    char pattern[4300]; snprintf(pattern, sizeof pattern, "%s\\%s-T3-label.txt", dir, key);
    WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA(pattern, &fd);     /* case-insensitive on Windows */
    char path[4400] = "";
    if (h != INVALID_HANDLE_VALUE) { snprintf(path, sizeof path, "%s\\%s", dir, fd.cFileName); FindClose(h); }
#else
    char path[4400] = "";
    DIR *d = opendir(dir); struct dirent *e;
    char want[96]; snprintf(want, sizeof want, "%s-T3-label.txt", key);
    while (d && (e = readdir(d))) if (!strcasecmp(e->d_name, want)) { snprintf(path, sizeof path, "%s/%s", dir, e->d_name); break; }
    if (d) closedir(d);
#endif
    FILE *f = path[0] ? fopen(path, "rb") : NULL;
    if (f) { char line[96]; if (fgets(line, sizeof line, f)) snprintf(out, size, "%s", trim(line)); fclose(f); }
    for (char *p = out; *p; p++) *p = (char)toupper((unsigned char)*p);
}
static const char *fighter_key(unsigned id) {
    if (id < STOCK) return stock_keys[id];
    if (id == PANDA_ID) return "panda";
    if (id == TIGER_ID) return "tiger";
    return id >= GUEST_FIRST && tekken3_guest_key ? tekken3_guest_key(id) : NULL;
}
static void fighter_name(unsigned id, char *out, size_t size) {
    if (id < STOCK) { snprintf(out, size, "%s", stock_names[id]); return; }
    if (id == PANDA_ID) { snprintf(out, size, "PANDA"); return; }
    if (id == TIGER_ID) { snprintf(out, size, "TIGER"); return; }
    const char *key = fighter_key(id);
    if (key) guest_label(key, out, size); else snprintf(out, size, "FIGHTER");
}
static unsigned grid_character(unsigned cell) {
    if (tekken3_ttt1_grid_character) return tekken3_ttt1_grid_character(cell);
    return cell < 22 ? psx_mod_read_byte(STOCK_GRID + cell * 6 + 4) / 4 : 22;
}
static unsigned cursor_fighter(void) {
    unsigned x = psx_mod_read_word(GRID_BLOCK + 8), y = psx_mod_read_word(GRID_BLOCK + 12);
    return x < 7 && y < 6 ? grid_character(y * 7 + x) : 0xffff;
}
/* A fighter's music id: byte 11 of its record (id, costume). */
static int record_music(unsigned id, unsigned costume) {
    uint32_t record = psx_mod_read_word(RECORDS + id * 16 + (costume & 3) * 4);
    if (record < 0x80000000u || record > 0x801ffff0u) return -1;
    return psx_mod_read_byte(record + 11);
}

/* ---- the screen ------------------------------------------------------------------ */
static struct {
    int pending, active, view;
    unsigned fighter; char key[KEY]; char name[48];
    int cat, sel, top;                        /* the list: category, chosen row, first row shown */
    int play_cat, play_sel;                   /* the preview's track, -1 none */
    int dialog, from;                         /* the dialog: 0 YES, 1 NO; the view it came from */
    int asks;                                 /* ASK_SAVE (SAVE?) or ASK_RESET (ARE YOU SURE?) */
    int exiting;                              /* Start + Select until the menu */
    unsigned frames, left_at;                 /* left_at: the frame the grid's screen was left */
    uint16_t previous; int held, repeat;
} jb = {.play_cat = -1, .play_sel = -1};
enum { ROWS = 7, COLS = 2 };
static int menu_row = -1;

static void stop_preview(void) { stream_close(&preview); jb.play_cat = jb.play_sel = -1; }
static int first_category(int from, int dir) {
    for (int k = 0; k < CATS; k++) {
        int c = ((from + dir * k) % CATS + CATS) % CATS;
        if (cats[c].count) return c;
    }
    return -1;
}
static int category_count(void) { int n = 0; for (int c = 0; c < CATS; c++) n += cats[c].count > 0; return n; }

static void begin(void) {
    jb.pending = 1; jb.active = 0; jb.exiting = 0;
    tmst_log("jukebox: chosen in the main menu");
}

/* The track a fighter has by default (to mark it): its default choice, else
 * the game's own, its record's music in the Options' music mode. */
static int default_cat = -1, default_sel = -1;
static void find_default(unsigned id, const char *key) {
    default_cat = default_sel = -1;
    Choice *d = default_of(key);
    for (unsigned i = 0; d && i < cats[d->cat].count; i++)
        if (!strcmp(cats[d->cat].track[i].name, d->name)) { default_cat = d->cat; default_sel = (int)i; return; }
    unsigned costume = id == PANDA_ID ? 1 : id == TIGER_ID ? 2 : 0, fid = id == PANDA_ID ? KUMA_ID : id == TIGER_ID ? EDDY_ID : id;
    int music = record_music(fid, costume);
    unsigned mode = psx_mod_read_byte(MUSIC_OPTION);
    if (music < 0 || music >= MUSIC_IDS) return;
    if (mode > 1) mode = 0;
    unsigned b = psx_mod_read_byte(MUSIC_TABLE + (unsigned)music * 4 + mode * 2);
    char name[NAME];
    if (b & 0x80) snprintf(name, sizeof name, "cd-track-%u", b & 0x7f); else snprintf(name, sizeof name, "xa-%02u", b);
    int c = (b & 0x80) || !mode ? category_of("t3-console") : category_of("t3-arcade");
    for (unsigned i = 0; i < cats[c].count; i++) if (!strcmp(cats[c].track[i].name, name)) { default_cat = c; default_sel = (int)i; }
}
/* The fighter's music now: its choice where that music is there (1), else its default (0). */
static int current_of(int *cat, int *sel) {
    *cat = default_cat; *sel = default_sel;
    Choice *ch = choice_of(jb.key);
    if (!ch) return 0;
    for (unsigned i = 0; i < cats[ch->cat].count; i++)
        if (!strcmp(cats[ch->cat].track[i].name, ch->name)) { *cat = ch->cat; *sel = (int)i; return 1; }
    return 0;
}
static void keep_visible(void) {
    int row = jb.sel / COLS;
    if (row < jb.top) jb.top = row;
    if (row >= jb.top + ROWS) jb.top = row - ROWS + 1;
    if (jb.top < 0) jb.top = 0;
}
static void open_list(unsigned id) {
    const char *key = fighter_key(id);
    if (!key || strlen(key) >= KEY) return;
    int first = first_category(0, 1);
    if (first < 0) { tmst_log("jukebox: no music found"); return; }
    jb.fighter = id; snprintf(jb.key, KEY, "%s", key); fighter_name(id, jb.name, sizeof jb.name);
    find_default(id, key);
    int cat, sel; current_of(&cat, &sel);
    jb.cat = cat >= 0 && cats[cat].count ? cat : first;
    jb.sel = cat == jb.cat && sel >= 0 ? sel : 0; jb.top = 0; keep_visible();
    jb.view = VIEW_LIST;
    tmst_log("jukebox: %s (%s), %s", jb.name, jb.key, cats[jb.cat].id);
}
static void exit_to_menu(void) {
    stop_preview(); jb.exiting = 1;
    if (menu_row >= 0) tmst_menu_select_next_time(menu_row);
    tmst_log("jukebox: back to the main menu");
}
static int unsaved(void) {
    if (choice_count != saved_count) return 1;
    for (unsigned i = 0; i < choice_count; i++) {
        unsigned j = 0;
        while (j < saved_count && strcmp(saved[j].key, choices[i].key)) j++;
        if (j == saved_count || saved[j].cat != choices[i].cat || strcmp(saved[j].name, choices[i].name)) return 1;
    }
    return 0;
}
/* SAVE? answered: from the list, back to the grid for the next fighter;
 * from the grid, out to the main menu. */
static void dialog_choice(int yes) {
    if (jb.asks == ASK_RESET) {               /* every fighter back to its default: YES only */
        if (yes) { choice_count = 0; tmst_log("jukebox: every fighter back to its default music"); }
        jb.view = VIEW_GRID;
        return;
    }
    if (yes) save_choices();
    else { memcpy(choices, saved, sizeof choices); choice_count = saved_count; }
    if (jb.from == VIEW_LIST) { stop_preview(); jb.view = VIEW_GRID; }
    else exit_to_menu();
}

static uint16_t screen_input(uint16_t buttons) {
    uint16_t pressed = (uint16_t)~buttons, edges = pressed & jb.previous;   /* previous: last read's released bits */
    jb.previous = buttons;
    int vert = pressed & (B_UP | B_DOWN);                 /* held up / down repeats in the list */
    if (vert && !(edges & (B_UP | B_DOWN))) {
        if (++jb.held > 18 && ++jb.repeat % 4 == 0) edges |= (uint16_t)vert;
    } else jb.held = jb.repeat = 0;
    const uint16_t none = 0xffff;
    if (jb.exiting) return (uint16_t)~(B_START | B_SELECT);              /* the game's own way out */
    if (jb.view == VIEW_DIALOG) {
        if (edges & (B_LEFT | B_RIGHT | B_UP | B_DOWN)) { jb.dialog ^= 1; }
        else if (edges & B_CROSS) dialog_choice(jb.dialog == 0);
        else if (edges & B_CIRCLE) { jb.view = jb.from; }
        return none;
    }
    /* Circle: the list goes back to the grid (its choices kept, saved or not
     * on the way out); the grid leaves, asking SAVE? first when a choice is
     * not saved yet. */
    if (edges & B_CIRCLE) {
        if (jb.view == VIEW_LIST) { stop_preview(); jb.view = VIEW_GRID; return none; }
        if (!unsaved()) { exit_to_menu(); return none; }
        jb.from = jb.view; jb.view = VIEW_DIALOG; jb.asks = ASK_SAVE; jb.dialog = 0; return none;
    }
    if (jb.view == VIEW_GRID) {
        /* The team size prompt, answered for the player: pressed for 4
         * frames, released for 4 (the pad may be read more than once a frame). */
        if (psx_mod_read_word(GRID_BLOCK) == TEAM_SIZE)
            return (jb.frames / 4 & 1 ? buttons : (uint16_t)(buttons & ~B_CROSS)) | (uint16_t)~(B_CROSS | B_UP | B_DOWN | B_LEFT | B_RIGHT);
        if (edges & B_CROSS) { unsigned id = cursor_fighter(); if (fighter_key(id)) open_list(id); return none; }
        /* Every fighter back to its default, asked first (NO highlighted). */
        if ((edges & B_SQUARE) && choice_count) {
            jb.from = VIEW_GRID; jb.view = VIEW_DIALOG; jb.asks = ASK_RESET; jb.dialog = 1;
            return none;
        }
        return buttons | (uint16_t)~(B_UP | B_DOWN | B_LEFT | B_RIGHT | B_L2 | B_R2);   /* the grid's moves and its page switch */
    }
    Category *c = &cats[jb.cat];
    if (edges & (B_L2 | B_R2)) {                         /* L2 / R2: the category */
        int dir = (edges & B_R2) ? 1 : -1, next = first_category(jb.cat + dir, dir);
        if (next >= 0 && next != jb.cat) {
            jb.cat = next;
            int cat, sel; current_of(&cat, &sel);
            jb.sel = cat == jb.cat && sel >= 0 ? sel : 0; jb.top = 0; keep_visible();
        }
        return none;
    }
    int n = (int)c->count, col = jb.sel % COLS, row = jb.sel / COLS, moved = -1;
    if ((edges & B_UP) && row > 0) moved = jb.sel - COLS;
    if (edges & B_DOWN) {
        if (jb.sel + COLS < n) moved = jb.sel + COLS;
        else if ((n - 1) / COLS > row) moved = n - 1;    /* the last row is shorter */
    }
    if ((edges & B_LEFT) && col > 0) moved = jb.sel - 1;
    if ((edges & B_RIGHT) && col + 1 < COLS && jb.sel + 1 < n) moved = jb.sel + 1;
    if (moved >= 0) { jb.sel = moved; keep_visible(); }
    if ((edges & B_TRIANGLE) && c->count) {
        if (jb.play_cat == jb.cat && jb.play_sel == jb.sel) stop_preview();
        else if (stream_open(&preview, jb.cat, &c->track[jb.sel])) {
            jb.play_cat = jb.cat; jb.play_sel = jb.sel;
            tmst_log("jukebox: preview %s", preview.what);
        } else stop_preview();
    }
    if ((edges & B_CROSS) && c->count) {
        set_choice(jb.key, jb.cat, c->track[jb.sel].name);
        tmst_log("jukebox: %s -> %s/%s", jb.key, c->id, c->track[jb.sel].name);
        stop_preview(); jb.view = VIEW_GRID;
    }
    if (edges & B_SQUARE) {
        clear_choice(jb.key);
        tmst_log("jukebox: %s back to its default music", jb.key);
    }
    return none;
}

/* Left the Jukebox: the way-out buttons are kept from the menu until
 * released, or the menu (back on the JUKEBOX row) would take them as a new
 * choice and open it again. */
static int release_wait;
static int team_grid(void);
static uint16_t input(int player, uint16_t buttons) {
    if (!on) return buttons;
    /* Out of the grid already, before the VBlank closes the Jukebox: no more
     * way-out presses. */
    if (jb.active && jb.exiting && !team_grid()) release_wait = 1;
    if (jb.active && release_wait) return buttons | B_START | B_SELECT | B_CROSS | B_CIRCLE;
    if (!jb.active) {
        if (!release_wait) return buttons;
        const uint16_t keys = B_START | B_SELECT | B_CROSS | B_CIRCLE;
        if (player == 0 && (buttons & keys) == keys) release_wait = 0;
        return buttons | keys;
    }
    return player == 0 ? screen_input(buttons) : 0xffff;
}

/* ---- the screen's drawing ----------------------------------------------------- */
/* As the Jukebox always looked: JUKEBOX in the selector's own title font on a
 * red band over the Team Battle title, a half see-through dark panel over the
 * teams under the grid (the game's primitives there moved out of sight: the
 * team panels, their frames, the size prompt, the name; the full-width
 * backdrop stays and shows through), the category's name in the title font
 * between two swaying arrows with L2 / R2, and under it the list, the hints
 * and the dialog in the UI font (Fira Sans) as GPU primitives (screen_text
 * below). In the grid view (choosing a fighter) the page switch of the
 * Expanded's grid (L2 / R2) stays.
 *
 * The title font: page (832,0) 4 bpp, palette (336,505), cells of 14 x 24
 * from u 172, six to a row: 1..9, A..Z, !. The game draws it every 13 px;
 * a primitive's colour multiplies it (128 = as is). It is the selector's: in
 * VRAM on the selector screens only, where the Jukebox is. */
enum { CENTRE_X = 184, FONT_PAGE = 832 / 64, FONT_CLUT = (505 << 6) | (336 / 16), FONT_U = 172,
       GLYPH_W = 14, GLYPH_H = 24, ADVANCE = 13, OUTLINE = 0x101010, ARROW_RED = 0x3040f0,
       JB_PANEL_TOP = 256, JB_TITLE_Y = 262, BANNER_TOP = 24, BANNER_H = 32, JB_PANEL = 0x140c10, JB_BANNER = 0x1018a8,
       HIDE_FROM_Y = 256, BACKDROP_W = 300 };
static int font_glyph(char c) {
    if (c >= '1' && c <= '9') return c - '1';
    if (c >= 'A' && c <= 'Z') return 9 + c - 'A';
    return c == '!' ? 35 : -1;
}
static uint32_t font_uv(int i, int du, int dv) {
    int u = FONT_U + i % 6 * GLYPH_W + du;
    return (uint32_t)(u > 255 ? 255 : u) | (uint32_t)(i / 6 * GLYPH_H + dv) << 8;
}
/* A line of the title font centred on cx, scaled by num/den, tinted. */
static void font_text(TmstPrims *k, const char *s, int cx, int y, int num, int den, uint32_t tint) {
    int n = (int)strlen(s), advance = ADVANCE * num / den, w = GLYPH_W * num / den, h = GLYPH_H * num / den;
    int x = cx - (n * advance + (w - advance)) / 2;
    for (; *s; s++, x += advance) {
        int i = font_glyph(*s);
        if (i < 0) continue;
        tmst_prim_room(k, 9);
        tmst_prim_word(k, 0x2c000000u | tint);                      /* textured quad, tinted */
        tmst_prim_word(k, tmst_xy(x, y)); tmst_prim_word(k, font_uv(i, 0, 0) | (uint32_t)FONT_CLUT << 16);
        tmst_prim_word(k, tmst_xy(x + w, y)); tmst_prim_word(k, font_uv(i, GLYPH_W, 0) | (uint32_t)FONT_PAGE << 16);
        tmst_prim_word(k, tmst_xy(x, y + h)); tmst_prim_word(k, font_uv(i, 0, GLYPH_H));
        tmst_prim_word(k, tmst_xy(x + w, y + h)); tmst_prim_word(k, font_uv(i, GLYPH_W, GLYPH_H));
    }
}
static void triangle(TmstPrims *k, uint32_t colour, int x0, int y0, int x1, int y1, int x2, int y2) {
    tmst_prim_room(k, 4);
    tmst_prim_word(k, 0x20000000u | colour);
    tmst_prim_word(k, tmst_xy(x0, y0)); tmst_prim_word(k, tmst_xy(x1, y1)); tmst_prim_word(k, tmst_xy(x2, y2));
}
/* An arrow, its tip at x pointing left (dir > 0) or right (dir < 0), outlined. */
static void arrow(TmstPrims *k, int x, int y, int dir, uint32_t colour) {
    triangle(k, OUTLINE, x - 2 * dir, y, x + 10 * dir, y - 9, x + 10 * dir, y + 9);
    triangle(k, colour, x, y, x + 8 * dir, y - 6, x + 8 * dir, y + 6);
}
static void flat(TmstPrims *k, uint32_t colour, int x, int y, int w, int h) {
    tmst_prim_room(k, 3);
    tmst_prim_word(k, 0x60000000u | colour); tmst_prim_word(k, tmst_xy(x, y)); tmst_prim_word(k, tmst_xy(w, h));
}
/* A band shaded from colour a (left) to b (right), opaque. */
static void shaded_band(TmstPrims *k, int x0, int x1, int y, int h, uint32_t a, uint32_t b) {
    tmst_prim_room(k, 8);
    tmst_prim_word(k, 0x38000000u | a); tmst_prim_word(k, tmst_xy(x0, y));
    tmst_prim_word(k, b);               tmst_prim_word(k, tmst_xy(x1, y));
    tmst_prim_word(k, a);               tmst_prim_word(k, tmst_xy(x0, y + h));
    tmst_prim_word(k, b);               tmst_prim_word(k, tmst_xy(x1, y + h));
}
static void panel(TmstPrims *k, int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    tmst_prim_room(k, 3);
    tmst_prim_word(k, 0x62000000u | JB_PANEL); tmst_prim_word(k, tmst_xy(x, y)); tmst_prim_word(k, tmst_xy(w, h));
}
static int list_view(void) { return jb.view == VIEW_LIST || (jb.view == VIEW_DIALOG && jb.from == VIEW_LIST); }
static int category_count(void);
static void screen_text(TmstPrims *k);
static void jukebox_prims(TmstPrims *k) {
    if (!jb.active) return;                      /* another mod's picture */
    /* The panel: see-through, half the backdrop, half dark (B/2 + F/2),
     * from under the grid to the screen's bottom. */
    tmst_prim_room(k, 1);
    tmst_prim_word(k, 0xe1000600u | FONT_PAGE);
    panel(k, -128, JB_PANEL_TOP, 624, 480 - JB_PANEL_TOP);
    /* The band over the title, the screen's whole width: red at its edges,
     * black behind JUKEBOX, shaded between. */
    /* Solid red past the 4:3 edges (the 16:9 screen's sides), shaded from
     * the 4:3 edge to the black behind the word. */
    int inner = 58;
    flat(k, JB_BANNER, -128, BANNER_TOP, 128, BANNER_H);
    shaded_band(k, 0, CENTRE_X - inner, BANNER_TOP, BANNER_H, JB_BANNER, 0x000000);
    flat(k, 0x000000, CENTRE_X - inner, BANNER_TOP, 2 * inner, BANNER_H);
    shaded_band(k, CENTRE_X + inner, 368, BANNER_TOP, BANNER_H, 0x000000, JB_BANNER);
    flat(k, JB_BANNER, 368, BANNER_TOP, 128, BANNER_H);
    font_text(k, "JUKEBOX", CENTRE_X, 28, 1, 1, 0x808080);
    screen_text(k);
    if (!list_view()) return;
    const char *name = cats[jb.cat].name;
    int n = (int)strlen(name), num = n > 20 ? 1 : 3, den = n > 20 ? 2 : 5;
    font_text(k, name, CENTRE_X, JB_TITLE_Y, num, den, 0x808080);
    if (category_count() > 1) {
        int half = (n * ADVANCE * num / den) / 2 + 16, ay = JB_TITLE_Y + GLYPH_H * num / den / 2;
        int t = (int)(jb.frames % 60), sway = (t < 30 ? t : 60 - t) / 8;
        arrow(k, CENTRE_X - half - sway, ay, 1, ARROW_RED);
        arrow(k, CENTRE_X + half + sway, ay, -1, ARROW_RED);
        font_text(k, "L2", CENTRE_X - half - 30, ay - 6, 1, 2, 0x808080);
        font_text(k, "R2", CENTRE_X + half + 30, ay - 6, 1, 2, 0x808080);
    }
}

/* The game's primitives lying wholly below the grid go off screen (their
 * vertices to -512, -512), but the full-width backdrop. */
static int vertex_x(uint32_t w) { return (int16_t)(w & 0xffff); }
static int vertex_y(uint32_t w) { return (int16_t)(w >> 16); }
static void hide_below_grid(uint32_t node) {
    if (!jb.active) return;                      /* another mod's picture */
    unsigned words = psx_mod_read_word(node) >> 24;
    for (unsigned i = 1; i <= words;) {
        uint32_t a = node + i * 4, cmd = psx_mod_read_word(a) >> 24;
        unsigned size = 1, pos[16], count = 0;
        int width = 0;
        if (cmd >= 0x20 && cmd < 0x40) {                 /* polygons */
            unsigned verts = cmd & 8 ? 4 : 3, gouraud = (cmd & 0x10) != 0, tex = (cmd & 4) != 0, w = 1;
            for (unsigned v = 0; v < verts; v++) {
                if (v && gouraud) w++;
                pos[count++] = w++;
                if (tex) w++;
            }
            size = w;
        } else if (cmd >= 0x60 && cmd < 0x80) {          /* rectangles and sprites */
            pos[count++] = 1;
            size = 2 + ((cmd & 4) ? 1 : 0) + ((cmd & 0x18) == 0 ? 1 : 0);
            if ((cmd & 0x18) == 0 && i + size - 1 <= words) width = (int)(psx_mod_read_word(a + (size - 1) * 4) & 0x3ff);
        } else if (cmd >= 0x40 && cmd < 0x60) {          /* lines, poly-lines up to their end mark */
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
        int low = 1, minx = 4096, maxx = -4096;
        for (unsigned v = 0; v < count; v++) {
            uint32_t word = psx_mod_read_word(a + pos[v] * 4);
            if (vertex_y(word) < HIDE_FROM_Y) low = 0;
            if (vertex_x(word) < minx) minx = vertex_x(word);
            if (vertex_x(word) > maxx) maxx = vertex_x(word);
        }
        if (count > 1) width = maxx - minx;
        if (count && low && width < BACKDROP_W)
            for (unsigned v = 0; v < count; v++) psx_mod_write_word(a + pos[v] * 4, tmst_xy(-512, -512));
        i += size;
    }
}

/* ---- the list, the hints and the dialog ------------------------------------------ */
/* Drawn by the GPU each frame, in screen units of the 368 x 480 screen, over
 * the panel: the UI font's SCREEN face (tmst_ui_prim_text: capitals 12 tall,
 * two texels to a unit, sharp where the renderer draws at a higher
 * resolution) with the pad's symbols among its glyphs, and flat rectangles.
 * No picture of our own: the roster's HD faces (the Expanded 1.5.1's stock
 * ones too) keep the upper VRAM half, and the text stays as sharp with them
 * (issue #10: a picture narrowed for them was too small and soft to read).
 * The list: two columns of ROWS (cut with an ellipsis to the column); the
 * fighter and its music now above it, the buttons under it. */
enum { X0 = 10, X1 = 358, HEAD_Y = 285, SEP_Y = 302, LIST_Y = 309, PITCH = 18, HINT_SEP_Y = 437, HINT_Y = 443,
       GRID_TEXT_Y = 304, GRID_HINT_SEP_Y = 384, GRID_HINT_Y = 390, COL_W = (X1 - X0) / COLS, MARK_W = 11,
       C_LINE = 0x5a4a6a, C_CHOSEN_BAR = 0x222a6b };
/* The tint for an ARGB colour (0xBBGGRR, 128 = as the glyph is). */
static uint32_t tint(uint32_t argb) {
    uint32_t r = (argb >> 16 & 255) * 128 / 255, g = (argb >> 8 & 255) * 128 / 255, b = (argb & 255) * 128 / 255;
    return b << 16 | g << 8 | r;
}
static int say(TmstPrims *k, int x, int y, const char *s, uint32_t argb) { return tmst_ui_prim_text(k, x, y, s, 1, tint(argb)); }
static int say_w(const char *s) { return tmst_ui_prim_width(s, 1); }
/* `s` cut with an ellipsis to max_w units. */
static void fit(const char *s, int max_w, char *out, size_t size) { tmst_ui_fit(&tmst_ui_face_screen, s, max_w * 2, out, size); }
static void say_fit(TmstPrims *k, int x, int y, const char *s, int max_w, uint32_t argb) {
    char buf[TITLE + 8];
    fit(s, max_w, buf, sizeof buf);
    say(k, x, y, buf, argb);
}
static void say_centred(TmstPrims *k, int cx, int y, const char *s, uint32_t argb) { say(k, cx - say_w(s) / 2, y, s, argb); }
static void rule(TmstPrims *k, int y) { flat(k, C_LINE, X0, y, X1 - X0, 1); }

/* A button line, centred: each an icon (or L2/R2) and its label. */
typedef struct { const char *icon; uint32_t colour; const char *label; } Hint;
#define L2_R2 "L2/R2"
static void hints(TmstPrims *k, int y, const Hint *h, int n) {
    int w = 0, gap = 12;
    for (int i = 0; i < n; i++) w += say_w(h[i].icon) + 3 + say_w(h[i].label);
    if (n > 1 && w + gap * (n - 1) > X1 - X0) gap = (X1 - X0 - w) / (n - 1);
    if (gap < 4) gap = 4;
    int x = CENTRE_X - (w + gap * (n - 1)) / 2;
    for (int i = 0; i < n; i++) {
        x += say(k, x, y, h[i].icon, h[i].colour) + 3;
        x += say(k, x, y, h[i].label, 0xffd8d0c8u) + gap;
    }
}
static const uint32_t C_TRIANGLE = 0xff3ecf8eu, C_CIRCLE = 0xffff6b6bu, C_CROSS = 0xff7aa7ffu, C_SQUARE = 0xffff8fd8u,
                      C_PLAIN = 0xffe8dccfu, C_PLAYING = 0xff57e38bu, C_CURRENT = 0xffffcb83u;

static void list_text(TmstPrims *k) {
    Category *c = &cats[jb.cat];
    int cur_cat, cur_sel, chosen_now = current_of(&cur_cat, &cur_sel);
    char head[TITLE + 32];
    int name_w = say(k, X0, HEAD_Y, jb.name, C_CURRENT);
    const char *now = cur_cat >= 0 && cur_sel >= 0 ? cats[cur_cat].track[cur_sel].title : "Tekken 3's own";
    snprintf(head, sizeof head, "Now: %s%s", now, chosen_now ? "" : " (default)");
    char fitted[TITLE + 40];
    fit(head, X1 - X0 - name_w - 16, fitted, sizeof fitted);
    say(k, X1 - say_w(fitted), HEAD_Y, fitted, 0xffa89c94u);
    rule(k, SEP_Y);
    for (int r = 0; r < ROWS; r++) for (int col = 0; col < COLS; col++) {
        int i = (jb.top + r) * COLS + col;
        if (i >= (int)c->count) continue;
        int x = X0 + col * COL_W, y = LIST_Y + r * PITCH;
        int chosen = i == jb.sel, playing = jb.play_cat == jb.cat && jb.play_sel == i, current = cur_cat == jb.cat && cur_sel == i;
        if (chosen) flat(k, C_CHOSEN_BAR, x - 3, y - 3, COL_W - 4, PITCH);
        if (playing) say(k, x, y, TMST_UI_ICON_PLAY, C_PLAYING);
        else if (current) say(k, x + 1, y + 2, "*", C_CURRENT);
        say_fit(k, x + MARK_W, y, c->track[i].title, COL_W - MARK_W - 8,
                playing ? C_PLAYING : chosen ? 0xfffff4e8u : current ? C_CURRENT : 0xffbdb2aau);
    }
    int rows = ((int)c->count + COLS - 1) / COLS, arrow_x = CENTRE_X - say_w(TMST_UI_ICON_UP) / 2;
    if (jb.top > 0) say(k, arrow_x, SEP_Y - 6, TMST_UI_ICON_UP, 0xfff8d830u);
    if (jb.top + ROWS < rows) say(k, arrow_x, HINT_SEP_Y - 6, TMST_UI_ICON_DOWN, 0xfff8d830u);
}
/* The dialog, centred between top and bottom (the rules around the list, or
 * the grid view's lines). */
static void dialog_text(TmstPrims *k, int top, int bottom) {
    int w = 200, h = 66, x = CENTRE_X - w / 2, y = top + (bottom - top - h) / 2;
    flat(k, 0x2921b5, x - 2, y - 2, w + 4, h + 4);
    flat(k, 0x18121c, x, y, w, h);
    say_centred(k, CENTRE_X, y + 8, jb.asks == ASK_RESET ? "Restore all to default?" : "Save?", 0xfff0e6dau);
    if (jb.asks == ASK_RESET) say_centred(k, CENTRE_X, y + 24, "Are you sure?", C_CURRENT);
    static const char *const opt[2] = {"Yes", "No"};
    for (int i = 0; i < 2; i++) {
        int bw = 64, bx = CENTRE_X - 76 + i * 88, by = y + 42;
        flat(k, i == jb.dialog ? C_CHOSEN_BAR : 0x26202a, bx, by, bw, 18);
        say_centred(k, bx + bw / 2, by + 3, opt[i], i == jb.dialog ? 0xfffff4e8u : 0xffa89c94u);
    }
}
static void screen_text(TmstPrims *k) {
    if (jb.view == VIEW_GRID) {
        say_centred(k, CENTRE_X, GRID_TEXT_Y, "Choose a fighter", 0xfff0e6dau);
        say_centred(k, CENTRE_X, GRID_TEXT_Y + 18, "to pick the music it plays", 0xffa89c94u);
    } else if (list_view()) list_text(k);
    int grid = !list_view(), hint_y = grid ? GRID_HINT_Y : HINT_Y;
    rule(k, grid ? GRID_HINT_SEP_Y : HINT_SEP_Y);
    if (jb.view == VIEW_DIALOG) {
        if (grid) dialog_text(k, GRID_TEXT_Y - 10, GRID_HINT_SEP_Y); else dialog_text(k, SEP_Y, HINT_SEP_Y);
        Hint keys[] = {{TMST_UI_ICON_LEFT_RIGHT, C_PLAIN, "Choose"}, {TMST_UI_ICON_CROSS, C_CROSS, "Confirm"},
                       {TMST_UI_ICON_CIRCLE, C_CIRCLE, "Back"}};
        hints(k, hint_y, keys, 3);
    } else if (jb.view == VIEW_GRID) {
        /* L2 / R2 switch the Expanded's grid pages (its TTT1 roster); with
         * Unified Character Select on there is one grid, nothing to switch. */
        Hint h[] = {{L2_R2, C_PLAIN, "Switch character panel"}, {TMST_UI_ICON_CROSS, C_CROSS, "Select"},
                    {TMST_UI_ICON_SQUARE, C_SQUARE, "Restore all to default"}, {TMST_UI_ICON_CIRCLE, C_CIRCLE, "Exit"}};
        int pages = tekken3_ttt1_selector_page && tekken3_ttt1_selector_page() >= 0 &&
                    !(tmst_unified_select_active && tmst_unified_select_active());
        hints(k, hint_y, pages ? h : h + 1, pages ? 4 : 3);
    } else {
        Hint all[] = {{L2_R2, C_PLAIN, "Category"}, {TMST_UI_ICON_TRIANGLE, C_TRIANGLE, "Play / Stop"},
                      {TMST_UI_ICON_CROSS, C_CROSS, "Assign"}, {TMST_UI_ICON_SQUARE, C_SQUARE, "Restore default"},
                      {TMST_UI_ICON_CIRCLE, C_CIRCLE, "Back"}};
        int many = category_count() > 1;
        hints(k, hint_y, many ? all : all + 1, many ? 5 : 4);
    }
}

/* ---- sound -------------------------------------------------------------------- */
/* The fight's choice: armed when the game starts the opponent's music, it
 * goes as the game's own music goes. The game starts a stand-in piece in its
 * place (fight_id, muted): its music state, 0x800A05D8 (state 2 / 3 while an
 * XA plays, +0x48 the piece, +0x44 1 once audible, +0x54 / +0x58 a resume
 * point, +0xB4 the level its fades ramp, of 127 x the piece's +0xB2 x the
 * master +0xB0 / 16129), starts, fades, pauses and stops ours. A stand-in,
 * not the fight's own piece: the Expanded plays a TTT stage's music over
 * that one (src/tekken3_ttt_music.c). */
enum { M_STATE = 0x800a05d8, M_AUDIBLE = 0x800a061c, M_CUR = 0x800a0620, M_RESUME_AT = 0x800a062c,
       M_RESUME = 0x800a0630, M_MASTER = 0x800a0688, M_VOLUME = 0x800a068a, M_LEVEL = 0x800a068c,
       ROUND_MUSIC = 0x800add08 };
static Choice fight_choice;
static int fight_armed, fight_id = -1, fight_seen, fight_paused, fight_waiting;
static int lead_in;                           /* tmst_jukebox_lead_in: streaming ahead of the game's piece */
static void fight_stop(void) {
    stream_close(&fight); fight_armed = 0; fight_id = -1; lead_in = 0;
    if (!jb.active) tmst_spu_mute_game_music(0);
}
/* A piece the game loops from the XA in both music modes, not the fight's
 * own or the one asked for: the stand-in. -1 none. */
static int stand_in(int id) {
    unsigned round = psx_mod_read_byte(ROUND_MUSIC);
    for (unsigned k = 0; k < MUSIC_IDS; k++) {
        if ((int)k == id || k == round) continue;
        int ok = 1;
        for (unsigned m = 0; m < 2 && ok; m++) {
            unsigned b = psx_mod_read_byte(MUSIC_TABLE + k * 4 + m * 2), v = psx_mod_read_byte(MUSIC_TABLE + k * 4 + m * 2 + 1);
            XaEntry e;
            ok = !(b & 0x80) && b < XA_COUNT && (v & 0x80) && (v & 0x7f) && xa_entry(b, &e);
        }
        if (ok) return (int)k;
    }
    return -1;
}

static void mix(int16_t *out, int frames, int audible, int gain) {
    if (!hooked) return;
    if (jb.active) { stream_mix(&preview, out, frames, audible, gain); return; }
    if (!fight_armed) return;
    /* The lead-in: ours alone, at the player's music volume, until the
     * game's start of the fight's music takes it over (music()). */
    if (lead_in) {
        if (!fight.kind) {
            Track t;
            if (!resolve(&fight_choice, &t) || !stream_open(&fight, fight_choice.cat, &t)) { fight_stop(); lead_in = 0; return; }
            tmst_log("jukebox: %s plays %s (lead-in)", fight_choice.key, fight.what);
        }
        stream_mix(&fight, out, frames, audible, gain);
        return;
    }
    int cur = (int16_t)psx_mod_read_half(M_CUR), state = psx_mod_read_half(M_STATE);
    if (cur != fight_id || (state != 2 && state != 3)) {
        if (!fight_seen) return;                          /* not started yet */
        /* Ours starting again (state 1), from the resume point a stop kept:
         * Mokujin's style loading between rounds (issue #15). Ours waits;
         * stopping here unmuted the stand-in (the select screen's music). */
        if (cur == fight_id && state == 1) return;
        /* Stopped keeping a resume point (0x8006B54C(1)): ours waits with it. */
        int kept = cur < 0 && (int32_t)psx_mod_read_word(M_RESUME) == fight_id && (int32_t)psx_mod_read_word(M_RESUME_AT) > 0;
        if (kept) { if (!fight_paused) { fight_paused = 1; tmst_log("jukebox: %s paused with the game's music", fight.what); } return; }
        tmst_log("jukebox: %s stops with the game's music", fight.what[0] ? fight.what : fight_choice.key);
        fight_stop();
        return;
    }
    if (!fight.kind) {
        Track t;
        if (!resolve(&fight_choice, &t) || !stream_open(&fight, fight_choice.cat, &t)) { fight_stop(); return; }
        fight_waiting = 1;
        tmst_log("jukebox: %s plays %s", fight_choice.key, fight.what);
    }
    if (fight_paused) {                                   /* resumed: audible again with the game's */
        fight_waiting = 1;
        tmst_log("jukebox: %s goes on with the game's music", fight.what);
    }
    fight_seen = 1; fight_paused = 0;
    if (fight_waiting) {
        if ((int16_t)psx_mod_read_half(M_AUDIBLE) != 1) return;   /* the game's not audible yet */
        fight_waiting = 0;
    }
    /* The game's level now of the piece's full: its fades and the pause's dip. */
    int full = psx_mod_read_half(M_VOLUME) * psx_mod_read_half(M_MASTER) / 127, level = (int16_t)psx_mod_read_half(M_LEVEL);
    int fade = full > 0 && level > 0 ? (level >= full ? 100 : level * 100 / full) : 0;
    stream_mix(&fight, out, frames, audible, gain * fade / 100);
}

/* The game starts a piece: in a fight, the opponent's (P2's: Arcade's CPU,
 * whose stage it is) with a Jukebox choice plays that choice instead, as the
 * fight's music (0x800ADD08: the game's choice, or the one the Expanded's
 * stage select gave the chosen stage) or the opponent's record piece. The
 * Jukebox follows the opponent, never the stage; without a choice the
 * game's own piece plays. */
/* The opponent's choice for its fight (its own, else a tournament's piece),
 * NULL none; *key its key. */
static Choice *opponent_choice(const char **key) {
    unsigned fid = psx_mod_read_half(P2_ID), costume = psx_mod_read_byte(P2_COSTUME);
    Choice *c = NULL;
    *key = NULL;
    /* Kuma's second costume is Panda, Eddy's third Tiger: their own choice
     * first, then their fighter's. The fighters' choices are the Jukebox's:
     * without it (the player only, issue #9.2) none. */
    if (on) {
        if (fid == KUMA_ID && costume == 1 && (c = heard_choice("panda"))) *key = "panda";
        else if (fid == EDDY_ID && costume == 2 && (c = heard_choice("tiger"))) *key = "tiger";
        else if ((*key = fighter_key(fid))) c = heard_choice(*key);
    }
    /* Tournament Mode's match (src/tournament/tmst_tournament.c): its
     * piece, unless the player chose one for this opponent. */
    const char *t_key, *t_cat, *t_name;
    static Choice tournament;
    if (!(c && *key && c == choice_of(*key)) && tmst_tournament_fight_music(&t_key, &t_cat, &t_name)) {
        Track t;
        memset(&tournament, 0, sizeof tournament);
        snprintf(tournament.key, KEY, "%s", t_key);
        snprintf(tournament.name, NAME, "%s", t_name);
        tournament.cat = category_of(t_cat);
        if (tournament.cat >= 0 && resolve(&tournament, &t)) { c = &tournament; *key = tournament.key; }
        else tmst_log("jukebox: the tournament's %s/%s is not there, %s", t_cat, t_name,
                      c ? "the default choice plays" : "the game's piece plays");
    }
    /* A Tekken 2 stage of the STAGE line (src/tekken2/tmst_tekken2_stages.c):
     * its own piece, unless the player chose one for this opponent. */
    static Choice stage_piece;
    if (!(c && *key && c == choice_of(*key)) && c != &tournament && tmst_tekken2_fight_music(&t_key, &t_cat, &t_name)) {
        Track t;
        memset(&stage_piece, 0, sizeof stage_piece);
        snprintf(stage_piece.key, KEY, "%s", t_key);
        snprintf(stage_piece.name, NAME, "%s", t_name);
        stage_piece.cat = category_of(t_cat);
        if (stage_piece.cat >= 0 && resolve(&stage_piece, &t)) { c = &stage_piece; *key = stage_piece.key; }
        else tmst_log("jukebox: the Tekken 2 stage's %s/%s is not there (Tekken 2 Content's Music off?), %s", t_cat,
                      t_name, c ? "the default choice plays" : "the game's piece plays");
    }
    return c;
}
/* The lead-in (tmst_core.h): the coming fight's piece streamed ahead of the
 * game's, at the player's music volume, until the game starts that fight's
 * music (music(): the same choice, taken over unbroken). */
int tmst_jukebox_lead_in(void) {
    if (!hooked || jb.active) return 0;
    const char *key;
    Choice *c = opponent_choice(&key);
    if (!c) return 0;
    fight_stop();
    fight_choice = *c; fight_id = -1; fight_seen = fight_paused = fight_waiting = 0; fight_armed = 1; lead_in = 1;
    tmst_log("jukebox: %s's music %s/%s leads in", key, cats[c->cat].id, c->name);
    return 1;
}

static int music(int id) {
    if (!hooked) return id;
    if (jb.active) return -1;                             /* the preview only, while open */
    unsigned screen = psx_mod_read_word(SCREEN);
    if (screen != FIGHT && screen != LOADING) { if (fight_armed) fight_stop(); lead_in = 0; return id; }
    unsigned fid = psx_mod_read_half(P2_ID), costume = psx_mod_read_byte(P2_COSTUME);
    Choice *c = NULL;
    const char *key = NULL;
    if (id == (int)psx_mod_read_byte(ROUND_MUSIC) || record_music(fid, costume) == id) c = opponent_choice(&key);
    /* The lead-in's piece: the game's start takes it over, the stream goes on. */
    if (lead_in && c && fight_armed && !strcmp(fight_choice.key, c->key) && fight_choice.cat == c->cat &&
        !strcmp(fight_choice.name, c->name)) {
        int stand = stand_in(id);
        lead_in = 0;
        if (stand >= 0) {
            fight_id = stand; fight_seen = 1; fight_waiting = 0;
            tmst_spu_mute_game_music(1);
            tmst_log("jukebox: %s's lead-in goes on as the fight's music (the game plays %d, muted)", key, stand);
            return stand;
        }
        fight_stop();
    }
    lead_in = 0;
    if (!c) { if (fight_armed) fight_stop(); return id; }
    /* The same choice again (the next round's setup): the game keeps its
     * piece playing, and so do we. */
    if (fight_armed && !strcmp(fight_choice.key, c->key) && fight_choice.cat == c->cat && !strcmp(fight_choice.name, c->name))
        return fight_id;
    int stand = stand_in(id);
    if (stand < 0) { tmst_log("jukebox: no stand-in piece, the game's plays"); if (fight_armed) fight_stop(); return id; }
    fight_stop();
    fight_choice = *c; fight_id = stand; fight_seen = fight_paused = fight_waiting = 0; fight_armed = 1;
    tmst_spu_mute_game_music(1);
    tmst_log("jukebox: %s's music is %s/%s (the game plays %d for it, muted)", key, cats[c->cat].id, c->name, stand);
    return stand;
}

/* ---- the frame ------------------------------------------------------------------ */
static int team_grid(void) { return psx_mod_read_word(SCREEN) == 10 && psx_mod_read_word(MODE) == TEAM_BATTLE; }
static void fight_screens(unsigned screen);
static void tick(void) {
    if (!on) return;
    tmst_menu_vblank();
    tmst_overlay_vblank();
    jb.frames++;
    unsigned screen = psx_mod_read_word(SCREEN);
    if (jb.pending && team_grid()) {
        jb.pending = 0; jb.active = 1; jb.view = VIEW_GRID; jb.exiting = 0; jb.frames = 0; jb.previous = 0xffff;
        scan(); load_choices(); stop_preview(); fight_stop();
        tmst_spu_mute_game_music(1);
        tmst_log("jukebox: open, %d categories", category_count());
    }
    /* Close the Jukebox overlay immediately once Team Battle's grid is left
     * so its header/footer do not float over the loading transition. */
    if (jb.active && !team_grid()) {
        jb.left_at = 0;
        jb.active = 0; jb.view = VIEW_OFF; stop_preview(); tmst_overlay_hide();
        tmst_spu_mute_game_music(0);
        release_wait = jb.exiting;
        tmst_log("jukebox: closed");
    }
    /* Shown until the grid's screen is left, the way out included: the
     * Team Battle screen never shows through. */
    if (jb.active) tmst_overlay_show_prims();
    if (jb.pending && screen == 4 && psx_mod_read_half(PHASE) == 1) jb.pending = 0;   /* the menu again, never left */
    fight_screens(screen);
}

/* A fight's choice ends with the fight's screens. */
static void fight_screens(unsigned screen) {
    if (fight_armed && screen != FIGHT && screen != LOADING && screen != 9 && screen != 10) fight_stop();
}

static void player_hooks(void) {
    if (hooked) return;
    hooked = 1;
    tmst_music_add_hook(music);
    tmst_spu_add_mixer(mix);
}
/* The player without the Jukebox (tmst_core.h, issue #9.2): Tekken 2's
 * stages and Tournament Mode play their own pieces with the Jukebox off too.
 * The Expanded runs only a switched-on feature's plugins, so its
 * activation and VBlank come from theirs. */
void tmst_jukebox_player_on(const char *who) {
    if (hooked || !tmst_guard("tmst.jukebox", TMST_NEED_ACTIVATION | TMST_NEED_VBLANK | TMST_NEED_GUEST_MEMORY)) return;
    player_hooks();
    tmst_log("jukebox: its player on for %s (the stage's and the tournament's pieces)", who);
}
void tmst_jukebox_player_tick(void) {
    if (hooked && !on) fight_screens(psx_mod_read_word(SCREEN));
}

static void activate(void) {
    on = tmst_guard("tmst.jukebox", TMST_NEED_ACTIVATION | TMST_NEED_VBLANK | TMST_NEED_GUEST_MEMORY | TMST_NEED_HIGH_VRAM);
    if (!on) return;
    menu_row = tmst_menu_add_row("JUKEBOX", TEAM_BATTLE, begin);
    if (menu_row < 0) tmst_log("jukebox: no main-menu row (no room)");
    tmst_menu_set_colour(menu_row, 0xA8E020);                  /* lime */
    tmst_input_add_filter(input);
    player_hooks();
    tmst_overlay_set_prims(jukebox_prims);
    tmst_overlay_prepare();
    tmst_overlay_set_visitor(hide_below_grid);
}

PSX_MOD_CONSTRUCTOR(tmst_register_jukebox) {
    (void)psx_mod_register_activation_plugin("tmst.jukebox", activate);
    (void)psx_mod_register_vblank_plugin("tmst.jukebox", tick);
}
