/* TMST compat (tmst_compat.h): the features found at build time. */
#include "tmst_compat.h"
#include "gpu.h"
#include "mod_plugins.h"
#include "tmst_core.h"
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

/* Weak: a release without them still links, and nothing counts as taken. */
extern int tekken3_stage_high_vram_used(int x, int y, int w, int h) __attribute__((weak));
extern unsigned tekken3_stage_high_vram_generation(void) __attribute__((weak));
/* The upper half's other users in the Expanded: TTT Hitspark (its fixed
 * cells, the common sparks' cells around the stage and their palettes) and
 * Unknown's wolf (tekken3_ttt1_combat.c, tekken3_ttt1_wolf.c). */
extern int tekken3_hit_effect_high_vram_used(int x, int y, int w, int h) __attribute__((weak));
extern int tekken3_ttt1_wolf_high_vram_used(int x, int y, int w, int h) __attribute__((weak));

#if TMST_HAS_UNKNOWN_WOLF
/* Ours, by name: a weak reference is not sent to the wrapper (PE/COFF). */
int __wrap_tekken3_ttt1_wolf_high_vram_used(int x, int y, int w, int h);
static int wolf_used(int x, int y, int w, int h) { return __wrap_tekken3_ttt1_wolf_high_vram_used(x, y, w, h); }
#else
static int wolf_used(int x, int y, int w, int h) {
    return tekken3_ttt1_wolf_high_vram_used && tekken3_ttt1_wolf_high_vram_used(x, y, w, h);
}
#endif
int tmst_compat_high_vram_effects(int x, int y, int w, int h) {
    return (tekken3_hit_effect_high_vram_used && tekken3_hit_effect_high_vram_used(x, y, w, h)) ||
           wolf_used(x, y, w, h);
}
int tmst_compat_high_vram_taken(int x, int y, int w, int h) {
    return (tekken3_stage_high_vram_used && tekken3_stage_high_vram_used(x, y, w, h)) ||
           tmst_compat_high_vram_effects(x, y, w, h);
}
unsigned tmst_compat_high_vram_generation(void) {
    return tekken3_stage_high_vram_generation ? tekken3_stage_high_vram_generation() : 0;
}

/* The TTT1 roster's HD tiles and portraits (Expanded 1.5.0,
 * src/tekken3_ttt1_roster.c hd_slot_place / hd_portrait_put), present with
 * the runtime's sprite substitutes (psx_mod_gpu_sprite_hd_set; weak: an
 * older release has neither). Fixed places, no query for them upstream:
 * slot s (18 guests, then Panda and Tiger) a 64 x 116 tile at
 * (64 * (s % 16), 512 + 116 * (s / 16)); player p's 168 x 252 portrait at
 * (192 * p, 768).
 * Expanded 1.5.1 adds the stock fighters' (B186, GH#239: t3_place,
 * t3_tiles_put): IDs 0..20 after the guests' 20 slots, the rest of the
 * second row (12, x 256..1023), then a row of 9 at y 768 from x 384, right of
 * the portraits. Told by tekken3_arcade_stages_dir (weak, new in 1.5.1: the
 * folder of their files); the doubled PS1 tiles come without any file, so
 * they are always there. */
extern int psx_mod_gpu_sprite_hd_set(unsigned slot, unsigned page, uint32_t uv, unsigned w, unsigned h,
                                     unsigned hx, unsigned hy, unsigned hw, unsigned hh) __attribute__((weak));
extern const char *tekken3_arcade_stages_dir(void) __attribute__((weak));
enum { HD_TILE_W = 64, HD_TILE_H = 116, HD_COLS = 16, HD_SLOTS = 20, HD_PORTRAIT_W = 168, HD_PORTRAIT_H = 252,
       HD_PORTRAIT_STEP = 192, HD_PORTRAIT_Y = 768,
       T3_TILES = 21, T3_ROW_X = 384, T3_ROW_Y = 768 };
static int stock_hd(void) { return psx_mod_gpu_sprite_hd_set && tekken3_arcade_stages_dir; }
int tmst_compat_stock_hd(void) { return stock_hd(); }
/* Slots in the two rows from y 512: the guests' 20, with 1.5.1 the whole second row. */
static int grid_slots(void) { return stock_hd() ? 2 * HD_COLS : HD_SLOTS; }
static int t3_row_tiles(void) { return stock_hd() ? HD_SLOTS + T3_TILES - 2 * HD_COLS : 0; }   /* 9 */
static int overlaps(int x, int y, int w, int h, int ax, int ay, int aw, int ah) {
    return x < ax + aw && ax < x + w && y < ay + ah && ay < y + h;
}
int tmst_compat_roster_hd_area(int x, int y, int w, int h) {
    if (!psx_mod_gpu_sprite_hd_set) return 0;
    return overlaps(x, y, w, h, 0, 512, HD_COLS * HD_TILE_W, HD_TILE_H) ||
           overlaps(x, y, w, h, 0, 512 + HD_TILE_H, (grid_slots() - HD_COLS) * HD_TILE_W, HD_TILE_H) ||
           overlaps(x, y, w, h, 0, HD_PORTRAIT_Y, HD_PORTRAIT_STEP + HD_PORTRAIT_W, HD_PORTRAIT_H) ||
           overlaps(x, y, w, h, T3_ROW_X, T3_ROW_Y, t3_row_tiles() * HD_TILE_W, HD_TILE_H);
}
extern const char *tekken3_guest_key(unsigned id) __attribute__((weak));
/* Looked for by listing mods/ttt1, else (a folder that cannot be listed:
 * the Android APK's, read through its fopen) by opening each guest's tile
 * under its prefix (the key with a capital: "kunimitsu" -> Kunimitsu-T3). */
int tmst_compat_roster_hd_present(void) {
    static int present = -1;
    if (!psx_mod_gpu_sprite_hd_set) return 0;
    if (present >= 0) return present;
    present = stock_hd();               /* 1.5.1: the stock fighters' always */
    const char *base = SDL_GetBasePath();
    char dir[4200]; snprintf(dir, sizeof dir, "%smods/ttt1", base ? base : "./");
#if !defined(PSX_SDL3)
    if (base) SDL_free((void *)base);
#endif
#ifdef _WIN32
    char pattern[4300]; snprintf(pattern, sizeof pattern, "%s\\*-T3-tile-hd.bin", dir);
    WIN32_FIND_DATAA fd; HANDLE h = FindFirstFileA(pattern, &fd);
    if (h != INVALID_HANDLE_VALUE) { present = 1; FindClose(h); }
#else
    DIR *d = opendir(dir); struct dirent *e;
    while (d && !present && (e = readdir(d))) {
        size_t n = strlen(e->d_name), s = strlen("-T3-tile-hd.bin");
        present = n > s && !strcasecmp(e->d_name + n - s, "-T3-tile-hd.bin");
    }
    if (d) closedir(d);
#endif
    for (unsigned id = 23; !present && tekken3_guest_key && id < 23 + 32; id++) {
        const char *key = tekken3_guest_key(id);
        if (!key || !key[0]) continue;
        char path[4400]; snprintf(path, sizeof path, "%s/%c%s-T3-tile-hd.bin", dir, toupper((unsigned char)key[0]), key + 1);
        FILE *f = fopen(path, "rb");
        if (f) { present = 1; fclose(f); }
    }
    tmst_log("compat: roster HD faces %s", present ? "installed: the selectors' pictures leave them room" : "not installed");
    return present;
}
int tmst_compat_roster_hd_write(int x, int y, int w, int h) {
    if (!psx_mod_gpu_sprite_hd_set) return 0;
    if (w == HD_TILE_W && h == HD_TILE_H && y == T3_ROW_Y && x >= T3_ROW_X && (x - T3_ROW_X) % HD_TILE_W == 0)
        return (x - T3_ROW_X) / HD_TILE_W < t3_row_tiles();
    if (w == HD_TILE_W && h == HD_TILE_H && x >= 0 && x % HD_TILE_W == 0 && y >= 512 && (y - 512) % HD_TILE_H == 0)
        return (x / HD_TILE_W) + HD_COLS * ((y - 512) / HD_TILE_H) < grid_slots();
    return w == HD_PORTRAIT_W && h == HD_PORTRAIT_H && y == HD_PORTRAIT_Y && (x == 0 || x == HD_PORTRAIT_STEP);
}

/* psxrecomp/runtime/src/gpu.c: the half's 1024 x 512 pixels, NULL before
 * psx_mod_gpu_high_vram_enable(). */
extern const uint16_t *gpu_high_vram_pixels(void) __attribute__((weak));

int tmst_compat_high_vram_read(int x, int y, int w, int h, uint16_t *out) {
    const uint16_t *half = gpu_high_vram_pixels ? gpu_high_vram_pixels() : NULL;
    if (!half || x < 0 || y < 512 || w <= 0 || h <= 0 || x + w > 1024 || y + h > 1024) return 0;
    for (int row = 0; row < h; row++)
        memcpy(out + (size_t)row * (size_t)w, half + (size_t)(y - 512 + row) * 1024u + (size_t)x, (size_t)w * 2);
    return 1;
}

/* The runtime's VRAM, the lower half (psxrecomp gpu_render.h; weak). */
extern uint16_t gr_vram_read(int x, int y) __attribute__((weak));
int tmst_compat_vram_read(int x, int y, int w, int h, uint16_t *out) {
    if (!gr_vram_read || x < 0 || y < 0 || w <= 0 || h <= 0 || x + w > 1024 || y + h > 512) return 0;
    for (int row = 0; row < h; row++)
        for (int col = 0; col < w; col++) out[(size_t)row * (size_t)w + (size_t)col] = gr_vram_read(x + col, y + row);
    return 1;
}

/* src/tekken3_ttt1_roster.c (weak: a release without guests has no Unknown):
 * the loading screen's fighters (0x800ADD5C + 2 * player, written before
 * the fight's packs are read; in Team Battle each member's again, before
 * the player's guest identity follows), a guest's key by its ID. */
extern const char *tekken3_guest_key(unsigned id) __attribute__((weak));
enum { LOADING_FIGHTERS = 0x800add5c };
int tmst_compat_unknown_fights(void) {
    if (!tekken3_guest_key) return 0;
    for (unsigned p = 0; p < 2; p++) {
        const char *key = tekken3_guest_key(psx_mod_read_half(LOADING_FIGHTERS + 2 * p));
        if (key && !strcmp(key, "unknown")) return 1;
    }
    return 0;
}

#if TMST_HAS_UNKNOWN_WOLF
#include "cpu_state.h"
/* Unknown's wolf only behind Unknown (hotfix/wolf). The Expanded 1.5.0's
 * wolf (src/tekken3_ttt1_wolf.c, unknown()) asks for the player's guest
 * identity, which keeps the last guest the player had: tekken3_ttt1_roster.c
 * follow() leaves it as it is for a native. A native on its TTT1 model and
 * its Tekken 3 moves (a guest's grafted rows, the identity not its own) then
 * passes as Unknown: after a fight with her on that side, Gun Jack or Ogre
 * fought with her wolf behind them, and the wolf's pack took the upper half's
 * pages 640..1023 x 512..767 from the stage. Here the fighter itself
 * decides: the actor's character ID (+0x18, the guest's own ID while a
 * guest fights, as the roster's state 8 reads it) is Unknown's. */
enum { ACTOR0 = 0x800a9228, ACTOR_SIZE = 0x188c, ACTOR_ID = 0x18 };
static int unknown_actor(unsigned p) {
    const char *key = tekken3_guest_key ? tekken3_guest_key(psx_mod_read_half(ACTOR0 + p * ACTOR_SIZE + ACTOR_ID)) : NULL;
    return key && !strcmp(key, "unknown");
}

extern void __real_tekken3_ttt1_wolf_after_actor(CPUState *cpu, uint32_t actor);
void __wrap_tekken3_ttt1_wolf_after_actor(CPUState *cpu, uint32_t actor) {
    /* Not Unknown: no call, so no wolf; theirs starts it again (comes out of
     * her) once she is drawn after a gap of 8 frames or more. */
    if ((actor == ACTOR0 || actor == ACTOR0 + ACTOR_SIZE) && !unknown_actor(actor != ACTOR0)) return;
    __real_tekken3_ttt1_wolf_after_actor(cpu, actor);
}
/* Its pages for the PS2 lasers and weapons, and for us: taken only while
 * Unknown is one of the fighters, as the loading screen names them (the
 * packs are chosen there, before the actors carry the new fighters) or as
 * an actor carries her (Survival's next opponent, without a loading screen). */
extern int __real_tekken3_ttt1_wolf_high_vram_used(int x, int y, int w, int h);
int __wrap_tekken3_ttt1_wolf_high_vram_used(int x, int y, int w, int h) {
    return (tmst_compat_unknown_fights() || unknown_actor(0) || unknown_actor(1)) &&
           __real_tekken3_ttt1_wolf_high_vram_used(x, y, w, h);
}
#endif

extern int tekken3_arcade_stages_t3_stage(void) __attribute__((weak));
int tmst_compat_arcade_stage(void) {
    int s = tekken3_arcade_stages_t3_stage ? tekken3_arcade_stages_t3_stage() : -1;
    return s >= 0 && s <= 12 ? s : -1;
}

/* mod_plugins.h; weak: a release without it has no wide screen. */
extern int32_t psx_mod_widescreen_x_margin(void) __attribute__((weak));
int tmst_compat_widescreen_margin(void) {
    int m = psx_mod_widescreen_x_margin ? (int)psx_mod_widescreen_x_margin() : 0;
    return m > 0 && m < 256 ? m : 0;
}

extern int ws_nw_extra(void) __attribute__((weak));
int tmst_compat_widescreen_side(void) {
    int e = ws_nw_extra ? ws_nw_extra() / 2 : -1;
    return e >= 0 && e < 256 ? e : tmst_compat_widescreen_margin();
}

unsigned tmst_compat_available(void) {
    unsigned m = 0;
#ifdef TMST_HAS_ACTIVATION_PLUGIN
    m |= TMST_NEED_ACTIVATION;
#endif
#ifdef TMST_HAS_VBLANK_PLUGIN
    m |= TMST_NEED_VBLANK;
#endif
#ifdef TMST_HAS_GUEST_MEMORY
    m |= TMST_NEED_GUEST_MEMORY;
#endif
#ifdef TMST_HAS_HIGH_VRAM_UPLOAD
    m |= TMST_NEED_HIGH_VRAM;
#endif
#ifdef TMST_HAS_GAME_OPTIONS
    m |= TMST_NEED_GAME_OPTIONS;
#endif
    return m;
}

const char *tmst_compat_describe(unsigned mask) {
    static const char *const names[] = { "activation plugins", "VBlank plugins", "guest memory",
                                         "upper VRAM uploads", "GAME OPTION rows" };
    static char out[160];
    out[0] = 0;
    for (unsigned i = 0; i < sizeof names / sizeof names[0]; i++) {
        if (!(mask >> i & 1)) continue;
        if (out[0]) strncat(out, ", ", sizeof out - strlen(out) - 1);
        strncat(out, names[i], sizeof out - strlen(out) - 1);
    }
    return out[0] ? out : "nothing";
}

/* The runtime's display readout (gpu.h, gpu_render.h, gpu_gl_renderer.h:
 * not the mod API; weak, absent = no photos), as its debug server's
 * screenshot_hires and wide_shot read it. */
extern void gpu_get_display_info(GpuDisplayInfo *out) __attribute__((weak));
extern int gr_scale(void) __attribute__((weak));
extern int gr_read_display_hires(uint32_t *out, int pitch, int dx, int dy, int dw, int dh) __attribute__((weak));
extern int gr_render_display(uint32_t *out, int pitch, int dx, int dy, int dw, int dh) __attribute__((weak));
extern int gr_wide_supported(void) __attribute__((weak));
extern int gr_render_wide_display(uint32_t *out, int pitch, int base_x, int disp_y, int disp_h) __attribute__((weak));
extern void gl_renderer_sync_cpu(void) __attribute__((weak));

extern int gr_backend(void) __attribute__((weak));     /* GrBackend: 0 software, 1 OpenGL, 2 Vulkan */
int tmst_compat_renderer(void) {
    int b = gr_backend ? gr_backend() : -1;
    return b >= 0 && b <= 2 ? b : TMST_RENDERER_UNKNOWN;
}

#ifdef __SWITCH__
extern int switch_deko3d_read_frame(uint32_t **out, int *w, int *h, int *scale, int *side);
#endif

int tmst_compat_read_frame(uint32_t **out, int *w, int *h, int *scale, int *side) {
    *out = NULL; *w = *h = *side = 0; *scale = 1;
#ifdef __SWITCH__
    if (switch_deko3d_read_frame(out, w, h, scale, side)) return 1;
#endif
    if (!gpu_get_display_info || !gr_scale || !gr_render_display) return 0;
    GpuDisplayInfo di;
    gpu_get_display_info(&di);
    if (di.disabled || di.depth24 || !di.width || !di.height) return 0;
    int s = gr_scale(); if (s < 1) s = 1;
    int dw = (int)(di.width > 640 ? 640 : di.width), dh = (int)(di.height > 512 ? 512 : di.height);
    int extra = ws_nw_extra ? ws_nw_extra() : 0;
    if (extra > 0 && gr_wide_supported && gr_wide_supported() && gr_render_wide_display) {
        /* 16:9: the composited wide frame, sides and all */
        int ow = (dw + extra) * s, oh = dh * s;
        uint32_t *buf = malloc((size_t)ow * (size_t)oh * 4);
        if (buf) {
            if (gl_renderer_sync_cpu) gl_renderer_sync_cpu();
            if (gr_render_wide_display(buf, ow * 4, (int)di.display_x, (int)di.display_y, dh) > 0) {
                *out = buf; *w = ow; *h = oh; *scale = s; *side = extra / 2 * s;
                goto opaque;
            }
            free(buf);
        }
    }
    {
        int ow = dw * s, oh = dh * s;
        uint32_t *buf = malloc((size_t)ow * (size_t)oh * 4);
        if (!buf) return 0;
        if (!(gr_read_display_hires && gr_read_display_hires(buf, ow * 4, (int)di.display_x, (int)di.display_y, dw, dh))) {
            s = 1; ow = dw; oh = dh;                 /* no hi-res surface: the native frame */
            if (!gr_render_display(buf, ow * 4, (int)di.display_x, (int)di.display_y, dw, dh)) { free(buf); return 0; }
        }
        *out = buf; *w = ow; *h = oh; *scale = s;
    }
opaque:
    for (size_t i = 0, n = (size_t)*w * (size_t)*h; i < n; i++) (*out)[i] |= 0xFF000000u;
    return 1;
}

/* The runtime's pads (psxrecomp sio.c; weak). */
extern int sio_get_pad_connected(int slot) __attribute__((weak));
extern void sio_set_pad_connected(int slot, int connected) __attribute__((weak));
extern void sio_set_pad_state_slot(int slot, uint16_t buttons) __attribute__((weak));
/* The Expanded's costume panel (runtime tekken3_outfits.c; weak): which pads
 * are there (the runtime tells it each frame, for the pads it reads). */
extern void tekken3_outfits_connect(int p, int on) __attribute__((weak));
int tmst_compat_pad_connected(int slot) { return sio_get_pad_connected ? sio_get_pad_connected(slot) : -1; }
int tmst_compat_pad_set(int slot, int connected, uint16_t buttons) {
    if (!sio_set_pad_connected || !sio_set_pad_state_slot) return 0;
    sio_set_pad_connected(slot, connected);
    sio_set_pad_state_slot(slot, buttons);
    if (tekken3_outfits_connect) tekken3_outfits_connect(slot, connected);
    return 1;
}
