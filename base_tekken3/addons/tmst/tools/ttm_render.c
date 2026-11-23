/* The TTT arcade stage pieces of the Expanded's music.ttm as .bgm packs, for
 * the Jukebox (run by tools/ttt_arcade_music.py, which inflates the file).
 *
 *     ttm_render <inflated body> <out folder> <gain percent>
 *
 * The C352 and the resampler are the Expanded's (src/tekken3_ttt_music.c:
 * its replay of the sound driver's writes, MAME 0.289's lofi resampler), the
 * stream as the game makes it: a piece plays from the request to `seam`, the
 * last `fade` samples crossfaded into a second chip started from `loop -
 * fade`, which goes on as the piece from `loop`. Each piece is written as
 * <out>/piece-NN.bgm (NN its number, hex): 'T3BGM001', u32 44 100, u32
 * frames, u32 loop frame, then s16 left and right at gain percent, the
 * frames from `loop frame` the same stream as from `frames` on. Prints
 * "NN frames loop" per piece.
 *
 * Credits: the C352 replay is Matthew Stan's, from Tekken 3 Expanded
 * (src/tekken3_ttt_music.c, PolyForm Noncommercial 1.0.0), which adapts
 * MAME's C352 (src/devices/sound/c352.cpp), copyright R. Belmont and
 * superctr, BSD-3-Clause: licenses/MAME-BSD-3-Clause.txt. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { BUSY = 0x8000, KEYON = 0x4000, KEYOFF = 0x2000, LOOPHIST = 0x800, PHASEFL = 0x100, PHASEFR = 0x80,
       LDIR = 0x40, LINK = 0x20, NOISE = 0x10, MULAW = 8, FILTER = 4, LOOP = 2, REVERSE = 1 };
enum { VOICES = 32, RING = 32, MAX_PIECES = 32 };

typedef struct { uint32_t n; uint16_t reg, value, mask, pad; } Write;
typedef struct {
    uint32_t number, loop, seam, fade, init_count, write_count;
    const uint8_t *init;
    Write *writes;
} Piece;
typedef struct { uint16_t regs[8]; uint32_t pos; int32_t counter, sample, last, vol[2]; } Voice;
typedef struct { Voice v[VOICES]; uint32_t noise, busy, t, cursor; } Chip;

static struct { uint32_t chip_start, out_start; int count; Piece pieces[MAX_PIECES]; const uint8_t *bank[256]; } lib;
static const uint8_t zero_bank[65536];
static int16_t MU[256], LIN[256];
static float f0[0x1001], f1[0x1001];

/* The player of one piece. */
static struct {
    const Piece *p;
    Chip a, b, snap;
    int snapshot_ok, fading;
    int64_t produced, d;
    int16_t ring[RING], ring_r[RING];
} pl;

static uint32_t u32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static uint16_t u16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }

static void tables(void) {
    int t[128], j = 0;
    for (int i = 0; i < 128; i++) {
        t[i] = j << 5;
        j += i < 16 ? 1 : i < 24 ? 2 : i < 48 ? 4 : i < 100 ? 8 : 16;
    }
    for (int i = 0; i < 128; i++) { MU[i] = (int16_t)t[i]; MU[i + 128] = (int16_t)(~t[i] & 0xffe0); }
    for (int b = 0; b < 256; b++) LIN[b] = (int16_t)(b << 8);
    for (int i = 1; i < 4096; i++) { float p = (float)(i / 4096.0); f0[i] = (p - p * p * p) / 6.0f; }
    for (int i = 1; i < 2049; i++) { float p = (float)(i / 4096.0); f1[i] = p + (p * p - p * p * p) / 2.0f; }
    for (int i = 2049; i < 4096; i++) f1[i] = (float)(1.0 + (double)f0[i] + (double)f0[4096 - i] - (double)f1[4096 - i]);
    f1[0x1000] = 1.0f;
}

/* The inflated body of music.ttm (format in the Expanded's tools/ttt_stages/music_install.py). */
static int load(const uint8_t *body, size_t raw) {
    const uint8_t *p = body, *end = body + raw;
    if (raw < 16) return 0;
    lib.chip_start = u32(p); lib.out_start = u32(p + 4);
    uint32_t pieces = u32(p + 8), banks = u32(p + 12);
    const uint8_t *bank_ids = p + 16;
    p = bank_ids + ((banks + 3) & ~3u);
    if (pieces > MAX_PIECES || p > end) return 0;
    for (uint32_t i = 0; i < pieces; i++) {
        Piece *q = &lib.pieces[i];
        if (end - p < 24) return 0;
        q->number = u32(p); q->loop = u32(p + 4); q->seam = u32(p + 8); q->fade = u32(p + 12);
        q->init_count = u32(p + 16); q->write_count = u32(p + 20);
        p += 24;
        q->init = p;
        p += 4 * (size_t)q->init_count;
        if (p > end || end - p < 10 * (int64_t)q->write_count || q->fade > q->loop || q->loop >= q->seam || !q->fade) return 0;
        q->writes = malloc(sizeof(Write) * (q->write_count ? q->write_count : 1));
        if (!q->writes) return 0;
        uint32_t n = 0;
        for (uint32_t w = 0; w < q->write_count; w++, p += 10) {
            n += u32(p);
            q->writes[w] = (Write){ n, u16(p + 4), u16(p + 6), u16(p + 8), 0 };
        }
    }
    for (int b = 0; b < 256; b++) lib.bank[b] = zero_bank;
    if ((uint64_t)(end - p) < (uint64_t)banks << 16) return 0;
    for (uint32_t b = 0; b < banks; b++, p += 65536) lib.bank[bank_ids[b]] = p;
    lib.count = (int)pieces;
    return 1;
}

static void chip_reset(Chip *c, const Piece *p) {
    memset(c, 0, sizeof *c);
    for (uint32_t i = 0; i < p->init_count; i++) {
        uint16_t reg = u16(p->init + 4 * i), value = u16(p->init + 4 * i + 2);
        if (reg < 0x100) c->v[reg / 8].regs[reg % 8] = value & ~BUSY;
    }
    c->noise = 0x1234;
}

static void chip_writes(Chip *c, const Piece *p) {
    while (c->cursor < p->write_count && p->writes[c->cursor].n <= c->t) {
        const Write *w = &p->writes[c->cursor++];
        if (w->reg < 0x100) {
            Voice *v = &c->v[w->reg / 8];
            int r = w->reg % 8;
            v->regs[r] = (uint16_t)((v->regs[r] & ~w->mask) | (w->value & w->mask));
            if (r == 3) {
                if (v->regs[3] & BUSY) c->busy |= 1u << (w->reg / 8);
                else c->busy &= ~(1u << (w->reg / 8));
            }
        } else if (w->reg == 0x202 && w->mask == 0xffff) {
            for (int i = 0; i < VOICES; i++) {
                Voice *v = &c->v[i];
                uint16_t f = v->regs[3];
                if (f & KEYON) {
                    v->pos = ((uint32_t)v->regs[4] << 16) | v->regs[5];
                    v->sample = v->last = 0;
                    v->counter = 0xffff;
                    f = (uint16_t)((f | BUSY) & ~(KEYON | LOOPHIST));
                    v->vol[0] = v->vol[1] = 0;
                }
                if (f & KEYOFF) { f &= (uint16_t)~(BUSY | KEYOFF); v->counter = 0xffff; }
                v->regs[3] = f;
                if (f & BUSY) c->busy |= 1u << i; else c->busy &= ~(1u << i);
            }
        }
    }
}

/* One 88 200 Hz sample: the writes due, then the voices. */
static void chip_step(Chip *c, const Piece *p, int16_t out[2]) {
    chip_writes(c, p);
    int32_t o0 = 0, o1 = 0;
    for (uint32_t m = c->busy; m; m &= m - 1) {
        int i = __builtin_ctz(m);
        Voice *v = &c->v[i];
        uint16_t fl = v->regs[3];
        int32_t cnt = v->counter, nc = cnt + v->regs[2];
        if (nc & 0x10000) {
            v->last = v->sample;
            if (fl & NOISE) {
                c->noise = (c->noise >> 1) ^ ((0u - (c->noise & 1)) & 0xfff6);
                v->sample = (int16_t)c->noise;
            } else {
                uint32_t a = v->pos & 0xffffff;
                uint8_t b = lib.bank[a >> 16][a & 0xffff];
                v->sample = fl & MULAW ? MU[b] : LIN[b];
                uint32_t q = v->pos & 0xffff;
                if ((fl & LOOP) && (fl & REVERSE)) {
                    if ((fl & LDIR) && q == v->regs[7]) fl &= (uint16_t)~LDIR;
                    else if (!(fl & LDIR) && q == v->regs[6]) fl |= LDIR;
                    v->pos += fl & LDIR ? (uint32_t)-1 : 1u;
                } else if (q == v->regs[6]) {
                    if ((fl & LINK) && (fl & LOOP)) { v->pos = ((uint32_t)v->regs[5] << 16) | v->regs[7]; fl |= LOOPHIST; }
                    else if (fl & LOOP) { v->pos = (v->pos & 0xff0000) | v->regs[7]; fl |= LOOPHIST; }
                    else { fl = (uint16_t)((fl | KEYOFF) & ~BUSY); v->sample = 0; }
                } else v->pos += fl & REVERSE ? (uint32_t)-1 : 1u;
            }
            v->regs[3] = fl;
            if (!(fl & BUSY)) c->busy &= ~(1u << i);
        }
        if ((nc ^ cnt) & 0x18000) {
            int tg[2] = { v->regs[0] >> 8, v->regs[0] & 0xff };
            for (int ch = 0; ch < 2; ch++) if (v->vol[ch] != tg[ch]) v->vol[ch] += v->vol[ch] < tg[ch] ? 1 : -1;
        }
        v->counter = nc & 0xffff;
        int32_t s = v->sample;
        if (!(fl & FILTER)) s = v->last + (int32_t)(((int64_t)v->counter * (s - v->last)) >> 16);
        o0 += ((fl & PHASEFL ? -s : s) * v->vol[0]) >> 8;
        o1 += ((fl & PHASEFR ? -s : s) * v->vol[1]) >> 8;
    }
    out[0] = (int16_t)(o0 >> 3);
    out[1] = (int16_t)(o1 >> 3);
    c->t++;
}

/* The next chip sample of the stream: A, crossfaded into B before the seam;
 * from the seam on, B (the piece again from `loop`). */
static void produce(void) {
    const Piece *p = pl.p;
    int16_t a[2], b[2];
    if (!pl.snapshot_ok && pl.a.t == p->loop - p->fade) { pl.snap = pl.a; pl.snapshot_ok = 1; }
    if (!pl.fading && pl.snapshot_ok && pl.a.t == p->seam - p->fade) { pl.b = pl.snap; pl.fading = 1; }
    chip_step(&pl.a, p, a);
    if (pl.fading) {
        int32_t i = (int32_t)(pl.a.t - 1 - (p->seam - p->fade)), n = (int32_t)p->fade;
        chip_step(&pl.b, p, b);
        for (int ch = 0; ch < 2; ch++) a[ch] = (int16_t)((a[ch] * (n - i) + b[ch] * i) / n);
        if (pl.a.t == p->seam) { pl.a = pl.b; pl.fading = 0; }
    }
    pl.ring[pl.produced % RING] = a[0];
    pl.ring_r[pl.produced % RING] = a[1];
    pl.produced++;
}

static float stream(int64_t s, int ch) {
    if (s < 0) return 0.0f;
    return (ch ? pl.ring_r : pl.ring)[s % RING] / 32768.0f;
}

/* MAME 0.289's lofi resampler, 88 200 to 44 100 Hz, at output sample d; the
 * stream position of its first average in *at. */
static void resample(int16_t out[2], int64_t *at) {
    int64_t d = pl.d, update = d - (d - 1) % 882, delta = (2 * update) % 3;
    int64_t phase = (((delta << 12) / 3) << 12) + (d - update) * 11184810;
    int64_t k = (2 * update - delta) / 3 - 4 + (phase >> 24);
    int c = (int)((phase & 0xffffff) >> 12);
    int64_t first = (int64_t)lib.chip_start;
    while (pl.produced <= 3 * (k + 3) + 2 - first) produce();
    *at = 3 * (k + 1) - first;
    for (int ch = 0; ch < 2; ch++) {
        float av[4];
        for (int j = 0; j < 4; j++) {
            int64_t i = 3 * (k + j) - first;
            av[j] = ((0.0f + stream(i, ch)) + stream(i + 1, ch) + stream(i + 2, ch)) / 3.0f;
        }
        float y = -av[0] * f0[4096 - c] + av[1] * f1[4096 - c];
        y = y + av[2] * f1[c];
        y = y - av[3] * f0[c];
        int64_t q = (int64_t)(y * 32768.0f);
        out[ch] = (int16_t)(q > 32767 ? 32767 : q < -32768 ? -32768 : q);
    }
    pl.d++;
}

static void put32(uint8_t *p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24); }

/* Frames until the stream's position reaches the seam again past the loop:
 * the frame at `loop` (loop frame) and the one a period later (frames). */
static int render(const Piece *p, const char *path, int gain) {
    memset(&pl, 0, sizeof pl);
    pl.p = p;
    pl.d = lib.out_start;
    chip_reset(&pl.a, p);
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    uint8_t head[20] = "T3BGM001";
    fwrite(head, 1, 20, f);
    int64_t loop_frame = -1, frames = 0, period = p->seam - p->loop;
    for (;;) {
        int16_t s[2]; int64_t at;
        resample(s, &at);
        if (loop_frame < 0 && at >= (int64_t)p->loop) loop_frame = frames;
        if (loop_frame >= 0 && frames == loop_frame + (period + 1) / 2) break;
        uint8_t o[4];
        for (int ch = 0; ch < 2; ch++) {
            int32_t v = s[ch] * gain / 100;
            v = v > 32767 ? 32767 : v < -32768 ? -32768 : v;
            o[ch * 2] = (uint8_t)v; o[ch * 2 + 1] = (uint8_t)(v >> 8);
        }
        fwrite(o, 1, 4, f);
        frames++;
    }
    put32(head + 8, 44100); put32(head + 12, (uint32_t)frames); put32(head + 16, (uint32_t)loop_frame);
    fseek(f, 0, SEEK_SET);
    fwrite(head, 1, 20, f);
    int ok = !ferror(f);
    if (fclose(f)) ok = 0;
    if (ok) printf("%02X %lld %lld\n", p->number, (long long)frames, (long long)loop_frame);
    return ok;
}

int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "usage: ttm_render <inflated body> <out folder> <gain percent>\n"); return 2; }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { fprintf(stderr, "ttm_render: %s unreadable\n", argv[1]); return 1; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *body = size > 0 ? malloc((size_t)size) : NULL;
    if (!body || fread(body, 1, (size_t)size, f) != (size_t)size) { fprintf(stderr, "ttm_render: %s unreadable\n", argv[1]); return 1; }
    fclose(f);
    tables();
    if (!load(body, (size_t)size)) { fprintf(stderr, "ttm_render: %s is not a music.ttm body\n", argv[1]); return 1; }
    int gain = atoi(argv[3]);
    for (int i = 0; i < lib.count; i++) {
        char path[4096];
        snprintf(path, sizeof path, "%s/piece-%02X.bgm", argv[2], lib.pieces[i].number);
        if (!render(&lib.pieces[i], path, gain)) { fprintf(stderr, "ttm_render: %s not written\n", path); return 1; }
    }
    return 0;
}
