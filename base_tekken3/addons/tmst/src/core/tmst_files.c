/* TMST core: where the add-on's files go (tmst_files.h). */
#include "tmst_files.h"
#include "psx_sdl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#define make_dir(p) _mkdir(p)
#else
#define make_dir(p) mkdir((p), 0755)
#endif

static char root[4096];
static char game_dir[4096];

const char *tmst_files_game_dir(void) {
    if (game_dir[0]) return game_dir;
    const char *base = SDL_GetBasePath();
    snprintf(game_dir, sizeof game_dir, "%s", base && *base ? base : "./");
#if !defined(PSX_SDL3)
    if (base) SDL_free((void *)base);
#endif
    size_t n = strlen(game_dir);
    if (n + 1 < sizeof game_dir && game_dir[n - 1] != '/' && game_dir[n - 1] != '\\') memcpy(game_dir + n, "/", 2);
    return game_dir;
}

int tmst_files_staged(char *out, size_t size, const char *rel) {
    return snprintf(out, size, "%smods/tmst-data/%s", tmst_files_game_dir(), rel) < (int)size;
}

void tmst_files_addon(char *out, size_t size, const char *rel) {
    snprintf(out, size, "%s/addons/tmst/%s", tmst_files_root(), rel);
    if (tmst_files_exist(out)) return;
    char staged[4300];
    if (tmst_files_staged(staged, sizeof staged, rel) && tmst_files_exist(staged)) snprintf(out, size, "%s", staged);
}

const char *tmst_files_root(void) {
    if (root[0]) return root;
#ifdef __SWITCH__
    snprintf(root, sizeof root, "sdmc:/switch/tekken3");
    return root;
#else
    char exe[4096], at[4200];
    snprintf(exe, sizeof exe, "%s", tmst_files_game_dir());
    int found = 0;
    snprintf(at, sizeof at, "%s", exe);
    for (int up = 0; up < 8 && !found; up++) {
        char probe[4300];
        snprintf(probe, sizeof probe, "%sgame.toml", at);
        if ((found = tmst_files_exist(probe)) != 0) break;
        size_t n = strlen(at);
        if (n + 4 >= sizeof at) break;
        memcpy(at + n, "../", 4);
    }
    if (!found) snprintf(at, sizeof at, "%s..", exe);       /* the game's parent, as a player has it */
    size_t n = strlen(at);
    while (n > 1 && (at[n - 1] == '/' || at[n - 1] == '\\')) at[--n] = 0;
    snprintf(root, sizeof root, "%s", at);
    return root;
#endif
}

int tmst_files_exist(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

static int is_dir(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && (st.st_mode & S_IFMT) == S_IFDIR;
}

int tmst_files_make_dirs(const char *path) {
    char p[4096];
    if (snprintf(p, sizeof p, "%s", path) >= (int)sizeof p) return 0;
    for (char *c = p + 1; *c; c++) {
        if (*c != '/' && *c != '\\') continue;
        if (c[-1] == ':' || c[-1] == '/' || c[-1] == '\\' || c[-1] == '.') continue;   /* a drive, //, .. */
        char keep = *c; *c = 0;
        if (!is_dir(p)) make_dir(p);
        *c = keep;
    }
    if (!is_dir(p)) make_dir(p);
    return is_dir(p);
}
