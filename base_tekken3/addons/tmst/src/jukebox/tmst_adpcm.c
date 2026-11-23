/* The Jukebox's ADPCM packs (tmst_adpcm.h). The decoding is the one of
 * tools/adpcm_pack.py's decode(), sample for sample (tests/test_adpcm_pack.py). */
#include "tmst_adpcm.h"
#include <math.h>
#include <string.h>

static uint32_t rd32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }

int tmst_adpcm_open(TmstAdpcm *a, FILE *f, const uint8_t head[TMST_ADPCM_HEAD]) {
    memset(a, 0, sizeof *a);
    if (memcmp(head, TMST_ADPCM_MAGIC, 8) || rd32(head + 8) != 44100 || !rd32(head + 12) ||
        rd32(head + 16) >= rd32(head + 12))
        return 0;
    a->f = f; a->frames = rd32(head + 12); a->loop = rd32(head + 16); a->gain = rd32(head + 20);
    return 1;
}

static const int F0[5] = {0, 60, 115, 98, 122}, F1[5] = {0, 0, -52, -55, -60};

/* One channel's block: 28 samples into out[0], out[2], ... */
static void block(const uint8_t *b, int32_t h[2], uint32_t gain, int16_t *out) {
    int shift = b[0] & 15, filter = b[0] >> 4;
    if (shift > 12) shift = 9;
    if (filter > 4) filter = 4;
    for (int i = 0; i < TMST_ADPCM_FRAMES; i++) {
        int nib = (b[2 + i / 2] >> ((i & 1) * 4)) & 15;
        int32_t v = (int32_t)(int16_t)(uint16_t)(nib << 12) >> shift;
        v += (h[0] * F0[filter] + h[1] * F1[filter] + 32) >> 6;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        h[1] = h[0]; h[0] = v;
        int64_t s = ((int64_t)v * gain) >> 16;
        out[i * 2] = (int16_t)(s > 32767 ? 32767 : s < -32768 ? -32768 : s);
    }
}

unsigned tmst_adpcm_fill(TmstAdpcm *a, int16_t *out, unsigned max, unsigned *first) {
    enum { MAX_BLOCKS = 128 };
    uint8_t raw[MAX_BLOCKS * TMST_ADPCM_BLOCK];
    unsigned skip = 0;
    *first = 0;
    if (a->pos >= a->frames) {                       /* the end: the loop's block, its history */
        uint32_t b = a->loop / TMST_ADPCM_FRAMES;
        if (fseek(a->f, TMST_ADPCM_HEAD + (long)b * TMST_ADPCM_BLOCK, SEEK_SET)) return 0;
        a->pos = b * TMST_ADPCM_FRAMES; skip = a->loop - a->pos;
        if (a->loop_saved) memcpy(a->h, a->loop_h, sizeof a->h); else memset(a->h, 0, sizeof a->h);
    }
    uint32_t left = (a->frames - a->pos + TMST_ADPCM_FRAMES - 1) / TMST_ADPCM_FRAMES;
    unsigned want = max / TMST_ADPCM_FRAMES;
    if (want > MAX_BLOCKS) want = MAX_BLOCKS;
    if (want > left) want = left;
    if (!want) return 0;
    unsigned got = (unsigned)fread(raw, TMST_ADPCM_BLOCK, want, a->f);
    for (unsigned k = 0; k < got; k++) {
        if (!a->loop_saved && a->pos + k * TMST_ADPCM_FRAMES == a->loop / TMST_ADPCM_FRAMES * TMST_ADPCM_FRAMES) {
            memcpy(a->loop_h, a->h, sizeof a->h); a->loop_saved = 1;
        }
        int16_t *o = out + k * TMST_ADPCM_FRAMES * 2;
        block(raw + k * TMST_ADPCM_BLOCK, a->h[0], a->gain, o);
        block(raw + k * TMST_ADPCM_BLOCK + 16, a->h[1], a->gain, o + 1);
    }
    uint32_t n = got * TMST_ADPCM_FRAMES;
    if (n > a->frames - a->pos) n = a->frames - a->pos;
    a->pos += n;
    if (n <= skip) return 0;
    *first = skip;
    return n;
}

/* ---- XA packs --------------------------------------------------------------- */
int tmst_xa_open(TmstXaPack *x, FILE *f, const uint8_t head[TMST_ADPCM_HEAD]) {
    memset(x, 0, sizeof *x);
    uint32_t frames = rd32(head + 12);
    if (memcmp(head, TMST_XA_MAGIC, 8) || rd32(head + 8) != 37800 || !frames || frames % TMST_XA_GROUP_FRAMES ||
        rd32(head + 16))
        return 0;
    x->f = f; x->frames = frames; x->gain = rd32(head + 20);
    x->out_frames = (uint32_t)((uint64_t)frames * 7 / 6);
    return 1;
}

/* The resampler's weights: phase p (output frame i with i * 6 % 7 == p, its
 * source position i * 6 / 7 + p / 7), tap k from -15 to 16 at [k + 15]. */
static double weights[7][2 * TMST_XA_TAPS];
static void make_weights(void) {
    static int made;
    if (made) return;
    const double pi = 3.14159265358979323846;
    for (int p = 0; p < 7; p++)
        for (int k = -TMST_XA_TAPS + 1; k <= TMST_XA_TAPS; k++) {
            double d = p / 7.0 - k;
            double s = d == 0 ? 1.0 : sin(pi * d) / (pi * d);
            weights[p][k + TMST_XA_TAPS - 1] = s * (0.5 + 0.5 * cos(pi * d / TMST_XA_TAPS));
        }
    made = 1;
}

static const int K0[4] = {0, 60, 115, 98}, K1[4] = {0, 0, -52, -55};

/* The next sound group into the ring; 0 on a read error. */
static int xa_group(TmstXaPack *x) {
    uint8_t g[TMST_XA_GROUP];
    if (fread(g, 1, sizeof g, x->f) != sizeof g) return 0;
    for (int u = 0; u < 8; u++) {
        int ch = u & 1, param = g[4 + u], shift = param & 15, filter = (param >> 4) & 3;
        if (shift > 12) shift = 9;
        int32_t *h = x->h[ch];
        for (int i = 0; i < 28; i++) {
            int nib = (g[16 + i * 4 + u / 2] >> (ch * 4)) & 15;
            int32_t v = (int32_t)(int16_t)(uint16_t)(nib << 12) >> shift;
            v += (h[0] * K0[filter] + h[1] * K1[filter] + 32) >> 6;
            if (v > 32767) v = 32767;
            if (v < -32768) v = -32768;
            h[1] = h[0]; h[0] = v;
            uint32_t at = (x->decoded + (uint32_t)(u / 2) * 28 + (uint32_t)i) % TMST_XA_RING;
            x->ring[at * 2 + ch] = (int16_t)v;
        }
    }
    x->decoded += TMST_XA_GROUP_FRAMES;
    return 1;
}

unsigned tmst_xa_fill(TmstXaPack *x, int16_t *out, unsigned max) {
    make_weights();
    for (unsigned n = 0; n < max; n++) {
        if (x->out_pos >= x->out_frames) {           /* the end: again from the beginning */
            if (fseek(x->f, TMST_ADPCM_HEAD, SEEK_SET)) return n;
            x->out_pos = 0; x->decoded = 0; memset(x->h, 0, sizeof x->h);
        }
        uint64_t six = (uint64_t)x->out_pos * 6;
        int64_t base = (int64_t)(six / 7);
        const double *w = weights[six % 7];
        double acc[2] = {0, 0};
        for (int k = -TMST_XA_TAPS + 1; k <= TMST_XA_TAPS; k++) {
            int64_t at = base + k;
            if (at < 0 || at >= (int64_t)x->frames) continue;
            while ((int64_t)x->decoded <= at)
                if (!xa_group(x)) return n;
            const int16_t *v = x->ring + (at % TMST_XA_RING) * 2;
            acc[0] += v[0] * w[k + TMST_XA_TAPS - 1];
            acc[1] += v[1] * w[k + TMST_XA_TAPS - 1];
        }
        for (int ch = 0; ch < 2; ch++) {
            double s = acc[ch] * x->gain / 65536.0;
            long r = s >= 0 ? (long)(s + 0.5) : -(long)(-s + 0.5);
            out[n * 2 + ch] = (int16_t)(r > 32767 ? 32767 : r < -32768 ? -32768 : r);
        }
        x->out_pos++;
    }
    return max;
}
