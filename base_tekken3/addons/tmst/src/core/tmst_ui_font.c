/* TMST core: the UI font's drawing (tmst_ui_font.h). */
#include "tmst_ui_font.h"
#include <string.h>

/* The next character of UTF-8 text (0 at its end); a broken sequence is one
 * byte that reads as 0xFFFD. */
unsigned tmst_ui_next_code(const char **p) {
    const unsigned char *s = (const unsigned char *)*p;
    unsigned c = s[0];
    if (!c) return 0;
    int n = c < 0x80 ? 0 : (c & 0xE0) == 0xC0 ? 1 : (c & 0xF0) == 0xE0 ? 2 : (c & 0xF8) == 0xF0 ? 3 : -1;
    if (n < 0) { *p += 1; return 0xFFFD; }
    unsigned code = n ? c & (0x3F >> n) : c;
    for (int i = 1; i <= n; i++) {
        if ((s[i] & 0xC0) != 0x80) { *p += 1; return 0xFFFD; }
        code = code << 6 | (s[i] & 0x3F);
    }
    *p += n + 1;
    return code;
}

static const TmstUiGlyph *find(const TmstUiFace *f, unsigned code) {
    int lo = 0, hi = f->count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (f->glyphs[mid].code == code) return &f->glyphs[mid];
        if (f->glyphs[mid].code < code) lo = mid + 1; else hi = mid - 1;
    }
    return NULL;
}
const TmstUiGlyph *tmst_ui_glyph(const TmstUiFace *f, unsigned code) {
    if (code == 0xA0) code = ' ';
    const TmstUiGlyph *g = find(f, code);
    if (g) return g;
    unsigned plain = 0;
    if (code >= 0xA0 && code < 0xA0 + sizeof tmst_ui_fold_latin) plain = (unsigned char)tmst_ui_fold_latin[code - 0xA0];
    else if (code == 0x2013 || code == 0x2014) plain = '-';
    else if (code == 0x2018 || code == 0x2019) plain = '\'';
    else if (code == 0x201C || code == 0x201D) plain = '"';
    else if (code == 0x2022) plain = '*';
    if (plain && (g = find(f, plain)) != NULL) return g;
    return find(f, '?');
}

void tmst_fill_argb(uint32_t *dst, int w, int h, int x, int y, int rw, int rh, uint32_t argb) {
    for (int yy = y < 0 ? 0 : y; yy < y + rh && yy < h; yy++)
        for (int xx = x < 0 ? 0 : x; xx < x + rw && xx < w; xx++) dst[yy * w + xx] = argb;
}

int tmst_ui_width(const TmstUiFace *f, const char *text) {
    int pen = 0;
    unsigned c;
    if (text) while ((c = tmst_ui_next_code(&text)) != 0) pen += tmst_ui_glyph(f, c)->advance;
    return (pen + 32) >> 6;
}

static uint32_t mix(uint32_t under, uint32_t over, int v) {   /* v of 15 */
    uint32_t out = 0xFF000000u;
    for (int s = 0; s < 24; s += 8) {
        int a = (int)(under >> s & 255), b = (int)(over >> s & 255);
        out |= (uint32_t)(a + (b - a) * v / 15) << s;
    }
    return out;
}

int tmst_ui_text_argb(uint32_t *dst, int w, int h, int x, int y, const TmstUiFace *f, const char *text,
                      uint32_t argb, uint32_t matte) {
    int pen = 0, base = y + f->cap;
    unsigned c;
    if (!text) return 0;
    while ((c = tmst_ui_next_code(&text)) != 0) {
        const TmstUiGlyph *g = tmst_ui_glyph(f, c);
        int gx = x + ((pen + 32) >> 6) + g->left, gy = base + g->top, stride = (g->w + 1) / 2;
        for (int r = 0; r < g->h; r++) {
            int py = gy + r;
            if (py < 0 || py >= h) continue;
            const uint8_t *row = f->bits + g->offset + (size_t)r * stride;
            for (int q = 0; q < g->w; q++) {
                int px = gx + q, v = q & 1 ? row[q / 2] & 15 : row[q / 2] >> 4;
                if (!v || px < 0 || px >= w) continue;
                uint32_t *d = &dst[py * w + px];
                if (*d >> 24 == 255) *d = mix(*d, argb, v);
                else if (v >= 4) *d = mix(matte, argb, v);
            }
        }
        pen += g->advance;
    }
    return (pen + 32) >> 6;
}

int tmst_ui_text_scaled(uint32_t *dst, int w, int h, double x, double y, double sx, double sy, const TmstUiFace *f,
                        const char *text, uint32_t argb) {
    enum { N = 4 };                                     /* samples a pixel each way */
    int pen = 0;
    unsigned c;
    if (!text || sx <= 0 || sy <= 0) return 0;
    while ((c = tmst_ui_next_code(&text)) != 0) {
        const TmstUiGlyph *g = tmst_ui_glyph(f, c);
        double gx = x + (pen / 64.0 + g->left) * sx, gy = y + (f->cap + g->top) * sy;
        int x0 = (int)gx, x1 = (int)(gx + g->w * sx) + 1, y0 = (int)gy, y1 = (int)(gy + g->h * sy) + 1, stride = (g->w + 1) / 2;
        for (int py = y0 < 0 ? 0 : y0; py < y1 && py < h; py++)
            for (int px = x0 < 0 ? 0 : x0; px < x1 && px < w; px++) {
                int sum = 0;
                for (int j = 0; j < N; j++) {
                    int r = (int)((py + (j + 0.5) / N - gy) / sy);
                    if (r < 0 || r >= g->h || py + (j + 0.5) / N < gy) continue;
                    const uint8_t *row = f->bits + g->offset + (size_t)r * stride;
                    for (int i = 0; i < N; i++) {
                        double sxp = px + (i + 0.5) / N - gx;
                        if (sxp < 0) continue;
                        int q = (int)(sxp / sx);
                        if (q < g->w) sum += q & 1 ? row[q / 2] & 15 : row[q / 2] >> 4;
                    }
                }
                if (sum) {
                    uint32_t *d = &dst[py * w + px];
                    *d = mix(*d, argb, (sum + N * N / 2) / (N * N));
                }
            }
        pen += g->advance;
    }
    return (pen + 32) >> 6;
}

void tmst_ui_fit(const TmstUiFace *f, const char *text, int max_w, char *out, size_t size) {
    if (!size) return;
    tmst_ui_copy(text, out, size);
    if (tmst_ui_width(f, out) <= max_w) return;
    static const char dots[] = "\xE2\x80\xA6";             /* U+2026 */
    int room = max_w - tmst_ui_width(f, dots), pen = 0;
    const char *p = out, *cut = out;
    unsigned c;
    while ((c = tmst_ui_next_code(&p)) != 0) {
        pen += tmst_ui_glyph(f, c)->advance;
        if ((pen + 32) >> 6 > room) break;
        cut = p;
    }
    size_t n = (size_t)(cut - out);
    while (n && out[n - 1] == ' ') n--;
    if (n + sizeof dots > size) n = size - sizeof dots;
    memcpy(out + n, dots, sizeof dots);
}

void tmst_ui_copy(const char *in, char *out, size_t size) {
    size_t o = 0;
    if (!size) return;
    const char *p = in ? in : "";
    while (*p) {
        const char *at = p;
        unsigned c = tmst_ui_next_code(&p);
        size_t n = (size_t)(p - at);
        if (c < 32 || c == 0x7F || c == 0xFFFD) { at = " "; n = 1; }
        if (o + n + 1 > size) break;
        memcpy(out + o, at, n); o += n;
    }
    out[o] = 0;
}
