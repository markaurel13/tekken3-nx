/* tmst.vs-plus: the VS lamp in GAME OPTION's mode box for DIFFICULTY
 * (tmst_vsplus_options.h).
 *
 * The box's eight lines (docs/handoffs/handoff_menu_colors.md): sprites
 * 96 x 14 of the options overlay's pre-drawn lines (VRAM page 704, line i
 * at v 14 i), VS the second; a line's lamp is its palette, dim (480, 505),
 * VS lit (288, 505). VS MODE PLUS's CPU takes GAME OPTION's difficulty (the
 * session holds the AI table's stage, the level is the game's): on the
 * DIFFICULTY row the VS line is lit, for that drawing only (the visitor).
 * The line is told by its texture page and its v, not by where it stands
 * (the tournament's box moves the lines up). */
#include "tmst_vsplus_options.h"
#include "tmst_core.h"
#include "tmst_overlay.h"
#include "mod_plugins.h"
#include <string.h>

enum { SCREEN = 0x800ae204, OPTIONS = 5, MENU = 4, LEVEL = 0x800ec020, GAME_OPTION = 1,
       CURSOR = 0x800ead58, ROWS_AT = CURSOR + 0x20, ROW_BYTES = 20,
       LINES_PAGE = 704 / 64, LINE_W = 96, LINE_H = 14, VS_LINE = 1,
       VS_CLUT = (505 << 6) | (288 / 16) };

static int here, lit;                     /* GAME OPTION on screen; the cursor on DIFFICULTY */
static unsigned page;                     /* the texture page the walk has reached */

static int on_difficulty(void) {
    uint32_t rows = psx_mod_read_word(ROWS_AT), cursor = psx_mod_read_half(CURSOR);
    uint32_t title = rows ? psx_mod_read_word(rows + cursor * ROW_BYTES + 4) : 0;
    if (title < 0x80000000u) return 0;
    char s[24];
    for (unsigned i = 0; i < sizeof s; i++) if (!(s[i] = (char)psx_mod_read_byte(title + i))) break;
    s[sizeof s - 1] = 0;
    return !strcmp(s, "DIFFICULTY LEVEL");
}

void tmst_vsplus_options_tick(void) {
    unsigned screen = psx_mod_read_word(SCREEN);
    int now = screen == OPTIONS && psx_mod_read_byte(LEVEL) == GAME_OPTION;
    if (!now) {
        if (here) {
            here = lit = 0;
            if (screen != MENU && !tmst_overlay_visible()) tmst_overlay_hide();   /* on the menu the core colours its rows */
        }
        return;
    }
    here = 1;
    int was = lit;
    lit = on_difficulty();
    if (lit != was) tmst_log("vs-plus: GAME OPTION's VS lamp %s", lit ? "lit (DIFFICULTY LEVEL)" : "as the game has it");
    tmst_overlay_show_prims();                            /* the visitor walks the box */
}

static void line(uint32_t a) {
    uint32_t uv = psx_mod_read_word(a + 8), wh = psx_mod_read_word(a + 12);
    if ((page & 0x1f) != LINES_PAGE || wh != tmst_xy(LINE_W, LINE_H) || (uv >> 8 & 0xff) != (uint32_t)(VS_LINE * LINE_H)) return;
    tmst_overlay_patch_word(a + 8, (uint32_t)VS_CLUT << 16 | (uv & 0xffff));
}
void tmst_vsplus_options_visit(uint32_t node) {
    if (!here || !lit) return;
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
            if (i + size - 1 <= words && (cmd & 0xfc) == 0x64) line(a);
        }
        i += size;
    }
}
