/* tmst.tournament: the pictures of its screens, put together on the CPU.
 *
 * One canvas of 854 x 480 square pixels, the 16:9 screen (its middle 640 the
 * 4:3 one), shown over the game by the core's overlay (tmst_overlay.h). On
 * it: a background (addons/tmst/data/tournament/<name>.png, any size, made
 * to fit; the player may draw their own), the fighters' faces, lines, and
 * text in Tekken 3's title font.
 *
 * The game's things are measured in its screen units (368 across the 4:3
 * screen); on the canvas a unit is ART_UNIT_NUM / ART_UNIT_DEN pixels wide
 * and one pixel tall, as the player sees them.
 *
 * What comes from the game, read while it is in VRAM, never shipped:
 *   - the title font (the selector's: 1..9, A..Z, !; no 0), from the grid;
 *   - the stock fighters' grid icons (32 x 58), from the grid's own drawing;
 *   - the guests' icon and VS portrait (126 x 252), from their
 *     mods/ttt1/<Key>-T3-ui.jui (Panda's and Tiger's too), or the HD ones
 *     beside it where the install has them (Expanded 1.5.0 on:
 *     <Key>-T3-tile-hd.bin 64 x 116, <Key>-T3-portrait-hd.bin 168 x 252);
 *   - the stock fighters' VS portraits, from the loading screen's drawing,
 *     kept in addons/tmst/workspace/tournament/portraits/<key>.png for the
 *     next time (until then their icon stands in). */
#ifndef TMST_TOURNAMENT_ART_H
#define TMST_TOURNAMENT_ART_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum { ART_W = 854, ART_H = 480, ART_SAFE_X = 107, ART_SAFE_W = 640, ART_UNIT_NUM = 854, ART_UNIT_DEN = 490 };
typedef struct { uint32_t *px; int w, h; } ArtImage;          /* ARGB, alpha 0 = see-through */

extern uint32_t art_canvas[ART_W * ART_H];

/* ---- drawing on the canvas ---- */
/* The background `name` (data/tournament/<name>.png), or a plain dark one
 * when there is none. Loaded once. */
void art_background(const char *name);
void art_fill(int x, int y, int w, int h, uint32_t argb);              /* alpha blended */
void art_frame(int x, int y, int w, int h, int t, uint32_t argb);      /* a border t thick */
enum { ART_PLAIN = 0, ART_GREY = 1, ART_MIRROR = 2 };   /* modes, or'ed */
/* src scaled onto (x, y, w, h), bilinear; `alpha` 0..255 over it all. */
void art_blit(const ArtImage *src, int x, int y, int w, int h, int mode, int alpha);

/* ---- the title font ---- */
/* Read from VRAM (the selector's, on screen): 1 once it has been read. */
int art_font_capture(void);
int art_font_ready(void);
/* A line at (x, y): align 0 from x, 1 centred, 2 ending at x; `scale` in
 * hundredths (100: the selector's 14 x 24 units). `colour` 0: the font's own
 * gold; otherwise its light and dark in that colour (0xRRGGBB). A shadow
 * under it. Letters it lacks are spaces; 0 is drawn as O. Returns the width. */
/* In a line, the pad's buttons: a character of its own, a space after it. */
enum { ART_CROSS = 1, ART_CIRCLE, ART_TRIANGLE, ART_SQUARE };
#define ART_X ""
#define ART_O ""
int art_text(const char *s, int x, int y, int align, int scale, uint32_t colour);
int art_text_width(const char *s, int scale);

/* ---- faces ---- */
/* The icon (32 x 58 units) and the VS portrait of a roster key, or NULL. */
const ArtImage *art_icon(const char *key);
const ArtImage *art_portrait(const char *key);
/* The loading screen's kept portrait (never the stock HD one), or NULL. */
const ArtImage *art_portrait_cached(const char *key);
/* A guest's from its .jui (the name of the file's key, as tekken3_guest_key). */
void art_load_guest(const char *key);
/* A stock fighter's (ID 0..20) HD tile and portrait of the Expanded 1.5.1,
 * where its setup made them: drawn in place of the grid's and the loading
 * screen's. Nothing before 1.5.1. */
void art_load_stock_hd(const char *key, unsigned id);
/* Texels of the lower VRAM as a picture: the texture page word (E1's bits
 * 0..8), the palette word, u, v, w x h; colour 0 is see-through. */
int art_read_texture(uint32_t tpage, uint32_t clut, int u, int v, int w, int h, ArtImage *out);
/* Kept as a key's icon / portrait (the art module owns `im` afterwards). */
void art_set_icon(const char *key, ArtImage *im);
void art_set_portrait(const char *key, ArtImage *im, int save);

/* The canvas as a PNG (the guides). */
int art_write_png(const char *path);

#ifdef __cplusplus
}
#endif
#endif
