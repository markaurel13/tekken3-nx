/* TMST core: our main-menu rows (tmst_menu.h). */
#include "tmst_menu.h"
#include "tmst_core.h"
#include "tmst_overlay.h"
#include "cpu_state.h"
#include "mod_plugins.h"
#include <stdint.h>
#include <string.h>

enum { SCREEN = 0x800ae204u, PHASE = 0x800ae224u, SELECTION = 0x80097f40u, MENU = 4,
       MENU_ENTRY = 0x800db5b4u, LIST_HI = 0x800db5d0u, LIST_LO = 0x800db5d8u,
       MAX_ROWS = 4, ROW_BYTES = 8, ROWS_AT = 16, MAX_COUNT = 16, AT_END = 0x100 };

typedef struct {
    uint32_t label; unsigned like_mode, after_mode; TmstMenuChosen chosen; int index;
    uint32_t rgb; int coloured, distance; char word[32];   /* the colour: the label without its spaces */
} Row;
static Row rows[MAX_ROWS];
static int row_count;
static int select_row = -1;           /* highlight this row when it comes back */

static int add_row(const char *label, unsigned like_mode, unsigned after_mode, TmstMenuChosen chosen) {
    if (row_count >= MAX_ROWS || !label) return -1;
    size_t n = strlen(label) + 1;
    uint32_t text = psx_mod_alloc_guest_memory((uint32_t)((n + 3) & ~3u), 4);
    if (!text) return -1;
    for (size_t i = 0; i < n; i++) psx_mod_write_byte(text + (uint32_t)i, (uint8_t)label[i]);
    rows[row_count] = (Row){ text, like_mode, after_mode, chosen, -1, 0, 0, -1, {0} };
    for (size_t i = 0, k = 0; label[i] && k + 1 < sizeof rows[0].word; i++)
        if (label[i] != ' ') rows[row_count].word[k++] = label[i];
    return row_count++;
}
int tmst_menu_add_row(const char *label, unsigned like_mode, TmstMenuChosen chosen) {
    return add_row(label, like_mode, AT_END, chosen);
}
int tmst_menu_add_row_after(const char *label, unsigned like_mode, unsigned after_mode, TmstMenuChosen chosen) {
    return add_row(label, like_mode, after_mode & 0xffu, chosen);
}

/* ---- a native row taken over (tmst_menu_native_row) ---- */
static struct { int set; unsigned mode; uint32_t label, orig_label, orig_word; int applied; } native;

void tmst_menu_native_row(unsigned mode, const char *label) {
    if (!label) return;
    size_t n = strlen(label) + 1;
    uint32_t text = psx_mod_alloc_guest_memory((uint32_t)((n + 3) & ~3u), 4);
    if (!text) return;
    for (size_t i = 0; i < n; i++) psx_mod_write_byte(text + (uint32_t)i, (uint8_t)label[i]);
    native.set = 1; native.mode = mode & 0xffu; native.label = text; native.applied = 0;
}
static uint32_t list_address(void);
static uint32_t row_at(uint32_t list, unsigned i);
static unsigned count_of(uint32_t list);
/* The row as the list has it, or ours on it: its index, -1 none. */
static int native_index(uint32_t list) {
    unsigned count = count_of(list);
    for (unsigned i = 0; i < count && i < 16; i++) {
        uint32_t label = psx_mod_read_word(row_at(list, i)), word = psx_mod_read_word(row_at(list, i) + 4);
        if ((word & 0xffu) != native.mode) continue;
        if (native.applied ? label == native.label : 1) return (int)i;
    }
    return -1;
}
static void native_apply(uint32_t list) {
    if (!native.set || native.applied) return;
    int i = native_index(list);
    if (i < 0) return;
    native.orig_label = psx_mod_read_word(row_at(list, (unsigned)i));
    native.orig_word = psx_mod_read_word(row_at(list, (unsigned)i) + 4);
    psx_mod_write_word(row_at(list, (unsigned)i), native.label);
    psx_mod_write_word(row_at(list, (unsigned)i) + 4, native.orig_word & ~0xff00u);   /* no controller restriction */
    native.applied = 1;
}
static void native_restore(uint32_t list) {
    if (!native.set || !native.applied) return;
    int i = native_index(list);
    if (i >= 0) {
        psx_mod_write_word(row_at(list, (unsigned)i), native.orig_label);
        psx_mod_write_word(row_at(list, (unsigned)i) + 4, native.orig_word);
    }
    native.applied = 0;
}
int tmst_menu_native_highlighted(unsigned mode) {
    if (!native.set || !native.applied || native.mode != (mode & 0xffu) || psx_mod_read_half(SCREEN) != MENU ||
        psx_mod_read_half(PHASE) != 1) return 0;
    uint32_t list = list_address();
    unsigned sel = psx_mod_read_byte(SELECTION);
    return list && sel < count_of(list) && psx_mod_read_word(row_at(list, sel)) == native.label;
}

void tmst_menu_select_next_time(int row) { select_row = row; }
int tmst_menu_highlighted(int row) {
    return row >= 0 && row < row_count && rows[row].index >= 0 && psx_mod_read_half(SCREEN) == MENU &&
           psx_mod_read_half(PHASE) == 1 && psx_mod_read_byte(SELECTION) == (unsigned)rows[row].index;
}

/* The working list the native menu reads: from the menu's own LUI and
 * ORI/ADDIU (as the game has them, or as the Expanded patched them). 0 when
 * the menu code is not the one we know. */
static uint32_t list_address(void) {
    if (psx_mod_read_word(MENU_ENTRY) != 0x27bdffa0u) return 0;
    uint32_t hi = psx_mod_read_word(LIST_HI), lo = psx_mod_read_word(LIST_LO);
    if ((hi & 0xffff0000u) != 0x3c020000u) return 0;
    uint32_t base = (hi & 0xffffu) << 16;
    if ((lo & 0xffff0000u) == 0x34560000u) return base | (lo & 0xffffu);                  /* ori s6, v0 */
    if ((lo & 0xffff0000u) == 0x24560000u) return base + (uint32_t)(int32_t)(int16_t)(lo & 0xffffu); /* addiu */
    return 0;
}

static uint32_t row_at(uint32_t list, unsigned i) { return list + ROWS_AT + i * ROW_BYTES; }
static unsigned count_of(uint32_t list) { return psx_mod_read_word(list + 4); }
static int ours(uint32_t label) {
    for (int r = 0; r < row_count; r++) if (rows[r].label == label) return r;
    return -1;
}

/* Our rows out of the list, wherever they are. Returns the highlight `sel`
 * counted in the list left (on a row of ours: the row after it). */
static unsigned take_out(uint32_t list, unsigned sel) {
    unsigned count = count_of(list), n = 0, out = sel;
    if (!count || count > MAX_COUNT) return sel;
    for (unsigned i = 0; i < count; i++) {
        uint32_t label = psx_mod_read_word(row_at(list, i)), word = psx_mod_read_word(row_at(list, i) + 4);
        if (ours(label) >= 0) { if (i < sel) out--; continue; }
        if (n != i) { psx_mod_write_word(row_at(list, n), label); psx_mod_write_word(row_at(list, n) + 4, word); }
        n++;
    }
    psx_mod_write_word(list + 4, n);
    return n && out >= n ? n - 1 : out;
}

/* Our rows back, each after its native row (or after the last row), where
 * the room is: never over a label the list's own memory holds after the
 * rows. Returns the highlight `sel` (counted without ours) counted with them. */
static unsigned put_back(uint32_t list, unsigned sel) {
    unsigned count = count_of(list);
    if (!count || count > MAX_COUNT) return sel;
    uint32_t limit = list + 256;                      /* never past what the menu itself had */
    uint32_t label[MAX_COUNT], word[MAX_COUNT], look[MAX_ROWS] = {0};
    for (unsigned i = 0; i < count; i++) {
        label[i] = psx_mod_read_word(row_at(list, i)); word[i] = psx_mod_read_word(row_at(list, i) + 4);
        if (label[i] > row_at(list, count) && label[i] < limit) limit = label[i];
    }
    for (int r = 0; r < row_count; r++) {
        rows[r].index = -1;
        for (unsigned i = 0; i < count && !look[r]; i++) if ((word[i] & 0xffu) == rows[r].like_mode) look[r] = word[i];
    }
    uint32_t out_label[MAX_COUNT], out_word[MAX_COUNT];
    unsigned n = 0, out_sel = sel, room = (limit - row_at(list, 0)) / ROW_BYTES;
    int placed[MAX_ROWS] = {0};
    room = room > MAX_COUNT ? MAX_COUNT : room;
    for (unsigned i = 0; i <= count; i++) {
        if (i < count) {
            if (i == sel) out_sel = n;
            out_label[n] = label[i]; out_word[n++] = word[i];
        }
        for (int r = 0; r < row_count; r++) {
            if (placed[r] || !look[r]) continue;
            if (i < count ? rows[r].after_mode != (word[i] & 0xffu) : 0) continue;
            placed[r] = 1;
            if (n + (count - (i < count ? i + 1 : count)) >= room) continue;   /* the native rows still to come keep theirs */
            rows[r].index = (int)n;
            out_label[n] = rows[r].label; out_word[n++] = look[r];
        }
    }
    for (unsigned i = 0; i < n; i++) {
        psx_mod_write_word(row_at(list, i), out_label[i]); psx_mod_write_word(row_at(list, i) + 4, out_word[i]);
    }
    psx_mod_write_word(list + 4, n);
    return out_sel;
}

extern void __tmst_chain_func_80028CD4(CPUState *cpu);
#ifdef TMST_BRIDGE_func_80028CD4
extern void __real_func_80028CD4(CPUState *cpu);
void __tmst_chain_func_80028CD4(CPUState *cpu) { __real_func_80028CD4(cpu); }
#endif

/* The native row whose mode a row of ours dispatches (not one of ours). */
static int like_index(uint32_t list, unsigned like_mode) {
    unsigned count = count_of(list);
    for (unsigned i = 0; i < count && i < MAX_COUNT; i++)
        if ((psx_mod_read_word(row_at(list, i) + 4) & 0xffu) == like_mode && ours(psx_mod_read_word(row_at(list, i))) < 0)
            return (int)i;
    return -1;
}

/* The screen-dispatch arm that hands off to the menu overlay (the Expanded's
 * hook point too). Our rows are out of the list while the Expanded's code
 * runs (it always sees its own list, its limit of eleven rows included, and
 * the highlight counted in it) and back after it for the native menu, which
 * draws them in every phase it draws (1 waiting, 2 the choice, 3 the
 * attract). A row of ours carries the mode word of its native row, so when
 * it is confirmed the game dispatches that mode itself, the highlight
 * staying on our row. */
static int told;                      /* the confirmation reported */
void __wrap_func_80028CD4(CPUState *cpu) {
    uint32_t list = (row_count || native.set) && psx_mod_game_started() && psx_mod_read_half(SCREEN) == MENU ? list_address() : 0;
    int was_selected = -1;
    unsigned phase = list ? psx_mod_read_half(PHASE) : 0;
    if (list) native_restore(list);           /* the Expanded's code sees the list as it is */
    if (list) {
        unsigned sel = psx_mod_read_byte(SELECTION);
        if (sel < count_of(list) && sel < MAX_COUNT) was_selected = ours(psx_mod_read_word(row_at(list, sel)));
        psx_mod_write_byte(SELECTION, (uint8_t)take_out(list, sel));
        for (int r = 0; r < row_count; r++) rows[r].index = -1;
        if (was_selected >= 0 && phase == 2 && !told) {
            told = 1;
            tmst_log("menu: row %d chosen, the game starts its mode", was_selected);
            if (rows[was_selected].chosen) rows[was_selected].chosen();
        }
        if (phase != 2) told = 0;
    }
    __tmst_chain_func_80028CD4(cpu);
    /* Only when handing off to the menu (not when Quit consumed the call). */
    if (!list || phase < 1 || phase > 3 || cpu->pc != MENU_ENTRY || cpu->gpr[31] != 0x80028cdcu) return;
    unsigned sel = put_back(list, psx_mod_read_byte(SELECTION));
    if (was_selected >= 0 && rows[was_selected].index >= 0) sel = (unsigned)rows[was_selected].index;
    else if (phase == 1 && select_row >= 0 && select_row < row_count && rows[select_row].index >= 0) {
        sel = (unsigned)rows[select_row].index;
        select_row = -1;
    }
    psx_mod_write_byte(SELECTION, (uint8_t)sel);
    native_apply(list);
}

/* Off the menu, the list must hold the native rows only, the game keeping
 * the highlighted index across modes: our rows out (a highlighted one gives
 * the highlight to the native row of its mode), and the Expanded's QUIT GAME
 * row (mode 0xFF) out too if it is still there. The Expanded takes it out at
 * its VBlank once the menu's last frame is drawn, but not while the list is
 * longer than its eleven rows, as it is with ours when that VBlank comes
 * first; taken out here the same way, its VBlank then finds nothing to do. */
int tmst_menu_distance(int row) {
    if (row < 0 || row >= row_count || rows[row].index < 0 || psx_mod_read_half(SCREEN) != MENU) return -1;
    uint32_t list = list_address();
    unsigned count = list ? count_of(list) : 0, sel = psx_mod_read_byte(SELECTION);
    if (!count || count > MAX_COUNT) return -1;
    int d = rows[row].index - (int)sel;
    if (d < 0) d = -d;
    return d * 2 > (int)count ? (int)count - d : d;
}

/* ---- a row's own colour ---------------------------------------------------------
 * The rows are in the title font (14 x 24 sprites from the page at x 832,
 * every 13 pixels), each mode's palette its colour, darker away from the
 * highlighted row. A coloured row's letters get the font's white palette
 * (352, 505), modulated (128 = as is): its white is about WHITE_LEVEL (measured),
 * so the colour asked for is reached at rgb * 128 / WHITE_LEVEL. The list is
 * the game's: our visitor colours the letters as the row's last one comes,
 * for that drawing only. A row: its letters on one line in one palette (as
 * the list turns, two rows can share a line). After a choice the menu is
 * drawn on for some frames on the next screens (7 then 1, OPTION MODE's 2,
 * measured): the same colour, as bright as on the menu's last frame, on any
 * screen while the font's letters are still drawn (the menu at 30 frames a
 * second: QUIET VBlanks without them end it), AFTER_MAX at most. Up to
 * LETTERS of them a frame (the menu draws 64 to 72). */
enum { FONT_PAGE = 832 / 64, FONT_U = 172, GLYPH_W = 14, GLYPH_H = 24, WHITE_CLUT = (505 << 6) | (352 / 16),
       QUIET = 8, AFTER_MAX = 240, LETTERS = 256, WHITE_LEVEL = 236 };
static const unsigned char fade[] = {255, 200, 150, 100, 60};        /* by rows from the highlighted one */
static struct { unsigned frame; int n; struct { int x, y; char c; uint32_t at, clut; } l[LETTERS]; } letters;
static unsigned frames, page;             /* VBlanks seen; the texture page the walk has reached */
static int coloured_rows, menu_drawn;     /* the menu drawn now (its screen, or right after it) */
static unsigned left_at;                  /* the VBlank the menu's screen was left (0: on it) */

void tmst_menu_set_colour(int row, uint32_t rgb) {
    if (row < 0 || row >= row_count) return;
    if (!rows[row].coloured) coloured_rows++;
    rows[row].coloured = 1; rows[row].rgb = rgb & 0xffffffu;
}

/* The letters of the row the letter `last` is on (its line, its palette),
 * left to right: their indices in order[], the word they spell. */
static int row_letters(int last, int *order, char *word) {
    int n = 0;
    for (int i = 0; i < letters.n; i++)
        if (letters.l[i].y == letters.l[last].y && letters.l[i].clut == letters.l[last].clut) order[n++] = i;
    for (int i = 1; i < n; i++)
        for (int j = i; j > 0 && letters.l[order[j]].x < letters.l[order[j - 1]].x; j--) { int t = order[j]; order[j] = order[j - 1]; order[j - 1] = t; }
    for (int i = 0; i < n; i++) word[i] = letters.l[order[i]].c;
    word[n] = 0;
    return n;
}

static void colour_row(int last) {
    int order[LETTERS];
    char word[LETTERS + 1];
    int n = row_letters(last, order, word);
    for (int r = 0; r < row_count; r++) {
        if (!rows[r].coloured || strcmp(word, rows[r].word)) continue;
        int d = rows[r].distance, k = d < 0 || d >= (int)sizeof fade ? fade[sizeof fade - 1] : fade[d];
        uint32_t tint = 0;
        for (int c = 0; c < 3; c++) {
            unsigned v = (rows[r].rgb >> (8 * c) & 0xffu) * 128u / WHITE_LEVEL * (unsigned)k / 255u;
            tint |= (v > 255 ? 255u : v) << (8 * (2 - c));     /* 0xRRGGBB -> the GPU's 0xBBGGRR */
        }
        for (int i = 0; i < n; i++) {
            uint32_t at = letters.l[order[i]].at;
            tmst_overlay_patch_word(at, 0x64000000u | tint);                 /* modulated */
            tmst_overlay_patch_word(at + 8, (psx_mod_read_word(at + 8) & 0xffffu) | (uint32_t)WHITE_CLUT << 16);
        }
        return;
    }
}
static void letter(uint32_t at, unsigned op, int x, int y, int w, int h) {
    if ((op & 0xfc) != 0x64 || w != GLYPH_W || h != GLYPH_H || (page & 0x1f) != FONT_PAGE) return;
    if (letters.frame != frames) { letters.frame = frames; letters.n = 0; }
    if (letters.n >= LETTERS) return;
    uint32_t uv = psx_mod_read_word(at + 8);
    int u = (int)(uv & 255) - FONT_U, v = (int)(uv >> 8 & 255), i = u / GLYPH_W + v / GLYPH_H * 6;
    char c = u < 0 || u % GLYPH_W ? '?' : i < 9 ? (char)('1' + i) : i < 35 ? (char)('A' + i - 9) : '!';
    letters.l[letters.n].x = x; letters.l[letters.n].y = y; letters.l[letters.n].c = c;
    letters.l[letters.n].at = at; letters.l[letters.n].clut = uv >> 16;
    letters.n++;
    colour_row(letters.n - 1);
}
/* Each primitive of a node: the texture page followed (E1, a textured
 * polygon's), the sprites handed to letter(). */
static void menu_visitor(uint32_t node) {
    if (!menu_drawn) return;
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
            if ((cmd & 0x18) == 0 && (cmd & 4) && i + size - 1 <= words) {
                uint32_t xy = psx_mod_read_word(a + 4), wh = psx_mod_read_word(a + 12);
                letter(a, cmd, (int16_t)(xy & 0xffff), (int16_t)(xy >> 16), (int)(wh & 0x3ff), (int)(wh >> 16 & 0x1ff));
            }
        }
        i += size;
    }
}
static void colour_vblank(void) {
    if (!coloured_rows || !psx_mod_game_started()) return;
    static int visiting;
    if (!visiting) { visiting = 1; tmst_overlay_set_visitor(menu_visitor); }
    frames++;
    unsigned screen = psx_mod_read_half(SCREEN);
    if (screen == MENU) {
        for (int r = 0; r < row_count; r++) rows[r].distance = tmst_menu_distance(r);
        menu_drawn = 1; left_at = 0;
        tmst_overlay_show_prims();
        return;
    }
    if (!menu_drawn) return;
    if (!left_at) left_at = frames;
    unsigned last = letters.frame > left_at ? letters.frame : left_at;
    if (frames - last <= QUIET && frames - left_at < AFTER_MAX) {
        tmst_overlay_show_prims();
    } else {
        menu_drawn = 0;
        if (!tmst_overlay_visible()) tmst_overlay_hide();   /* another mod's picture stays */
    }
}

enum { HOST_QUIT_MODE = 0xff };
void tmst_menu_vblank(void) {
    colour_vblank();
    if (!row_count || !psx_mod_game_started() || psx_mod_read_half(SCREEN) == MENU) return;
    uint32_t list = list_address();
    if (!list) return;
    unsigned count = count_of(list), sel = psx_mod_read_byte(SELECTION);
    if (!count || count > MAX_COUNT) return;
    int like = -1, any = 0;
    for (unsigned i = 0; i < count; i++) {
        int r = ours(psx_mod_read_word(row_at(list, i)));
        if (r < 0) continue;
        any = 1;
        if (sel == i) like = (int)rows[r].like_mode;
    }
    for (int r = 0; r < row_count; r++) rows[r].index = -1;
    if (!any) return;                              /* ours were not in: nothing of ours to undo */
    sel = take_out(list, sel);
    unsigned n = count_of(list);
    for (unsigned i = 0; i < n; i++) {
        if ((psx_mod_read_word(row_at(list, i) + 4) & 0xffu) != HOST_QUIT_MODE) continue;
        for (unsigned j = i; j + 1 < n; j++) {
            psx_mod_write_word(row_at(list, j), psx_mod_read_word(row_at(list, j + 1)));
            psx_mod_write_word(row_at(list, j) + 4, psx_mod_read_word(row_at(list, j + 1) + 4));
        }
        n--;
        if (like < 0 && sel >= i) sel = sel == i ? 0 : sel - 1;
        break;
    }
    psx_mod_write_word(list + 4, n);
    if (like >= 0) { int to = like_index(list, (unsigned)like); sel = to >= 0 ? (unsigned)to : 0; }
    psx_mod_write_byte(SELECTION, (uint8_t)sel);
}
