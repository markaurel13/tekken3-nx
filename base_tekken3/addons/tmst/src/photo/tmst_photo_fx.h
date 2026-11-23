/* tmst.photo-mode: the panel's filters, frames and depth of field
 * (src/photo/tmst_photo_fx.c).
 *
 * One description draws each of them two ways: live, as the GPU's own
 * primitives over the paused frame, and onto the photo, by the CPU over the
 * read-back frame at the renderer's resolution, with the same arithmetic
 * (the GPU's blend modes, its Gouraud shading, the core's UI font), so the
 * photo is what was seen.
 *
 * The GPU has no mixing between colour channels (only per-channel adds,
 * subtracts, halves and textured multiplies): COLD, WARM, VINTAGE, HIGH
 * CONTRAST and SCANLINES are its own; the others need every pixel's
 * colour from all three channels (BLACK & WHITE, SEPIA, NOIR, DUOTONE,
 * TEAL & ORANGE, POSTER, NIGHT VISION, NEGATIVE): the CPU's, on a frame read
 * back while the camera stands (shown as the overlay's picture), a close
 * look of the GPU's while it moves (none for NEGATIVE and POSTER).
 *
 * The frames are drawn on the screen's whole height (tmst_prim_whole_height:
 * live as on the photo). T3E RED and T3E BLACK (one frame, two colourings)
 * write their name in Tekken 3's own big letters, read from the fight's VRAM
 * (the core's UI font where they are not there), and carry the T3E logo
 * (data/photo/t3e_logo.png: live from the upper VRAM half, tmst_overlay_set_
 * sheet; on the photo from the PNG, its soft edge whole). */
#ifndef TMST_PHOTO_FX_H
#define TMST_PHOTO_FX_H
#include "tmst_overlay.h"
#include <stdint.h>

enum { FX_NONE, FX_BW, FX_SEPIA, FX_NOIR, FX_VINTAGE, FX_CONTRAST, FX_COLD, FX_WARM, FX_TEAL_ORANGE, FX_DUOTONE,
       FX_POSTER, FX_NIGHT, FX_SCANLINES, FX_NEGATIVE, FX_FILTERS };
enum { FR_NONE, FR_LETTERBOX, FR_POLAROID, FR_FILM, FR_VIGNETTE, FR_TEKKEN_RED, FR_TEKKEN_BLACK, FR_CRT, FR_COMIC, FR_FRAMES };

extern const char *const tmst_photo_filter_names[FX_FILTERS];
extern const char *const tmst_photo_frame_names[FR_FRAMES];

/* Nonzero: the filter needs the CPU (see above). */
int tmst_photo_filter_cpu(int filter);

/* Live, over the whole screen (the wide screen's sides too), in this order:
 * the filter (a CPU filter's close look), then the frame. */
void tmst_photo_filter_prims(TmstPrims *k, int filter);
void tmst_photo_frame_prims(TmstPrims *k, int frame);

/* A read-back frame (ARGB, w x h, `scale` pixels to a native one, `side`
 * the wide side's width in pixels): the whole filter (the exact preview),
 * or the filter and then the frame (the photo). `seed` varies NIGHT
 * VISION's grain. */
void tmst_photo_look(uint32_t *px, int w, int h, int scale, int side, int filter, unsigned seed);
void tmst_photo_finish(uint32_t *px, int w, int h, int scale, int side, int filter, int frame, unsigned seed);

/* Depth of field by the CPU, on a read-back frame (the wide sides too), as
 * strong as the live one at `level`: FIGHTERS, `stage` (the same frame
 * without the fighters and the HUD) blurred wherever `px` is the same as
 * it, `px` kept where they differ (the fighters, the HUD); the others, the
 * band of screen rows `band_top` .. + 160 kept sharp, the rest blurred, half
 * as much in the 40 rows next to it. 0: no memory. */
int tmst_photo_blur_behind(uint32_t *px, const uint32_t *stage, int w, int h, int scale, int level);
int tmst_photo_blur_band(uint32_t *px, int w, int h, int scale, int level, int band_top);

/* Depth of field, live (the photo reads it back drawn): the frame being
 * drawn blurred by `level` (1..5) between screen rows y0 and y1, by half
 * see-through copies of itself moved a few pixels each way. Within the
 * 368-pixel screen (the wide sides are not in VRAM). */
void tmst_photo_blur_prims(TmstPrims *k, int level, int y0, int y1);
/* Zero on the software renderer: its moved copies of the frame smear and
 * pile up from frame to frame (measured headless), so no live blur there
 * (the CPU's, on the exact preview and the photo, only). */
int tmst_photo_blur_available(void);

#endif
