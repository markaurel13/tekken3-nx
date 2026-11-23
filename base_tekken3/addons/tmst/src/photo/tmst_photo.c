/* tmst.photo-mode: a free camera in a paused fight, and photos of it.
 *
 * Way in: PHOTO in the fight's pause menu, above RESET (src/core/tmst_pause.c:
 * Arcade, VS, Team Battle, Time Attack, Survival, Tekken Ball); in Practice,
 * whose panels come from its overlay and have no room for a row, SELECT
 * while paused (a hint says so under the panel). Tekken Force (its corridor
 * stages make no picture), the menus, the selectors, the loading screens
 * and the Theater have none. In Tekken Ball the ball is drawn by its
 * overlay, which the free camera's frame calls too (src/core/tmst_camera.c);
 * TRIANGLE hides its HUD as well (the power bars, the floating numbers).
 *
 * While it is on the fight stays paused as the game paused it: the pause
 * menu is not run (tmst_pause_hold), every button is ours (src/core/
 * tmst_input.c: the game sees none), and the fight's state is never written
 * (the paused frame draws more, which moves only drawing buffers: measured),
 * so CIRCLE gives back the pause menu (Practice's panel), on our row, as it
 * was. The buttons
 * are the pad's 14 (a keyboard plays them as the player bound them); the
 * buttons held on the way out are kept from the game until released.
 *
 * Drawn with native primitives only (src/core/tmst_overlay.c): in a fight the
 * stage engine fills the upper VRAM half, so there is no room for a picture.
 * The hint bar at the bottom is the UI font (Fira Sans) and the pad's symbols as
 * lines.
 *
 * The camera (src/core/tmst_camera.c): the paused frame draws the whole
 * scene from ours (Practice's panel goes with the rest of the paused
 * frame), which starts where the game's was on the way in. D-pad: forward /
 * back on the floor plane where the camera looks, left / right sideways; L1 / R1 down / up; SQUARE held +
 * D-pad: turn (left / right) and look down / up; L2 / R2: zoom (the
 * projection's distance, 50 % .. 200 %); SELECT: back to the way in. Held, a
 * direction speeds up over a second, and every motion eases in and out. The
 * eye stays inside the stage (see camera_frame: it slides along the edge as
 * along a wall), above the floor and below a ceiling, the pitch within 80
 * degrees. Out (CIRCLE) the
 * game's camera is drawn again at once, as it was. Where the game draws
 * nothing (it never clears the screen) a black backdrop shows.
 *
 * START: the panel view (the camera stands), top left: ZOOM (the same as
 * L2 / R2, steps of 10 %), FOCUS and BLUR (the depth of field: FIGHTERS,
 * the stage blurred under them, drawn after its ranks in the ordering
 * table; CENTER, TOP, BOTTOM, a sharp band and the rest blurred), FILTER,
 * FRAME (src/photo/tmst_photo_fx.c: drawn live over the frame, under our
 * own drawing), RESET (everything, the camera too). UP / DOWN a row, LEFT /
 * RIGHT its value. Every way in starts from the defaults (ZOOM 100 %, BLUR
 * OFF, FILTER NONE, FRAME NONE); nothing is kept.
 *
 * A filter that needs the CPU (tmst_photo_fx.h) is shown by a close look
 * while anything moves; once the picture has stood for STILL_FRAMES, a
 * frame is drawn without our look (CLEAN_FRAMES), read back
 * (tmst_compat_read_frame), filtered and shown as the overlay's picture
 * under our drawing (the stage engine's half of VRAM borrowed, given back
 * when it goes), until anything in it changes.
 *
 * CROSS: the photo. A frame drawn without our look (the blur kept, the HUD
 * as it is), read back at the renderer's resolution (16:9: with the sides),
 * the filter and the frame drawn on it by the CPU as they were seen, saved
 * as addons/tmst/screenshots/photo_YYYYMMDD_HHMMSS.png (src/core/
 * tmst_png.c); a flash, then PHOTO SAVED for a second and a half.
 *
 * TRIANGLE hides the game's HUD (its ranks in the ordering table hidden; its
 * routine still runs: TTT Hitspark swaps the hit effects in there), our
 * panel and hint bar (the filter and frame stay).
 * Benches: TMST_PHOTO_RANKLOG=1 (each entry, the ranks of the ordering table
 * that hold primitives, logged once), TMST_PHOTO_HIDE=<lo>-<hi> (those ranks
 * hidden while on), TMST_PHOTO_CAMLOG=1 (the camera, twice a second),
 * TMST_PHOTO_DIR=<folder> (the photos there), TMST_PHOTO_VRAMCHECK=1 (the
 * upper VRAM half on the way out against the way in, logged). */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_overlay.h"
#include "tmst_input.h"
#include "tmst_pause.h"
#include "tmst_camera.h"
#include "tmst_photo_fx.h"
#include "tmst_files.h"
#include "tmst_png.h"
#include "mod_plugins.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { B_SELECT = TMST_PAD_SELECT, B_START = TMST_PAD_START, B_UP = TMST_PAD_UP, B_RIGHT = TMST_PAD_RIGHT,
       B_DOWN = TMST_PAD_DOWN, B_LEFT = TMST_PAD_LEFT, B_L2 = TMST_PAD_L2, B_R2 = TMST_PAD_R2,
       B_L1 = TMST_PAD_L1, B_R1 = TMST_PAD_R1, B_TRIANGLE = TMST_PAD_TRIANGLE, B_CIRCLE = TMST_PAD_CIRCLE,
       B_CROSS = TMST_PAD_CROSS, B_SQUARE = TMST_PAD_SQUARE };
enum { VIEW_CAMERA, VIEW_PANEL };
#define BACKDROP 0x000000u             /* where the game draws nothing (the free camera): black */

static int on;                         /* the feature, at activation */

static struct {
    int pending;                       /* PHOTO chosen: in at the next VBlank */
    int active, kind, player, view, hud_hidden;
    int armed;                         /* every button released once since the way in */
    uint16_t held, before;             /* the controlling pad, active high: now, last frame */
    unsigned frames;
    int have_camera;                   /* the game's camera seen since the way in */
    TmstCamera start;                  /* the game's, on the way in */
    double wall, ceiling;              /* the stage's edge: the eye's farthest from the fighters (floor plane), highest */
    double x, y, z, yaw, pitch, zoom;  /* ours: the eye, angles (TMST_TURN a turn), zoom (1 = the game's H) */
    double v_fwd, v_side, v_up, v_yaw, v_pitch, v_zoom;   /* their speeds now, per frame */
    unsigned hold_move, hold_turn, hold_zoom;             /* frames each kind of input has been held */
    int row, focus, blur, filter, frame;                  /* the panel (all 0 on the way in: the defaults) */
    unsigned hold_change;                                 /* frames LEFT / RIGHT held in panel view */
    TmstCamera cam;                                       /* ours, as drawn this frame */
    unsigned still;                                       /* frames the picture has not changed */
    int capture, capture_frames;                          /* CAPTURE_*: frames left drawn clean before the read-back */
    int capture_stage;                                    /* the frame drawn now is the stage alone (FIGHTERS blur) */
    int preview, preview_failed;                          /* the exact CPU filter shown under our drawing (-1: again after a photo) */
    int flash, saved, saved_ok;                           /* frames left of the flash and of PHOTO SAVED */
} ph;
enum { CAPTURE_NONE, CAPTURE_PREVIEW, CAPTURE_PHOTO };
enum { CLEAN_FRAMES = 2,                                  /* drawn without our look before a read-back */
       STILL_FRAMES = 18,                                 /* the picture unchanged this long: the exact preview */
       FLASH_FRAMES = 12, SAVED_FRAMES = 90, PREVIEW_MAX_W = 800 };
enum { ROW_ZOOM, ROW_FOCUS, ROW_BLUR, ROW_FILTER, ROW_FRAME, ROW_RESET, ROWS };
enum { FOCUS_FIGHTERS, FOCUS_CENTRE, FOCUS_TOP, FOCUS_BOTTOM, FOCUSES };
enum { BLUR_MAX = 5 };
static const char *const focus_names[FOCUSES] = {"Fighters", "Center", "Top", "Bottom"};
static int preview_wanted(void);
static int release_wait = -1;          /* a player whose buttons are kept until released */
static int practice_hint;              /* "SELECT PHOTO MODE" under Practice's panel */
static int select_down;                /* Practice: SELECT held now (pause player's pad) */

/* ---- benches ------------------------------------------------------------------- */
static int hide_lo = -2, hide_hi = -2, rank_log, census_frames, cam_log, vram_check;
static uint16_t *vram_before;                /* TMST_PHOTO_VRAMCHECK: the upper half on the way in */
static unsigned census_nodes[1025];        /* [1024]: lists that are not the ordering table */
static uint32_t census_first[1025];
static unsigned census_ops[1025][8];        /* polygons, lines, rects, copies, fills, uploads, E*, other */
static int census_box[1025][4];             /* x0 y0 x1 y1 of the vertices seen */
static void benches(void) {
    const char *h = getenv("TMST_PHOTO_HIDE");
    if (h && sscanf(h, "%d-%d", &hide_lo, &hide_hi) == 2) tmst_log("photo: bench hides ranks %d..%d", hide_lo, hide_hi);
    const char *r = getenv("TMST_PHOTO_RANKLOG");
    rank_log = r && *r && *r != '0';
    const char *c = getenv("TMST_PHOTO_CAMLOG");
    cam_log = c && *c && *c != '0';
    const char *v = getenv("TMST_PHOTO_VRAMCHECK");
    vram_check = v && *v && *v != '0';
}
/* The upper VRAM half on the way in, and on the way out what differs. */
static void vram_snapshot(int out) {
    if (!vram_check) return;
    if (!vram_before) vram_before = malloc(1024 * 512 * 2);
    static uint16_t now[1024 * 512];
    if (!vram_before || !tmst_compat_high_vram_read(0, 512, 1024, 512, now)) return;
    if (!out) { memcpy(vram_before, now, sizeof now); return; }
    int n = 0, x0 = 9999, y0 = 9999, x1 = -1, y1 = -1;
    for (int y = 0; y < 512; y++)
        for (int x = 0; x < 1024; x++)
            if (now[y * 1024 + x] != vram_before[y * 1024 + x]) {
                n++;
                if (x < x0) x0 = x; if (x > x1) x1 = x; if (y < y0) y0 = y; if (y > y1) y1 = y;
            }
    tmst_log("photo: upper VRAM on the way out: %d pixels differ (x %d..%d, y %d..%d)", n, x0, x1, y0 + 512, y1 + 512);
}
static void census_report(void) {
    for (int r = 1024; r >= 0; r--)
        if (census_nodes[r])
            tmst_log("photo: rank %d: %u nodes, first %08x: %08x %08x %08x; poly %u line %u rect %u copy %u fill %u upload %u env %u other %u",
                     r, census_nodes[r], census_first[r], psx_mod_read_word(census_first[r] + 4),
                     psx_mod_read_word(census_first[r] + 8), psx_mod_read_word(census_first[r] + 12),
                     census_ops[r][0], census_ops[r][1], census_ops[r][2], census_ops[r][3], census_ops[r][4],
                     census_ops[r][5], census_ops[r][6], census_ops[r][7]);
    for (int r = 1024; r >= 0; r--)
        if (census_nodes[r]) tmst_log("photo: rank %d box %d,%d..%d,%d", r, census_box[r][0], census_box[r][1], census_box[r][2], census_box[r][3]);
}

/* ---- in and out ---------------------------------------------------------------- */
static void chosen(void) { ph.pending = 1; }        /* from inside the pause routine: in at the VBlank */

static void enter(int kind) {
    memset(&ph, 0, sizeof ph);
    ph.active = 1; ph.kind = kind; ph.view = VIEW_CAMERA;
    ph.player = tmst_pause_player() > 0 ? tmst_pause_player() : 0;
    practice_hint = 0;
    tmst_pause_hold(1);
    tmst_overlay_show_prims();
    tmst_overlay_backdrop(1, BACKDROP);
    vram_snapshot(0);
    if (rank_log) { memset(census_nodes, 0, sizeof census_nodes); census_frames = 2; }
    tmst_log("photo: on (%s, player %d)", kind == TMST_PAUSE_PRACTICE ? "Practice" : "pause menu", ph.player + 1);
}

static void leave(const char *why) {
    if (!ph.active) return;
    ph.active = 0;
    tmst_pause_hold(0);
    tmst_camera_restore();
    tmst_overlay_backdrop(0, 0);
    ph.capture = CAPTURE_NONE; ph.capture_stage = 0; ph.preview = 0;
    if (ph.kind == TMST_PAUSE_MENU) tmst_pause_select_next_time();
    tmst_overlay_hide();
    vram_snapshot(1);
    release_wait = ph.player;
    tmst_log("photo: off%s%s", why ? ": " : "", why ? why : "");
}

/* ---- the pad ------------------------------------------------------------------- */
static uint16_t input(int player, uint16_t buttons) {
    if (!on) return buttons;
    if (ph.active) {
        if (player == ph.player) ph.held = (uint16_t)~buttons;
        return 0xffff;                 /* the game sees nothing */
    }
    if (release_wait >= 0) {
        if (player == release_wait && buttons == 0xffff) release_wait = -1;
        else return 0xffff;
    }
    if (tmst_pause_kind() == TMST_PAUSE_PRACTICE && player == (tmst_pause_player() > 0 ? 1 : 0)) {
        select_down = !(buttons & B_SELECT);
        return buttons | B_SELECT;     /* SELECT is ours while Practice is paused */
    }
    return buttons;
}

/* ---- drawing --------------------------------------------------------------------- */
/* Text in the UI font (Fira Sans, tmst_ui_prim_text), its capitals' top 2
 * below the line's top (centred on the pad's symbols, ICON_H from y + 1). */
enum { SPACE = 4, TEXT_DY = 2, LINE = 17, GAP = 14, ICON_W = 9, ICON_H = 14, BAR_LINES = 3,
       BAR_TOP = 480 - 14 - BAR_LINES * LINE, TEXT = 0x7c7c7c, KEY = 0x4c9cb4, DARK = 0x100808 };
enum { I_NONE, I_CROSS, I_TRIANGLE, I_CIRCLE, I_SQUARE };

static void line(TmstPrims *k, int x0, int y0, int x1, int y1, uint32_t colour) {
    tmst_prim_room(k, 3);
    tmst_prim_word(k, 0x40000000u | colour); tmst_prim_word(k, tmst_xy(x0, y0)); tmst_prim_word(k, tmst_xy(x1, y1));
}
/* Two pixels wide: once more one pixel to the right. */
static void stroke(TmstPrims *k, int x0, int y0, int x1, int y1, uint32_t colour) {
    line(k, x0, y0, x1, y1, colour); line(k, x0 + 1, y0, x1 + 1, y1, colour);
}
/* The pad's symbols in their colours, ICON_W x ICON_H from (x, y). */
static void icon(TmstPrims *k, int x, int y, int which) {
    int r = x + ICON_W - 2, b = y + ICON_H - 1, cx = x + (ICON_W - 1) / 2, cy = y + ICON_H / 2;
    switch (which) {
    case I_CROSS:
        stroke(k, x, y + 1, r, b - 1, 0xf0a070); stroke(k, r, y + 1, x, b - 1, 0xf0a070); break;
    case I_TRIANGLE:
        stroke(k, cx, y + 1, x, b - 1, 0x80d040); stroke(k, cx, y + 1, r, b - 1, 0x80d040); stroke(k, x, b - 1, r, b - 1, 0x80d040); break;
    case I_CIRCLE: {
        static const int px[12] = {0, 2, 3, 4, 3, 2, 0, -2, -3, -4, -3, -2}, py[12] = {-6, -5, -3, 0, 3, 5, 6, 5, 3, 0, -3, -5};
        for (int i = 0; i < 12; i++) stroke(k, cx + px[i], cy + py[i], cx + px[(i + 1) % 12], cy + py[(i + 1) % 12], 0x5050e0);
        break;
    }
    case I_SQUARE:
        stroke(k, x, y + 1, r, y + 1, 0xc070e0); stroke(k, x, b - 1, r, b - 1, 0xc070e0);
        stroke(k, x, y + 1, x, b - 1, 0xc070e0); stroke(k, r, y + 1, r, b - 1, 0xc070e0); break;
    }
}

static int say(TmstPrims *k, int x, int y, const char *s, uint32_t tint) { return tmst_ui_prim_text(k, x, y + TEXT_DY, s, 1, tint); }

typedef struct { int icon; const char *key, *label; } Hint;
static int hint_width(const Hint *h, int n) {
    int w = 0;
    for (int i = 0; i < n; i++) {
        if (i) w += GAP;
        if (h[i].icon) w += ICON_W + 3;
        if (h[i].key) w += tmst_ui_prim_width(h[i].key, 1) + SPACE;
        w += tmst_ui_prim_width(h[i].label, 1);
    }
    return w;
}
static void hint_line(TmstPrims *k, int y, const Hint *h, int n) {
    int x = 184 - hint_width(h, n) / 2;
    for (int i = 0; i < n; i++) {
        if (i) x += GAP;
        if (h[i].icon) { icon(k, x, y + 1, h[i].icon); x += ICON_W + 3; }
        if (h[i].key) x += say(k, x, y, h[i].key, KEY) + SPACE;
        x += say(k, x, y, h[i].label, TEXT);
    }
}
/* A half see-through dark band, the screen's whole width (16:9 sides too). */
static void band(TmstPrims *k, int y, int h) {
    int margin = tmst_compat_widescreen_margin();
    tmst_prim_room(k, 4);
    tmst_prim_word(k, 0xe1000600u);                    /* B/2 + F/2, dither, drawing to the display */
    tmst_prim_word(k, 0x62000000u | DARK);
    tmst_prim_word(k, tmst_xy(-margin - 8, y));
    tmst_prim_word(k, tmst_xy(368 + 2 * margin + 16, h));
}

static void hint_bar(TmstPrims *k) {
    int camera = ph.view == VIEW_CAMERA;
    static const Hint top_camera[] = {{I_CROSS, NULL, "Take photo"}, {I_TRIANGLE, NULL, "Hide HUD"},
                                      {I_CIRCLE, NULL, "Exit"}, {I_NONE, "START", "Panel"}};
    static const Hint top_panel[] = {{I_CROSS, NULL, "Take photo"}, {I_TRIANGLE, NULL, "Hide HUD"},
                                     {I_CIRCLE, NULL, "Exit"}, {I_NONE, "START", "Camera"}};
    static const Hint move[] = {{I_NONE, "D-PAD", "Move"}, {I_NONE, "L1/R1", "Down/Up"}, {I_SQUARE, "+D-PAD", "Turn"}};
    static const Hint more[] = {{I_NONE, "L2/R2", "Zoom"}, {I_NONE, "SELECT", "Reset camera"}};
    static const Hint rows[] = {{I_NONE, "UP/DOWN", "Select"}, {I_NONE, "LEFT/RIGHT", "Change"}};
    band(k, BAR_TOP - 6, BAR_LINES * LINE + 10);
    hint_line(k, BAR_TOP, camera ? top_camera : top_panel, 4);
    if (camera) {
        hint_line(k, BAR_TOP + LINE, move, 3);
        hint_line(k, BAR_TOP + 2 * LINE, more, 2);
    } else {
        hint_line(k, BAR_TOP + LINE, rows, 2);
    }
}

/* The panel, top left under the game's HUD (its life bars and names reach
 * y 85): ZOOM, FOCUS, BLUR, FILTER, FRAME, RESET; the row
 * being changed (panel view only) on a lighter band, its value between
 * arrows. A CPU filter shown by its close look says so below. */
enum { PANEL_X = 10, PANEL_Y = 92, PANEL_W = 168, LABEL_W = 42, VALUE_X = PANEL_X + 8 + LABEL_W + 12,
       HIGHLIGHT = 0x183450 };
static void row_value(int row, char *out, size_t n) {
    switch (row) {
    case ROW_ZOOM: snprintf(out, n, "%d%%", (int)lround(ph.zoom * 100)); break;
    case ROW_FOCUS: snprintf(out, n, "%s", focus_names[ph.focus]); break;
    case ROW_BLUR: if (ph.blur) snprintf(out, n, "%d", ph.blur); else snprintf(out, n, "Off"); break;
    case ROW_FILTER: snprintf(out, n, "%s", tmst_photo_filter_names[ph.filter]); break;
    case ROW_FRAME: snprintf(out, n, "%s", tmst_photo_frame_names[ph.frame]); break;
    default: snprintf(out, n, "All"); break;
    }
}
static void panel(TmstPrims *k) {
    static const char *const labels[ROWS] = {"Zoom", "Focus", "Blur", "Filter", "Frame", "Reset"};
    const char *note = preview_wanted() && ph.preview <= 0 && !ph.preview_failed ? "Exact when still" : NULL;
    int lines = 1 + ROWS + (note ? 1 : 0), editing = ph.view == VIEW_PANEL;
    tmst_prim_room(k, 4);
    tmst_prim_word(k, 0xE1000600u);                    /* B/2 + F/2, dither, drawing to the display */
    tmst_prim_word(k, 0x62000000u | DARK);
    tmst_prim_word(k, tmst_xy(PANEL_X, PANEL_Y));
    tmst_prim_word(k, (uint32_t)(lines * LINE + 10) << 16 | PANEL_W);
    int y = PANEL_Y + 5;
    say(k, PANEL_X + 8, y, "Photo Mode", KEY);
    for (int r = 0; r < ROWS; r++) {
        y += LINE;
        char value[24];
        row_value(r, value, sizeof value);
        int chosen = editing && r == ph.row;
        if (chosen) {
            tmst_prim_room(k, 4);
            tmst_prim_word(k, 0xE1000600u);
            tmst_prim_word(k, 0x62000000u | HIGHLIGHT);
            tmst_prim_word(k, tmst_xy(PANEL_X + 3, y - 1));
            tmst_prim_word(k, (uint32_t)(LINE) << 16 | (PANEL_W - 6));
        }
        say(k, PANEL_X + 8, y, labels[r], chosen ? 0x808080 : TEXT);
        int w = say(k, VALUE_X, y, value, chosen ? 0x808080 : TEXT);
        if (chosen) {
            say(k, VALUE_X - tmst_ui_prim_width("<", 1) - 4, y, "<", KEY);
            say(k, VALUE_X + w + 4, y, ">", KEY);
        }
    }
    if (note) say(k, PANEL_X + 8, y + LINE, note, KEY);
}

/* Depth of field, FOCUS other than FIGHTERS: a sharp band (CENTER, TOP,
 * BOTTOM), the rest blurred, half as much next to it. */
static const int band_top[FOCUSES] = {0, 160, 40, 280};
static void tilt_shift(TmstPrims *k) {
    enum { BAND = 160, EASE_ROWS = 40 };
    int top = band_top[ph.focus], bottom = top + BAND, soft = (ph.blur + 1) / 2;
    tmst_photo_blur_prims(k, ph.blur, 0, top - EASE_ROWS);
    tmst_photo_blur_prims(k, soft, top - EASE_ROWS, top);
    tmst_photo_blur_prims(k, soft, bottom, bottom + EASE_ROWS);
    tmst_photo_blur_prims(k, ph.blur, bottom + EASE_ROWS, 480);
}
/* FOCUS FIGHTERS: the stage blurred under them (after its ranks, before
 * theirs: the overlay's rank builder at 1006). */
static void blur_behind(TmstPrims *k) {
    if (on && ph.active && ph.blur && ph.focus == FOCUS_FIGHTERS && !ph.capture && ph.preview <= 0)
        tmst_photo_blur_prims(k, ph.blur, 0, 480);
}

/* The flash after a photo, and PHOTO SAVED (or NOT SAVED) in the middle. */
static void shot_feedback(TmstPrims *k) {
    if (ph.flash) {
        uint32_t v = (uint32_t)(0xFF * ph.flash / FLASH_FRAMES);
        int m = tmst_compat_widescreen_margin();
        tmst_prim_room(k, 4);
        tmst_prim_word(k, 0xE1000420u);                /* B + F */
        tmst_prim_word(k, 0x62000000u | v * 0x010101u);
        tmst_prim_word(k, tmst_xy(-m - 8, 0));
        tmst_prim_word(k, (uint32_t)480 << 16 | (uint32_t)(368 + 2 * m + 16));
    }
    if (ph.saved) {
        const char *t = ph.saved_ok ? "Photo saved" : "Photo not saved";
        int w = tmst_ui_prim_width(t, 1), x = 184 - w / 2, y = 232;
        tmst_prim_room(k, 4);
        tmst_prim_word(k, 0xE1000600u);
        tmst_prim_word(k, 0x62000000u | DARK);
        tmst_prim_word(k, tmst_xy(x - 12, y - 7));
        tmst_prim_word(k, (uint32_t)30 << 16 | (uint32_t)(w + 24));
        say(k, x, y, t, ph.saved_ok ? 0x808080 : KEY);
    }
}

static void photo_prims(TmstPrims *k) {
    if (!on) return;
    if (practice_hint && !ph.active) {
        static const Hint h[] = {{I_NONE, "SELECT", "Photo Mode"}};
        band(k, 480 - 14 - LINE - 6, LINE + 10);
        hint_line(k, 480 - 14 - LINE, h, 1);
        return;
    }
    if (!ph.active) return;
    if (ph.capture) return;                            /* the frame read back: the scene alone */
    /* the picture's look first (the exact preview has the blur and the
     * filter in it), our own drawing over it */
    if (ph.preview <= 0) {
        if (ph.blur && ph.focus != FOCUS_FIGHTERS) tilt_shift(k);
        tmst_photo_filter_prims(k, ph.filter);
    }
    tmst_photo_frame_prims(k, ph.frame);
    if (!ph.hud_hidden) { panel(k); hint_bar(k); }
    shot_feedback(k);
}

/* The game's HUD, by its ranks in the ordering table (measured in Arcade
 * and Tekken Force): the top HUD (life bars, names, timer, rounds, score) at
 * 1006, and the nearest ranks, 0..7 (Tekken Force's progress bar at the
 * bottom). The ranks around 1006 hold the fighters' shadows (1007..1009,
 * 1020, by stage): kept. Without the fighters (the stage alone, for the
 * FIGHTERS blur) the top HUD goes further back, to 1009 where their shadows
 * were (measured 2026-11-02, Arcade): there everything from 1006 to the
 * stage's ranks (1017..) goes, the shadows with the fighters. */
static int hud_node(uint32_t node, int rank) {
    (void)node;
    if (ph.capture_stage && rank >= 1006 && rank <= 1016) return 1;
    return rank == 1006 || (rank >= 0 && rank <= 7);
}

/* The game's primitives while on: the HUD or the bench's ranks hidden; the
 * census. */
static void photo_visitor(uint32_t node) {
    if (!on || !ph.active) return;
    int rank = tmst_overlay_rank();
    if (census_frames == 1 && psx_mod_read_word(node) >> 24) {
        int at = rank < 0 ? 1024 : rank;
        if (!census_nodes[at]++) { census_first[at] = node; census_box[at][0] = census_box[at][1] = 9999; census_box[at][2] = census_box[at][3] = -9999; }
        unsigned nw = psx_mod_read_word(node) >> 24;
        for (unsigned i = 0; i < nw;) {
            uint32_t w = psx_mod_read_word(node + 4 + 4 * i); unsigned o = w >> 24, len = 1, v[4], c = 0;
            if (o >= 0x20 && o < 0x40) { unsigned vs = o & 8 ? 4 : 3, t = o & 4 ? 1 : 0, g = o & 0x10 ? 1 : 0, k = 1;
                for (unsigned q = 0; q < vs; q++) { if (q && g) k++; v[c++] = k; k += 1 + t; } len = k; }
            else if (o >= 0x60 && o < 0x80) { v[c++] = 1; len = 2 + (o & 4 ? 1 : 0) + ((o & 0x18) == 0 ? 1 : 0); }
            else if (o >= 0x40 && o < 0x60) { v[c++] = 1; v[c++] = 2; len = 3; }
            for (unsigned q = 0; q < c && i + v[q] < nw; q++) {
                uint32_t xy = psx_mod_read_word(node + 4 + 4 * (i + v[q])); int x = (int16_t)(xy & 0xffff), y = (int16_t)(xy >> 16);
                if (x < census_box[at][0]) census_box[at][0] = x; if (y < census_box[at][1]) census_box[at][1] = y;
                if (x > census_box[at][2]) census_box[at][2] = x; if (y > census_box[at][3]) census_box[at][3] = y;
            }
            i += len;
        }
        unsigned op = psx_mod_read_word(node + 4) >> 24;
        census_ops[at][op >= 0x20 && op < 0x40 ? 0 : op >= 0x40 && op < 0x60 ? 1 : op >= 0x60 && op < 0x80 ? 2 :
                       op >= 0x80 && op < 0xA0 ? 3 : op == 0x02 ? 4 : op >= 0xA0 && op < 0xC0 ? 5 : op >= 0xE0 ? 6 : 7]++;
    }
    if (rank >= hide_lo && rank <= hide_hi) tmst_overlay_hide_node(node);
    else if ((ph.hud_hidden || ph.capture_stage) && hud_node(node, rank)) tmst_overlay_hide_node(node);
}

/* ---- the camera ------------------------------------------------------------------ */
/* Units: the game's (a fighter is about 1 000 tall, the fight camera 5 500
 * from them), angles TMST_TURN a turn, speeds per frame. */
#define MOVE_SLOW 6.3
#define MOVE_FAST 42.0
#define TURN_SLOW 250.0
#define TURN_FAST 1000.0
#define ZOOM_RATE 0.012
#define EASE 0.18                      /* each frame a speed goes this part of the way to its target */
#define RAMP 45.0                      /* frames to full speed */
#define WALL_MIN 5500.0                /* the stage's edge: at least this far from the fighters ... */
#define WALL_ROOM 1.12                 /* ... and this much farther than the game's camera on the way in */
#define CEILING 3500.0                 /* above the floor (y 0), or the game's camera if higher */
#define ABOVE_FLOOR 60.0
#define MAX_PITCH (TMST_TURN * 80.0 / 360.0)
#define ZOOM_MIN 0.5
#define ZOOM_MAX 2.0

/* The stage is built around the fighters' midpoint (its tiles' origin
 * follows it, turned with the fight's axis), its walls and edges about
 * 6 000 .. 7 000 from it (measured: Gon's PS1 arena, arcade stages 1 and
 * 9), and the game's camera, some 5 500 from it, never sees past them.
 * Ours stays within a circle a little wider than the game's distance on
 * the way in (passing a wall it would see the void behind it), and over
 * the floor's grid, a cell in from its border (the grid snaps its centre
 * to a cell); above CEILING it would look over the walls. */
static void stage_edge(void) {
    int32_t fx, fy, fz, hx, hz, cell;
    tmst_camera_fighters(&fx, &fy, &fz);
    double dx = ph.start.x - fx, dz = ph.start.z - fz, d = sqrt(dx * dx + dz * dz);
    ph.wall = (d > WALL_MIN ? d : WALL_MIN) * WALL_ROOM;
    ph.ceiling = -ph.start.y > CEILING ? -ph.start.y : CEILING;
    if (tmst_camera_floor(&hx, &hz, &cell)) {
        double floor_edge = (hx < hz ? hx : hz) - cell;
        if (floor_edge > d && floor_edge < ph.wall) ph.wall = floor_edge;
    }
}

static void camera_from(const TmstCamera *c) {
    ph.x = c->x; ph.y = c->y; ph.z = c->z; ph.yaw = c->yaw; ph.pitch = c->pitch; ph.zoom = 1.0;
    ph.v_fwd = ph.v_side = ph.v_up = ph.v_yaw = ph.v_pitch = ph.v_zoom = 0;
}
static double ramp(unsigned held, double slow, double fast) {
    double t = held / RAMP;
    return slow + (fast - slow) * (t > 1 ? 1 : t);
}
static double ease(double v, double target) { return v + (target - v) * EASE; }
static double clampd(double v, double lo, double hi) { return v < lo ? lo : v > hi ? hi : v; }

/* One frame of the free camera, from the buttons held now. */
static void camera_frame(void) {
    if (!ph.have_camera) { ph.have_camera = 1; tmst_camera_game(&ph.start); camera_from(&ph.start); stage_edge(); }
    uint16_t h = ph.view == VIEW_CAMERA && ph.armed ? ph.held : 0;
    int turning = (h & B_SQUARE) != 0;
    int fwd = 0, side = 0, up = 0, yaw = 0, pitch = 0, zoom = 0;
    int dy = !!(h & B_UP) - !!(h & B_DOWN), dx = !!(h & B_RIGHT) - !!(h & B_LEFT);
    if (turning) { yaw = -dx; pitch = -dy; } else { fwd = dy; side = dx; }  /* the yaw grows turning left, the pitch looking down */
    up = !!(h & B_R1) - !!(h & B_L1);
    zoom = !!(h & B_R2) - !!(h & B_L2);
    ph.hold_move = fwd || side || up ? ph.hold_move + 1 : 0;
    ph.hold_turn = yaw || pitch ? ph.hold_turn + 1 : 0;
    ph.hold_zoom = zoom ? ph.hold_zoom + 1 : 0;
    double move = ramp(ph.hold_move, MOVE_SLOW, MOVE_FAST), turn = ramp(ph.hold_turn, TURN_SLOW, TURN_FAST);
    ph.v_fwd = ease(ph.v_fwd, fwd * move);
    ph.v_side = ease(ph.v_side, side * move);
    ph.v_up = ease(ph.v_up, up * move);
    ph.v_yaw = ease(ph.v_yaw, yaw * turn);
    ph.v_pitch = ease(ph.v_pitch, pitch * turn);
    ph.v_zoom = ease(ph.v_zoom, zoom * ramp(ph.hold_zoom, ZOOM_RATE / 3, ZOOM_RATE));
    /* The camera looks along +z at yaw 0; a growing yaw turns it left,
     * towards -x: forward (-sin, cos), right (cos, sin). */
    double a = ph.yaw * 6.283185307179586 / TMST_TURN, sn = sin(a), cs = cos(a);
    ph.x += -ph.v_fwd * sn + ph.v_side * cs;
    ph.z += ph.v_fwd * cs + ph.v_side * sn;
    ph.y -= ph.v_up;                   /* y grows downwards */
    ph.yaw = fmod(ph.yaw + ph.v_yaw, TMST_TURN);
    ph.pitch = clampd(ph.pitch + ph.v_pitch, -MAX_PITCH, MAX_PITCH);
    ph.zoom = clampd(ph.zoom * (1.0 + ph.v_zoom), ZOOM_MIN, ZOOM_MAX);
    /* Limits: inside the stage's edge (only the motion across it is
     * stopped: along it the eye slides), above the floor, below the
     * ceiling. */
    int32_t fx, fy, fz;
    tmst_camera_fighters(&fx, &fy, &fz);
    double ox = ph.x - fx, oz = ph.z - fz, reach = sqrt(ox * ox + oz * oz);
    if (reach > ph.wall) { ph.x = fx + ox * ph.wall / reach; ph.z = fz + oz * ph.wall / reach; }
    if (ph.y < -ph.ceiling || ph.y > -ABOVE_FLOOR) ph.v_up = 0;
    ph.y = clampd(ph.y, -ph.ceiling, -ABOVE_FLOOR);
    TmstCamera c = { (int32_t)lround(ph.x), (int32_t)lround(ph.y), (int32_t)lround(ph.z),
                     (int32_t)lround(ph.pitch), (int32_t)lround(ph.yaw), (int32_t)lround(ph.start.h * ph.zoom) };
    /* The HUD routine always runs (the paused frame's effect update goes with
     * it: without it TTT Hitspark never swaps its effects in, and the game's
     * own draw with its 64 x 64 sizes); hidden, its ranks are (photo_visitor). */
    tmst_camera_free(&c, 1);
    tmst_camera_hide_fighters(ph.capture_stage);
    tmst_camera_hide_ball_hud(ph.hud_hidden || ph.capture_stage);
    ph.cam = c;
    if (cam_log && ph.frames % 30 == 0) {
        int32_t hx, hz, cell;
        tmst_camera_floor(&hx, &hz, &cell);
        tmst_log("photo: camera %d %d %d pitch %d yaw %d h %d; fighters %d %d %d; floor %d x %d cell %d; edge %.0f", c.x, c.y,
                 c.z, c.pitch, c.yaw, c.h, fx, fy, fz, hx, hz, cell, ph.wall);
    }
}

/* ---- the panel's input -------------------------------------------------------------- */
static int wrap(int v, int n) { return (v % n + n) % n; }
static void panel_defaults(void) { ph.focus = FOCUS_FIGHTERS; ph.blur = 0; ph.filter = FX_NONE; ph.frame = FR_NONE; }

/* Panel view: UP / DOWN the row, LEFT / RIGHT its value (held, it repeats
 * after a third of a second, 6 times a second). */
static void panel_input(uint16_t pressed) {
    if (pressed & B_UP) ph.row = wrap(ph.row - 1, ROWS);
    if (pressed & B_DOWN) ph.row = wrap(ph.row + 1, ROWS);
    int dx = !!(ph.held & B_RIGHT) - !!(ph.held & B_LEFT);
    ph.hold_change = dx ? ph.hold_change + 1 : 0;
    int step = ph.hold_change == 1 || (ph.hold_change > 20 && (ph.hold_change - 20) % 10 == 0) ? dx : 0;
    if (!step) return;
    switch (ph.row) {
    case ROW_ZOOM: {
        double z = floor(ph.zoom * 10 + 0.5) / 10 + step * 0.1;
        ph.zoom = clampd(z, ZOOM_MIN, ZOOM_MAX); ph.v_zoom = 0;
        break;
    }
    case ROW_FOCUS: ph.focus = wrap(ph.focus + step, FOCUSES); break;
    case ROW_BLUR: ph.blur = (int)clampd(ph.blur + step, 0, BLUR_MAX); break;
    case ROW_FILTER: ph.filter = wrap(ph.filter + step, FX_FILTERS); break;
    case ROW_FRAME: ph.frame = wrap(ph.frame + step, FR_FRAMES); break;
    case ROW_RESET:
        if (ph.hold_change == 1) { panel_defaults(); if (ph.have_camera) camera_from(&ph.start); }
        break;
    }
}

/* ---- read-backs: the exact preview and the photo ----------------------------------- */
/* The exact preview is wanted while the live look is not the photo's: a
 * CPU filter; the blur on the software renderer (none live there); in 16:9
 * any blur or filter (the GPU's copies of the frame do not reach the sides,
 * which are not in VRAM). */
static int preview_wanted(void) {
    int wide = tmst_compat_widescreen_margin() > 0, blur_live = tmst_photo_blur_available();
    return tmst_photo_filter_cpu(ph.filter) || (ph.blur && (wide || !blur_live)) || (wide && ph.filter != FX_NONE);
}

/* The preview's picture, kept to be shown again after a photo. */
static uint32_t *preview_px;
static int preview_w, preview_h, preview_side;   /* side: the wide side, screen units */

static void show_preview(void) {
    if (!preview_px) return;
    ph.preview = tmst_overlay_show_scaled(preview_px, preview_w, preview_h, -preview_side, 0, 368 + 2 * preview_side, 480,
                                          TMST_OVERLAY_BORROW | TMST_OVERLAY_UNDER);
    if (!ph.preview) { ph.preview_failed = 1; tmst_log("photo: the exact preview cannot be shown"); }
}
static void hide_preview(void) {
    if (ph.preview > 0) tmst_overlay_hide_picture();
    ph.preview = 0;
}

/* The read-back frame boxed down to at most max_w x max_h, its 15-bit
 * steps dithered. */
static void make_preview(const uint32_t *px, int w, int h, int scale, int side, int max_w, int max_h) {
    int fx = (w + max_w - 1) / max_w, fy = (h + max_h - 1) / max_h;
    int pw = w / fx, phh = h / fy;
    uint32_t *out = realloc(preview_px, sizeof(uint32_t) * (size_t)pw * (size_t)phh);
    if (!out) return;
    static const uint8_t bayer[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};
    for (int y = 0; y < phh; y++)
        for (int x = 0; x < pw; x++) {
            unsigned r = 0, g = 0, b = 0, n = (unsigned)(fx * fy);
            for (int j = 0; j < fy; j++)
                for (int i = 0; i < fx; i++) {
                    uint32_t p = px[(size_t)(y * fy + j) * (size_t)w + (size_t)(x * fx + i)];
                    r += p >> 16 & 0xFF; g += p >> 8 & 0xFF; b += p & 0xFF;
                }
            unsigned d = bayer[(y & 3) * 4 + (x & 3)] / 2u;
            r = r / n + d; g = g / n + d; b = b / n + d;
            out[(size_t)y * (size_t)pw + (size_t)x] =
                0xFF000000u | (r > 255 ? 255u : r) << 16 | (g > 255 ? 255u : g) << 8 | (b > 255 ? 255u : b);
        }
    preview_px = out; preview_w = pw; preview_h = phh; preview_side = (int)lround((double)side / scale);
}

/* addons/tmst/screenshots/photo_YYYYMMDD_HHMMSS.png (_2, _3 ... in
 * the same second); TMST_PHOTO_DIR another folder (benches). 0: none. */
static int photo_path(char *out, size_t n) {
    char dir[4200];
    const char *e = getenv("TMST_PHOTO_DIR");
    if (e && *e) snprintf(dir, sizeof dir, "%s", e);
    else snprintf(dir, sizeof dir, "%s/addons/tmst/screenshots", tmst_files_root());
    if (!tmst_files_make_dirs(dir)) { tmst_log("photo: cannot make %s", dir); return 0; }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char stamp[32];
    if (!t || !strftime(stamp, sizeof stamp, "%Y%m%d_%H%M%S", t)) return 0;
    for (int i = 1; i < 100; i++) {
        if (i == 1) snprintf(out, n, "%s/photo_%s.png", dir, stamp);
        else snprintf(out, n, "%s/photo_%s_%d.png", dir, stamp, i);
        if (!tmst_files_exist(out)) return 1;
    }
    return 0;
}

/* A read-back: the FIGHTERS blur first wants a frame of the stage alone
 * (no fighters, no HUD), then every read-back the scene as the game draws
 * it, both without our look, CLEAN_FRAMES each. */
static uint32_t *stage_px;
static int stage_w, stage_h;

/* From the next frame on: the stage alone, or the scene again. */
static void stage_alone(int on) {
    ph.capture_stage = on;
    tmst_camera_hide_fighters(on);
    if (ph.have_camera) tmst_camera_free(&ph.cam, 1);
}

static void start_capture(int what) {
    int reshow = ph.preview > 0;
    hide_preview();
    ph.capture = what; ph.capture_frames = CLEAN_FRAMES;
    ph.capture_stage = ph.blur && ph.focus == FOCUS_FIGHTERS;
    if (what == CAPTURE_PHOTO && reshow) ph.preview = -1;
    stage_alone(ph.capture_stage);
}

/* The scene, its blur by the CPU. */
static void blur_pixels(uint32_t *px, int w, int h, int scale) {
    if (!ph.blur) return;
    int ok = ph.focus == FOCUS_FIGHTERS
                 ? stage_px && stage_w == w && stage_h == h && tmst_photo_blur_behind(px, stage_px, w, h, scale, ph.blur)
                 : tmst_photo_blur_band(px, w, h, scale, ph.blur, band_top[ph.focus]);
    if (!ok) tmst_log("photo: no blur (no memory, or the frames differ in size)");
}

/* The clean frame is on the screen: read it back. */
static void finish_capture(void) {
    int what = ph.capture, reshow = ph.preview == -1;
    uint32_t *px; int w, h, scale, side;
    if (!tmst_compat_read_frame(&px, &w, &h, &scale, &side)) {
        tmst_log("photo: the frame cannot be read back");
        ph.capture = CAPTURE_NONE; ph.capture_stage = 0; ph.preview = 0;
        if (what == CAPTURE_PREVIEW) ph.preview_failed = 1;
        else { ph.saved = SAVED_FRAMES; ph.saved_ok = 0; }
        return;
    }
    if (ph.capture_stage) {                             /* the stage alone: now the scene */
        free(stage_px);
        stage_px = px; stage_w = w; stage_h = h;
        stage_alone(0); ph.capture_frames = CLEAN_FRAMES;
        return;
    }
    ph.capture = CAPTURE_NONE; ph.preview = 0;
    if (what == CAPTURE_PREVIEW) {
        if (ph.still >= STILL_FRAMES) {                 /* nothing changed meanwhile */
            blur_pixels(px, w, h, scale);
            tmst_photo_look(px, w, h, scale, side, ph.filter, ph.frames);
            /* as big as it can be where TTT Hitspark's cells are not (its
             * effects may be drawn over the picture: the HUD hidden) */
            static const int sizes[3][2] = {{PREVIEW_MAX_W, 512}, {544, 512}, {544, 256}};
            for (int i = 0; i < 3; i++) {
                make_preview(px, w, h, scale, side, sizes[i][0], sizes[i][1]);
                show_preview();
                if (ph.preview <= 0 || !tmst_overlay_picture_contested()) break;
            }
        }
    } else {
        char path[4400];
        blur_pixels(px, w, h, scale);
        tmst_photo_finish(px, w, h, scale, side, ph.filter, ph.frame, ph.frames);
#ifdef __SWITCH__
        extern int switch_save_album_screenshot(const uint32_t *argb, int w, int h, int side);
        if (switch_save_album_screenshot(px, w, h, side)) {
            ph.saved_ok = 1;
            tmst_log("photo: saved to Switch Album (%d x %d)", w, h);
        } else
#endif
        {
            ph.saved_ok = photo_path(path, sizeof path) && tmst_png_write(path, px, w, h, w);
            if (ph.saved_ok) tmst_log("photo: saved %s (%d x %d)", path, w, h);
            else tmst_log("photo: the photo was not saved");
        }
        ph.saved = SAVED_FRAMES; ph.flash = FLASH_FRAMES;
        if (reshow && ph.still >= STILL_FRAMES) show_preview();
    }
    free(px);
    free(stage_px); stage_px = NULL;
}

/* Once a frame: the exact preview, wanted (preview_wanted) while the
 * picture stands, dropped as soon as anything in it changes. */
static void preview_frame(void) {
    static TmstCamera last_cam;
    static int last[4];
    int now[4] = {ph.filter, ph.blur, ph.focus, ph.hud_hidden};
    int same = !memcmp(&last_cam, &ph.cam, sizeof last_cam) && !memcmp(last, now, sizeof last);
    last_cam = ph.cam; memcpy(last, now, sizeof last);
    if (!same) { ph.still = 0; if (ph.preview > 0) hide_preview(); }
    else ph.still++;
    if (!preview_wanted()) { if (ph.preview > 0) hide_preview(); return; }
    if (!ph.capture && !ph.preview && !ph.preview_failed && ph.still == STILL_FRAMES) start_capture(CAPTURE_PREVIEW);
}

/* ---- the frame ------------------------------------------------------------------- */
static void tick(void) {
    if (!on) return;
    tmst_overlay_vblank();
    tmst_camera_vblank();
    int kind = tmst_pause_kind();
    if (ph.pending) { ph.pending = 0; if (!ph.active && kind == TMST_PAUSE_MENU) enter(kind); }
    if (!ph.active) {
        int hint = kind == TMST_PAUSE_PRACTICE;
        if (hint && select_down && release_wait < 0) { select_down = 0; enter(kind); return; }
        if (hint != practice_hint) {
            practice_hint = hint;
            if (hint) tmst_overlay_show_prims(); else tmst_overlay_hide();
        }
        return;
    }
    if (kind == TMST_PAUSE_NONE) { leave("the fight is no longer paused"); return; }
    if (census_frames && !--census_frames) census_report();
    ph.frames++;
    if (ph.flash) ph.flash--;
    if (ph.saved) ph.saved--;
    if (ph.capture && !--ph.capture_frames) finish_capture();
    camera_frame();
    preview_frame();
    /* The button that chose PHOTO (or SELECT) is still held: nothing counts
     * until every button has been let go. */
    if (!ph.armed) { ph.armed = ph.frames > 2 && !ph.held; ph.before = ph.held; return; }
    uint16_t pressed = (uint16_t)(ph.held & ~ph.before);
    ph.before = ph.held;
    if (pressed & B_CIRCLE) { leave(NULL); return; }
    if (pressed & B_START) ph.view = ph.view == VIEW_CAMERA ? VIEW_PANEL : VIEW_CAMERA;
    if (pressed & B_TRIANGLE) ph.hud_hidden = !ph.hud_hidden;
    if ((pressed & B_SELECT) && ph.have_camera && ph.view == VIEW_CAMERA) camera_from(&ph.start);
    if (ph.view == VIEW_PANEL) panel_input(pressed); else ph.hold_change = 0;
    if ((pressed & B_CROSS) && ph.capture != CAPTURE_PHOTO && ph.have_camera) start_capture(CAPTURE_PHOTO);
}

static void activate(void) {
    on = tmst_guard("tmst.photo-mode", TMST_NEED_ACTIVATION | TMST_NEED_VBLANK | TMST_NEED_GUEST_MEMORY | TMST_NEED_HIGH_VRAM);
    if (!on) return;
    if (!tmst_pause_add_row("PHOTO", chosen)) tmst_log("photo: no pause-menu row");
    tmst_input_add_filter(input);
    tmst_overlay_set_prims(photo_prims);
    tmst_overlay_set_visitor(photo_visitor);
    tmst_overlay_set_rank_prims(1006, blur_behind);
    tmst_overlay_prepare();
    benches();
}

PSX_MOD_CONSTRUCTOR(tmst_register_photo) {
    (void)psx_mod_register_activation_plugin("tmst.photo-mode", activate);
    (void)psx_mod_register_vblank_plugin("tmst.photo-mode", tick);
}
