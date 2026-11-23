/* tmst.tournament: the story before the draw (tmst_tournament_story.h). */
#include "tmst_tournament_story.h"
#include <string.h>

/* A line of the file: from *at to its end, without the '\r' and the spaces
 * around it; *at moved past its '\n'. */
static const char *next_line(const char **at, int *len) {
    const char *p = *at, *e = strchr(p, '\n');
    if (!e) e = p + strlen(p);
    *at = *e ? e + 1 : e;
    while (p < e && (*p == ' ' || *p == '\t')) p++;
    while (e > p && (e[-1] == '\r' || e[-1] == ' ' || e[-1] == '\t')) e--;
    *len = (int)(e - p);
    return p;
}

int tmst_story_parse(TmstStory *s, const char *text) {
    memset(s->title, 0, sizeof s->title);
    s->body[0] = 0; s->lines = 0; s->wrapped_w = 0;
    if (!text) return 0;
    if (!strncmp(text, "\xEF\xBB\xBF", 3)) text += 3;         /* a BOM */
    const char *at = text;
    int len, n = 0, gap = 0, titled = 0;
    while (*at) {
        const char *p = next_line(&at, &len);
        if (!titled) {
            if (!len) continue;
            if (len >= (int)sizeof s->title) len = (int)sizeof s->title - 1;
            memcpy(s->title, p, (size_t)len);
            titled = 1;
            continue;
        }
        if (!len) { gap = n > 0; continue; }
        /* '\n' starts the next paragraph, a space joins its lines */
        const char *sep = gap ? "\n" : n ? " " : "";
        if (n + (int)strlen(sep) + len >= TMST_STORY_TEXT) break;
        n += (int)strlen(strcpy(s->body + n, sep));
        memcpy(s->body + n, p, (size_t)len); n += len; s->body[n] = 0;
        gap = 0;
    }
    return titled;
}

void tmst_story_wrap(TmstStory *s, int max_w, int (*width)(const char *text, void *ctx), void *ctx) {
    int n = 0;
    s->lines = 0; s->wrapped_w = max_w;
    char cand[TMST_STORY_TEXT];
    const char *p = s->body;
    while (*p && s->lines < TMST_STORY_LINES - 1) {
        const char *pe = strchr(p, '\n');
        if (!pe) pe = p + strlen(p);
        cand[0] = 0;
        int clen = 0;
        while (p < pe) {
            while (p < pe && *p == ' ') p++;
            if (p >= pe) break;
            const char *w = p;
            while (p < pe && *p != ' ') p++;
            int wl = (int)(p - w), before = clen;
            if (clen + 1 + wl >= (int)sizeof cand) break;
            if (clen) cand[clen++] = ' ';
            memcpy(cand + clen, w, (size_t)wl); clen += wl; cand[clen] = 0;
            if (before && width(cand, ctx) > max_w) {             /* the word to the next line */
                cand[before] = 0;
                if (n + before + 1 > TMST_STORY_TEXT || s->lines >= TMST_STORY_LINES - 1) break;
                s->line[s->lines++] = n; memcpy(s->out + n, cand, (size_t)before + 1); n += before + 1;
                memmove(cand, w, (size_t)wl); clen = wl; cand[clen] = 0;
            }
        }
        if (clen && n + clen + 1 <= TMST_STORY_TEXT && s->lines < TMST_STORY_LINES) {
            s->line[s->lines++] = n; memcpy(s->out + n, cand, (size_t)clen + 1); n += clen + 1;
        }
        p = pe;
        if (*p == '\n') {
            p++;
            if (n + 1 <= TMST_STORY_TEXT && s->lines < TMST_STORY_LINES) { s->line[s->lines++] = n; s->out[n++] = 0; }
        }
    }
}
