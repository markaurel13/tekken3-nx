/* TMST compat: the one place that knows how the Expanded is built inside.
 * Everything that is not a stable surface (retail guest addresses, the
 * manifest format) goes through here, so that a new Expanded release is
 * fixed in this folder only (docs/COMPAT.md). */
#ifndef TMST_COMPAT_H
#define TMST_COMPAT_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* What a mod may need from the Expanded (tmst_guard's mask). */
enum {
    TMST_NEED_ACTIVATION = 1u << 0,      /* psx_mod_register_activation_plugin */
    TMST_NEED_VBLANK = 1u << 1,          /* psx_mod_register_vblank_plugin */
    TMST_NEED_GUEST_MEMORY = 1u << 2,    /* psx_mod_alloc_guest_memory */
    TMST_NEED_HIGH_VRAM = 1u << 3,       /* psx_mod_gpu_high_vram_upload */
    TMST_NEED_GAME_OPTIONS = 1u << 4,    /* tekken3_game_options_add */
};

/* The Expanded's users of the upper VRAM half (found at link time, absent =
 * nothing taken): the stage engine (tekken3_arcade_stages.c), TTT Hitspark
 * and Unknown's wolf. Nonzero when the rectangle holds pixels of theirs now;
 * the stage engine's generation changes whenever it uploads. (The PS2 lasers
 * of Devil and Angel have no such query: API_GAPS.) */
int tmst_compat_high_vram_taken(int x, int y, int w, int h);
/* Only TTT Hitspark's and the wolf's cells: written again whenever their
 * effect plays (each frame, "unless already there"). */
int tmst_compat_high_vram_effects(int x, int y, int w, int h);
unsigned tmst_compat_high_vram_generation(void);
/* The TTT1 roster's HD tiles and portraits (Expanded 1.5.0 on; no query for
 * them upstream: their fixed places, 0 on an older release). _area: the
 * rectangle overlaps one of their places (our small sheets keep off them);
 * _write: an upload of exactly one of them (the overlay refuses it while its
 * picture holds the place: tmst_overlay.c, __wrap_psx_mod_gpu_high_vram_upload). */
int tmst_compat_roster_hd_area(int x, int y, int w, int h);
int tmst_compat_roster_hd_write(int x, int y, int w, int h);
/* Nonzero when the roster draws HD faces at all: the runtime has the
 * substitutes and mods/ttt1 holds at least one <Key>-T3-tile-hd.bin (looked
 * for once), or the Expanded 1.5.1 on (the stock fighters' always). */
int tmst_compat_roster_hd_present(void);
/* Nonzero on the Expanded 1.5.1 on, whose roster draws the stock fighters'
 * HD tiles too (the whole second row and a row at y 768 from x 384). */
int tmst_compat_stock_hd(void);
/* The upper half's pixels of a rectangle (y 512..1023) into `out` (w x h);
 * 0 when this runtime cannot read them back (gpu_high_vram_pixels). */
int tmst_compat_high_vram_read(int x, int y, int w, int h, uint16_t *out);
/* The same for the lower half (the game's own: fonts, the selector's icons),
 * through the runtime's gr_vram_read; 0 without it. */
int tmst_compat_vram_read(int x, int y, int w, int h, uint16_t *out);

/* The screen's extra width on each side in fights (the Expanded's true
 * widescreen, psx_mod_widescreen_x_margin), 0 without it: what a fill must
 * cover (it includes the culling's guard, past the visible edge). */
int tmst_compat_widescreen_margin(void);
/* The visible side's width now (the wide compositor's, ws_nw_extra / 2):
 * where a frame's edge goes. The margin above without the compositor. */
int tmst_compat_widescreen_side(void);

/* Unknown (the TTT1 guest whose wolf draws in the upper VRAM half) is one
 * of the fight's two fighters, as the loading screen names them: the
 * fight's packs are read after that (tekken3_ttt1_roster.c, weak; 0
 * without guests). */
int tmst_compat_unknown_fights(void);

/* The T3 arcade stage the Expanded's stage engine draws in place of the
 * PS1's (0..12, its arena), or -1: none (the PS1's own, a TTT stage, or a
 * release without the engine; tekken3_arcade_stages_t3_stage, weak). */
int tmst_compat_arcade_stage(void);

/* The frame on the screen, as the player sees it, at the renderer's
 * resolution (16:9: the wide frame, its sides too): *out (malloc'd ARGB,
 * the caller frees) w x h, `scale` output pixels to a native one each way,
 * `side` the width of each wide side in output pixels (0: none). Waits for
 * the GPU (never every frame). Returns 0 when it cannot be read (24-bit
 * display, a runtime without the readout). */
int tmst_compat_read_frame(uint32_t **out, int *w, int *h, int *scale, int *side);

/* The runtime's pads (psxrecomp sio.c, weak). tmst_compat_pad_connected:
 * 1 / 0, -1 when the runtime has no such call. tmst_compat_pad_set: the
 * slot (0 or 1) taken as connected (or not) and its buttons (active low)
 * until the runtime samples it again: a slot without a real pad keeps
 * them, as the debug port's second-port injection does; the Expanded's
 * costume panel told so too. 0 when missing. */
int tmst_compat_pad_connected(int slot);
int tmst_compat_pad_set(int slot, int connected, uint16_t buttons);

/* The renderer drawing (gr_backend, weak): TMST_RENDERER_*. */
enum { TMST_RENDERER_UNKNOWN = -1, TMST_RENDERER_SOFTWARE, TMST_RENDERER_OPENGL, TMST_RENDERER_VULKAN };
int tmst_compat_renderer(void);

/* The TMST_NEED_* this build found (src/compat/tmst_features.cmake). */
unsigned tmst_compat_available(void);
/* The names of the needs in `mask`, for the log. */
const char *tmst_compat_describe(unsigned mask);

#ifdef __cplusplus
}
#endif
#endif
