/* tmst.tournament: the boss intros' .cine files (tmst_tournament_cine.h).
 *
 * The .cine (the Expanded's src/tekken3_cine.c, cine_read): a header, the
 * camera (a line a frame: eye x y z, target x y z, int16) and each actor's
 * poses (a line a frame: 57 TTT1 channels, uint16). Channels 0..2 are the
 * root: heading, height, distance, polar from the actor's `place` and its
 * facing (a clip's distance grows as it walks forward); then 18 joints' XYZ
 * rotations (0x10000 a turn). Fighters of one body (Kazuya, Devil) share the
 * channels' meaning: a clip of one plays on the other.
 *
 * The TTT1 combat pack (the Expanded's tools/ttt1/pack_edit.py): 8 words
 * (magic "JUM1", 4, size, records, neutral, relocations, their count, the
 * record table: 32 + 16 * records), a 16-byte entry a record, the 56-byte
 * records; a record's word 0 is its clip: "\0NUJ" | frames, 57, the decoded
 * poses. */
#include "tmst_tournament_cine.h"
#include <string.h>

enum { CHANNELS = 57, RECORD = 56, PACK_MAGIC = 0x314d554a, CLIP_MAGIC = 0x4a554e00, NO_SOUND = 0xffff, SOUND_INDEX = 0x7ff };

/* ---- the entries ---------------------------------------------------------------- */
/* Devil (the user's T2 Grand Final, 2026-11-07): Kazuya standing in the
 * dark, the camera coming to him; then his TTT1 tag move with Devil, the
 * one both packs hold as record 1 (arms up, bent forward, rising in
 * lightning from its frame 34, the arcade's Kazuya / Devil change, the
 * user's video), at 70 %; as he rises a white flash and he is Devil (the
 * engine's swap, clip frame 38); Devil in his stance, the lightning dying,
 * then black. The lightning: the move's own bones (its timed properties:
 * 14, 16, 11, 9, 4, 2) and a few more, every other frame. */
static const TmstCinePart devil_parts[] = {
    {"Kazuya-TTT1", -1, 0, 100, 80, 0},          /* Kazuya's stance */
    {"Kazuya-TTT1", 1, 0, 70, 70, 12},           /* the tag move: clip frames 0..48 */
    {NULL, -1, 0, 100, 80, 18},                  /* Devil's stance */
};
static const TmstCineKey devil_cam[] = {
    {0, {600, -800, -3000}, {0, -1100, 1200}},
    {80, {300, -1050, -1000}, {0, -1100, 1200}},
    {110, {320, -900, -1700}, {0, -1000, 1000}},     /* back as he bends and steps forward */
    {140, {350, -1000, -1700}, {0, -1250, 900}},
    {230, {400, -900, -2000}, {0, -1300, 700}},
};
/* Heihachi (the user's T1 Grand Final, 2026-11-09): sitting cross-legged on
 * the ground, his back to the camera (his TTT1 record 815's first frames,
 * barely moving; not 824's bent kneeling: the user's choice), the camera
 * coming to him; he gets up (815 on, turning as he stands), turns to
 * the camera in his lightning (826: its own lightning properties, frames
 * 1..40 on bones 2, 4, 11, 16, 17), into his stance, then black. A native:
 * his own TTT1 pack (mods/ttt1), whatever moves he fights on. */
static const TmstCinePart heihachi_parts[] = {
    {NULL, 815, 0, 5, 100, 0},                   /* sitting cross-legged, his back to us: clip frames 0..5 */
    {NULL, 815, 5, 90, 120, 0},                  /* getting up: 5..112 */
    {NULL, 826, 0, 100, 59, 10},                 /* the turn in lightning: 0..58 */
    {NULL, -1, 0, 100, 70, 18},                  /* his stance */
};
/* He sits at z 2100 (his root at y -570); getting up he goes away from us,
 * a little to the right: he ends about (300, 2380) (TEKKEN3_CINE_RECORD). */
static const TmstCineKey heihachi_cam[] = {
    {0, {400, -450, -1300}, {0, -550, 2100}},
    {100, {250, -550, -500}, {0, -550, 2100}},      /* behind him, low */
    {170, {350, -850, -1100}, {100, -850, 2500}},   /* back and up with him */
    {230, {500, -900, -1500}, {250, -950, 2450}},
    {330, {550, -950, -1450}, {300, -1000, 2380}},  /* his whole figure */
};
/* True Ogre (the user's T3 Grand Final, 2026-11-09): Ogre, beaten, lying on
 * the ground (his TTT1 record 812's first frames, barely moving), gets up
 * slowly (812 on), draws himself up, arms spread, in his lightning (record
 * 2, his tag move: lightning properties 25..85; the user's pick for his
 * "invulnerable" move), rises into the air (733's frames 30..77, high up:
 * the blend lifts him there), a white flash and he is True Ogre (the
 * engine's swap), who lands on his wings (his own 735 from frame 60) into
 * his stance, then black. Both natives: their own TTT1 packs. */
static const TmstCinePart trueogre_parts[] = {
    {"Ogre-TTT1", 812, 0, 10, 30, 0},            /* lying on the ground: clip frames 0..3 */
    {"Ogre-TTT1", 812, 4, 80, 86, 0},            /* getting up: 4..72 */
    {"Ogre-TTT1", 2, 0, 80, 120, 14},            /* arms spread, lightning: 0..95 */
    {"Ogre-TTT1", 733, 30, 60, 80, 24},          /* rising, slowly: 30..77 */
    {NULL, 735, 60, 75, 118, 12},                /* True Ogre landing, slowly: 60..147 */
    {NULL, -1, 0, 100, 50, 16},                  /* his stance */
};
/* The transformation slower, its lightning and flash longer: the user,
 * 2026-11-13 (the lightning goes on round True Ogre after the flash). */
static const TmstCineKey trueogre_cam[] = {
    {0, {600, -700, -1400}, {0, -400, 1800}},       /* low, by him on the ground */
    {116, {400, -850, -1700}, {0, -950, 1800}},     /* up with him */
    {210, {350, -1000, -1600}, {0, -1100, 1800}},   /* his lightning */
    {280, {450, -600, -2800}, {0, -1900, 1800}},    /* back and low: he rises */
    {320, {450, -500, -3200}, {0, -2300, 1800}},    /* the flash, high up */
    {390, {450, -800, -2800}, {0, -1500, 1800}},    /* True Ogre coming down */
    {484, {400, -950, -1900}, {0, -1050, 1800}},    /* landed: his whole figure */
};
/* The fallback: standing in its stance, the camera coming closer. */
static const TmstCinePart stance_parts[] = {{NULL, -1, 0, 100, 240, 0}};
static const TmstCineKey stance_cam[] = {
    {0, {900, -500, -2600}, {0, -1000, 1800}},
    {240, {250, -1400, -300}, {0, -1400, 1800}},
};
#define N(a) (int)(sizeof a / sizeof *a)
static const TmstBossIntro intros[] = {
    {"devil", devil_parts, N(devil_parts), 230, {0, 0, 1200, 0x8000}, 24, 24, devil_cam, N(devil_cam),
     "kazuya", 134, 6, 129, 200, 2, {14, 16, 11, 9, 1, 4, 17, 2}, 8, NULL},
    {"heihachi", heihachi_parts, N(heihachi_parts), 330, {0, 0, 2100, 0x8000}, 30, 24, heihachi_cam, N(heihachi_cam),
     NULL, 0, 0, 221, 266, 2, {2, 4, 11, 16, 17, 14, 9}, 7, "Heihachi-TTT1"},
    {"trueogre", trueogre_parts, N(trueogre_parts), 484, {0, 0, 1800, 0x8000}, 30, 24, trueogre_cam, N(trueogre_cam),
     "ogre", 316, 16, 147, 390, 2, {2, 5, 8, 10, 13, 14, 16, 17}, 8, "Trueogre-TTT1"},
};
static const TmstBossIntro fallback = {NULL, stance_parts, N(stance_parts), 240, {0, 0, 1800, 0x8000}, 30, 24,
                                       stance_cam, N(stance_cam), NULL, 0, 0, 0, 0, 0, {0}, 0, NULL};

const TmstBossIntro *tmst_boss_intro(const char *key) {
    for (size_t i = 0; key && i < sizeof intros / sizeof *intros; i++)
        if (!strcmp(intros[i].key, key)) return &intros[i];
    return &fallback;
}

/* ---- the packs ------------------------------------------------------------------ */
static uint32_t word(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }
static uint16_t half(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static int pack_ok(const uint8_t *pack, size_t size) {
    return pack && size >= 32 && word(pack) == PACK_MAGIC && word(pack + 4) == 4;
}
/* Record r's clip: its poses (frames x 57), NULL when not one. */
static const uint8_t *clip_of(const uint8_t *pack, size_t size, uint32_t r, int *frames) {
    uint32_t records = word(pack + 12);
    uint64_t at = 32 + (uint64_t)records * 16 + (uint64_t)r * RECORD;
    if (r >= records || at + RECORD > size) return NULL;
    uint32_t clip = word(pack + at);
    if ((uint64_t)clip + 8 > size || (word(pack + clip) & 0xffffff00u) != CLIP_MAGIC || word(pack + clip + 4) != CHANNELS)
        return NULL;
    *frames = (int)(word(pack + clip) & 0xff);
    if (*frames < 2 || (uint64_t)clip + 8 + (uint64_t)*frames * CHANNELS * 2 > size) return NULL;
    return pack + clip + 8;
}
/* Part p's clip (clip -1: its pack's neutral; no pack: the boss's own,
 * `own`); the boss's neutral for one missing. *pack_out, *record: the pack
 * and record it came from. */
static const uint8_t *part_record(const TmstCinePart *p, const char *own, TmstPackOf pack_of, void *ctx, int *frames,
                                  const uint8_t **pack_out, size_t *size_out, uint32_t *record) {
    size_t size = 0;
    const uint8_t *pack = pack_of(p->pack ? p->pack : own, &size, ctx), *c = NULL;
    if (pack_ok(pack, size)) c = clip_of(pack, size, *record = p->clip >= 0 ? (uint32_t)p->clip : word(pack + 16), frames);
    if (!c) {
        pack = pack_of(own, &size, ctx);
        if (!pack_ok(pack, size)) return NULL;
        c = clip_of(pack, size, *record = word(pack + 16), frames);
    }
    *pack_out = pack; *size_out = size;
    return c;
}
static const uint8_t *part_clip(const TmstCinePart *p, const char *own, TmstPackOf pack_of, void *ctx, int *frames) {
    const uint8_t *pack; size_t size; uint32_t record;
    return part_record(p, own, pack_of, ctx, frames, &pack, &size, &record);
}
/* A clip that stays where it is (its distance never changes): a stance. */
static int still(const uint8_t *c, int n) {
    for (int i = 1; i < n; i++) if (half(c + (size_t)i * CHANNELS * 2 + 4) != half(c + 4)) return 0;
    return 1;
}
static int s16(unsigned v) { return v > 32767 ? (int)v - 65536 : (int)v; }
static unsigned lerp_angle(unsigned a, unsigned b, int t, int den) {
    int d = (int)((b - a + 32768u) & 0xffff) - 32768;
    return (unsigned)(a + d * t / den) & 0xffff;
}

/* ---- the poses ------------------------------------------------------------------ */
/* Clip c (n frames) at time t (hundredths of its frames, looped). raw: the
 * root's distance its own, lap after lap. */
static void clip_at(const uint8_t *c, int n, long t, unsigned v[CHANNELS], int *raw) {
    int k = (int)(t / 100), frac = (int)(t % 100), lap = k / (n - 1), i = k % (n - 1);
    const uint8_t *a = c + (size_t)i * CHANNELS * 2, *b = a + CHANNELS * 2, *last = c + (size_t)(n - 1) * CHANNELS * 2;
    int cycle = s16(half(last + 4)) - s16(half(c + 4));
    v[0] = half(c);
    v[1] = (unsigned)(half(a + 2) + ((int)half(b + 2) - (int)half(a + 2)) * frac / 100);
    *raw = lap * cycle + s16(half(a + 4)) + (s16(half(b + 4)) - s16(half(a + 4))) * frac / 100;
    for (int ch = 3; ch < CHANNELS; ch++) v[ch] = lerp_angle(half(a + ch * 2), half(b + ch * 2), frac, 100);
}
static void put_pose(FILE *out, const unsigned v[CHANNELS]) {
    for (int ch = 0; ch < CHANNELS; ch++) fprintf(out, ch ? " %u" : "%u", v[ch]);
    fputc('\n', out);
}

/* ---- the camera ----------------------------------------------------------------- */
/* Eased between its keys (smoothstep), held before the first, after the last. */
static void camera_at(const TmstBossIntro *e, int f, int out[6]) {
    const TmstCineKey *k = e->cam;
    int n = e->cam_count, i = 0;
    while (i + 1 < n && f >= k[i + 1].frame) i++;
    for (int c = 0; c < 6; c++) out[c] = c < 3 ? k[i].eye[c] : k[i].target[c - 3];
    if (i + 1 >= n || f <= k[i].frame) return;
    double t = (double)(f - k[i].frame) / (k[i + 1].frame - k[i].frame);
    t = t * t * (3 - 2 * t);
    for (int c = 0; c < 6; c++) {
        int b = c < 3 ? k[i + 1].eye[c] : k[i + 1].target[c - 3];
        out[c] = out[c] + (int)((b - out[c]) * t + (b >= out[c] ? 0.5 : -0.5));
    }
}

/* ---- the file ------------------------------------------------------------------- */
int tmst_boss_intro_write(FILE *out, const TmstBossIntro *e, TmstPackOf pack_of, void *ctx, unsigned player_id,
                          unsigned player_costume, unsigned boss_id, unsigned boss_costume) {
    size_t size = 0;
    if (!out || !e || !pack_of || !pack_ok(pack_of(e->own, &size, ctx), size) || e->frames <= 0 || e->cam_count < 1 ||
        e->part_count < 1)
        return 0;
    int n;
    for (int p = 0; p < e->part_count; p++)
        if (e->parts[p].speed <= 0 || e->parts[p].frames <= 0 || e->parts[p].from < 0 ||
            !part_clip(&e->parts[p], e->own, pack_of, ctx, &n))
            return 0;
    fprintf(out, "cine 1\nname boss-intro\nlength %d\narena 0\ndark 0\n", e->frames);
    fprintf(out, "actor 0 %u %u\nhide 0 0\n", player_id, player_costume);
    /* Another character first (`start`), the boss from `swap` on. */
    if (e->start && e->swap > 0 && e->swap < e->frames)
        fprintf(out, "actor 1 %s 0\nswap 1 %u %u %d\n", e->start, boss_id, boss_costume, e->swap);
    else fprintf(out, "actor 1 %u %u\n", boss_id, boss_costume);
    if (e->aura_bone_count > 0 && e->aura_to > e->aura_from) {
        fprintf(out, "aura 1 %d %d %d", e->aura_from, e->aura_to, e->aura_step > 0 ? e->aura_step : 1);
        for (int i = 0; i < e->aura_bone_count && i < TMST_AURA_BONES; i++) fprintf(out, " %u", e->aura_bones[i]);
        fputc('\n', out);
    }
    fprintf(out, "place 1 %d %d %d %d\n", (int)e->place[0], (int)e->place[1], (int)e->place[2], (int)e->place[3]);
    fprintf(out, "fade 0 %d -256 0\n", e->fade_in);
    /* The flash: up to white for `flash` frames to the swap, down after
     * (twice as long). */
    if (e->start && e->flash > 0 && e->swap - e->flash > e->fade_in && e->swap + 2 * e->flash < e->frames - e->fade_out)
        fprintf(out, "fade %d %d 0 256\nfade %d %d 256 0\n", e->swap - e->flash, e->swap - 1, e->swap, e->swap + 2 * e->flash);
    fprintf(out, "fade %d %d 0 -256\n", e->frames - e->fade_out, e->frames - 1);
    fprintf(out, "camera 0 %d\n", e->frames);
    for (int f = 0; f < e->frames; f++) {
        int c[6];
        camera_at(e, f, c);
        fprintf(out, "%d %d %d %d %d %d\n", c[0], c[1], c[2], c[3], c[4], c[5]);
    }
    unsigned v[CHANNELS], last[CHANNELS];
    int raw, raw0, base = 0, f = 0;
    const uint8_t *c = part_clip(&e->parts[0], e->own, pack_of, ctx, &n);
    /* Actor 0, hidden: one pose, far behind the boss. */
    clip_at(c, n, 0, v, &raw);
    v[2] = 12000;
    fprintf(out, "poses 0 0 1\n");
    put_pose(out, v);
    fprintf(out, "poses 1 0 %d\n", e->frames);
    /* The parts one after another: each one's distance from where the last
     * left (its own from its `from`), its first frames eased from the last
     * pose; past them the last pose held. */
    for (int p = 0; p < e->part_count && f < e->frames; p++) {
        const TmstCinePart *part = &e->parts[p];
        c = part_clip(part, e->own, pack_of, ctx, &n);
        long start = (long)part->from * 100;
        if (part->from >= n) start = (long)(n - 1) * 100;
        clip_at(c, n, start, v, &raw0);
        /* A stance after a move keeps the move's heading: its distance (the
         * move's, carried on) stays where the move left it. */
        int keep = p && still(c, n);
        for (int i = 0; i < part->frames && f < e->frames; i++, f++) {
            clip_at(c, n, start + (long)i * part->speed, v, &raw);
            v[2] = (unsigned)(base + raw - raw0) & 0xffff;
            if (keep) v[0] = last[0];
            if (p && i < part->blend)
                for (int ch = 0; ch < CHANNELS; ch++)
                    if (ch != 2) v[ch] = ch == 1 ? last[1] + ((int)v[1] - (int)last[1]) * i / part->blend
                                                 : lerp_angle(last[ch], v[ch], i, part->blend);
            put_pose(out, v);
        }
        base = s16(v[2]);
        memcpy(last, v, sizeof v);
    }
    for (; f < e->frames; f++) put_pose(out, v);
    return !ferror(out);
}

/* ---- the sounds ----------------------------------------------------------------- */
/* A record's sound list (+28): two sound events and the clip frame they
 * play at (0xFFFF: none); a TTT1 sound is an event's low 11 bits. */
int tmst_boss_intro_sounds(const TmstBossIntro *e, TmstPackOf pack_of, void *ctx, TmstCineSound *out, int max) {
    int count = 0, start = 0;
    for (int p = 0; e && pack_of && p < e->part_count && start < e->frames; start += e->parts[p++].frames) {
        const TmstCinePart *part = &e->parts[p];
        const uint8_t *pack, *own; size_t size, own_size = 0; uint32_t record; int n;
        if (part->speed <= 0 || !part_record(part, e->own, pack_of, ctx, &n, &pack, &size, &record)) continue;
        uint64_t at = 32 + (uint64_t)word(pack + 12) * 16 + (uint64_t)record * RECORD;
        uint32_t list = word(pack + at + 28);
        if ((uint64_t)list + 6 > size) continue;
        long t = ((long)half(pack + list + 4) - part->from) * 100;
        if (t < 0 || t >= (long)part->frames * part->speed) continue;
        const char *stem = part->pack ? part->pack : e->own;
        own = pack_of(stem, &own_size, ctx);             /* the part's pack, or the boss's in its place */
        for (int k = 0; k < 2 && count < max; k++) {
            unsigned event = half(pack + list + k * 2);
            if (event == NO_SOUND || !(event & SOUND_INDEX)) continue;
            out[count].frame = start + (int)(t / part->speed);
            out[count].stem = pack == own ? stem : e->own;
            out[count].index = event & SOUND_INDEX;
            count++;
        }
    }
    return count;
}
