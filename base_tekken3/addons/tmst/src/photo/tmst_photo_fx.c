/* tmst.photo-mode: filters, frames and depth of field (tmst_photo_fx.h).
 * Screen units: the fight's 368 x 480 (a pixel about 1.7 times as wide as
 * tall), the wide screen's sides from -margin to 368 + margin. Colours of
 * the primitives 0xBBGGRR (the GPU's), of the pixels 0xAARRGGBB. */
#include "tmst_photo_fx.h"
#include "tmst_compat.h"
#include "tmst_ui_font.h"
#include "tmst_core.h"
#include "tmst_files.h"
#include "tmst_png.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *const tmst_photo_filter_names[FX_FILTERS] = {
    "None", "Black & white", "Sepia", "Noir", "Vintage", "High contrast", "Cold", "Warm", "Teal & orange",
    "Duotone", "Poster", "Night vision", "Scanlines", "Negative"};
const char *const tmst_photo_frame_names[FR_FRAMES] = {"None", "Letterbox", "Polaroid", "Film strip", "Vignette", "T3E red", "T3E black",
                                                       "CRT TV", "Comic"};

int tmst_photo_filter_cpu(int f) {
    return f == FX_BW || f == FX_SEPIA || f == FX_NOIR || f == FX_TEAL_ORANGE || f == FX_DUOTONE || f == FX_POSTER ||
           f == FX_NIGHT || f == FX_NEGATIVE;
}

enum { W = 368, H = 480, OPAQUE = -1, HALF = 0, ADD = 1, SUB = 2, QUARTER = 3 };

/* ---- the canvas: the GPU's primitives, or the CPU's pixels -------------------------- */
typedef struct {
    TmstPrims *k;                      /* live */
    uint32_t *px; int w, h;            /* the photo: pixels ... */
    double s;                          /* ... `s` to a screen unit each way */
    int left, right;                   /* the screen's extent */
} Canvas;

static Canvas live(TmstPrims *k) {
    Canvas c = {k, NULL, 0, 0, 1.0, -tmst_compat_widescreen_side(), W + tmst_compat_widescreen_side()};
    return c;
}
static Canvas pixels(uint32_t *px, int w, int h, int scale, int side) {
    double s = scale > 0 ? scale : 1;
    int m = (int)lround(side / s);
    Canvas c = {NULL, px, w, h, s, -m, W + m};
    return c;
}

static int chan(uint32_t v, int sh) { return (int)(v >> sh & 0xFF); }
static int clamp8(int v) { return v < 0 ? 0 : v > 255 ? 255 : v; }
static uint32_t rgb(int r, int g, int b) { return 0xFF000000u | (uint32_t)clamp8(r) << 16 | (uint32_t)clamp8(g) << 8 | (uint32_t)clamp8(b); }
/* A pixel under a primitive's colour (0xBBGGRR) in a blend mode, as the GPU. */
static uint32_t blend(uint32_t d, uint32_t bgr, int m) {
    int fr = chan(bgr, 0), fg = chan(bgr, 8), fb = chan(bgr, 16), br = chan(d, 16), bg = chan(d, 8), bb = chan(d, 0);
    switch (m) {
    case HALF: return rgb((br + fr) / 2, (bg + fg) / 2, (bb + fb) / 2);
    case ADD: return rgb(br + fr, bg + fg, bb + fb);
    case SUB: return rgb(br - fr, bg - fg, bb - fb);
    case QUARTER: return rgb(br + fr / 4, bg + fg / 4, bb + fb / 4);
    default: return rgb(fr, fg, fb);
    }
}
/* Screen x / y to pixel columns / rows (where the GPU at that scale puts the
 * primitive's edge). */
static int col(const Canvas *c, double x) { int v = (int)lround((x - c->left) * c->s); return v < 0 ? 0 : v > c->w ? c->w : v; }
static int row(const Canvas *c, double y) { int v = (int)lround(y * c->s); return v < 0 ? 0 : v > c->h ? c->h : v; }

static void mode(TmstPrims *k, int m) {   /* dithered, drawing to the display */
    tmst_prim_room(k, 1);
    tmst_prim_word(k, 0xE1000600u | (uint32_t)(m < 0 ? 0 : m) << 5);
}
static void rect(Canvas *c, int x, int y, int w, int h, uint32_t bgr, int m) {
    if (w <= 0 || h <= 0) return;
    if (c->k) {
        if (m >= 0) mode(c->k, m);
        tmst_prim_room(c->k, 3);
        tmst_prim_word(c->k, (m < 0 ? 0x60000000u : 0x62000000u) | (bgr & 0xFFFFFFu));
        tmst_prim_word(c->k, tmst_xy(x, y));
        tmst_prim_word(c->k, (uint32_t)h << 16 | (uint32_t)w);
        return;
    }
    int x0 = col(c, x), x1 = col(c, x + w), y0 = row(c, y), y1 = row(c, y + h);
    for (int py = y0; py < y1; py++)
        for (uint32_t *p = c->px + (size_t)py * c->w + x0, *e = p + (x1 - x0); p < e; p++) *p = blend(*p, bgr, m);
}
static void whole(Canvas *c, uint32_t bgr, int m) { rect(c, c->left - 8, 0, c->right - c->left + 16, H, bgr, m); }

/* A Gouraud quad (corners 0 1 on one edge, 2 3 on the other: triangles 0 1 2
 * and 1 2 3), half see-through in mode `m` (OPAQUE: not). */
static void triangle(Canvas *c, const double x[3], const double y[3], const uint32_t k[3], int m) {
    double ax = (x[0] - c->left) * c->s, ay = y[0] * c->s, bx = (x[1] - c->left) * c->s, by = y[1] * c->s;
    double cx = (x[2] - c->left) * c->s, cy = y[2] * c->s;
    double area = (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
    if (fabs(area) < 1e-9) return;
    int x0 = (int)floor(fmin(ax, fmin(bx, cx))), x1 = (int)ceil(fmax(ax, fmax(bx, cx)));
    int y0 = (int)floor(fmin(ay, fmin(by, cy))), y1 = (int)ceil(fmax(ay, fmax(by, cy)));
    if (x0 < 0) x0 = 0; if (y0 < 0) y0 = 0; if (x1 > c->w) x1 = c->w; if (y1 > c->h) y1 = c->h;
    for (int py = y0; py < y1; py++)
        for (int qx = x0; qx < x1; qx++) {
            double px = qx + 0.5, pyc = py + 0.5;
            double w0 = ((bx - px) * (cy - pyc) - (cx - px) * (by - pyc)) / area;
            double w1 = ((cx - px) * (ay - pyc) - (ax - px) * (cy - pyc)) / area;
            double w2 = 1 - w0 - w1;
            if (w0 < 0 || w1 < 0 || w2 < 0) continue;
            int r = (int)lround(w0 * chan(k[0], 0) + w1 * chan(k[1], 0) + w2 * chan(k[2], 0));
            int g = (int)lround(w0 * chan(k[0], 8) + w1 * chan(k[1], 8) + w2 * chan(k[2], 8));
            int b = (int)lround(w0 * chan(k[0], 16) + w1 * chan(k[1], 16) + w2 * chan(k[2], 16));
            uint32_t *p = c->px + (size_t)py * c->w + qx;
            *p = blend(*p, (uint32_t)b << 16 | (uint32_t)g << 8 | (uint32_t)r, m);
        }
}
static void quad(Canvas *c, const int x[4], const int y[4], const uint32_t k[4], int m) {
    if (c->k) {
        if (m >= 0) mode(c->k, m);
        tmst_prim_room(c->k, 8);
        for (int i = 0; i < 4; i++) {
            tmst_prim_word(c->k, (i ? 0u : m < 0 ? 0x38000000u : 0x3A000000u) | (k[i] & 0xFFFFFFu));
            tmst_prim_word(c->k, tmst_xy(x[i], y[i]));
        }
        return;
    }
    double ax[3] = {x[0], x[1], x[2]}, ay[3] = {y[0], y[1], y[2]}, bx[3] = {x[1], x[2], x[3]}, by[3] = {y[1], y[2], y[3]};
    uint32_t ak[3] = {k[0], k[1], k[2]}, bk[3] = {k[1], k[2], k[3]};
    triangle(c, ax, ay, ak, m);
    triangle(c, bx, by, bk, m);
}
/* The UI font (Fira Sans, tmst_ui_font.h), its capitals' top at y + 1
 * (where the old 5 x 7 font's 14-unit capitals were centred, in a cell from
 * y): the GPU's quads live, the same glyphs over the photo's pixels, white
 * times tint / 128. Returns the width in screen units. */
static int text(Canvas *c, int x, int y, const char *s, uint32_t tint) {
    if (c->k) return tmst_ui_prim_text(c->k, x, y + 1, s, 1, tint);
    uint32_t colour = rgb(255 * chan(tint, 0) / 128, 255 * chan(tint, 8) / 128, 255 * chan(tint, 16) / 128);
    tmst_ui_text_scaled(c->px, c->w, c->h, (x - c->left) * c->s, (y + 1) * c->s, c->s / 2, c->s / 2,
                        &tmst_ui_face_screen, s, colour);
    return tmst_ui_prim_width(s, 1);
}
/* The frame plus itself times tint / 128 (the 368-pixel screen only, as the
 * GPU's copy). */
static void boost(Canvas *c, int tint) {
    if (c->k) { tmst_prim_screen_copy(c->k, 0, 0, W, H, 0, 0, (uint32_t)tint * 0x010101u, ADD); return; }
    int x0 = col(c, 0), x1 = col(c, W);
    for (int py = 0; py < c->h; py++)
        for (uint32_t *p = c->px + (size_t)py * c->w + x0, *e = p + (x1 - x0); p < e; p++) {
            int r = chan(*p, 16), g = chan(*p, 8), b = chan(*p, 0);
            *p = rgb(r + r * tint / 128, g + g * tint / 128, b + b * tint / 128);
        }
}

/* Darker towards the edges: two rings, from an ellipse at 55 % of the
 * screen to one at 90 % (nothing to `mid` taken off), and from there to past
 * the screen's border (`mid` to `edge`), subtracted. */
static void vignette(Canvas *c, uint32_t mid, uint32_t edge) {
    enum { N = 24 };
    double cx = (c->left + c->right) / 2.0, rx = (c->right - c->left) / 2.0, cy = H / 2.0, ry = H / 2.0;
    for (int i = 0; i < N; i++) {
        int px[2][3], py[2][3];
        for (int j = 0; j < 2; j++) {
            double a = (i + j) * 6.283185307179586 / N, co = cos(a), si = sin(a);
            double out = fmin(fabs(co) > 1e-6 ? (rx + 6) / (rx * fabs(co)) : 9, fabs(si) > 1e-6 ? (ry + 6) / (ry * fabs(si)) : 9);
            const double scale[3] = {0.55, 0.9, out};
            for (int r = 0; r < 3; r++) { px[j][r] = (int)lround(cx + co * rx * scale[r]); py[j][r] = (int)lround(cy + si * ry * scale[r]); }
        }
        for (int r = 0; r < 2; r++) {
            int x[4] = {px[0][r], px[1][r], px[0][r + 1], px[1][r + 1]};
            int y[4] = {py[0][r], py[1][r], py[0][r + 1], py[1][r + 1]};
            uint32_t a = r ? mid : 0, b = r ? edge : mid;
            uint32_t k[4] = {a, a, b, b};
            quad(c, x, y, k, SUB);
        }
    }
}
/* Every other line of the screen darker (its native lines). */
static void scanlines(Canvas *c, uint32_t dark) {
    for (int y = 1; y < H; y += 2) rect(c, c->left - 8, y, c->right - c->left + 16, 1, dark, SUB);
}

/* ---- filters ---------------------------------------------------------------------- */
/* The GPU's part of filter f; `close`: its close look too (a CPU filter
 * live while the camera moves). */
static void filter_ops(Canvas *c, int f, int close) {
    switch (f) {
    case FX_BW:
        if (close) { whole(c, 0x606060, HALF); whole(c, 0x0c0c0c, SUB); boost(c, 0x20); }
        break;
    case FX_SEPIA:
        if (close) { whole(c, 0x3c6484, HALF); whole(c, 0x100602, SUB); boost(c, 0x18); }
        break;
    case FX_NOIR:
        if (close) { whole(c, 0x606060, HALF); whole(c, 0x1c1c1c, SUB); boost(c, 0x38); }
        vignette(c, 0x141414, 0x585858);
        break;
    case FX_VINTAGE:                               /* less blue, a warm lift, darker corners */
        whole(c, 0x2c0c04, SUB);
        whole(c, 0x4070a0, QUARTER);
        vignette(c, 0x101010, 0x404040);
        break;
    case FX_CONTRAST:                              /* (x - c) * 1.375 */
        whole(c, 0x161616, SUB);
        boost(c, 0x30);
        break;
    case FX_COLD:
        whole(c, 0x1c0800, ADD);
        whole(c, 0x000a16, SUB);
        break;
    case FX_WARM:
        whole(c, 0x000a1c, ADD);
        whole(c, 0x160000, SUB);
        break;
    case FX_TEAL_ORANGE:
        if (close) { whole(c, 0x041430, ADD); whole(c, 0x160406, SUB); boost(c, 0x20); }
        break;
    case FX_DUOTONE:
        if (close) { whole(c, 0x3038b0, HALF); boost(c, 0x20); }
        break;
    case FX_POSTER:
        if (close) { whole(c, 0x101010, SUB); boost(c, 0x30); }
        break;
    case FX_NIGHT:
        if (close) { whole(c, 0x30a018, HALF); whole(c, 0x200820, SUB); boost(c, 0x30); }
        scanlines(c, 0x202020);
        vignette(c, 0x181818, 0x686868);
        break;
    case FX_SCANLINES:
        scanlines(c, 0x303030);
        break;
    default:                                       /* NONE; NEGATIVE: nothing close */
        break;
    }
}

void tmst_photo_filter_prims(TmstPrims *k, int f) {
    Canvas c = live(k);
    filter_ops(&c, f, tmst_photo_filter_cpu(f));
}

static int luma(uint32_t p) { return (299 * chan(p, 16) + 587 * chan(p, 8) + 114 * chan(p, 0) + 500) / 1000; }
static int mix(int a, int b, int t, int n) { return a + (b - a) * t / n; }
static unsigned grain(unsigned x, unsigned y, unsigned seed) {
    unsigned h = x * 374761393u + y * 668265263u + seed * 2246822519u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

static void filter_pixels(uint32_t *px, int w, int h, int f, unsigned seed) {
    if (!tmst_photo_filter_cpu(f)) return;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            uint32_t *p = px + (size_t)y * w + x;
            int r = chan(*p, 16), g = chan(*p, 8), b = chan(*p, 0), l = luma(*p);
            switch (f) {
            case FX_BW: *p = rgb(l, l, l); break;
            case FX_SEPIA:
                *p = rgb((393 * r + 769 * g + 189 * b) / 1000, (349 * r + 686 * g + 168 * b) / 1000,
                         (272 * r + 534 * g + 131 * b) / 1000);
                break;
            case FX_NOIR: { int v = (l - 128) * 150 / 100 + 118; *p = rgb(v, v, v); break; }
            case FX_TEAL_ORANGE: {
                /* the blockbuster grade: more colour and contrast, the shadows
                 * pushed to teal, the lights (skin) to orange */
                int c = clamp8((l - 128) * 118 / 100 + 128);
                int sr = c + (r - l) * 140 / 100, sg = c + (g - l) * 140 / 100, sb = c + (b - l) * 140 / 100;
                if (c < 140) { int t = 140 - c; sr -= 70 * t / 140; sg += 18 * t / 140; sb += 60 * t / 140; }
                else { int t = c - 140; sr += 62 * t / 115; sg += 18 * t / 115; sb -= 64 * t / 115; }
                *p = rgb(sr, sg, sb);
                break;
            }
            case FX_DUOTONE: {
                /* deep crimson shadows, Tekken red middle, cream lights */
                int v = clamp8((l - 128) * 115 / 100 + 128);
                if (v < 128) *p = rgb(mix(16, 176, v, 128), mix(4, 22, v, 128), mix(10, 34, v, 128));
                else *p = rgb(mix(176, 255, v - 128, 127), mix(22, 238, v - 128, 127), mix(34, 204, v - 128, 127));
                break;
            }
            case FX_POSTER: {
                /* five steps a channel, a little more colour first */
                int q[3] = {l + (r - l) * 120 / 100, l + (g - l) * 120 / 100, l + (b - l) * 120 / 100};
                for (int i = 0; i < 3; i++) q[i] = (clamp8(q[i]) * 4 + 127) / 255 * 255 / 4;
                *p = rgb(q[0], q[1], q[2]);
                break;
            }
            case FX_NIGHT: {
                int v = l * 130 / 100 + 12 + (int)(grain((unsigned)x, (unsigned)y, seed) % 25) - 12;
                *p = rgb(v * 30 / 100, v, v * 35 / 100);
                break;
            }
            case FX_NEGATIVE: *p = rgb(255 - r, 255 - g, 255 - b); break;
            }
        }
}

/* ---- frames ---------------------------------------------------------------------- */
static void letterbox(Canvas *c) {
    /* bars to 2.35 : 1 on the screen's own aspect (4:3, wider with the sides) */
    double aspect = 4.0 / 3.0 * (c->right - c->left) / (double)W;
    int bar = (int)lround(H * (1.0 - aspect / 2.35) / 2.0);
    if (bar < 8) bar = 8;
    rect(c, c->left - 8, 0, c->right - c->left + 16, bar, 0x000000, OPAQUE);
    rect(c, c->left - 8, H - bar, c->right - c->left + 16, bar, 0x000000, OPAQUE);
}

static void polaroid(Canvas *c) {
    enum { SIDE = 20, TOP = 32, BOTTOM = 108, PAPER = 0xd0dce4, SHADE = 0x282828 };
    int l = c->left, r = c->right;
    rect(c, l - 8, 0, SIDE + 8, H, PAPER, OPAQUE);
    rect(c, r - SIDE, 0, SIDE + 8, H, PAPER, OPAQUE);
    rect(c, l + SIDE, 0, r - l - 2 * SIDE, TOP, PAPER, OPAQUE);
    rect(c, l + SIDE, H - BOTTOM, r - l - 2 * SIDE, BOTTOM, PAPER, OPAQUE);
    /* the print's edge, a little sunk */
    rect(c, l + SIDE, TOP, r - l - 2 * SIDE, 2, SHADE, OPAQUE);
    rect(c, l + SIDE, TOP + 2, 1, H - TOP - BOTTOM - 2, SHADE, OPAQUE);
}

static void film(Canvas *c) {
    enum { BAND = 30, HOLE_W = 12, HOLE_H = 18, PITCH = 44, BASE = 0x101418, HOLE = 0xb0c4cc, EDGE = 0x404850 };
    int l = c->left, r = c->right;
    rect(c, l - 8, 0, BAND + 8, H, BASE, OPAQUE);
    rect(c, r - BAND, 0, BAND + 8, H, BASE, OPAQUE);
    rect(c, l + BAND - 2, 0, 2, H, EDGE, OPAQUE);
    rect(c, r - BAND, 0, 2, H, EDGE, OPAQUE);
    for (int y = 10; y + HOLE_H <= H; y += PITCH) {
        rect(c, l + (BAND - HOLE_W) / 2, y, HOLE_W, HOLE_H, HOLE, OPAQUE);
        rect(c, r - BAND + (BAND - HOLE_W) / 2, y, HOLE_W, HOLE_H, HOLE, OPAQUE);
    }
}

/* Tekken 3's own big letters (its ROUND / FIGHT words), as the fight keeps
 * them in VRAM, paused too (measured: Arcade, Tekken Ball): page (896, 0),
 * 4 bpp, cells of 24 x 40 (the letter in rows 1..38), eight to a row: A..H,
 * I..P, Q..X, Y Z ! ? . ' 1 2, 3..9 -; the palette at (384, 506) (0x7E98):
 * silver, a dark red edge. Read back once in a while (the lower VRAM half,
 * tmst_compat_vram_read) for each letter's width and for the photo's CPU,
 * which draws them filtered at its scale; live, textured quads from the page
 * itself. Not there (no read-back, or not these letters): the UI font (Fira Sans). */
enum { T3F_PAGE_X = 896, T3F_TW = 192, T3F_TH = 200, T3F_CW = 24, T3F_CH = 40, T3F_V0 = 1, T3F_V1 = 39,
       T3F_CLUT_X = 384, T3F_CLUT_Y = 506, T3F_RECHECK = 60 };
static const char t3f_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!?.'123456789-";
static struct {
    int ok, age;
    uint16_t tex[T3F_TH][T3F_TW / 4], pal[16];
    int x0[sizeof t3f_chars], x1[sizeof t3f_chars];       /* each letter's ink, from its cell's left */
} t3f;
static int t3f_index(char ch) {
    const char *p = ch ? strchr(t3f_chars, ch) : NULL;
    return p ? (int)(p - t3f_chars) : -1;
}
static int t3f_texel(int u, int v) { return t3f.tex[v][u / 4] >> (u % 4 * 4) & 15; }
static int t3f_load(int force) {
    if (!force && t3f.age && --t3f.age) return t3f.ok;
    t3f.age = T3F_RECHECK; t3f.ok = 0;
    if (!tmst_compat_vram_read(T3F_PAGE_X, 0, T3F_TW / 4, T3F_TH, &t3f.tex[0][0]) ||
        !tmst_compat_vram_read(T3F_CLUT_X, T3F_CLUT_Y, 16, 1, t3f.pal))
        return 0;
    if (t3f.pal[0] || !t3f.pal[1]) return 0;
    for (int i = 0; t3f_chars[i]; i++) {
        int cu = i % 8 * T3F_CW, cv = i / 8 * T3F_CH, lo = T3F_CW, hi = -1, gap = 1;
        for (int v = cv; v < cv + T3F_CH; v++)
            for (int u = cu; u < cu + T3F_CW; u++)
                if (t3f_texel(u, v)) {
                    if (v == cv || v == cv + T3F_CH - 1) gap = 0;   /* the rows between the letters are clear */
                    if (u - cu < lo) lo = u - cu;
                    if (u - cu > hi) hi = u - cu;
                }
        if (!gap || hi - lo < 3) return 0;
        t3f.x0[i] = lo; t3f.x1[i] = hi + 1;
    }
    t3f.ok = 1;
    return 1;
}
/* A texel's colour (0xAARRGGBB, A 0 where see-through), the palette's
 * 5-bit channels to 8. */
static uint32_t t3f_argb(int u, int v) {
    int i = t3f_texel(u, v);
    if (!i) return 0;
    uint16_t p = t3f.pal[i];
    int r = (p & 31) * 255 / 31, g = (p >> 5 & 31) * 255 / 31, b = (p >> 10 & 31) * 255 / 31;
    return 0xFF000000u | (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b;
}
/* The text's width at letter height h (screen units: a letter (x1 - x0) / 2
 * wide at h 19, the game's look halved), `gap` between letters. */
static double t3f_scale(int h) { return h / (double)(T3F_V1 - T3F_V0); }
static int t3f_width(const char *s, int h, int gap) {
    double k = t3f_scale(h), w = 0;
    for (; *s; s++) {
        int i = t3f_index(*s);
        w += i < 0 ? h * 0.3 : (t3f.x1[i] - t3f.x0[i]) * k + gap;
    }
    return (int)lround(w - gap);
}
static void t3f_letter(Canvas *c, double x, int y, int i, int h) {
    double k = t3f_scale(h), w = (t3f.x1[i] - t3f.x0[i]) * k;
    int u0 = i % 8 * T3F_CW + t3f.x0[i], u1 = i % 8 * T3F_CW + t3f.x1[i];
    int v0 = i / 8 * T3F_CH + T3F_V0, v1 = i / 8 * T3F_CH + T3F_V1;
    if (c->k) {
        int xa = (int)lround(x), xb = (int)lround(x + w);
        uint32_t clut = (uint32_t)T3F_CLUT_Y << 6 | T3F_CLUT_X / 16, tpage = T3F_PAGE_X / 64;
        tmst_prim_room(c->k, 9);
        tmst_prim_word(c->k, 0x2C808080u);
        tmst_prim_word(c->k, tmst_xy(xa, y));     tmst_prim_word(c->k, clut << 16 | (uint32_t)v0 << 8 | (uint32_t)u0);
        tmst_prim_word(c->k, tmst_xy(xb, y));     tmst_prim_word(c->k, tpage << 16 | (uint32_t)v0 << 8 | (uint32_t)u1);
        tmst_prim_word(c->k, tmst_xy(xa, y + h)); tmst_prim_word(c->k, (uint32_t)v1 << 8 | (uint32_t)u0);
        tmst_prim_word(c->k, tmst_xy(xb, y + h)); tmst_prim_word(c->k, (uint32_t)v1 << 8 | (uint32_t)u1);
        return;
    }
    /* the photo: each pixel from the texels around its centre (bilinear,
     * the colour weighted by the ink), over what is there */
    int px0 = col(c, x), px1 = col(c, x + w), py0 = row(c, y), py1 = row(c, y + h);
    double du = (u1 - u0) / ((x + w - x) * c->s), dv = (v1 - v0) / (h * c->s);
    double ox = (x - c->left) * c->s, oy = y * c->s;
    for (int py = py0; py < py1; py++)
        for (int qx = px0; qx < px1; qx++) {
            double u = u0 + (qx + 0.5 - ox) * du - 0.5, v = v0 + (py + 0.5 - oy) * dv - 0.5;
            int iu = (int)floor(u), iv = (int)floor(v);
            double fu = u - iu, fv = v - iv, a = 0, r = 0, g = 0, b = 0;
            for (int n = 0; n < 4; n++) {
                int tu = iu + (n & 1), tv = iv + (n >> 1);
                if (tu < u0 || tu >= u1 || tv < v0 || tv >= v1) continue;
                uint32_t t = t3f_argb(tu, tv);
                if (!t) continue;
                double wgt = (n & 1 ? fu : 1 - fu) * (n >> 1 ? fv : 1 - fv);
                a += wgt; r += wgt * chan(t, 16); g += wgt * chan(t, 8); b += wgt * chan(t, 0);
            }
            if (a <= 0) continue;
            uint32_t *p = c->px + (size_t)py * c->w + qx;
            double keep = 1 - a;                               /* r g b: already times a */
            *p = rgb((int)lround(chan(*p, 16) * keep + r), (int)lround(chan(*p, 8) * keep + g),
                     (int)lround(chan(*p, 0) * keep + b));
        }
}
static void t3f_text(Canvas *c, int x, int y, const char *s, int h, int gap) {
    double k = t3f_scale(h), at = x;
    for (; *s; s++) {
        int i = t3f_index(*s);
        if (i < 0) { at += h * 0.3; continue; }
        t3f_letter(c, at, y, i, h);
        at += (t3f.x1[i] - t3f.x0[i]) * k + gap;
    }
}

/* The T3E logo (data/photo/t3e_logo.png, ARGB with soft edges), drawn
 * LOGO_W x LOGO_H screen units (square to the eye: a unit about 1.74 times
 * as wide as tall). Live: LIVE_W x LIVE_H texels in the upper VRAM half
 * (tmst_overlay_set_sheet), averaged down, the soft edge in three steps (see-
 * through, half, opaque: the GPU's own); the photo: from the PNG itself at
 * the photo's scale, every edge as soft as drawn. */
enum { LOGO_W = 69, LOGO_H = 120, LIVE_W = 64, LIVE_H = 112 };
static struct {
    int tried, ok, sheet_set;
    uint32_t *px; int w, h;                            /* the PNG, alpha premultiplied */
    uint32_t live[LIVE_W * LIVE_H];
} logo;
static int logo_load(void) {
    if (logo.tried) return logo.ok;
    logo.tried = 1;
    char path[4200];
    tmst_files_addon(path, sizeof path, "data/photo/t3e_logo.png");   /* or the copy in mods/tmst-data */
    if (!tmst_png_read(path, &logo.px, &logo.w, &logo.h)) { tmst_log("photo: no logo at %s", path); return 0; }
    for (int i = 0; i < logo.w * logo.h; i++) {
        uint32_t p = logo.px[i], a = p >> 24;
        logo.px[i] = a << 24 | (chan(p, 16) * a / 255) << 16 | (chan(p, 8) * a / 255) << 8 | chan(p, 0) * a / 255;
    }
    /* the live sheet: each texel the average of its source box */
    for (int ty = 0; ty < LIVE_H; ty++)
        for (int tx = 0; tx < LIVE_W; tx++) {
            int sx0 = tx * logo.w / LIVE_W, sx1 = (tx + 1) * logo.w / LIVE_W, sy0 = ty * logo.h / LIVE_H, sy1 = (ty + 1) * logo.h / LIVE_H;
            unsigned a = 0, r = 0, g = 0, b = 0, n = 0;
            for (int y = sy0; y < sy1; y++)
                for (int x = sx0; x < sx1; x++, n++) {
                    uint32_t p = logo.px[y * logo.w + x];
                    a += p >> 24; r += chan(p, 16); g += chan(p, 8); b += chan(p, 0);
                }
            a /= n; r /= n; g /= n; b /= n;
            uint32_t out = 0;
            if (a >= 72) {                             /* the colour unpremultiplied; alpha in the sheet's steps */
                unsigned sa = a >= 200 ? 255 : 160;
                out = sa << 24 | (r * 255 / a) << 16 | (g * 255 / a) << 8 | b * 255 / a;
            }
            logo.live[ty * LIVE_W + tx] = out;
        }
    logo.ok = 1;
    return 1;
}
static uint32_t logo_at(int x, int y) {
    if (x < 0 || y < 0 || x >= logo.w || y >= logo.h) return 0;
    return logo.px[y * logo.w + x];
}
static void logo_draw(Canvas *c, int x, int y) {
    if (!logo_load()) return;
    if (c->k) {
        if (!logo.sheet_set) logo.sheet_set = tmst_overlay_set_sheet(logo.live, LIVE_W, LIVE_H);
        tmst_prim_sheet(c->k, x, y, LOGO_W, LOGO_H);
        return;
    }
    /* the photo: 2 x 2 bilinear samples a pixel from the premultiplied PNG, over */
    int px0 = col(c, x), px1 = col(c, x + LOGO_W), py0 = row(c, y), py1 = row(c, y + LOGO_H);
    double ox = (x - c->left) * c->s, oy = y * c->s;
    double su = logo.w / (LOGO_W * c->s), sv = logo.h / (LOGO_H * c->s);
    for (int py = py0; py < py1; py++)
        for (int qx = px0; qx < px1; qx++) {
            double a = 0, r = 0, g = 0, b = 0;
            for (int s = 0; s < 4; s++) {
                double u = (qx + 0.25 + 0.5 * (s & 1) - ox) * su - 0.5, v = (py + 0.25 + 0.5 * (s >> 1) - oy) * sv - 0.5;
                int iu = (int)floor(u), iv = (int)floor(v);
                double fu = u - iu, fv = v - iv;
                for (int n = 0; n < 4; n++) {
                    uint32_t t = logo_at(iu + (n & 1), iv + (n >> 1));
                    double wgt = (n & 1 ? fu : 1 - fu) * (n >> 1 ? fv : 1 - fv) / 4;
                    a += wgt * (t >> 24); r += wgt * chan(t, 16); g += wgt * chan(t, 8); b += wgt * chan(t, 0);
                }
            }
            if (a <= 0.5) continue;
            uint32_t *p = c->px + (size_t)py * c->w + qx;
            double keep = 1 - a / 255;
            *p = rgb((int)lround(chan(*p, 16) * keep + r), (int)lround(chan(*p, 8) * keep + g), (int)lround(chan(*p, 0) * keep + b));
        }
}

static void tekken(Canvas *c, uint32_t base, uint32_t line, uint32_t marks) {
    enum { SIDE = 10, TOP = 18, BOTTOM = 30, ARM_X = 28, ARM_Y = 44, T = 3 };
    int l = c->left, r = c->right;
    rect(c, l - 8, 0, r - l + 16, TOP, base, OPAQUE);
    rect(c, l - 8, H - BOTTOM, r - l + 16, BOTTOM, base, OPAQUE);
    rect(c, l - 8, TOP, SIDE + 8, H - TOP - BOTTOM, base, OPAQUE);
    rect(c, r - SIDE, TOP, SIDE + 8, H - TOP - BOTTOM, base, OPAQUE);
    /* a line inside, and corners over it */
    int x0 = l + SIDE, x1 = r - SIDE, y0 = TOP, y1 = H - BOTTOM;
    rect(c, x0, y0, x1 - x0, 1, line, OPAQUE);
    rect(c, x0, y1 - 1, x1 - x0, 1, line, OPAQUE);
    rect(c, x0, y0, 1, y1 - y0, line, OPAQUE);
    rect(c, x1 - 1, y0, 1, y1 - y0, line, OPAQUE);
    rect(c, x0, y0, ARM_X, T, line, OPAQUE);            rect(c, x0, y0, T, ARM_Y, line, OPAQUE);
    rect(c, x1 - ARM_X, y0, ARM_X, T, line, OPAQUE);   rect(c, x1 - T, y0, T, ARM_Y, line, OPAQUE);
    rect(c, x0, y1 - T, ARM_X, T, line, OPAQUE);       rect(c, x0, y1 - ARM_Y, T, ARM_Y, line, OPAQUE);
    rect(c, x1 - ARM_X, y1 - T, ARM_X, T, line, OPAQUE); rect(c, x1 - T, y1 - ARM_Y, T, ARM_Y, line, OPAQUE);
    /* the logo over the bottom left corner, into the picture */
    int lx = l + 2;
    logo_draw(c, lx, H - LOGO_H - 4);
    /* the name in the bottom bar, marks either side, centred right of the
     * logo: Tekken 3's letters (the UI font, gold, where they are not
     * there) */
    static const char name[] = "TEKKEN 3 EXPANDED";
    enum { NAME_H = 19, NAME_GAP = 1 };
    int mid = (lx + LOGO_W + r) / 2;
    if (t3f_load(!c->k)) {
        int w = t3f_width(name, NAME_H, NAME_GAP), tx = mid - w / 2, ty = H - BOTTOM + (BOTTOM - NAME_H) / 2 + 1;
        rect(c, tx - 22, ty + NAME_H / 2 - 2, 12, 4, marks, OPAQUE);
        rect(c, tx + w + 10, ty + NAME_H / 2 - 2, 12, 4, marks, OPAQUE);
        t3f_text(c, tx, ty, name, NAME_H, NAME_GAP);
        return;
    }
    int w = tmst_ui_prim_width(name, 1), tx = mid - w / 2, ty = H - BOTTOM + 7;
    rect(c, tx - 22, ty + 6, 12, 4, marks, OPAQUE);
    rect(c, tx + w + 10, ty + 6, 12, 4, marks, OPAQUE);
    text(c, tx, ty, name, 0x205878);
}

/* A quarter disc of `bgr` filling the corner (cx, cy) outside the circle of
 * radius r (rx, ry in screen units: the pixels are not square) about the
 * corner's inner centre, (dx, dy) the way into the screen. */
static void round_corner(Canvas *c, int cx, int cy, int dx, int dy, int rx, int ry, uint32_t bgr) {
    enum { STEPS = 8 };
    int ox = cx + dx * rx, oy = cy + dy * ry;          /* the circle's centre */
    for (int i = 0; i < STEPS; i++) {
        double a0 = i * 1.5707963267948966 / STEPS, a1 = (i + 1) * 1.5707963267948966 / STEPS;
        int x[4] = {cx, ox - (int)lround(dx * rx * cos(a0)), cx, ox - (int)lround(dx * rx * cos(a1))};
        int y[4] = {cy, oy - (int)lround(dy * ry * sin(a0)), cy, oy - (int)lround(dy * ry * sin(a1))};
        uint32_t k[4] = {bgr, bgr, bgr, bgr};
        quad(c, x, y, k, OPAQUE);
    }
}

/* An old television's bezel: dark plastic, the glass's rounded corners, its
 * lines and darker edges, a red light under it. */
static void crt(Canvas *c) {
    enum { SIDE = 22, TOP = 26, BOTTOM = 40, PLASTIC = 0x181a1c, RIM = 0x383c40, RX = 20, RY = 34 };
    int l = c->left, r = c->right, x0 = l + SIDE, x1 = r - SIDE, y0 = TOP, y1 = H - BOTTOM;
    scanlines(c, 0x141414);
    vignette(c, 0x101010, 0x383838);
    rect(c, l - 8, 0, r - l + 16, TOP, PLASTIC, OPAQUE);
    rect(c, l - 8, y1, r - l + 16, BOTTOM, PLASTIC, OPAQUE);
    rect(c, l - 8, TOP, SIDE + 8, y1 - y0, PLASTIC, OPAQUE);
    rect(c, x1, TOP, SIDE + 8, y1 - y0, PLASTIC, OPAQUE);
    round_corner(c, x0, y0, 1, 1, RX, RY, PLASTIC);
    round_corner(c, x1, y0, -1, 1, RX, RY, PLASTIC);
    round_corner(c, x0, y1, 1, -1, RX, RY, PLASTIC);
    round_corner(c, x1, y1, -1, -1, RX, RY, PLASTIC);
    rect(c, x0 + RX, y0 - 2, x1 - x0 - 2 * RX, 2, RIM, OPAQUE);
    rect(c, x0 + RX, y1, x1 - x0 - 2 * RX, 2, RIM, OPAQUE);
    rect(c, x1 - 26, y1 + 18, 6, 6, 0x1010d0, OPAQUE);   /* the power light */
    rect(c, x1 - 60, y1 + 19, 28, 4, 0x2c3034, OPAQUE);
}

/* Halftone dots in the corner (cx, cy) of the panel, (dx, dy) the way into
 * it: rows of dots, every other row moved half a step, all but touching at
 * the corner and smaller towards the middle until they are gone. Screen
 * units; a pixel is about 1.7 times as wide as tall, so a step is 8 across
 * and 13 down (about the same length), a dot 0.6 as wide as tall. */
static void halftone(Canvas *c, int cx, int cy, int dx, int dy, uint32_t bgr) {
    enum { STEP_X = 8, STEP_Y = 13, ROWS = 10, COLS = 17, REACH = 13 };   /* REACH: in steps */
    for (int j = 0; j < ROWS; j++)
        for (int i = 0; i < COLS; i++) {
            double u = i + (j & 1) * 0.5 + 0.5, v = j + 0.5;          /* steps from the corner */
            double fade = 1.0 - sqrt(u * u + v * v) / REACH;
            if (fade <= 0.3) continue;                                /* smaller ones round to bars */
            double ry = 7.4 * fade, rx = ry * 0.6;                    /* the dot's half sizes */
            double px = cx + dx * u * STEP_X, py = cy + dy * v * STEP_Y;
            /* an octagon, as three quads across it: p0 p1 p7 p2, p7 p2 p6 p3, p6 p3 p5 p4 */
            int ox[8], oy[8];
            for (int n = 0; n < 8; n++) {
                double a = (n + 0.5) * 0.7853981633974483;
                ox[n] = (int)lround(px + rx * cos(a)); oy[n] = (int)lround(py + ry * sin(a));
            }
            static const int strip[3][4] = {{0, 1, 7, 2}, {7, 2, 6, 3}, {6, 3, 5, 4}};
            uint32_t k[4] = {bgr, bgr, bgr, bgr};
            for (int q = 0; q < 3; q++) {
                int x[4], y[4];
                for (int e = 0; e < 4; e++) { x[e] = ox[strip[q][e]]; y[e] = oy[strip[q][e]]; }
                quad(c, x, y, k, OPAQUE);
            }
        }
}

/* A comic book's panel: a white gutter, a heavy black line, a yellow caption
 * box top left and halftone dots in the bottom right corner. */
static void comic(Canvas *c) {
    enum { GUTTER = 12, GUTTER_Y = 18, LINE_X = 4, LINE_Y = 6, INK = 0x101010, PAPER = 0xe8f4f8, CAPTION = 0x60e8f8 };
    int l = c->left, r = c->right, x0 = l + GUTTER, x1 = r - GUTTER, y0 = GUTTER_Y, y1 = H - GUTTER_Y;
    rect(c, l - 8, 0, r - l + 16, y0, PAPER, OPAQUE);
    rect(c, l - 8, y1, r - l + 16, H - y1, PAPER, OPAQUE);
    rect(c, l - 8, y0, GUTTER + 8, y1 - y0, PAPER, OPAQUE);
    rect(c, x1, y0, GUTTER + 8, y1 - y0, PAPER, OPAQUE);
    rect(c, x0, y0, x1 - x0, LINE_Y, INK, OPAQUE);
    rect(c, x0, y1 - LINE_Y, x1 - x0, LINE_Y, INK, OPAQUE);
    rect(c, x0, y0, LINE_X, y1 - y0, INK, OPAQUE);
    rect(c, x1 - LINE_X, y0, LINE_X, y1 - y0, INK, OPAQUE);
    halftone(c, x1 - LINE_X, y1 - LINE_Y, -1, -1, INK);
    /* the caption, top left */
    static const char text_[] = "MEANWHILE...";               /* a comic's caption: capitals */
    int tw = tmst_ui_prim_width(text_, 1), bx = x0 + LINE_X + 6, by = y0 + LINE_Y + 6;
    rect(c, bx, by, tw + 14, 24, INK, OPAQUE);
    rect(c, bx + 2, by + 3, tw + 10, 18, CAPTION, OPAQUE);
    text(c, bx + 7, by + 4, text_, 0x101010);
}

static void frame_ops(Canvas *c, int f) {
    switch (f) {
    case FR_LETTERBOX: letterbox(c); break;
    case FR_POLAROID: polaroid(c); break;
    case FR_FILM: film(c); break;
    case FR_VIGNETTE: vignette(c, 0x181818, 0x707070); break;
    case FR_TEKKEN_RED: tekken(c, 0x1414c8, 0x080808, 0x080808); break;         /* red, black lines */
    case FR_TEKKEN_BLACK: tekken(c, 0x080808, 0x1818d0, 0x1818d0); break;   /* black, red lines */
    case FR_CRT: crt(c); break;
    case FR_COMIC: comic(c); break;
    default: break;
    }
}

void tmst_photo_frame_prims(TmstPrims *k, int f) {
    Canvas c = live(k);
    if (f == FR_NONE) return;
    tmst_prim_whole_height(k, 1);      /* the frame's bars as the photo has them, the black rows too */
    frame_ops(&c, f);
    tmst_prim_whole_height(k, 0);
}

void tmst_photo_look(uint32_t *px, int w, int h, int scale, int side, int f, unsigned seed) {
    Canvas c = pixels(px, w, h, scale, side);
    filter_pixels(px, w, h, f, seed);
    filter_ops(&c, f, 0);
}

void tmst_photo_finish(uint32_t *px, int w, int h, int scale, int side, int f, int frame, unsigned seed) {
    Canvas c = pixels(px, w, h, scale, side);
    tmst_photo_look(px, w, h, scale, side, f, seed);
    frame_ops(&c, frame);
}

/* ---- depth of field ---------------------------------------------------------------- */
/* The CPU's: three box passes each way (close to a Gaussian), the box
 * `r` pixels to either side, r the live blur's reach at the level times
 * the scale. */
static int blur_reach(int level, int scale) {
    static const int reach[5] = {1, 2, 3, 4, 6};
    return reach[(level < 1 ? 1 : level > 5 ? 5 : level) - 1] * (scale > 0 ? scale : 1);
}
static void box_line(uint32_t *p, int n, int stride, int r, uint32_t *tmp) {
    for (int i = 0; i < n; i++) tmp[i] = p[(size_t)i * stride];
    int sr = 0, sg = 0, sb = 0, len = 2 * r + 1;
    for (int i = -r; i <= r; i++) {
        uint32_t v = tmp[i < 0 ? 0 : i >= n ? n - 1 : i];
        sr += chan(v, 16); sg += chan(v, 8); sb += chan(v, 0);
    }
    for (int i = 0; i < n; i++) {
        p[(size_t)i * stride] = rgb(sr / len, sg / len, sb / len);
        uint32_t out = tmp[i - r < 0 ? 0 : i - r], in = tmp[i + r + 1 >= n ? n - 1 : i + r + 1];
        sr += chan(in, 16) - chan(out, 16); sg += chan(in, 8) - chan(out, 8); sb += chan(in, 0) - chan(out, 0);
    }
}
static int blur_all(uint32_t *px, int w, int h, int r) {
    uint32_t *tmp = malloc(sizeof(uint32_t) * (size_t)(w > h ? w : h));
    if (!tmp) return 0;
    for (int pass = 0; pass < 3; pass++) {
        for (int y = 0; y < h; y++) box_line(px + (size_t)y * w, w, 1, r, tmp);
        for (int x = 0; x < w; x++) box_line(px + x, h, w, r, tmp);
    }
    free(tmp);
    return 1;
}

int tmst_photo_blur_behind(uint32_t *px, const uint32_t *stage, int w, int h, int scale, int level) {
    size_t n = (size_t)w * (size_t)h;
    uint32_t *bg = malloc(sizeof(uint32_t) * n);
    if (!bg) return 0;
    memcpy(bg, stage, sizeof(uint32_t) * n);
    if (!blur_all(bg, w, h, blur_reach(level, scale))) { free(bg); return 0; }
    for (size_t i = 0; i < n; i++) {
        int d = abs(chan(px[i], 16) - chan(stage[i], 16)) + abs(chan(px[i], 8) - chan(stage[i], 8)) +
                abs(chan(px[i], 0) - chan(stage[i], 0));
        if (d <= 24) px[i] = bg[i];                    /* the stage's: blurred */
    }
    free(bg);
    return 1;
}

int tmst_photo_blur_band(uint32_t *px, int w, int h, int scale, int level, int band_top) {
    enum { BAND = 160, EASE_ROWS = 40 };
    size_t n = (size_t)w * (size_t)h;
    uint32_t *hard = malloc(sizeof(uint32_t) * n), *soft = malloc(sizeof(uint32_t) * n);
    if (!hard || !soft) { free(hard); free(soft); return 0; }
    memcpy(hard, px, sizeof(uint32_t) * n); memcpy(soft, px, sizeof(uint32_t) * n);
    int ok = blur_all(hard, w, h, blur_reach(level, scale)) && blur_all(soft, w, h, blur_reach((level + 1) / 2, scale));
    double s = scale > 0 ? scale : 1;
    for (int y = 0; ok && y < h; y++) {
        double sy = y / s;                              /* screen rows */
        const uint32_t *from = sy < band_top - EASE_ROWS || sy >= band_top + BAND + EASE_ROWS ? hard
                             : sy < band_top || sy >= band_top + BAND ? soft : NULL;
        if (from) memcpy(px + (size_t)y * w, from + (size_t)y * w, sizeof(uint32_t) * (size_t)w);
    }
    free(hard); free(soft);
    return ok;
}
int tmst_photo_blur_available(void) { return tmst_compat_renderer() != TMST_RENDERER_SOFTWARE; }

/* Each step: the rows averaged with themselves moved r pixels right, left,
 * down and up (B/2 + F/2 copies; the OpenGL renderer samples the frame as
 * it was before each copy). */
void tmst_photo_blur_prims(TmstPrims *k, int level, int y0, int y1) {
    static const int radii[5][4] = {{1, 0, 0, 0}, {1, 2, 0, 0}, {1, 2, 3, 0}, {1, 2, 4, 0}, {1, 3, 5, 7}};
    if (level < 1 || y1 <= y0 || !tmst_photo_blur_available()) return;
    if (level > 5) level = 5;
    const int *r = radii[level - 1];
    for (int i = 0; i < 4 && r[i]; i++) {
        tmst_prim_screen_copy(k, 0, y0, W, y1 - y0, r[i], 0, 0x808080, HALF);
        tmst_prim_screen_copy(k, 0, y0, W, y1 - y0, -r[i], 0, 0x808080, HALF);
        tmst_prim_screen_copy(k, 0, y0, W, y1 - y0, 0, r[i], 0x808080, HALF);
        tmst_prim_screen_copy(k, 0, y0, W, y1 - y0, 0, -r[i], 0x808080, HALF);
    }
}
