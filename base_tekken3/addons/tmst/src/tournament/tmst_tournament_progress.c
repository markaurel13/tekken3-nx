/* tmst.tournament: the progress file (tmst_tournament_progress.h). */
#include "tmst_tournament_progress.h"
#include "tmst_tournament_pools.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tmst_progress_load(TmstProgress *p, const char *path) {
    p->won = 0;
    FILE *f = path && *path ? fopen(path, "rb") : NULL;
    char line[256];
    while (f && fgets(line, sizeof line, f)) {
        char *s = line;
        while (isspace((unsigned char)*s)) s++;
        if (strncmp(s, "won", 3)) continue;
        s += 3;
        while (isspace((unsigned char)*s)) s++;
        if (*s++ != '=') continue;
        for (char *w = strtok(s, ", \t\r\n"); w; w = strtok(NULL, ", \t\r\n"))
            for (int t = 0; t < TMST_TOURNEYS; t++) if (!strcmp(w, tmst_tourney(t)->id)) p->won |= 1u << t;
    }
    if (f) fclose(f);
}

int tmst_progress_save(const TmstProgress *p, const char *path) {
    FILE *f = path && *path ? fopen(path, "wb") : NULL;
    if (!f) return 0;
    fprintf(f, "# Tournament Mode (TMST add-on): progress, written by the game.\nwon =");
    for (int t = 0, n = 0; t < TMST_TOURNEYS; t++) if (p->won >> t & 1) fprintf(f, "%s %s", n++ ? "," : "", tmst_tourney(t)->id);
    fprintf(f, "\n");
    return fclose(f) == 0;
}

extern int config_get_unlock_all_characters(void) __attribute__((weak));

static int all_open(void) {
    const char *e = getenv("TMST_TOURNAMENT_ALL_OPEN");
    return TMST_TOURNAMENT_ALL_OPEN || (e && *e == '1') ||
           (config_get_unlock_all_characters && config_get_unlock_all_characters() != 0);
}
int tmst_progress_open(const TmstProgress *p, int t) {
    if (t < 0 || t >= TMST_TOURNEYS) return 0;
    if (all_open() || t == 0) return 1;
    return (p->won >> (t - 1) & 1) || (p->won >> t & 1);
}

int tmst_progress_win(TmstProgress *p, int t) {
    if (t < 0 || t >= TMST_TOURNEYS) return -1;
    int before = t + 1 < TMST_TOURNEYS && tmst_progress_open(p, t + 1);
    p->won |= 1u << t;
    return t + 1 < TMST_TOURNEYS && !before ? t + 1 : -1;
}
