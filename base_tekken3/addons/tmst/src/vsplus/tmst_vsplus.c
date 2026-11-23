/* TMST mod: VS Mode+ (tmst.vs-plus).
 *
 * The main menu's VS MODE row becomes VS MODE PLUS (the menu's font has no
 * +: the user's, 2026-11-08, "PLUS" in full) and can be chosen whatever
 * is connected (the native row asks for two pads: with one the menu skips
 * it; tmst_menu_native_row takes that away). Its choice does not start VS
 * at once: a band like the Expanded's STAGE SELECT one opens over the menu
 * and the player picks PLAYER VS CPU, PLAYER VS PLAYER or CPU VS CPU with
 * left and right (round the three); PLAYER VS PLAYER only with a second pad
 * (see-through otherwise, it does not take cross). Cross or start takes the
 * choice and hands the game a cross of its own, so the game starts VS as it
 * always does; circle or triangle closes the band.
 *
 * PLAYER VS PLAYER is the game's VS as it is. PLAYER VS CPU (and CPU VS CPU)
 * is a session until the main menu comes back (the fights, the rematches):
 *  - VS BATTLE SELECT (screen 10): player 1 chooses with its own cursor;
 *    once its block is done (state 12) its pad plays player 2's block
 *    (choice, handicap, MOVESET card), the second port given player 1's
 *    buttons through the runtime (tmst_compat_pad_set: the second port taken
 *    as connected when no pad is there, as the debug port's injection does,
 *    the buttons through every pad filter, ours and the Expanded's: the TTT1
 *    page, the MOVESET card, the STAGE band) and player 1's block nothing. A
 *    real second pad plays nothing in a session. Every hand-over waits until
 *    player 1 lets go of every button (the cross that took a choice would
 *    take the next at once).
 *  - CPU VS CPU: both cursors the CPU's, player 1's pad plays both blocks;
 *    in the fight it only pauses (the pause menu takes every button).
 *  - Player 2's cursor in the CPU's look of the Practice grid (the MOVESET
 *    card and the STAGE band's arrow too): its frame lines (the yellow
 *    0x00FFFF / D6 / 8F shades) in the CPU's green, its 2P tag (uv EC,00,
 *    palette 7F1F) the CPU tag (uv C4,00, palette 7ED2, both in VS BATTLE
 *    SELECT's VRAM).
 *  - The fight: the fighter record's +0xC5 nonzero makes its input the AI's
 *    (0x8002C278); held on player 2's record (CVC: both) through the fight
 *    screen. The human flags (0x800AE1F8) stay as VS has them: zeroed while
 *    choosing, the game draws the CPU's fighter itself; kept, the HUD names
 *    and player 1's START (pause) stay.
 *  - The CPU's level: 0x800AE211 (the AI table's stage, 0..9) held at 9 as
 *    Team Battle has it, on GAME OPTION's difficulty (0x800AE208, taken by
 *    the game when the mode starts; the Expanded's extra levels go through
 *    it). What it was is put back when the session ends. */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_input.h"
#include "tmst_menu.h"
#include "tmst_overlay.h"
#include "tmst_pause.h"
#include "tmst_vsplus_options.h"
#include "mod_plugins.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum { SCREEN = 0x800ae204u, PHASE = 0x800ae224u, PHASE_TIMER = 0x800ae214u, PADS = 0x800a9184u,
       MENU = 4, VS = 1 };
enum { PVC = 0, PVP = 1, CVC = 2, CHOICES = 3 };
static const char *const choice_name[CHOICES] = { "PLAYER VS CPU", "PLAYER VS PLAYER", "CPU VS CPU" };

static int on;
static unsigned frame;
static struct {
    int open, choice, last;              /* the band; the choice on it; the last one taken */
    int inject;                          /* frames of cross handed to the game */
    uint16_t held[2];                    /* the pads' buttons pressed (active high) last time */
} vp;
int tmst_vsplus_mode = -1;               /* the choice taken (PVC, PVP, CVC), -1 none yet */

static int second_pad(void) { return (psx_mod_read_byte(PADS) & 2) != 0; }

/* ---- the session (PLAYER VS CPU, CPU VS CPU) ---- */
enum { MODE = 0x800afa88u, BLOCKS = 0x800b8d70u, BLOCK = 172, DONE = 12, VS_SELECT = 10, LOADING = 11, FIGHT = 8,
       AI_STAGE = 0x800ae211u, AI_LEVEL = 9, RECORD = 0x800a9228u, RECORD_SIZE = 0x188c, AI_INPUT = 0xc5 };
static struct {
    int on, mode, forced;                /* a session; its choice; the second port taken as connected by us */
    int routing, release;                /* player 1's pad plays player 2; waiting for every button let go */
    uint16_t p1;                         /* player 1's buttons this frame (for a real second pad's read) */
    uint8_t ai;                          /* the CPU's level before the session */
    int shown;                           /* our primitives (the cursor) asked for on VS BATTLE SELECT */
    int seen_vs;                         /* VS BATTLE SELECT reached (the menu after it ends the session) */
} ss;
static unsigned block_state(int p) { return psx_mod_read_word(BLOCKS + (uint32_t)p * BLOCK) & 0xffffu; }
static void session_begin(int mode) {
    ss.on = 1; ss.mode = mode; ss.routing = 0; ss.release = 1;
    ss.ai = psx_mod_read_byte(AI_STAGE);
    ss.forced = tmst_compat_pad_connected(1) == 0 && tmst_compat_pad_set(1, 1, 0xffff);
    tmst_log("vs-plus: session %s (second port %s)", choice_name[mode],
             ss.forced ? "taken as connected" : tmst_compat_pad_connected(1) == 1 ? "a real pad, kept silent" : "unknown");
}
static void session_end(void) {
    if (!ss.on) return;
    psx_mod_write_byte(AI_STAGE, ss.ai);
    if (ss.forced) tmst_compat_pad_set(1, 0, 0xffff);
    tmst_log("vs-plus: session over");
    memset(&ss, 0, sizeof ss);
    tmst_vsplus_mode = -1;
}
/* Player 1's pad plays player 2's block: VS BATTLE SELECT, player 1 done,
 * player 2 not. */
static int wants_routing(void) {
    return ss.on && psx_mod_read_half(SCREEN) == VS_SELECT && psx_mod_read_word(MODE) == VS &&
           block_state(0) == DONE && block_state(1) != DONE;
}
int tmst_vsplus_forced_second_port(void) { return ss.on && ss.forced; }


/* ---- the game's primitives (a visitor) ---- */
static int16_t vx(uint32_t w) { return (int16_t)(w & 0xffff); }
static int16_t vy(uint32_t w) { return (int16_t)(w >> 16); }
typedef void (*PrimFn)(uint32_t at, unsigned cmd, unsigned size, int x0, int y0, int x1, int y1);
static void each_prim(uint32_t node, PrimFn fn) {
    unsigned words = psx_mod_read_word(node) >> 24;
    for (unsigned i = 1; i <= words;) {
        uint32_t a = node + i * 4, cmd = psx_mod_read_word(a) >> 24;
        unsigned size = 1, pos[16], count = 0;
        int sw = 0, sh = 0;
        if (cmd >= 0x20 && cmd < 0x40) {
            unsigned verts = cmd & 8 ? 4 : 3, gouraud = (cmd & 0x10) != 0, tex = (cmd & 4) != 0, w = 1;
            for (unsigned v = 0; v < verts; v++) { if (v && gouraud) w++; pos[count++] = w++; if (tex) w++; }
            size = w;
        } else if (cmd >= 0x60 && cmd < 0x80) {
            pos[count++] = 1;
            size = 2 + ((cmd & 4) ? 1 : 0) + ((cmd & 0x18) == 0 ? 1 : 0);
            unsigned fixed = (cmd >> 3) & 3;
            if (fixed == 1) sw = sh = 1; else if (fixed == 2) sw = sh = 8; else if (fixed == 3) sw = sh = 16;
            else if (i + size - 1 <= words) { uint32_t wh = psx_mod_read_word(a + (size - 1) * 4); sw = (int)(wh & 0x3ff); sh = (int)(wh >> 16 & 0x1ff); }
        } else if (cmd >= 0x40 && cmd < 0x60) {
            unsigned gouraud = (cmd & 0x10) != 0, w = 1;
            for (unsigned v = 0; i + w <= words && count < 16; v++) {
                if (v && gouraud) w++;
                if (i + w > words) break;
                uint32_t word = psx_mod_read_word(a + w * 4);
                if (v >= 2 && (cmd & 8) && (word & 0xf000f000u) == 0x50005000u) break;
                pos[count++] = w++;
                if (v == 1 && !(cmd & 8)) break;
            }
            size = w + ((cmd & 8) ? 1 : 0);
        }
        if (i + size - 1 > words) break;
        int x0 = 4096, x1 = -4096, y0 = 4096, y1 = -4096;
        for (unsigned v = 0; v < count; v++) {
            uint32_t w = psx_mod_read_word(a + pos[v] * 4);
            if (vx(w) < x0) x0 = vx(w); if (vx(w) > x1) x1 = vx(w);
            if (vy(w) < y0) y0 = vy(w); if (vy(w) > y1) y1 = vy(w);
        }
        if (count == 1) { x1 = x0 + sw; y1 = y0 + sh; }
        if (count) fn(a, cmd, size, x0, y0, x1, y1);
        i += size;
    }
}
static int primlog = -1, primlog_frames;
static void log_prim(uint32_t at, unsigned cmd, unsigned size, int x0, int y0, int x1, int y1) {
    char buf[200]; int n = 0;
    for (unsigned k = 0; k < size && k < 12 && n < 180; k++) n += snprintf(buf + n, sizeof buf - (size_t)n, " %08x", psx_mod_read_word(at + k * 4));
    tmst_log("vs-plus prim %02x (%d,%d)-(%d,%d)%s", cmd, x0, y0, x1, y1, buf);
}
/* The CPU's cursor (Practice's): frame lines of player 2 (CVC: player 1's
 * too) in the CPU's green, by shade; the 2P (1P) tag the CPU tag. */
enum { TAG_2P = 0x00ec, CLUT_2P = 0x7f1f, TAG_1P = 0x00d8, CLUT_1P = 0x7f1e, TAG_CPU = 0x00c4, CLUT_CPU = 0x7ed2, GRID_BOTTOM = 270 };
/* Under the grid, the Expanded's MOVESET card (and STAGE band arrows) in
 * the player's colours (tekken3_native_moves.c: letters tinted, arrows
 * flat, 0xBBGGRR): player 2's yellow (CVC: player 1's red too) the CPU's
 * green, which the Expanded has for Practice. */
enum { TINT_P1 = 0x00317c, ARROW_P1 = 0x3040f0, TINT_P2 = 0x009880, ARROW_P2 = 0x38c8f8, TINT_CPU = 0x009236, ARROW_CPU = 0x48c068 };
static void cpu_card(uint32_t at, unsigned cmd) {
    uint32_t w = psx_mod_read_word(at), c = w & 0xffffffu, to;
    if ((cmd & 0xfc) == 0x2c) to = c == TINT_P2 || (ss.mode == CVC && c == TINT_P1) ? TINT_CPU : c;
    else if (cmd == 0x20) to = c == ARROW_P2 || (ss.mode == CVC && c == ARROW_P1) ? ARROW_CPU : c;
    else return;
    if (to != c) tmst_overlay_patch_word(at, (w & 0xff000000u) | to);
}
static void cpu_cursor(uint32_t at, unsigned cmd, unsigned size, int x0, int y0, int x1, int y1) {
    (void)size; (void)x0; (void)x1; (void)y1;
    if (y0 > GRID_BOTTOM) { cpu_card(at, cmd); return; }
    uint32_t w = psx_mod_read_word(at);
    if (cmd == 0x40 || cmd == 0x42) {
        unsigned r = w & 255, g = w >> 8 & 255, b = w >> 16 & 255;
        int p2 = r && r == g && !b, p1 = r && !g && !b;
        if (!p2 && !(p1 && ss.mode == CVC)) return;
        unsigned k = r, nr = k * 144 / 255, ng = k * 232 / 255, nb = k * 120 / 255;   /* Practice's CPU green */
        tmst_overlay_patch_word(at, (w & 0xff000000u) | nb << 16 | ng << 8 | nr);
    } else if ((cmd & 0xfc) == 0x64) {
        uint32_t t = psx_mod_read_word(at + 8);
        unsigned u = t & 0xffff, clut = t >> 16;
        if ((u == TAG_2P && clut == CLUT_2P) || (ss.mode == CVC && u == TAG_1P && clut == CLUT_1P))
            tmst_overlay_patch_word(at + 8, (uint32_t)CLUT_CPU << 16 | TAG_CPU);
    }
}
static void visitor(uint32_t node) {
    if (!on) return;
    unsigned screen = psx_mod_read_half(SCREEN);
    if (ss.on && screen == VS_SELECT && psx_mod_read_word(MODE) == VS) each_prim(node, cpu_cursor);
    tmst_vsplus_options_visit(node);                  /* GAME OPTION's VS lamp (its own screen) */
    if (primlog > 0 && primlog_frames > 0 && (screen == 9 || screen == 10)) each_prim(node, log_prim);
}

/* ---- the pads ---- */
/* The second port without a pad: the runtime neither reads nor filters it;
 * player 1's buttons for it (ss.p1) go through the whole chain here (ours,
 * where player 2's filter hands them on, then the Expanded's: the TTT1
 * page's R2 / L2, the MOVESET card, the STAGE band, the costume panel), as
 * the debug port's second port does. With a real second pad the runtime
 * runs the chain itself. */
static void feed_second_port(void) {
    if (ss.forced) tmst_compat_pad_set(1, 1, tmst_input_run(1, ss.p1));
}
enum { FACE = TMST_PAD_CROSS | TMST_PAD_CIRCLE | TMST_PAD_SQUARE | TMST_PAD_TRIANGLE | TMST_PAD_START };
static uint16_t input(int player, uint16_t buttons) {
    if (!on || player < 0 || player > 1) return buttons;
    uint16_t held = (uint16_t)~buttons, down = held & (uint16_t)~vp.held[player];
    vp.held[player] = held;
    if (ss.on && vp.inject <= 0) {
        if (player == 1) return ss.routing && !ss.release ? ss.p1 : 0xffff;   /* a real second pad never plays */
        /* CPU VS CPU's fight: player 1's pad only pauses (the pause menu
         * takes every button). */
        if (ss.mode == CVC && psx_mod_read_half(SCREEN) == FIGHT && tmst_pause_player() < 0)
            return buttons | (uint16_t)~TMST_PAD_START;
        int r = wants_routing();
        if (r != ss.routing) { ss.routing = r; ss.release = 1; tmst_log("vs-plus: player 1's pad %s", r ? "plays player 2" : "its own again"); }
        if (ss.release) {
            if (held) { ss.p1 = 0xffff; feed_second_port(); return 0xffff; }
            ss.release = 0;
        }
        ss.p1 = ss.routing ? buttons : 0xffff;
        feed_second_port();
        return ss.routing ? 0xffff : buttons;
    }
    if (vp.inject > 0) {                              /* the game's cross: the choice taken */
        if (player == 0) { vp.inject--; return (uint16_t)(0xffffu & ~TMST_PAD_CROSS); }
        return 0xffff;
    }
    if (!vp.open) {
        if (!tmst_menu_native_highlighted(VS)) return buttons;
        if (down & (TMST_PAD_CROSS | TMST_PAD_START)) {
            vp.open = 1; vp.choice = vp.last;
            tmst_log("vs-plus: the band opened (%s)", choice_name[vp.choice]);
        }
        return buttons | FACE;                        /* no face button starts VS by itself */
    }
    if (down & TMST_PAD_LEFT) vp.choice = (vp.choice + CHOICES - 1) % CHOICES;
    if (down & TMST_PAD_RIGHT) vp.choice = (vp.choice + 1) % CHOICES;
    if (down & (TMST_PAD_CIRCLE | TMST_PAD_TRIANGLE)) {
        vp.open = 0;
        tmst_log("vs-plus: the band closed");
    } else if (down & (TMST_PAD_CROSS | TMST_PAD_START)) {
        if (vp.choice == PVP && !second_pad()) {
            tmst_log("vs-plus: PLAYER VS PLAYER needs a second pad");
        } else {
            vp.open = 0; vp.last = vp.choice; tmst_vsplus_mode = vp.choice;
            vp.inject = 3;
            if (vp.choice != PVP) session_begin(vp.choice);
            tmst_log("vs-plus: %s chosen, VS starts", choice_name[vp.choice]);
        }
    }
    return 0xffff;                                    /* the menu sees nothing while the band is open */
}

/* ---- drawing: the menu's title font (page 832, 4 bpp, cells of 14 x 24
 * from u 172, six to a row: 1..9, A..Z, !; every 13 px) ---- */
enum { FONT_PAGE = 832 / 64, FONT_U = 172, GLYPH_W = 14, GLYPH_H = 24, ADVANCE = 13,
       TITLE_CLUT = (505 << 6) | (336 / 16), WHITE_CLUT = (505 << 6) | (352 / 16), OUTLINE = 0x101010 };
static int glyph(char c) {
    if (c >= '1' && c <= '9') return c - '1';
    if (c >= 'A' && c <= 'Z') return 9 + c - 'A';
    return c == '!' ? 35 : -1;
}
static uint32_t uv(int u, int v) { return (uint32_t)(u > 255 ? 255 : u) | (uint32_t)v << 8; }
static void put(TmstPrims *k, uint32_t w) { tmst_prim_word(k, w); }
/* A textured quad of the font: screen corners TL TR BL BR, texels for each. */
static void quad(TmstPrims *k, uint32_t op_colour, uint32_t clut, const int xy[8], const int st[8]) {
    tmst_prim_room(k, 9);
    put(k, op_colour);
    put(k, tmst_xy(xy[0], xy[1])); put(k, uv(st[0], st[1]) | clut << 16);
    put(k, tmst_xy(xy[2], xy[3])); put(k, uv(st[2], st[3]) | (uint32_t)FONT_PAGE << 16);
    put(k, tmst_xy(xy[4], xy[5])); put(k, uv(st[4], st[5]));
    put(k, tmst_xy(xy[6], xy[7])); put(k, uv(st[6], st[7]));
}
static void cell(TmstPrims *k, uint32_t op_colour, uint32_t clut, int i, int x, int y, int w, int h) {
    int u = FONT_U + i % 6 * GLYPH_W, v = i / 6 * GLYPH_H;
    const int xy[8] = { x, y, x + w, y, x, y + h, x + w, y + h };
    const int st[8] = { u, v, u + GLYPH_W, v, u, v + GLYPH_H, u + GLYPH_W, v + GLYPH_H };
    quad(k, op_colour, clut, xy, st);
}
/* A line of the font centred on cx, scaled num/den, `tint` (0xBBGGRR,
 * 0x808080 as it is); semi: half see-through (B/2 + F/2). Returns the
 * right edge. */
static int text(TmstPrims *k, const char *s, int cx, int y, int num, int den, uint32_t tint, uint32_t clut, int semi) {
    int n = (int)strlen(s), advance = ADVANCE * num / den, w = GLYPH_W * num / den, h = GLYPH_H * num / den;
    int x = cx - (n * advance + (w - advance)) / 2;
    uint32_t op = (semi ? 0x2e000000u : 0x2c000000u) | (tint & 0xffffffu);
    for (; *s; s++, x += advance) {
        int i = glyph(*s);
        if (i >= 0) cell(k, op, clut, i, x, y, w, h);
    }
    return x;
}
static void triangle(TmstPrims *k, uint32_t colour, int x0, int y0, int x1, int y1, int x2, int y2) {
    tmst_prim_room(k, 4);
    put(k, 0x20000000u | colour); put(k, tmst_xy(x0, y0)); put(k, tmst_xy(x1, y1)); put(k, tmst_xy(x2, y2));
}
/* An arrow, its tip at x pointing left (dir > 0) or right (dir < 0), outlined
 * (the Expanded's MOVESET / STAGE SELECT arrows). */
static void arrow(TmstPrims *k, int x, int y, int dir, uint32_t colour) {
    triangle(k, OUTLINE, x - 2 * dir, y, x + 10 * dir, y - 9, x + 10 * dir, y + 9);
    triangle(k, colour, x, y, x + 8 * dir, y - 6, x + 8 * dir, y + 6);
}

/* The band: the width of the Expanded's (224), four dark half see-through
 * layers, VS MODE PLUS small in the title's orange, the choice under it in the
 * white of the VS names (see-through grey: PLAYER VS PLAYER without a second
 * pad), between player 1's red arrows that sway. Centred on the VS row. */
enum { CENTRE_X = 184, BAND_W = 224, BAND_H = 46, P1_ARROW = 0x3040f0 };
static void band(TmstPrims *k, int row_y) {
    int top = row_y + GLYPH_H / 2 - BAND_H / 2, left = CENTRE_X - BAND_W / 2;
    int t = (int)(frame % 60), sway = (t < 30 ? t : 60 - t) / 8;
    tmst_prim_room(k, 13);
    put(k, 0xe1000600u | FONT_PAGE);                  /* semi-transparency B/2 + F/2 */
    for (int layer = 0; layer < 4; layer++) { put(k, 0x62000000u); put(k, tmst_xy(left, top)); put(k, tmst_xy(BAND_W, BAND_H)); }
    text(k, "VS MODE PLUS", CENTRE_X, top + 5, 1, 2, 0x808080, TITLE_CLUT, 0);
    int dim = vp.choice == PVP && !second_pad();
    text(k, choice_name[vp.choice], CENTRE_X, top + 22, 3, 5, dim ? 0x505050 : 0x808080, WHITE_CLUT, dim);
    int half = BAND_W / 2 - 6, y = top + 22 + 7;
    arrow(k, CENTRE_X - half - sway, y, 1, P1_ARROW);
    arrow(k, CENTRE_X + half + sway, y, -1, P1_ARROW);
}

/* The band's row: the highlighted one, its place as the menu draws it
 * (measured: the highlighted row's letters at y 364). */
enum { ROW_Y = 364 };
static void prims(TmstPrims *k) {
    if (!on || !vp.open || psx_mod_read_half(SCREEN) != MENU) return;
    band(k, ROW_Y);
}

static int showing;                      /* we asked for the primitives (the band) */
static void session_tick(void) {
    if (!ss.on) return;
    unsigned screen = psx_mod_read_half(SCREEN);
    if (screen == VS_SELECT) ss.seen_vs = 1;
    if (screen == MENU && ss.seen_vs) { session_end(); return; }
    if (psx_mod_read_word(MODE) != VS) return;
    if (screen == VS_SELECT || screen == LOADING || screen == FIGHT) psx_mod_write_byte(AI_STAGE, AI_LEVEL);
    if (screen == FIGHT) {
        psx_mod_write_byte(RECORD + RECORD_SIZE + AI_INPUT, 1);
        if (ss.mode == CVC) psx_mod_write_byte(RECORD + AI_INPUT, 1);
    }
    if (screen == VS_SELECT) { tmst_overlay_show_prims(); ss.shown = 1; }
    else if (ss.shown) { ss.shown = 0; if (!tmst_overlay_visible()) tmst_overlay_hide(); }
}
static void tick(void) {
    if (!on) return;
    frame++;
    /* The band gone: our primitives too, unless someone else's picture
     * stays; before the core's menu VBlank, which shows them again if it
     * colours rows. */
    if (showing && !vp.open) {
        showing = 0; tmst_overlay_short_lists(0);
        if (!tmst_overlay_visible()) tmst_overlay_hide();
    }
    tmst_menu_vblank();
    tmst_overlay_vblank();
    if (!psx_mod_game_started()) return;
    if (primlog < 0) { const char *e = getenv("TMST_VSPLUS_PRIMLOG"); primlog = e && *e == '1'; }
    if (primlog > 0 && (psx_mod_read_half(SCREEN) == 10 || psx_mod_read_half(SCREEN) == 9)) {
        tmst_overlay_show_prims();
        static unsigned n10; n10++;
        primlog_frames = n10 % 300 == 0;
        if (primlog_frames) tmst_log("vs-plus primlog frame %u", n10);
    }
    session_tick();
    tmst_vsplus_options_tick();
    if (vp.open) {
        if (psx_mod_read_half(SCREEN) != MENU || psx_mod_read_half(PHASE) != 1) {
            vp.open = 0;
            tmst_log("vs-plus: the band closed (the menu left)");
        } else {
            psx_mod_write_word(PHASE_TIMER, 0);       /* no attract while the band is open */
            tmst_overlay_short_lists(1); tmst_overlay_show_prims(); showing = 1;
        }
    }
}

static void activate(void) {
    on = tmst_guard("tmst.vs-plus", TMST_NEED_ACTIVATION | TMST_NEED_VBLANK | TMST_NEED_GUEST_MEMORY | TMST_NEED_HIGH_VRAM);
    if (!on) return;
    tmst_menu_native_row(VS, "VS MODE PLUS");
    tmst_input_add_filter(input);
    tmst_overlay_set_prims(prims);
    tmst_overlay_set_visitor(visitor);
    tmst_overlay_prepare();
}

PSX_MOD_CONSTRUCTOR(tmst_register_vsplus) {
    (void)psx_mod_register_activation_plugin("tmst.vs-plus", activate);
    (void)psx_mod_register_vblank_plugin("tmst.vs-plus", tick);
}
