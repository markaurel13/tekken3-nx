/* TMST core: PNG writer and reader (tmst_png.h). */
#include "tmst_png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- growing output ---- */
typedef struct { uint8_t *p; size_t n, cap; int bad; } Buf;
static void put(Buf *b, const void *d, size_t n) {
    if (b->bad) return;
    if (b->n + n > b->cap) {
        size_t cap = b->cap ? b->cap : 65536;
        while (cap < b->n + n) cap *= 2;
        uint8_t *p = realloc(b->p, cap);
        if (!p) { b->bad = 1; return; }
        b->p = p; b->cap = cap;
    }
    memcpy(b->p + b->n, d, n); b->n += n;
}
static void put32(Buf *b, uint32_t v) { uint8_t q[4] = {(uint8_t)(v >> 24), (uint8_t)(v >> 16), (uint8_t)(v >> 8), (uint8_t)v}; put(b, q, 4); }

/* ---- deflate, one fixed-Huffman block ---- */
typedef struct { Buf *out; uint32_t bits; int count; } Bits;
static void bits(Bits *w, uint32_t v, int n) {            /* LSB first */
    w->bits |= v << w->count; w->count += n;
    while (w->count >= 8) { uint8_t c = (uint8_t)w->bits; put(w->out, &c, 1); w->bits >>= 8; w->count -= 8; }
}
static uint32_t reverse(uint32_t v, int n) { uint32_t r = 0; while (n--) { r = r << 1 | (v & 1); v >>= 1; } return r; }
/* A literal / length symbol in the fixed code (Huffman codes go MSB first). */
static void symbol(Bits *w, int s) {
    if (s < 144) bits(w, reverse(0x30 + s, 8), 8);
    else if (s < 256) bits(w, reverse(0x190 + s - 144, 9), 9);
    else if (s < 280) bits(w, reverse(s - 256, 7), 7);
    else bits(w, reverse(0xC0 + s - 280, 8), 8);
}
static const uint16_t len_base[29] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115,
                                      131, 163, 195, 227, 258};
static const uint8_t len_extra[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
static const uint16_t dist_base[30] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537,
                                       2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static const uint8_t dist_extra[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
static void match(Bits *w, int len, int dist) {
    int i = 28;
    while (len_base[i] > len) i--;
    symbol(w, 257 + i);
    if (len_extra[i]) bits(w, (uint32_t)(len - len_base[i]), len_extra[i]);
    int d = 29;
    while (dist_base[d] > dist) d--;
    bits(w, reverse((uint32_t)d, 5), 5);
    if (dist_extra[d]) bits(w, (uint32_t)(dist - dist_base[d]), dist_extra[d]);
}

enum { WINDOW = 32768, HASH_BITS = 15, MAX_CHAIN = 24, MIN_MATCH = 3, MAX_MATCH = 258, GOOD = 64 };
static unsigned hash3(const uint8_t *p) { return ((unsigned)p[0] << 10 ^ (unsigned)p[1] << 5 ^ p[2]) & ((1u << HASH_BITS) - 1); }

static int deflate_fixed(const uint8_t *d, size_t n, Buf *out) {
    int32_t *head = malloc(sizeof(int32_t) << HASH_BITS), *prev = malloc(sizeof(int32_t) * WINDOW);
    if (!head || !prev) { free(head); free(prev); return 0; }
    for (size_t i = 0; i < (1u << HASH_BITS); i++) head[i] = -1;
    Bits w = {out, 0, 0};
    bits(&w, 1, 1); bits(&w, 1, 2);                    /* the last block, fixed Huffman */
    size_t i = 0;
    while (i < n) {
        int best = 0, best_d = 0;
        if (i + MIN_MATCH <= n) {
            unsigned h = hash3(d + i);
            int32_t c = head[h];
            size_t limit = n - i < MAX_MATCH ? n - i : MAX_MATCH;
            for (int chain = 0; c >= 0 && i - (size_t)c <= WINDOW && chain < MAX_CHAIN; chain++) {
                const uint8_t *a = d + c, *b = d + i;
                if (a[best] == b[best]) {
                    size_t l = 0;
                    while (l < limit && a[l] == b[l]) l++;
                    if ((int)l > best) { best = (int)l; best_d = (int)(i - (size_t)c); if (l >= GOOD || l == limit) break; }
                }
                int32_t nx = prev[c % WINDOW];
                if (nx >= c) break;
                c = nx;
            }
        }
        size_t step = best >= MIN_MATCH ? (size_t)best : 1;
        if (best >= MIN_MATCH) match(&w, best, best_d); else symbol(&w, d[i]);
        for (size_t k = 0; k < step; k++, i++)
            if (i + MIN_MATCH <= n) { unsigned h = hash3(d + i); prev[i % WINDOW] = head[h]; head[h] = (int32_t)i; }
    }
    symbol(&w, 256);
    if (w.count) bits(&w, 0, 8 - w.count);
    free(head); free(prev);
    return !out->bad;
}

/* ---- checksums ---- */
static uint32_t crc_table[256];
static void crc_init(void) {
    if (crc_table[1]) return;
    for (uint32_t n = 0; n < 256; n++) {
        uint32_t c = n;
        for (int k = 0; k < 8; k++) c = c & 1 ? 0xEDB88320u ^ (c >> 1) : c >> 1;
        crc_table[n] = c;
    }
}
static uint32_t crc(uint32_t c, const uint8_t *p, size_t n) {
    c = ~c;
    while (n--) c = crc_table[(c ^ *p++) & 0xFF] ^ (c >> 8);
    return ~c;
}
static uint32_t adler(const uint8_t *p, size_t n) {
    uint32_t a = 1, b = 0;
    while (n) {
        size_t k = n < 5552 ? n : 5552;
        n -= k;
        while (k--) { a += *p++; b += a; }
        a %= 65521; b %= 65521;
    }
    return b << 16 | a;
}
static void chunk(Buf *b, const char *type, const uint8_t *d, size_t n) {
    put32(b, (uint32_t)n);
    size_t at = b->n;
    put(b, type, 4); put(b, d, n);
    if (!b->bad) put32(b, crc(0, b->p + at, n + 4));
}

/* ---- rows ---- */
static int paeth(int a, int b, int c) {
    int p = a + b - c, pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    return pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
}

int tmst_png_encode(const uint32_t *argb, int w, int h, int pitch, uint8_t **out, size_t *size) {
    *out = NULL; *size = 0;
    if (!argb || w <= 0 || h <= 0 || pitch < w) return 0;
    size_t row = (size_t)w * 3, n = (size_t)h * (row + 1);
    uint8_t *raw = malloc(n), *line = malloc(row * 2), *trial = malloc(row);
    if (!raw || !line || !trial) { free(raw); free(line); free(trial); return 0; }
    uint8_t *cur = line, *up = line + row;
    memset(up, 0, row);
    for (int y = 0; y < h; y++) {
        const uint32_t *src = argb + (size_t)y * (size_t)pitch;
        for (int x = 0; x < w; x++) { cur[3 * x] = (uint8_t)(src[x] >> 16); cur[3 * x + 1] = (uint8_t)(src[x] >> 8); cur[3 * x + 2] = (uint8_t)src[x]; }
        uint8_t *dst = raw + (size_t)y * (row + 1);
        unsigned long best_sum = ~0ul;
        for (int f = 0; f < 5; f++) {
            if (f == 3) continue;                         /* Average: rarely the best, left out */
            unsigned long sum = 0;
            for (size_t i = 0; i < row; i++) {
                int a = i >= 3 ? cur[i - 3] : 0, b = up[i], c = i >= 3 ? up[i - 3] : 0, v = cur[i];
                int p = f == 0 ? v : f == 1 ? v - a : f == 2 ? v - b : v - paeth(a, b, c);
                trial[i] = (uint8_t)p;
                sum += (unsigned long)abs((int8_t)trial[i]);
            }
            if (sum < best_sum) { best_sum = sum; dst[0] = (uint8_t)f; memcpy(dst + 1, trial, row); }
        }
        uint8_t *t = up; up = cur; cur = t;
    }
    free(line); free(trial);
    crc_init();
    Buf z = {0}, png = {0};
    uint8_t zh[2] = {0x78, 0x01};
    put(&z, zh, 2);
    int ok = deflate_fixed(raw, n, &z);
    put32(&z, adler(raw, n));
    free(raw);
    if (!ok || z.bad) { free(z.p); return 0; }
    static const uint8_t sig[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    uint8_t ihdr[13] = {(uint8_t)(w >> 24), (uint8_t)(w >> 16), (uint8_t)(w >> 8), (uint8_t)w,
                        (uint8_t)(h >> 24), (uint8_t)(h >> 16), (uint8_t)(h >> 8), (uint8_t)h, 8, 2, 0, 0, 0};
    put(&png, sig, 8);
    chunk(&png, "IHDR", ihdr, 13);
    chunk(&png, "IDAT", z.p, z.n);
    chunk(&png, "IEND", NULL, 0);
    free(z.p);
    if (png.bad) { free(png.p); return 0; }
    *out = png.p; *size = png.n;
    return 1;
}

int tmst_png_write(const char *path, const uint32_t *argb, int w, int h, int pitch) {
    uint8_t *data; size_t n;
    if (!tmst_png_encode(argb, w, h, pitch, &data, &n)) return 0;
    FILE *f = fopen(path, "wb");
    int ok = f && fwrite(data, 1, n, f) == n;
    if (f && fclose(f)) ok = 0;
    free(data);
    if (!ok) remove(path);
    return ok;
}

/* ---- the reader: inflate (stored, fixed and dynamic blocks) ---- */
typedef struct { const uint8_t *p; size_t n, at; uint32_t bits; int count; int bad; } In;
static int need(In *s, int n) {
    while (s->count < n) {
        if (s->at >= s->n) { s->bad = 1; return 0; }
        s->bits |= (uint32_t)s->p[s->at++] << s->count; s->count += 8;
    }
    return 1;
}
static uint32_t take(In *s, int n) {
    if (!n || !need(s, n)) return 0;
    uint32_t v = s->bits & ((1u << n) - 1); s->bits >>= n; s->count -= n;
    return v;
}
/* A canonical Huffman code: how many codes of each length, the symbols in order. */
typedef struct { uint16_t count[16], symbol[320]; } Code;
static void code_make(Code *c, const uint8_t *lengths, int n) {
    uint16_t offs[16];
    memset(c->count, 0, sizeof c->count);
    for (int i = 0; i < n; i++) c->count[lengths[i]]++;
    c->count[0] = 0;
    offs[1] = 0;
    for (int l = 1; l < 15; l++) offs[l + 1] = (uint16_t)(offs[l] + c->count[l]);
    for (int i = 0; i < n; i++) if (lengths[i]) c->symbol[offs[lengths[i]]++] = (uint16_t)i;
}
static int decode_symbol(In *s, const Code *c) {
    int code = 0, first = 0, index = 0;
    for (int l = 1; l < 16; l++) {
        code |= (int)take(s, 1);
        if (s->bad) return -1;
        int count = c->count[l];
        if (code - count < first) return c->symbol[index + (code - first)];
        index += count; first += count; first <<= 1; code <<= 1;
    }
    s->bad = 1;
    return -1;
}
static int inflate_block(In *s, Buf *out, const Code *lit, const Code *dist) {
    for (;;) {
        int sym = decode_symbol(s, lit);
        if (sym < 0) return 0;
        if (sym < 256) { uint8_t c = (uint8_t)sym; put(out, &c, 1); continue; }
        if (sym == 256) return !out->bad;
        sym -= 257;
        if (sym >= 29) return 0;
        size_t len = len_base[sym] + take(s, len_extra[sym]);
        int d = decode_symbol(s, dist);
        if (d < 0 || d >= 30) return 0;
        size_t back = dist_base[d] + take(s, d < 4 ? 0 : (d - 2) / 2);
        if (s->bad || back > out->n) return 0;
        for (size_t i = 0; i < len; i++) { uint8_t c = out->p[out->n - back]; put(out, &c, 1); }
        if (out->bad) return 0;
    }
}
static int inflate_all(const uint8_t *p, size_t n, Buf *out) {
    In s = {p, n, 0, 0, 0, 0};
    static const uint8_t order[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    int last;
    do {
        last = (int)take(&s, 1);
        int type = (int)take(&s, 2);
        if (s.bad) return 0;
        if (type == 0) {                                  /* stored: from the next byte */
            s.bits = 0; s.count = 0;
            if (s.at + 4 > s.n) return 0;
            size_t len = s.p[s.at] | (size_t)s.p[s.at + 1] << 8;
            s.at += 4;
            if (s.at + len > s.n) return 0;
            put(out, s.p + s.at, len); s.at += len;
        } else if (type == 1) {
            static Code lit, dist;
            static int made;
            if (!made) {
                uint8_t l[288];
                for (int i = 0; i < 288; i++) l[i] = i < 144 ? 8 : i < 256 ? 9 : i < 280 ? 7 : 8;
                code_make(&lit, l, 288);
                for (int i = 0; i < 30; i++) l[i] = 5;
                code_make(&dist, l, 30);
                made = 1;
            }
            if (!inflate_block(&s, out, &lit, &dist)) return 0;
        } else if (type == 2) {
            int nlen = (int)take(&s, 5) + 257, ndist = (int)take(&s, 5) + 1, ncode = (int)take(&s, 4) + 4;
            uint8_t lengths[320] = {0};
            for (int i = 0; i < ncode; i++) lengths[order[i]] = (uint8_t)take(&s, 3);
            Code lc, lit, dist;
            code_make(&lc, lengths, 19);
            memset(lengths, 0, sizeof lengths);
            int i = 0;
            while (i < nlen + ndist) {
                int sym = decode_symbol(&s, &lc), rep = 0, val = 0;
                if (sym < 0) return 0;
                if (sym < 16) { lengths[i++] = (uint8_t)sym; continue; }
                if (sym == 16) { if (!i) return 0; val = lengths[i - 1]; rep = 3 + (int)take(&s, 2); }
                else if (sym == 17) rep = 3 + (int)take(&s, 3);
                else rep = 11 + (int)take(&s, 7);
                if (i + rep > nlen + ndist) return 0;
                while (rep--) lengths[i++] = (uint8_t)val;
            }
            code_make(&lit, lengths, nlen);
            code_make(&dist, lengths + nlen, ndist);
            if (!inflate_block(&s, out, &lit, &dist)) return 0;
        } else return 0;
    } while (!last);
    return !out->bad;
}

static uint32_t be32(const uint8_t *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
int tmst_png_decode(const uint8_t *data, size_t size, uint32_t **argb, int *w, int *h) {
    static const uint8_t signature[8] = {0x89, 'P', 'N', 'G', 13, 10, 26, 10};
    *argb = NULL;
    if (!data || size < 8 || memcmp(data, signature, 8)) return 0;
    Buf idat = {0};
    uint32_t pal[256];
    for (int i = 0; i < 256; i++) pal[i] = 0xFF000000u;
    int width = 0, height = 0, depth = 0, colour = -1, interlace = 0;
    for (size_t at = 8; at + 12 <= size;) {
        uint32_t n = be32(data + at);
        const uint8_t *type = data + at + 4, *body = data + at + 8;
        if (n > size - at - 12) break;
        if (!memcmp(type, "IHDR", 4) && n >= 13) {
            width = (int)be32(body); height = (int)be32(body + 4); depth = body[8]; colour = body[9]; interlace = body[12];
        } else if (!memcmp(type, "PLTE", 4)) {
            for (uint32_t i = 0; i < n / 3 && i < 256; i++)
                pal[i] = 0xFF000000u | (uint32_t)body[i * 3] << 16 | (uint32_t)body[i * 3 + 1] << 8 | body[i * 3 + 2];
        } else if (!memcmp(type, "tRNS", 4) && colour == 3) {
            for (uint32_t i = 0; i < n && i < 256; i++) pal[i] = (pal[i] & 0xFFFFFFu) | (uint32_t)body[i] << 24;
        } else if (!memcmp(type, "IDAT", 4)) put(&idat, body, n);
        else if (!memcmp(type, "IEND", 4)) break;
        at += 12 + n;
    }
    int channels = colour == 0 ? 1 : colour == 2 ? 3 : colour == 3 ? 1 : colour == 4 ? 2 : colour == 6 ? 4 : 0;
    if (!channels || depth != 8 || interlace || width <= 0 || height <= 0 || width > 8192 || height > 8192 ||
        idat.bad || idat.n < 6) { free(idat.p); return 0; }
    Buf raw = {0};
    size_t stride = (size_t)width * (size_t)channels;
    int ok = inflate_all(idat.p + 2, idat.n - 2, &raw) && raw.n >= (stride + 1) * (size_t)height;   /* past the zlib header */
    free(idat.p);
    uint32_t *px = ok ? malloc(sizeof(uint32_t) * (size_t)width * (size_t)height) : NULL;
    if (!px) { free(raw.p); return 0; }
    for (int y = 0; y < height; y++) {
        uint8_t *line = raw.p + (size_t)y * (stride + 1), f = line[0], *cur = line + 1;
        const uint8_t *up = y ? raw.p + (size_t)(y - 1) * (stride + 1) + 1 : NULL;
        for (size_t i = 0; i < stride; i++) {
            int a = i >= (size_t)channels ? cur[i - channels] : 0, b = up ? up[i] : 0;
            int c = up && i >= (size_t)channels ? up[i - channels] : 0;
            int v = f == 1 ? a : f == 2 ? b : f == 3 ? (a + b) / 2 : f == 4 ? paeth(a, b, c) : 0;
            cur[i] = (uint8_t)(cur[i] + v);
        }
        for (int x = 0; x < width; x++) {
            const uint8_t *q = cur + (size_t)x * (size_t)channels;
            uint32_t v;
            if (colour == 0) v = 0xFF000000u | q[0] * 0x010101u;
            else if (colour == 2) v = 0xFF000000u | (uint32_t)q[0] << 16 | (uint32_t)q[1] << 8 | q[2];
            else if (colour == 3) v = pal[q[0]];
            else if (colour == 4) v = (uint32_t)q[1] << 24 | q[0] * 0x010101u;
            else v = (uint32_t)q[3] << 24 | (uint32_t)q[0] << 16 | (uint32_t)q[1] << 8 | q[2];
            px[(size_t)y * (size_t)width + (size_t)x] = v;
        }
    }
    free(raw.p);
    *argb = px; *w = width; *h = height;
    return 1;
}
int tmst_png_read(const char *path, uint32_t **argb, int *w, int *h) {
    *argb = NULL;
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *data = n > 0 ? malloc((size_t)n) : NULL;
    int ok = data && fread(data, 1, (size_t)n, f) == (size_t)n;
    fclose(f);
    ok = ok && tmst_png_decode(data, (size_t)n, argb, w, h);
    free(data);
    return ok;
}
