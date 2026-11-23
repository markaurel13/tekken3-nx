/* TMST core: the add-on's UI font, Fira Sans Medium (SIL Open Font License
 * 1.1), drawn ahead by tools/ui_font.py into anti-aliased glyphs of 16
 * levels (tmst_ui_font_data.c), proportional, UTF-8: ASCII, Latin-1, Latin
 * Extended-A and a few punctuation marks; any other character draws '?'
 * (tmst_ui_glyph: a face without a letter draws its plain one).
 * It is the add-on's own lettering (in place of its 5 x 7 font before,
 * 2026-11-06); where the game's Tekken 3 font is drawn, that stays. */
#ifndef TMST_UI_FONT_H
#define TMST_UI_FONT_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* A glyph: its box from the pen at the baseline (left, top: texels, top
 * negative above the baseline), the pen's advance in 1/64 texel, its rows
 * at bits + offset, two texels a byte (the left one in the high nibble). */
typedef struct { uint16_t code; int8_t left, top; uint8_t w, h; uint16_t advance; uint32_t offset; } TmstUiGlyph;
typedef struct {
    const TmstUiGlyph *glyphs; int count;   /* by code */
    const uint8_t *bits;
    int cap, ascent, descent;               /* texels: capitals' height, above and below the baseline */
} TmstUiFace;

/* The faces (tools/ui_font.py's FACES). */
extern const TmstUiFace tmst_ui_face_list;     /* 22 texels to the em, texels 1.25 as wide as tall: the Jukebox's picture */
extern const TmstUiFace tmst_ui_face_screen;   /* 34 texels to the em, 2 to a unit of the 368 x 480 screen each way
                                                  (texels 1.75 as wide as tall), ASCII and the icons: the GPU's
                                                  (tmst_ui_prim_text) */

/* The SCREEN face's icons (tools/ui_font.py's ICONS), as tall as its
 * capitals, one colour each (the text's): in text as their UTF-8 (the
 * TMST_UI_ICON_* strings). */
#define TMST_UI_ICON_TRIANGLE   "\xEE\x80\x80"   /* U+E000: the pad's triangle (outline) */
#define TMST_UI_ICON_CIRCLE     "\xEE\x80\x81"   /* U+E001: circle (ring) */
#define TMST_UI_ICON_CROSS      "\xEE\x80\x82"   /* U+E002: cross */
#define TMST_UI_ICON_SQUARE     "\xEE\x80\x83"   /* U+E003: square (outline) */
#define TMST_UI_ICON_LEFT_RIGHT "\xEE\x80\x84"   /* U+E004: left and right arrows */
#define TMST_UI_ICON_PLAY       "\xEE\x80\x85"   /* U+E005: play */
#define TMST_UI_ICON_UP         "\xEE\x80\x86"   /* U+E006: up (filled) */
#define TMST_UI_ICON_DOWN       "\xEE\x80\x87"   /* U+E007: down (filled) */

/* U+00A0..U+017F: each one's plain ASCII letter, 0 none (tools/ui_font.py). */
extern const char tmst_ui_fold_latin[224];
/* The next character of UTF-8 text (0 at its end; a broken byte 0xFFFD). */
unsigned tmst_ui_next_code(const char **p);
/* The face's glyph for `code`: its own, else (a face without the letter) its
 * plain letter's (a for a-acute; dashes, quotes, bullet as ASCII), else '?'. */
const TmstUiGlyph *tmst_ui_glyph(const TmstUiFace *f, unsigned code);

/* A filled rectangle into the w x h ARGB picture, clipped. */
void tmst_fill_argb(uint32_t *dst, int w, int h, int x, int y, int rw, int rh, uint32_t argb);
/* Width in texels of `text`. */
int tmst_ui_width(const TmstUiFace *f, const char *text);
/* `text` into the w x h ARGB picture, its capitals' top at y (the baseline
 * at y + cap), clipped; returns its width. Edges blend into the picture's
 * opaque texels; on see-through ones (the picture's alpha below 255) into
 * `matte`, the colour that will be behind them, and only where the glyph
 * covers enough of the texel, so a faint edge stays see-through. */
int tmst_ui_text_argb(uint32_t *dst, int w, int h, int x, int y, const TmstUiFace *f, const char *text,
                      uint32_t argb, uint32_t matte);
/* `text` over an opaque w x h ARGB picture (a photo), each texel sx x sy
 * pixels from (x, y) (pixels: the capitals' top), each pixel its share of
 * the glyph (4 x 4 samples) of argb over itself; returns the width in texels. */
int tmst_ui_text_scaled(uint32_t *dst, int w, int h, double x, double y, double sx, double sy, const TmstUiFace *f,
                        const char *text, uint32_t argb);
/* `text` into out, cut (whole characters) with an ellipsis to fit max_w. */
void tmst_ui_fit(const TmstUiFace *f, const char *text, int max_w, char *out, size_t size);
/* `in` into out (size bytes), UTF-8 kept, cut at a whole character; control
 * characters and broken sequences become spaces. */
void tmst_ui_copy(const char *in, char *out, size_t size);

#ifdef __cplusplus
}
#endif
#endif
