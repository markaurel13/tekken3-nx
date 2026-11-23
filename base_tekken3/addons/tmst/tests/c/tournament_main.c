/* tests/test_tournament.py: the Tournament Mode's data side, driven from the command line.
 *   pools                               each tournament: "id|boss|key key ..." ("*" = whole roster, "-" = drawn boss)
 *   bracket <seed> <player> <boss> <n> <results>
 *                                       roster 0..n-1; results: W / L per player match, then the rest finished.
 *                                       Prints "slots ...", "step <mask> <winners...>" per step, "out|champion ..."
 *   progress <path> load|win:<t>        prints "won <mask> open <t...> [opens <t>]"
 *   stage <t> <school> <key...>         prints "<key> <kind> <stage> <cat>/<music>" per key
 *   option-stage <school> <key...>     STAGES = TEKKEN 2: "<key> <kind> <stage> <random-t2>" per key
 *   cine <key|-> <own.jmv> [<stem>=<file>...]
 *                                       "entry <key|->", then the boss intro's .cine (player 7/1, boss 33/0)
 *   sounds <key|-> <own.jmv> [<stem>=<file>...]
 *                                       "entry <key|->", then "<frame> <stem|-> <index>" per sound
 *   story <file> <width>                "title|<header>", then "|<line>" per line, cut at <width> characters */
#include "tmst_bracket.h"
#include "tmst_tournament_pools.h"
#include "tmst_tournament_progress.h"
#include "tmst_tournament_stages.h"
#include "tmst_tournament_cine.h"
#include "tmst_tournament_story.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void winners(const TmstBracket *b) {
    for (int m = 0; m < TMST_BRACKET_MATCHES; m++) printf(" %d", b->winner[m]);
    printf("\n");
}

/* The boss intro's packs, read from the command line (cine). */
enum { PACKS = 4 };
static struct { char stem[64]; uint8_t data[1 << 23]; size_t size; } packs[PACKS];   /* a real pack too (up to 8 MB) */
static int pack_count;
static const uint8_t *pack_of(const char *stem, size_t *size, void *ctx) {
    (void)ctx;
    for (int i = 0; i < pack_count; i++)
        if (stem ? !strcmp(packs[i].stem, stem) : !packs[i].stem[0]) { *size = packs[i].size; return packs[i].data; }
    return NULL;
}

/* A line's width in characters (UTF-8: its lead bytes). */
static int story_chars(const char *text, void *ctx) {
    (void)ctx;
    int n = 0;
    for (; *text; text++) n += ((unsigned char)*text & 0xc0) != 0x80;
    return n;
}

int main(int argc, char **argv) {
    if (argc >= 2 && !strcmp(argv[1], "pools")) {
        for (int t = 0; t < TMST_TOURNEYS; t++) {
            const TmstTourney *d = tmst_tourney(t);
            printf("%s|%s%s%s|", d->id, d->boss ? d->boss : "-", d->boss2 ? "+" : "", d->boss2 ? d->boss2 : "");
            if (!d->keys) printf("*");
            for (const char *const *k = d->out; k && *k; k++) printf(" -%s", *k);
            for (const char *const *k = d->keys; k && *k; k++) printf("%s%s", k == d->keys ? "" : " ", *k);
            printf("\n");
        }
        return 0;
    }
    if (argc == 7 && !strcmp(argv[1], "bracket")) {
        int n = atoi(argv[5]), pool[256];
        for (int i = 0; i < n && i < 256; i++) pool[i] = i;
        TmstBracket b;
        if (!tmst_bracket_new(&b, atoi(argv[3]), atoi(argv[4]), pool, n, (uint32_t)strtoul(argv[2], NULL, 10))) {
            printf("refused\n");
            return 0;
        }
        printf("slots");
        for (int s = 0; s < TMST_BRACKET_SLOTS; s++) printf(" %d", b.slot[s]);
        printf("\n");
        for (const char *r = argv[6]; *r && tmst_bracket_player_match(&b) >= 0; r++) {
            int m = tmst_bracket_player_match(&b);
            printf("match %d vs %d\n", m, tmst_bracket_opponent(&b));
            printf("step %u", tmst_bracket_player_result(&b, *r == 'W'));
            winners(&b);
        }
        printf("finish %u", tmst_bracket_finish(&b));
        winners(&b);
        printf("%s %d lost_at %d\n", tmst_bracket_champion(&b) == b.player ? "champion" : "out",
               tmst_bracket_champion(&b), tmst_bracket_lost_at(&b, b.player));
        return 0;
    }
    if (argc == 4 && !strcmp(argv[1], "progress")) {
        TmstProgress p;
        tmst_progress_load(&p, argv[2]);
        int opens = -2;
        if (!strncmp(argv[3], "win:", 4)) {
            opens = tmst_progress_win(&p, atoi(argv[3] + 4));
            if (!tmst_progress_save(&p, argv[2])) return 1;
        }
        printf("won %u open", p.won);
        for (int t = 0; t < TMST_TOURNEYS; t++) if (tmst_progress_open(&p, t)) printf(" %d", t);
        if (opens != -2) printf(" opens %d", opens);
        printf("\n");
        return 0;
    }
    if (argc >= 4 && !strcmp(argv[1], "story")) {
        static char text[TMST_STORY_TEXT]; static TmstStory s;
        FILE *f = fopen(argv[2], "rb");
        size_t n = f ? fread(text, 1, sizeof text - 1, f) : 0;
        if (f) fclose(f);
        text[n] = 0;
        if (!tmst_story_parse(&s, text)) { printf("none\n"); return 0; }
        tmst_story_wrap(&s, atoi(argv[3]), story_chars, NULL);
        printf("title|%s\n", s.title);
        for (int i = 0; i < s.lines; i++) printf("|%s\n", s.out + s.line[i]);
        return 0;
    }
    if (argc >= 5 && (!strcmp(argv[1], "stage") || !strcmp(argv[1], "stage-arcade"))) {
        int arcade = !strcmp(argv[1], "stage-arcade");
        for (int i = 4; i < argc; i++) {
            int stage, kind = tmst_tourney_stage(atoi(argv[2]), argv[i], atoi(argv[3]), &stage);
            const char *cat, *music = tmst_tourney_music(atoi(argv[2]), argv[i], kind, stage, arcade, &cat);
            printf("%s %d %d %s/%s\n", argv[i], kind, stage, cat ? cat : "-", music ? music : "-");
        }
        return 0;
    }
    if (argc >= 4 && !strcmp(argv[1], "option-stage")) {
        for (int i = 3; i < argc; i++) {
            int stage, t2, kind = tmst_option_t2_stage(argv[i], atoi(argv[2]), &stage);
            int lot = tmst_random_t2_stage(argv[i], atoi(argv[2]), &t2) ? t2 : -1;
            printf("%s %d %d %d\n", argv[i], kind, stage, lot);
        }
        return 0;
    }
    if (argc >= 4 && (!strcmp(argv[1], "cine") || !strcmp(argv[1], "sounds"))) {
        /* The packs: the boss's own (stem NULL), then <stem>=<file> each. */
        for (int i = 3; i < argc && pack_count < PACKS; i++) {
            const char *eq = i == 3 ? NULL : strchr(argv[i], '=');
            FILE *f = fopen(eq ? eq + 1 : argv[i], "rb");
            if (!f) continue;
            packs[pack_count].size = fread(packs[pack_count].data, 1, sizeof packs[pack_count].data, f);
            fclose(f);
            if (eq) snprintf(packs[pack_count].stem, sizeof packs[pack_count].stem, "%.*s", (int)(eq - argv[i]), argv[i]);
            pack_count++;
        }
        const char *key = strcmp(argv[2], "-") ? argv[2] : NULL;
        const TmstBossIntro *e = tmst_boss_intro(key);
        printf("entry %s\n", e->key ? e->key : "-");
        if (!strcmp(argv[1], "sounds")) {
            TmstCineSound out[8];
            int n = tmst_boss_intro_sounds(e, pack_of, NULL, out, 8);
            for (int i = 0; i < n; i++) printf("%d %s %u\n", out[i].frame, out[i].stem ? out[i].stem : "-", out[i].index);
            return 0;
        }
        return tmst_boss_intro_write(stdout, e, pack_of, NULL, 7, 1, 33, 0) ? 0 : 1;
    }
    return 2;
}
