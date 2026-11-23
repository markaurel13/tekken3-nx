/* tmst.tournament: the screens' pictures (tmst_tournament_art.h). */
#include "tmst_tournament_art.h"
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_files.h"
#include "tmst_png.h"
#include "tmst_ui_font.h"
#include "psx_sdl.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <strings.h>
#endif

uint32_t art_canvas[ART_W * ART_H];

static uint32_t argb_of(uint16_t c) {                  /* a 15-bit texel, 0 see-through */
    if (!(c & 0x7fff) && !(c & 0x8000)) return 0;
    uint32_t r = (c & 31) * 255 / 31, g = (c >> 5 & 31) * 255 / 31, b = (c >> 10 & 31) * 255 / 31;
    return 0xff000000u | r << 16 | g << 8 | b;
}
static uint32_t blend(uint32_t dst, uint32_t src, int alpha) {     /* src over dst, alpha 0..255 times src's */
    int a = (int)(src >> 24) * alpha / 255;
    if (a <= 0) return dst;
    if (a >= 255) return src | 0xff000000u;
    uint32_t out = 0xff000000u;
    for (int s = 0; s < 24; s += 8) {
        int d = (int)(dst >> s & 255), c = (int)(src >> s & 255);
        out |= (uint32_t)(d + (c - d) * a / 255) << s;
    }
    return out;
}

/* ---- the canvas ---- */
void art_fill(int x, int y, int w, int h, uint32_t argb) {
    for (int row = y < 0 ? 0 : y; row < y + h && row < ART_H; row++)
        for (int col = x < 0 ? 0 : x; col < x + w && col < ART_W; col++)
            art_canvas[row * ART_W + col] = blend(art_canvas[row * ART_W + col], argb, 255);
}
void art_frame(int x, int y, int w, int h, int t, uint32_t argb) {
    art_fill(x, y, w, t, argb); art_fill(x, y + h - t, w, t, argb);
    art_fill(x, y + t, t, h - 2 * t, argb); art_fill(x + w - t, y + t, t, h - 2 * t, argb);
}
static uint32_t sample(const ArtImage *s, int fx, int fy) {        /* 16.16 texel coordinates, bilinear */
    int x0 = fx >> 16, y0 = fy >> 16, ax = fx >> 8 & 255, ay = fy >> 8 & 255;
    if (x0 < 0) { x0 = 0; ax = 0; }
    if (y0 < 0) { y0 = 0; ay = 0; }
    int x1 = x0 + 1 < s->w ? x0 + 1 : s->w - 1, y1 = y0 + 1 < s->h ? y0 + 1 : s->h - 1;
    if (x0 >= s->w) x0 = x1 = s->w - 1;
    if (y0 >= s->h) y0 = y1 = s->h - 1;
    uint32_t p[4] = {s->px[y0 * s->w + x0], s->px[y0 * s->w + x1], s->px[y1 * s->w + x0], s->px[y1 * s->w + x1]};
    int wt[4] = {(256 - ax) * (256 - ay), ax * (256 - ay), (256 - ax) * ay, ax * ay};
    int sa = 0, sc[3] = {0, 0, 0};
    for (int i = 0; i < 4; i++) {
        int a = (int)(p[i] >> 24) * wt[i] >> 8;                     /* colours weighted by their alpha */
        sa += a;
        for (int c = 0; c < 3; c++) sc[c] += (int)(p[i] >> (16 - 8 * c) & 255) * a;
    }
    if (sa <= 0) return 0;
    uint32_t out = (uint32_t)(sa >> 8 > 255 ? 255 : sa >> 8) << 24;
    for (int c = 0; c < 3; c++) out |= (uint32_t)(sc[c] / sa) << (16 - 8 * c);
    return out;
}
void art_blit(const ArtImage *src, int x, int y, int w, int h, int mode, int alpha) {
    if (!src || !src->px || w <= 0 || h <= 0) return;
    int sx = (int)(((int64_t)src->w << 16) / w), sy = (int)(((int64_t)src->h << 16) / h);
    for (int row = 0; row < h; row++) {
        int cy = y + row;
        if (cy < 0 || cy >= ART_H) continue;
        for (int col = 0; col < w; col++) {
            int cx = x + col;
            if (cx < 0 || cx >= ART_W) continue;
            int sc = mode & ART_MIRROR ? w - 1 - col : col;
            uint32_t c = sample(src, sc * sx + sx / 2 - 32768, row * sy + sy / 2 - 32768);
            if ((mode & ART_GREY) && c >> 24) {
                int l = (int)((c >> 16 & 255) * 30 + (c >> 8 & 255) * 59 + (c & 255) * 11) / 100 * 45 / 100;
                c = (c & 0xff000000u) | (uint32_t)l * 0x010101u;
            }
            art_canvas[cy * ART_W + cx] = blend(art_canvas[cy * ART_W + cx], c, alpha);
        }
    }
}

/* ---- backgrounds ---- */
static struct { char name[16]; ArtImage im; int tried; } backgrounds[4];
static void default_background(uint32_t *px) {
    for (int y = 0; y < ART_H; y++)
        for (int x = 0; x < ART_W; x++) {
            int v = 6 + y * 18 / ART_H, red = y < 64 ? (64 - y) / 3 : 0;
            px[y * ART_W + x] = 0xff000000u | (uint32_t)(v + red) << 16 | (uint32_t)v << 8 | (uint32_t)(v + 4);
        }
}
void art_background(const char *name) {
    int slot = -1;
    for (int i = 0; i < 4 && slot < 0; i++) if (backgrounds[i].tried && !strcmp(backgrounds[i].name, name)) slot = i;
    for (int i = 0; i < 4 && slot < 0; i++) if (!backgrounds[i].tried) slot = i;
    if (slot < 0) slot = 0;
    if (!backgrounds[slot].tried || strcmp(backgrounds[slot].name, name)) {
        free(backgrounds[slot].im.px); memset(&backgrounds[slot], 0, sizeof backgrounds[slot]);
        snprintf(backgrounds[slot].name, sizeof backgrounds[slot].name, "%s", name);
        backgrounds[slot].tried = 1;
        char path[4200], rel[200]; ArtImage file = {0};
        snprintf(rel, sizeof rel, "data/tournament/%s.png", name);
        tmst_files_addon(path, sizeof path, rel);              /* or the copy in mods/tmst-data */
        uint32_t *px = malloc(sizeof art_canvas);
        if (px) {
            if (tmst_png_read(path, &file.px, &file.w, &file.h)) {
                default_background(art_canvas);                   /* under a see-through PNG */
                art_blit(&file, 0, 0, ART_W, ART_H, ART_PLAIN, 255);
                memcpy(px, art_canvas, sizeof art_canvas);
                tmst_log("tournament: background %s (%d x %d)", path, file.w, file.h);
                free(file.px);
            } else {
                default_background(px);
                tmst_log("tournament: no %s, a plain background", path);
            }
            backgrounds[slot].im = (ArtImage){px, ART_W, ART_H};
        }
    }
    if (backgrounds[slot].im.px) memcpy(art_canvas, backgrounds[slot].im.px, sizeof art_canvas);
    else default_background(art_canvas);
}

/* ---- the title font: page (832, 0) 4 bpp, palette (336, 505), cells of
 * 14 x 24 from u 172, six to a row: 1..9, A..Z, ! ---- */
enum { GLYPHS = 36, GLYPH_W = 14, GLYPH_H = 24, ADVANCE = 13, FONT_X = 832, FONT_U = 172, CLUT_X = 336, CLUT_Y = 505,
       SHADOW = 0x010101 };                                 /* nearly black: 0 would mean no shadow */
static uint32_t glyph_px[GLYPHS][GLYPH_W * GLYPH_H];
static int font_read;
int art_font_ready(void) { return font_read; }
int art_font_capture(void) {
    if (font_read) return 1;
    uint16_t clut[16], page[64 * 6 * GLYPH_H];
    if (!tmst_compat_vram_read(CLUT_X, CLUT_Y, 16, 1, clut) || !tmst_compat_vram_read(FONT_X, 0, 64, 6 * GLYPH_H, page))
        return 0;
    int lit = 0;
    for (int g = 0; g < GLYPHS; g++)
        for (int v = 0; v < GLYPH_H; v++)
            for (int u = 0; u < GLYPH_W; u++) {
                int tu = FONT_U + g % 6 * GLYPH_W + u, tv = g / 6 * GLYPH_H + v;
                uint32_t c = 0;
                if (tu < 256) {
                    unsigned idx = page[tv * 64 + tu / 4] >> (tu % 4 * 4) & 15;
                    c = idx ? argb_of(clut[idx]) : 0;
                }
                glyph_px[g][v * GLYPH_W + u] = c;
                lit += c != 0;
            }
    if (lit < 200) return 0;                                  /* not the font (yet) */
    font_read = 1;
    tmst_log("tournament: the title font read from VRAM");
    return 1;
}
static int glyph_of(char c) {
    if (c == '0' || c == 'o') c = 'O';
    if (c >= '1' && c <= '9') return c - '1';
    c = (char)toupper((unsigned char)c);
    if (c >= 'A' && c <= 'Z') return 9 + c - 'A';
    return c == '!' ? 35 : -1;
}
static int unit_x(int units) { return units * ART_UNIT_NUM / ART_UNIT_DEN; }
int art_text_width(const char *s, int scale) {
    int n = (int)strlen(s);
    return n ? unit_x((n * ADVANCE + GLYPH_W - ADVANCE) * scale / 100) : 0;
}
static void glyph_blit(int g, int x, int y, int w, int h, uint32_t colour, uint32_t shadow) {
    ArtImage im = {glyph_px[g], GLYPH_W, GLYPH_H};
    if (!colour && !shadow) { art_blit(&im, x, y, w, h, ART_PLAIN, 255); return; }
    uint32_t tmp[GLYPH_W * GLYPH_H];
    for (int i = 0; i < GLYPH_W * GLYPH_H; i++) {
        uint32_t c = glyph_px[g][i];
        if (!(c >> 24)) { tmp[i] = 0; continue; }
        if (shadow) { tmp[i] = (c & 0xff000000u) | (shadow & 0xffffff); continue; }
        int l = (int)((c >> 16 & 255) * 30 + (c >> 8 & 255) * 59 + (c & 255) * 11) / 100;
        uint32_t out = c & 0xff000000u;
        for (int s = 0; s < 24; s += 8) {
            int v = (int)(colour >> s & 255) * (l + 40) / 200;
            out |= (uint32_t)(v > 255 ? 255 : v) << s;
        }
        tmp[i] = out;
    }
    im.px = tmp;
    art_blit(&im, x, y, w, h, ART_PLAIN, shadow ? 150 : 255);
}
/* A pad button in a line of text (ART_CROSS .. ART_SQUARE, in the two cells
 * it is given): the symbol alone in the PlayStation's colour (the hints'
 * colours: the Jukebox's, Photo Mode's), its strokes thick. */
static void put(int x, int y, uint32_t argb) {
    if (x >= 0 && x < ART_W && y >= 0 && y < ART_H) art_canvas[y * ART_W + x] = argb;
}
static void button(char b, int cx, int cy, int r) {
    int t = r / 6 > 1 ? r / 6 : 1;                               /* the canvas's units are not square: unit_x() */
    static const uint32_t ink[4] = {0xff7aa7ffu, 0xffff6b6bu, 0xff3ecf8eu, 0xffff8fd8u};
    uint32_t c = ink[b - ART_CROSS];
    int s = r * 75 / 100, sx = unit_x(s);
    if (b == ART_CROSS)
        for (int i = -s; i <= s; i++)
            for (int k = -t; k <= t; k++) {
                put(cx + unit_x(i) + k, cy + i, c); put(cx - unit_x(i) + k, cy + i, c);
            }
    else if (b == ART_CIRCLE) {                                   /* a ring: inside the outer ellipse, not the inner */
        int ir = s - 2 * t, irx = unit_x(ir);
        for (int y = -s; y <= s; y++)
            for (int x = -sx; x <= sx; x++) {
                int out = (int64_t)x * x * s * s + (int64_t)y * y * sx * sx <= (int64_t)s * s * sx * sx;
                int in = ir > 0 && (int64_t)x * x * ir * ir + (int64_t)y * y * irx * irx < (int64_t)ir * ir * irx * irx;
                if (out && !in) put(cx + x, cy + y, c);
            }
    }
    else if (b == ART_TRIANGLE)
        for (int y = -s; y <= s; y++) {
            int half = unit_x((y + s) / 2);
            for (int k = -t; k <= t; k++) { put(cx - half + k, cy + y, c); put(cx + half + k, cy + y, c); }
            if (y >= s - t) for (int x = -half; x <= half; x++) put(cx + x, cy + y, c);
        }
    else
        for (int y = -s; y <= s; y++)
            for (int x = -sx; x <= sx; x++)
                if (y < -s + 2 * t || y > s - 2 * t || x < -sx + unit_x(2 * t) || x > sx - unit_x(2 * t)) put(cx + x, cy + y, c);
}
int art_text(const char *s, int x, int y, int align, int scale, uint32_t colour) {
    int h = GLYPH_H * scale / 100;
    if (!font_read) {
        /* the UI font (Fira Sans, its SCREEN face) till the title font is
         * read: capitals 7/8 of the title font's line (as the old font's),
         * its texels 1.75 as wide as tall on the canvas's square pixels */
        const TmstUiFace *f = &tmst_ui_face_screen;
        double sy = h * 7.0 / 8 / f->cap, sx = sy * 1.75;
        int width = (int)(tmst_ui_width(f, s) * sx + 0.5);
        x -= align == 1 ? width / 2 : align == 2 ? width : 0;
        tmst_ui_text_scaled(art_canvas, ART_W, ART_H, x, y, sx, sy, f, s, 0xff000000u | (colour ? colour : 0xe8b830));
        return width;
    }
    int width = art_text_width(s, scale);
    x -= align == 1 ? width / 2 : align == 2 ? width : 0;
    int w = unit_x(GLYPH_W * scale / 100), off = scale >= 80 ? 2 : 1;
    for (int pass = 0; pass < 2; pass++)                     /* the shadow, then the letters */
        for (int i = 0; s[i]; i++) {
            int cx = x + unit_x(i * ADVANCE * scale / 100), g = glyph_of(s[i]);
            if (pass && s[i] >= ART_CROSS && s[i] <= ART_SQUARE)    /* over this cell and the next */
                button(s[i], cx + unit_x(ADVANCE * scale / 100), y + h / 2, h * 6 / 10);
            if (s[i] == '.') {               /* the title font has no full stop: a square on the line (DOCTOR B., issue #14) */
                int d = h / 5 > 2 ? h / 5 : 2, dx = cx, dy = y + h * 7 / 8 - d;
                if (pass == 0) art_fill(dx + off, dy + off, unit_x(d), d, 0xff000000u | SHADOW);
                else art_fill(dx, dy, unit_x(d), d, 0xff000000u | (colour ? colour : 0xe8b830));
                continue;
            }
            if (g < 0) continue;
            if (pass == 0) glyph_blit(g, cx + off, y + off, w, h, 0, SHADOW);
            else glyph_blit(g, cx, y, w, h, colour, 0);
        }
    return width;
}

/* ---- faces ---- */
enum { FACES = 72, KEY = 24 };
/* icon / portrait: the grid's, the loading screen's (or a guest's .jui and
 * HD files); hd_icon / hd_portrait: a stock fighter's HD faces of the
 * Expanded 1.5.1 (art_load_stock_hd), drawn in their place. */
static struct { char key[KEY]; ArtImage icon, portrait, hd_icon, hd_portrait; int cache_tried, guest_tried, stock_tried; } faces[FACES];
static int face_count;
static int face_slot(const char *key, int make) {
    for (int i = 0; i < face_count; i++) if (!strcmp(faces[i].key, key)) return i;
    if (!make || face_count >= FACES || strlen(key) >= KEY) return -1;
    memset(&faces[face_count], 0, sizeof faces[0]);
    snprintf(faces[face_count].key, KEY, "%s", key);
    return face_count++;
}
static void cache_path(const char *key, char *out, size_t size) {
    snprintf(out, size, "%s/addons/tmst/workspace/tournament/portraits/%s.png", tmst_files_root(), key);
}
enum { KEY_COLOUR = 0xff00ff };                            /* see-through in the cache's RGB files */
const ArtImage *art_icon(const char *key) {
    int i = key ? face_slot(key, 0) : -1;
    if (i >= 0 && faces[i].hd_icon.px) return &faces[i].hd_icon;
    if (i >= 0 && faces[i].icon.px) return &faces[i].icon;
    /* True Ogre has no cell on the grid: Ogre's icon stands in (his own VS
     * portrait, once a loading screen has shown it, comes first anyway). */
    return key && !strcmp(key, "trueogre") ? art_icon("ogre") : NULL;
}
const ArtImage *art_portrait(const char *key) {
    int i = key ? face_slot(key, 0) : -1;
    if (i >= 0 && faces[i].hd_portrait.px) return &faces[i].hd_portrait;
    return art_portrait_cached(key);
}
/* The loading screen's portrait as kept (the cache), never the HD one: what
 * the loading screen's own read is checked against. */
const ArtImage *art_portrait_cached(const char *key) {
    int i = key ? face_slot(key, 1) : -1;
    if (i < 0) return NULL;
    if (!faces[i].portrait.px && !faces[i].cache_tried) {
        faces[i].cache_tried = 1;
        char path[4200]; ArtImage im = {0};
        cache_path(key, path, sizeof path);
        if (tmst_png_read(path, &im.px, &im.w, &im.h)) {
            for (int p = 0; p < im.w * im.h; p++) if ((im.px[p] & 0xffffff) == KEY_COLOUR) im.px[p] = 0;
            faces[i].portrait = im;
        }
    }
    return faces[i].portrait.px ? &faces[i].portrait : NULL;
}
void art_set_icon(const char *key, ArtImage *im) {
    int i = face_slot(key, 1);
    if (i < 0) { free(im->px); return; }
    free(faces[i].icon.px); faces[i].icon = *im;
}
void art_set_portrait(const char *key, ArtImage *im, int save) {
    int i = face_slot(key, 1);
    if (i < 0) { free(im->px); return; }
    free(faces[i].portrait.px); faces[i].portrait = *im; faces[i].cache_tried = 1;
    if (!save) return;
    char path[4200];
    cache_path(key, path, sizeof path);
    char *slash = strrchr(path, '/');
    if (slash) { *slash = 0; tmst_files_make_dirs(path); *slash = '/'; }
    uint32_t *rgb = malloc(sizeof(uint32_t) * (size_t)im->w * (size_t)im->h);
    if (!rgb) return;
    for (int p = 0; p < im->w * im->h; p++) rgb[p] = im->px[p] >> 24 ? im->px[p] : 0xff000000u | KEY_COLOUR;
    if (tmst_png_write(path, rgb, im->w, im->h, im->w)) tmst_log("tournament: %s's portrait kept in %s", key, path);
    free(rgb);
}

int art_read_texture(uint32_t tpage, uint32_t clut, int u, int v, int w, int h, ArtImage *out) {
    int depth = (int)(tpage >> 7 & 3), bx = (int)(tpage & 15) * 64, by = (int)(tpage >> 4 & 1) * 256;
    int cx = (int)(clut & 63) * 16, cy = (int)(clut >> 6 & 511);
    if (depth == 3 || w <= 0 || h <= 0 || w > 512 || h > 512) return 0;
    int per = depth == 0 ? 4 : depth == 1 ? 2 : 1, hw = (u + w + per - 1) / per - u / per;
    uint16_t pal[256], *rows = malloc(sizeof(uint16_t) * (size_t)hw * (size_t)h);
    uint32_t *px = malloc(sizeof(uint32_t) * (size_t)w * (size_t)h);
    int ok = rows && px && tmst_compat_vram_read(bx + u / per, by + v, hw, h, rows) &&
             (depth == 2 || tmst_compat_vram_read(cx, cy, depth == 0 ? 16 : 256, 1, pal));
    if (ok)
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++) {
                int tu = u + x;
                uint16_t word = rows[y * hw + tu / per - u / per], c;
                if (depth == 0) c = pal[word >> (tu % 4 * 4) & 15];
                else if (depth == 1) c = pal[word >> (tu % 2 * 8) & 255];
                else c = word;
                px[y * w + x] = argb_of(c);
            }
    free(rows);
    if (!ok) { free(px); return 0; }
    *out = (ArtImage){px, w, h};
    return 1;
}

/* A guest's mods/ttt1/<Key>-T3-ui.jui: "JUI1", the count at +8, (offset,
 * length) pairs from +16; TIM 0 the VS portrait, TIM 2 the grid icon, each an
 * 8-bit TIM with one palette. The portrait is banded as the game's own: four
 * bands of 64 rows, each on its quarter of the palette (indices 0..63). */
/* mods/ttt1/<Key>-T3-<suffix>, whatever the key's case. */
static int guest_path(const char *key, const char *suffix, char *path, size_t size) {
    const char *base = SDL_GetBasePath();
    char dir[4200]; snprintf(dir, sizeof dir, "%smods/ttt1", base ? base : "./");
#if !defined(PSX_SDL3)
    if (base) SDL_free((void *)base);
#endif
    path[0] = 0;
#ifdef _WIN32
    char pattern[4300]; snprintf(pattern, sizeof pattern, "%s\\%s-T3-%s", dir, key, suffix);
    WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) { snprintf(path, size, "%s\\%s", dir, fd.cFileName); FindClose(h); }
#else
    DIR *d = opendir(dir); struct dirent *e;
    char want[96]; snprintf(want, sizeof want, "%s-T3-%s", key, suffix);
    while (d && (e = readdir(d))) if (!strcasecmp(e->d_name, want)) { snprintf(path, size, "%s/%s", dir, e->d_name); break; }
    if (d) closedir(d);
#endif
    return path[0] != 0;
}
static int jui_path(const char *key, char *path, size_t size) { return guest_path(key, "ui.jui", path, size); }
/* The Expanded's HD faces (1.5.0, src/tekken3_ttt1_roster.c hd_file_load):
 * <Key>-T3-tile-hd.bin, the grid tile's framing at 64 x 116, and
 * <Key>-T3-portrait-hd.bin, the arcade portrait at 168 x 252 (the 126 x 252
 * one of the .jui is it squeezed, in 64 colours a band); raw 15-bit texels,
 * 0 see-through. 0 when missing or not of that size (the .jui's stays). */
enum { HD_TILE_W = 64, HD_TILE_H = 116, HD_PORTRAIT_W = 168, HD_PORTRAIT_H = 252 };
static int raw_image(const char *path, int w, int h, ArtImage *out);
static int hd_image(const char *key, const char *suffix, int w, int h, ArtImage *out) {
    char path[4400];
    return guest_path(key, suffix, path, sizeof path) && raw_image(path, w, h, out);
}
/* The Expanded 1.5.1's stock faces (B186, src/tekken3_ttt1_roster.c
 * t3_faces_load): face-NN-tile-hd.bin (64 x 116) and face-NN-portrait-hd.bin
 * (168 x 252), NN the fighter's ID, beside the Tekken 3 arcade stage packs
 * (tekken3_arcade_stages_dir, weak: absent before 1.5.1, then none used).
 * Made by its setup from tekken3.zip; none for Gon, Anna, Dr. B (the
 * grid's and the loading screen's pictures stay theirs). */
extern const char *tekken3_arcade_stages_dir(void) __attribute__((weak));
void art_load_stock_hd(const char *key, unsigned id) {
    int i = face_slot(key, 1);
    const char *dir = tekken3_arcade_stages_dir ? tekken3_arcade_stages_dir() : NULL;
    if (i < 0 || faces[i].stock_tried || !dir) return;
    faces[i].stock_tried = 1;
    char path[4400];
    snprintf(path, sizeof path, "%s/face-%02u-tile-hd.bin", dir, id);
    int tile = raw_image(path, HD_TILE_W, HD_TILE_H, &faces[i].hd_icon);
    snprintf(path, sizeof path, "%s/face-%02u-portrait-hd.bin", dir, id);
    int portrait = raw_image(path, HD_PORTRAIT_W, HD_PORTRAIT_H, &faces[i].hd_portrait);
    if (tile || portrait)
        tmst_log("tournament: %s's stock HD %s%s%s", key, tile ? "tile" : "", tile && portrait ? " and " : "",
                 portrait ? "portrait" : "");
}
/* A raw file of w x h 15-bit texels (0 see-through); 0 when missing (quietly)
 * or not of that size. */
static int raw_image(const char *path, int w, int h, ArtImage *out) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    size_t n = (size_t)w * (size_t)h;
    uint8_t *d = malloc(n * 2);
    uint32_t *px = malloc(n * sizeof(uint32_t));
    int ok = d && px && fread(d, 2, n, f) == n && fgetc(f) == EOF;
    fclose(f);
    if (ok) for (size_t i = 0; i < n; i++) px[i] = argb_of((uint16_t)(d[2 * i] | d[2 * i + 1] << 8));
    free(d);
    if (!ok) { free(px); tmst_log("tournament: %s is not %d x %d texels, not used", path, w, h); return 0; }
    *out = (ArtImage){px, w, h};
    return 1;
}
enum { VS_PORTRAIT_W = 126, VS_PORTRAIT_H = 252, VS_BAND_ROWS = 64, VS_BAND_COLOURS = 64 };
static uint32_t le32(const uint8_t *p) { return p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static int tim8(const uint8_t *d, size_t n, size_t at, ArtImage *out) {
    if (at + 20 > n || le32(d + at) != 0x10 || le32(d + at + 4) != 9) return 0;
    size_t c = at + 8, clen = le32(d + c);
    if (c + clen + 12 > n || clen < 12 + 512) return 0;
    uint16_t pal[256];
    for (int i = 0; i < 256; i++) pal[i] = (uint16_t)(d[c + 12 + 2 * i] | d[c + 13 + 2 * i] << 8);
    size_t p = c + clen, plen = le32(d + p);
    int w = (d[p + 8] | d[p + 9] << 8) * 2, h = d[p + 10] | d[p + 11] << 8;
    if (p + plen > n || w <= 0 || h <= 0 || (size_t)w * (size_t)h + 12 > plen) return 0;
    uint32_t *px = malloc(sizeof(uint32_t) * (size_t)w * (size_t)h);
    if (!px) return 0;
    int banded = w == VS_PORTRAIT_W && h == VS_PORTRAIT_H;
    for (int i = 0; i < w * h; i++) {
        unsigned idx = d[p + 12 + (size_t)i];
        if (banded && idx < VS_BAND_COLOURS) idx += (unsigned)(i / w / VS_BAND_ROWS) * VS_BAND_COLOURS;
        px[i] = argb_of(pal[idx & 255]);
    }
    *out = (ArtImage){px, w, h};
    return 1;
}
void art_load_guest(const char *key) {
    int i = face_slot(key, 1);
    if (i < 0 || faces[i].guest_tried) return;
    faces[i].guest_tried = 1;
    char path[4400];
    if (!jui_path(key, path, sizeof path)) return;
    FILE *f = fopen(path, "rb");
    if (!f) return;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *d = n > 16 ? malloc((size_t)n) : NULL;
    int ok = d && fread(d, 1, (size_t)n, f) == (size_t)n && !memcmp(d, "JUI1", 4);
    fclose(f);
    unsigned count = ok ? le32(d + 8) : 0;
    ArtImage im;
    if (count >= 1 && 16 + 8 <= (size_t)n && tim8(d, (size_t)n, le32(d + 16), &im)) {
        free(faces[i].portrait.px); faces[i].portrait = im; faces[i].cache_tried = 1;
    }
    if (count >= 3 && 16 + 24 <= (size_t)n && tim8(d, (size_t)n, le32(d + 32), &im)) {
        free(faces[i].icon.px); faces[i].icon = im;
    }
    free(d);
    /* The HD faces over the .jui's, where the install has them. */
    int hd_tile = hd_image(key, "tile-hd.bin", HD_TILE_W, HD_TILE_H, &im);
    if (hd_tile) { free(faces[i].icon.px); faces[i].icon = im; }
    int hd_portrait = hd_image(key, "portrait-hd.bin", HD_PORTRAIT_W, HD_PORTRAIT_H, &im);
    if (hd_portrait) { free(faces[i].portrait.px); faces[i].portrait = im; faces[i].cache_tried = 1; }
    if (hd_tile || hd_portrait)
        tmst_log("tournament: %s's HD %s%s%s", key, hd_tile ? "tile" : "", hd_tile && hd_portrait ? " and " : "",
                 hd_portrait ? "portrait" : "");
}

int art_write_png(const char *path) { return tmst_png_write(path, art_canvas, ART_W, ART_H, ART_W); }
