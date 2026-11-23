/* TMST core: log and the version guard (tmst_core.h). */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "psx_sdl.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef TMST_VERSION
#define TMST_VERSION "dev"
#endif
#ifndef TMST_UPSTREAM_REV
#define TMST_UPSTREAM_REV "unknown"
#endif
#ifndef TMST_TESTED_WITH
#define TMST_TESTED_WITH ""
#endif

/* Also into tmst.log beside the game (new each start): the launcher keeps
 * none of the game's output. */
static FILE *log_file(void) {
    static FILE *f;
    static int tried;
    if (tried) return f;
    tried = 1;
    const char *base = SDL_GetBasePath();
    char path[4200];
    snprintf(path, sizeof path, "%stmst.log", base ? base : "./");
#if !defined(PSX_SDL3)
    if (base) SDL_free((void *)base);
#endif
    f = fopen(path, "w");
    return f;
}
void tmst_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fputs("TMST: ", stderr);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
    FILE *f = log_file();
    if (f) {
        va_start(args, fmt);
        vfprintf(f, fmt, args);
        fputc('\n', f);
        fflush(f);
        va_end(args);
    }
}

const char *tmst_upstream_rev(void) { return TMST_UPSTREAM_REV; }

/* The release part of the rev: "v1.4.1" of "v1.4.1-3-gabcdef0-dirty". */
static size_t release_length(const char *rev) {
    const char *dash = strchr(rev, '-');
    return dash ? (size_t)(dash - rev) : strlen(rev);
}

int tmst_upstream_tested(void) {
    const char *rev = TMST_UPSTREAM_REV, *list = TMST_TESTED_WITH;
    size_t n = release_length(rev);
    /* Only an exact tag counts: commits after it are untested. */
    if (rev[n] != '\0') return 0;
    while (*list) {
        size_t m = strcspn(list, " ;,");
        if (m == n && !strncmp(list, rev, n)) return 1;
        list += m;
        while (*list == ' ' || *list == ';' || *list == ',') list++;
    }
    return 0;
}

int tmst_guard(const char *mod, unsigned needs) {
    static int said;
    if (!said) {
        said = 1;
        tmst_log("add-on %s on Tekken 3 Expanded %s (%s)", TMST_VERSION, TMST_UPSTREAM_REV,
                 tmst_upstream_tested() ? "tested" : "untested: features checked one by one");
    }
    unsigned missing = needs & ~tmst_compat_available();
    if (missing) {
        tmst_log("%s is off: this Expanded lacks %s", mod, tmst_compat_describe(missing));
        return 0;
    }
    return 1;
}
