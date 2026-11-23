/* tmst.tournament: TOURNAMENT in GAME OPTION's mode box
 * (tmst_tournament_options.h).
 *
 * The box (measured, docs/handoffs/handoff_menu_colors.md): a white
 * rectangle (246, 40) 98 x 116, a black one in it (247, 42) 96 x 112, and
 * eight sprites 96 x 14 of the options overlay's pre-drawn lines (4-bit,
 * VRAM (704, 0), line i at v 14 i, on the screen at (247, 42 + 14 i)): ARCADE,
 * VS, TEAM BATTLE, TIME ATTACK, SURVIVAL, TEKKEN BALL, TEKKEN FORCE,
 * PRACTICE. A line's lamp is its palette (row 505): dim (480, 505), lit each
 * mode's own (Time Attack's the white one, (352, 505)). The lines' background
 * is their colour 15 (16, 16, 16), their colour 0 see-through; the letters
 * stand on a grid of 8 pixels.
 *
 * Here, for that drawing only (the visitor): the box a line taller, from
 * (246, 30), 98 x 130, its inside the lines' background, the eight lines
 * 10 higher; the ninth line, TOURNAMENT in the lines' own letters (cut from
 * them: T, E, N, O, R of TEKKEN FORCE, U of SURVIVAL, A of ARCADE, M of
 * TEAM), a picture of ours at (255, 144): lit, the white palette in old
 * gold; dim, the dim palette. Lit where Team Battle's line is lit (the
 * tournament is a Team Battle), but for the rows the tournament sets itself. */
#include "tmst_tournament_options.h"
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_overlay.h"
#include "mod_plugins.h"
#include <string.h>

enum { SCREEN = 0x800ae204, OPTIONS = 5, MENU = 4, LEVEL = 0x800ec020, GAME_OPTION = 1,
       CURSOR = 0x800ead58, ROWS_AT = CURSOR + 0x20, ROW_BYTES = 20,
       LINES_PAGE = 704 / 64, LINE_W = 96, LINE_H = 14, LINES = 8, TEAM_BATTLE_LINE = 2,
       DIM_CLUT = (505 << 6) | (480 / 16), WHITE_X = 352, DIM_X = 480, CLUT_Y = 505,
       BOX_X = 246, BOX_Y = 40, BOX_W = 98, BOX_H = 116,
       NEW_X = BOX_X, NEW_Y = 30, NEW_W = BOX_W, NEW_H = BOX_H + LINE_H,
       LINES_X = NEW_X + 1 + (NEW_W - 2 - LINE_W) / 2, LINES_Y = NEW_Y + 2,
       CELL = 8, NAME_LEN = 10, OUR_W = NAME_LEN * CELL, OUR_X = NEW_X + 1 + (NEW_W - 2 - OUR_W) / 2,
       OUR_Y = LINES_Y + LINES * LINE_H, INSIDE = 0x101010, GOLD = 0xE8C030 };
static const char name[] = "TOURNAMENT";
/* Each letter's cell in the lines (u, v). */
static const struct { char c; unsigned char u, v; } cells[] = {
    {'T', 0, 84}, {'E', 8, 84}, {'N', 40, 84}, {'O', 64, 84}, {'R', 72, 84},
    {'U', 24, 56}, {'A', 24, 0}, {'M', 28, 28},
};
/* The rows whose Team Battle lamp the tournament does not follow, its own
 * lamp: the rounds to win are its own (dim); Tekken 2's stages are its
 * Tekken 1 and 2 tournaments' (lit; Team Battle's lamp dim). */
static const struct { const char *title; int lit; } own_rows[] = {
    { "FIGHT COUNT", 0 }, { "TEKKEN 2 STAGES", 1 },
};

static unsigned frames, box_frame, team_battle_lit;
static unsigned page;                     /* the texture page the walk has reached */
static int here, shown;                   /* GAME OPTION on screen; our line shown (1 dim, 2 lit) */
static int ready;
static uint32_t lit_line[OUR_W * LINE_H], dim_line[OUR_W * LINE_H];

static uint32_t argb(uint16_t c, int alpha) {
    return (uint32_t)alpha << 24 | (uint32_t)(c & 31) << 19 | (uint32_t)(c >> 5 & 31) << 11 | (uint32_t)(c >> 10 & 31) << 3;
}
static uint32_t gold(uint16_t c) {
    uint32_t out = 0xff000000u;
    for (int k = 0; k < 3; k++) {
        unsigned v = (unsigned)(c >> (5 * k) & 31) << 3, g = GOLD >> (8 * (2 - k)) & 0xff, t = v * g / 248;
        out |= (t > 255 ? 255u : t) << (8 * (2 - k));
    }
    return out;
}
/* Our line from the lines' letters in VRAM (once, when the box was seen). */
static int build(void) {
    uint16_t white[16], dim[16];
    if (!tmst_compat_vram_read(WHITE_X, CLUT_Y, 16, 1, white) || !tmst_compat_vram_read(DIM_X, CLUT_Y, 16, 1, dim)) return 0;
    uint32_t bg = argb(dim[15], 255);
    for (int i = 0; i < NAME_LEN; i++) {
        uint16_t rows[LINE_H * 2];
        int found = -1;
        for (int g = 0; g < (int)(sizeof cells / sizeof cells[0]); g++) if (cells[g].c == name[i]) found = g;
        if (found >= 0 && !tmst_compat_vram_read(LINES_PAGE * 64 + cells[found].u / 4, cells[found].v, 2, LINE_H, rows)) return 0;
        for (int y = 0; y < LINE_H; y++)
            for (int x = 0; x < CELL; x++) {
                int at = y * OUR_W + i * CELL + x;
                unsigned idx = found < 0 ? 15 : rows[y * 2 + x / 4] >> (x % 4 * 4) & 15;
                if (idx == 15) { lit_line[at] = dim_line[at] = bg; continue; }
                if (idx == 0) { lit_line[at] = dim_line[at] = 0; continue; }     /* see-through */
                lit_line[at] = gold(white[idx]);
                dim_line[at] = argb(dim[idx], 255);
            }
    }
    tmst_log("tournament: GAME OPTION's TOURNAMENT line built");
    return 1;
}

/* The row under the cursor: the tournament's own lamp (0 dim, 1 lit), -1
 * Team Battle's. */
static int own_row(void) {
    uint32_t rows = psx_mod_read_word(ROWS_AT), cursor = psx_mod_read_half(CURSOR);
    uint32_t title = rows ? psx_mod_read_word(rows + cursor * ROW_BYTES + 4) : 0;
    if (title < 0x80000000u) return -1;
    char s[32];
    for (unsigned i = 0; i < sizeof s; i++) if (!(s[i] = (char)psx_mod_read_byte(title + i))) break;
    s[sizeof s - 1] = 0;
    for (unsigned i = 0; i < sizeof own_rows / sizeof own_rows[0]; i++)
        if (!strcmp(s, own_rows[i].title)) return own_rows[i].lit;
    return -1;
}

void tmst_tournament_options_tick(void) {
    frames++;
    unsigned screen = psx_mod_read_word(SCREEN);
    int now = screen == OPTIONS && psx_mod_read_byte(LEVEL) == GAME_OPTION;
    if (!now) {
        if (here) {
            here = 0;
            if (shown) { tmst_overlay_hide_picture(); shown = 0; }
            if (screen != MENU) tmst_overlay_hide();      /* on the menu the core colours its rows */
        }
        return;
    }
    here = 1;
    tmst_overlay_show_prims();                            /* the visitor walks the box */
    if (frames - box_frame > 2) {
        if (shown) { tmst_overlay_hide_picture(); shown = 0; }
        return;
    }
    if (!ready && !(ready = build())) return;
    int own = own_row(), want = (own < 0 ? frames - team_battle_lit <= 2 : own) ? 2 : 1;
    if (shown != want || !tmst_overlay_visible()) {
        tmst_overlay_show_ex(want == 2 ? lit_line : dim_line, OUR_W, LINE_H, OUR_X, OUR_Y, TMST_OVERLAY_BORROW);
        shown = want;
    }
}

/* ---- the box, for this drawing ---- */
static void rect(uint32_t a) {
    uint32_t xy = psx_mod_read_word(a + 4), wh = psx_mod_read_word(a + 8);
    if (xy == tmst_xy(BOX_X, BOX_Y) && wh == tmst_xy(BOX_W, BOX_H)) {
        tmst_overlay_patch_word(a + 4, tmst_xy(NEW_X, NEW_Y));
        tmst_overlay_patch_word(a + 8, tmst_xy(NEW_W, NEW_H));
        box_frame = frames;
    } else if (xy == tmst_xy(BOX_X + 1, BOX_Y + 2) && wh == tmst_xy(BOX_W - 2, BOX_H - 4)) {
        tmst_overlay_patch_word(a, 0x60000000u | INSIDE);
        tmst_overlay_patch_word(a + 4, tmst_xy(NEW_X + 1, NEW_Y + 2));
        tmst_overlay_patch_word(a + 8, tmst_xy(NEW_W - 2, NEW_H - 4));
    }
}
static void line(uint32_t a) {
    uint32_t xy = psx_mod_read_word(a + 4), uv = psx_mod_read_word(a + 8), wh = psx_mod_read_word(a + 12);
    int x = (int16_t)(xy & 0xffff), y = (int16_t)(xy >> 16), i = (y - (BOX_Y + 2)) / LINE_H;
    if ((page & 0x1f) != LINES_PAGE || wh != tmst_xy(LINE_W, LINE_H) || x != BOX_X + 1 || i < 0 || i >= LINES ||
        y != BOX_Y + 2 + i * LINE_H || (uv >> 8 & 0xff) != (uint32_t)(i * LINE_H))
        return;
    tmst_overlay_patch_word(a + 4, tmst_xy(LINES_X, LINES_Y + i * LINE_H));
    if (i == TEAM_BATTLE_LINE && (uv >> 16) != DIM_CLUT) team_battle_lit = frames;
}
void tmst_tournament_options_visit(uint32_t node) {
    if (!here) return;
    unsigned words = psx_mod_read_word(node) >> 24;
    for (unsigned i = 1; i <= words;) {
        uint32_t a = node + i * 4, cmd = psx_mod_read_word(a) >> 24;
        unsigned size = 1;
        if (cmd == 0xe1) page = psx_mod_read_word(a) & 0x9ff;
        else if (cmd >= 0x20 && cmd < 0x40) {
            unsigned verts = cmd & 8 ? 4 : 3, per = 1 + ((cmd & 4) ? 1 : 0) + ((cmd & 0x10) ? 1 : 0);
            size = 1 + verts * per - ((cmd & 0x10) ? 1 : 0);
            if ((cmd & 4) && i + size - 1 <= words) page = psx_mod_read_word(a + ((cmd & 0x10) ? 20 : 16)) >> 16 & 0x9ff;
        } else if (cmd >= 0x40 && cmd < 0x60) {
            unsigned gouraud = (cmd & 0x10) != 0, w = 1;
            for (unsigned v = 0; i + w <= words; v++) {
                if (v && gouraud) w++;
                if (i + w > words) break;
                uint32_t xy = psx_mod_read_word(a + w * 4);
                if (v >= 2 && (cmd & 8) && (xy & 0xf000f000u) == 0x50005000u) break;
                w++;
                if (v == 1 && !(cmd & 8)) break;
            }
            size = w + ((cmd & 8) ? 1 : 0);
        } else if (cmd >= 0x60 && cmd < 0x80) {
            size = 2 + ((cmd & 4) ? 1 : 0) + ((cmd & 0x18) == 0 ? 1 : 0);
            if (i + size - 1 <= words) {
                if ((cmd & 0xfe) == 0x60) rect(a);
                else if ((cmd & 0xfc) == 0x64) line(a);
            }
        }
        i += size;
    }
}
