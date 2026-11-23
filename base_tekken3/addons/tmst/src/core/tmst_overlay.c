/* TMST core: our drawing over the game (tmst_overlay.h).
 *
 * Where the picture is: the upper VRAM half from x 0; a picture up to 256
 * rows tall at y 768..1023, a taller one from y 512 (a selector's, with
 * room for the TTT1 roster's HD faces: narrowed, in two bands around them,
 * see bands()). Texture pages of 256 x 256
 * 15-bit texels: one sprite or quad per 256 x 256 tile, each naming its page
 * with the upper-half bit 11. What the area held is kept before our upload
 * and given back on hide (others' pixels that the stage engine does not
 * report live there too); while the stage engine says the area is its own,
 * the picture moves to a place it leaves free (x in steps of a page, the
 * other row of pages), or, finding none, is not drawn, unless it borrows the
 * area (TMST_OVERLAY_BORROW: always at its first place).
 *
 * When: the runtime calls gpu_ws_prepass_linked_list() at the start of every
 * linked-list GPU DMA, before the walk (psxrecomp/runtime/src/dma.c). Our
 * wrapper walks each list once, following the drawing mode the GPU will have
 * (E1, and textured polygons' texture page), lets the mod's visitor see the
 * game's nodes, and, on the frame's first long list (the ordering table,
 * hundreds of nodes), links our chain after its last node. Our chain ends the
 * list (0xFFFFFF), so a list that is not cleared never loops; its last word
 * puts back the drawing mode the list had before it, so the game's later
 * primitives (and the upper-half sprite page the runtime keeps) are theirs
 * again. Hidden, the chain goes out of any list it is found in.
 *
 * The chain lives in GPU-DMA memory named as a trusted upper-half source
 * (psx_mod_gpu_high_vram_source), the only packets the runtime lets sample
 * the upper half. */
#include "tmst_overlay.h"
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_ui_font.h"
#include "mod_plugins.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum { TILE = 256, BUF_WORDS = 8192, BUF_BYTES = BUF_WORDS * 4, NODE_MAX = 255,
       LONG_LIST = 256, SHORT_LIST = 32, MAX_WALK = 200000 };
/* A mod's screen whose whole frame is one short list (the main menu's: 159
 * nodes, between two of one node): linked into lists of SHORT_LIST nodes too. */
static int short_lists;
void tmst_overlay_short_lists(int on) { short_lists = on != 0; }
static int cut_tail;
void tmst_overlay_cut_tail(int on) { cut_tail = on != 0; }

static uint32_t packet;                 /* guest address of our chain's first node */
static uint32_t backdrop_node;          /* our node linked after the ordering table's first entry */
static int backdrop_on;
static uint32_t backdrop_bgr;
static int ready, failed;
static int shown, prims_shown, sx, sy, sw, sh, pw, ph, vx, vy;
static unsigned show_flags;
static uint16_t pixels[TMST_OVERLAY_MAX_W * TMST_OVERLAY_MAX_H];
static int dirty;
static int yielded;                     /* TMST_OVERLAY_YIELD: the stage engine took the area */
static unsigned vram_generation;        /* the stage engine's, at our upload */
static uint16_t kept[TMST_OVERLAY_MAX_W * TMST_OVERLAY_MAX_H];
/* Where our picture's pixels are now (uploaded there, its place this
 * frame): one rectangle per band (a split picture's two), held_n 0 when
 * none; held_vx.. the place they were uploaded for. See
 * __wrap_psx_mod_gpu_high_vram_upload. */
static struct { int x, y, w, h; } held_r[2];
static int held_n, held_vx, held_vy, held_pw, held_ph;
static int own_upload;
/* Every upload of ours into the upper half; TMST_OVERLAY_VRAM_LOG=1 logs
 * each with the game's screen and the stage engine's generation. */
static void vram_up(const char *what, int x, int y, int w, int h, const uint16_t *px) {
    static int logging = -1; static unsigned n; n++;
    if (logging < 0) { const char *e = getenv("TMST_OVERLAY_VRAM_LOG"); logging = e && *e == '1'; }
    if (logging) tmst_log("overlay: upload %s %d,%d %dx%d screen %u gen %u (%u)", what, x, y, w, h,
                          psx_mod_read_word(0x800ae204), tmst_compat_high_vram_generation(), n);
    own_upload = 1;
    psx_mod_gpu_high_vram_upload(x, y, w, h, px);
    own_upload = 0;
}

/* The TTT1 roster's HD tiles and portraits (Expanded 1.5.0) have fixed
 * places in the upper half, inside our full-screen picture's (854 x 480
 * from y 512: the whole half, no other place for it). On the selector and
 * the grids the roster writes them again each frame they differ, over our
 * picture: its top rows showed the HD faces at the picture's scale (the
 * Jukebox's and Tournament Mode's grids, the bracket). While the picture
 * holds a place, such an upload is refused: the roster then finds its tile
 * missing and draws the cell from its own 32 x 58 tile
 * (psx_mod_gpu_sprite_hd_clear), the portrait from the 126 x 252 one, until
 * the picture is gone and it writes them again. Others' uploads pass.
 * A split picture's bands are held too (TMST_OVERLAY_ROOM_FOR_HD): on
 * 1.5.1 they lie clear of every tile (split_151 below) but in the big
 * portraits' copies, which the roster may write again from an earlier
 * screen (the Team Battle grid draws no big portrait: none lost). */
int __real_psx_mod_gpu_high_vram_upload(int x, int y, int w, int h, const uint16_t *pixels);
int __wrap_psx_mod_gpu_high_vram_upload(int x, int y, int w, int h, const uint16_t *pixels) {
    if (!own_upload && held_n && tmst_compat_roster_hd_write(x, y, w, h))
        for (int i = 0; i < held_n; i++)
            if (x < held_r[i].x + held_r[i].w && held_r[i].x < x + w && y < held_r[i].y + held_r[i].h && held_r[i].y < y + h) {
                static int said;
                if (!said) { said = 1; tmst_log("overlay: the TTT1 roster's HD faces stay out of the picture's place (%d,%d %dx%d)", x, y, w, h); }
                return 0;
            }
    return __real_psx_mod_gpu_high_vram_upload(x, y, w, h, pixels);
}
/* What the area held before our picture: one rectangle per band (bands
 * below), one after the other in `kept`; kept_w nonzero while any is kept. */
static int kept_w, kept_n;
static struct { int x, y, w, h; } kept_r[2];
static unsigned frame, linked_frame = ~0u, uploaded_frame;
enum { MAX_CLIENTS = 6 };
static TmstPrimsBuilder builders[MAX_CLIENTS];
static TmstNodeVisitor visitors[MAX_CLIENTS];
static int builder_count, visitor_count;
enum { RANK_BYTES = 16384, RANKS = 2 };
static struct {                         /* tmst_overlay_set_rank_prims */
    TmstPrimsBuilder builder; int target; uint32_t packet;
    uint32_t entry, e1;                 /* this walk: the entry's node, the drawing mode there */
} ranks[RANKS];
/* The drawing mode as the GPU will have it: E1, which textured polygons set
 * too (their texture page word: bits 0-8 and 11), the drawing offset (E5)
 * and the mask setting (E6), followed through every list drawn (the drawing
 * environment comes in a short list of its own). */
static uint32_t gpu_e1 = 0xE1000400u, gpu_e5 = 0xE5000000u, gpu_e6 = 0xE6000000u;
static uint32_t gpu_e3 = 0xE3000000u, gpu_e4 = 0xE4000000u | 479u << 10 | 367u;   /* the drawing area */

void tmst_overlay_set_prims(TmstPrimsBuilder b) {
    for (int i = 0; i < builder_count; i++) if (builders[i] == b) return;
    if (b && builder_count < MAX_CLIENTS) builders[builder_count++] = b;
}
void tmst_overlay_set_visitor(TmstNodeVisitor v) {
    for (int i = 0; i < visitor_count; i++) if (visitors[i] == v) return;
    if (v && visitor_count < MAX_CLIENTS) visitors[visitor_count++] = v;
}

/* At the mod's activation (start-up): the OpenGL renderer builds its upper-
 * half sampling only when the half exists before its context does. */
int tmst_overlay_prepare(void) { return psx_mod_gpu_high_vram_enable(); }

static int setup(void) {
    if (ready || failed) return ready;
    if (!psx_mod_gpu_high_vram_enable()) { failed = 1; tmst_log("overlay: no upper VRAM half"); return 0; }
    packet = psx_mod_alloc_gpu_dma_memory(BUF_BYTES, 16);
    if (!packet || !psx_mod_gpu_high_vram_source(packet, BUF_BYTES)) {
        failed = 1; tmst_log("overlay: no packet memory"); return 0;
    }
    psx_mod_write_word(packet, 0x00FFFFFFu);    /* empty, end of list */
    backdrop_node = psx_mod_alloc_gpu_dma_memory(16, 16);
    ready = 1;
    return 1;
}

static uint16_t texel(uint32_t argb) {
    unsigned a = argb >> 24;
    if (a < 128) return 0;                      /* 0: see-through */
    unsigned r = (argb >> 19) & 31, g = (argb >> 11) & 31, b = (argb >> 3) & 31;
    uint16_t c = (uint16_t)(b << 10 | g << 5 | r);
    if (a < 255) return (uint16_t)(0x8000u | c);   /* STP: half see-through */
    return c ? c : 0x0421u;                     /* opaque; black must not read as see-through */
}

static void give_back(void) {
    if (!kept_w) return;
    const uint16_t *at = kept;
    for (int i = 0; i < kept_n; i++) {
        vram_up("give back", kept_r[i].x, kept_r[i].y, kept_r[i].w, kept_r[i].h, at);
        at += (size_t)kept_r[i].w * (size_t)kept_r[i].h;
    }
    kept_w = 0; kept_n = 0;
}
/* Asked from the VBlank (a hide, a move): done at the next list instead.
 * The OpenGL renderer draws textured primitives in batches, and one may
 * still be waiting with our picture in it; the half's pixels it samples
 * are copied when it is drawn. Given back at once, the frame showed what
 * the area held before (the last fight's stage textures: the noise over
 * the whole screen between the tournament's match screen and the loading
 * screen). Not given back if the stage engine wrote in between: theirs. */
static int give_back_due, atlas_back_due, sheet_back_due;
static unsigned give_back_generation;
static void give_back_later(void) {
    if (!kept_w || give_back_due) return;
    give_back_due = 1; give_back_generation = tmst_compat_high_vram_generation();
}
static void atlas_give_back(void);
static void sheet_give_back(void);
static void give_back_due_now(void) {
    if (give_back_due) {
        give_back_due = 0;
        if (tmst_compat_high_vram_generation() == give_back_generation) give_back();
        else kept_w = 0;
    }
    if (atlas_back_due) { atlas_back_due = 0; atlas_give_back(); }
    if (sheet_back_due) { sheet_back_due = 0; sheet_give_back(); }
}

/* The picture's bands: rows row0.. (rows of them) at x, y of the upper
 * half. One band (the whole picture at vx, vy) but for a split picture
 * (TMST_OVERLAY_ROOM_FOR_HD with the roster's HD faces): its first page row
 * of texels at SPLIT0 (x 384..959, y 768..1023: right of the big portraits,
 * x 0..359), the rest at SPLIT1 (x 256..831 from y 628: under the HD tiles'
 * first row, right of their second, within one page row: at most 140 rows),
 * x 832..1023 there left to the font's sheet.
 * The Expanded 1.5.1 puts the stock fighters' HD tiles there (the whole
 * second row, and x 384..959 at y 768..883): the picture is narrowed to 384
 * and goes to x 0..383 at y 768..1023 (the big portraits' copies, which the
 * Team Battle grid, the Jukebox's and Tournament Mode's, never shows) and
 * x 384..767 from y 884 (under the stock tiles' row); x 768..1023 there left
 * to the font's sheet. Each band within one page row. */
typedef struct { int room_w, x0, y0, x1, y1, rows1; } SplitLayout;
static const SplitLayout split_150 = {TMST_OVERLAY_ROOM_W, 384, 768, 256, 628, 140},
                         split_151 = {TMST_OVERLAY_ROOM_W_151, 0, 768, 384, 884, 140};
enum { SPLIT_ROWS_MAX = 140 };
static const SplitLayout *lay = &split_150;
static int split;
static int bands(int b[2][4]) {                 /* {x, y, row0, rows} each; returns how many */
    if (!split) { b[0][0] = vx; b[0][1] = vy; b[0][2] = 0; b[0][3] = ph; return 1; }
    b[0][0] = lay->x0; b[0][1] = lay->y0; b[0][2] = 0; b[0][3] = ph < TILE ? ph : TILE;
    b[1][0] = lay->x1; b[1][1] = lay->y1; b[1][2] = TILE; b[1][3] = ph - TILE;
    return ph > TILE ? 2 : 1;
}
/* Whether a split picture's bands meet the rectangle (0 for any other). */
static int split_overlaps(int x, int y, int w, int h) {
    if (!split || !shown) return 0;
    int b[2][4], n = bands(b);
    for (int i = 0; i < n; i++)
        if (x < b[i][0] + pw && b[i][0] < x + w && y < b[i][1] + b[i][3] && b[i][1] < y + h) return 1;
    return 0;
}
static void keep(void) {
    int b[2][4], n = bands(b);
    uint16_t *at = kept;
    kept_w = 0; kept_n = 0;
    for (int i = 0; i < n; i++) {
        if (!tmst_compat_high_vram_read(b[i][0], b[i][1], pw, b[i][3], at)) { kept_n = 0; return; }
        kept_r[i].x = b[i][0]; kept_r[i].y = b[i][1]; kept_r[i].w = pw; kept_r[i].h = b[i][3];
        at += (size_t)pw * (size_t)b[i][3]; kept_n = i + 1;
    }
    kept_w = kept_n > 0;
}
static void upload_picture(void) {
    int b[2][4], n = bands(b);
    for (int i = 0; i < n; i++) vram_up("picture", b[i][0], b[i][1], pw, b[i][3], pixels + (size_t)b[i][2] * (size_t)pw);
}

/* Where a borrowed picture goes: the first place (rows of pages from the
 * half's top for a tall one, the lower row first for one up to a page tall,
 * x in steps of a page) clear of TTT Hitspark's and the wolf's cells, which
 * are written again while their effects play (the stage engine's own pixels
 * are borrowed); none clear: the first place, contested. */
static int borrow_contested;
static void borrow_place(int w, int h, int *bx, int *by) {
    static const int tall[1] = {512}, low[2] = {768, 512};
    const int *rows = h <= TILE ? low : tall;
    int n = h <= TILE ? 2 : 1;
    for (int r = 0; r < n; r++)
        for (int x = 0; x + w <= 1024; x += 64)
            if (!tmst_compat_high_vram_effects(x, rows[r], w, h)) { *bx = x; *by = rows[r]; borrow_contested = 0; return; }
    *bx = 0; *by = rows[0]; borrow_contested = 1;
}
int tmst_overlay_picture_contested(void) { return shown && (show_flags & TMST_OVERLAY_BORROW) && borrow_contested; }

int tmst_overlay_show_scaled(const uint32_t *argb, int w, int h, int x, int y, int dw, int dh, unsigned flags) {
    if (!argb || w <= 0 || h <= 0 || dw <= 0 || dh <= 0 || w > TMST_OVERLAY_MAX_W || h > TMST_OVERLAY_MAX_H || !setup())
        return 0;
    /* Room for the roster's HD faces: narrowed (each texel the average of
     * the source's under it, colours weighted by their alpha) and split. */
    const SplitLayout *want_lay = tmst_compat_stock_hd() ? &split_151 : &split_150;
    int want_split = (flags & TMST_OVERLAY_ROOM_FOR_HD) && h > TILE && h <= TILE + want_lay->rows1 &&
                     tmst_compat_roster_hd_present();
    int rw = want_lay->room_w;
    if (want_split && w > rw) {
        static uint32_t narrow[TMST_OVERLAY_ROOM_W * (TILE + SPLIT_ROWS_MAX)];
        for (int col = 0; col < rw; col++) {
            int a0 = col * w * 256 / rw, a1 = (col + 1) * w * 256 / rw;  /* 1/256 texels */
            for (int row = 0; row < h; row++) {
                const uint32_t *src = argb + (size_t)row * (size_t)w;
                uint32_t sa = 0, sc[3] = {0, 0, 0};
                for (int sxi = a0 >> 8; sxi <= (a1 - 1) >> 8 && sxi < w; sxi++) {
                    int lo = sxi * 256 > a0 ? sxi * 256 : a0, hi = sxi * 256 + 256 < a1 ? sxi * 256 + 256 : a1;
                    uint32_t c = src[sxi], wa = (c >> 24) * (uint32_t)(hi - lo);
                    sa += wa;
                    for (int k = 0; k < 3; k++) sc[k] += (c >> (16 - 8 * k) & 255) * wa;
                }
                uint32_t span = (uint32_t)(a1 - a0), out = 0;
                if (sa) {
                    out = (sa / span) << 24;
                    for (int k = 0; k < 3; k++) out |= (sc[k] / sa) << (16 - 8 * k);
                }
                narrow[row * rw + col] = out;
            }
        }
        argb = narrow; w = rw;
    }
    int changed = w != pw || h != ph || want_split != split || (want_split && want_lay != lay);
    if (changed && kept_w) give_back_later();  /* a new size: a new area, at its first place */
    split = want_split;
    if (split) {
        lay = want_lay;
        vx = lay->x0; vy = lay->y0;
    } else if (flags & TMST_OVERLAY_BORROW) {
        int bx, by;
        borrow_place(w, h, &bx, &by);
        if (kept_w && (vx != bx || vy != by)) { give_back_later(); changed = 1; }
        vx = bx; vy = by;
    } else if (changed) {
        vx = 0; vy = h <= TILE ? 1024 - TILE : 512;
    }
    for (int row = 0; row < h; row++) {
        uint16_t *line = pixels + row * w;
        for (int col = 0; col < w; col++) {
            uint16_t t = texel(argb[row * w + col]);
            if (line[col] != t) { line[col] = t; changed = 1; }
        }
    }
    pw = w; ph = h; sx = x; sy = y; sw = dw; sh = dh; shown = 1; show_flags = flags;
    if (changed) dirty = 1;
    return 1;
}
int tmst_overlay_show_ex(const uint32_t *argb, int w, int h, int x, int y, unsigned flags) {
    return tmst_overlay_show_scaled(argb, w, h, x, y, w, h, flags);
}
int tmst_overlay_show(const uint32_t *argb, int w, int h, int x, int y) {
    return tmst_overlay_show_scaled(argb, w, h, x, y, w, h, 0);
}

void tmst_overlay_move(int x, int y) { sx = x; sy = y; }
void tmst_overlay_hide_picture(void) {
    yielded = 0;
    if (!shown) return;
    shown = 0;
    give_back_later();
    dirty = 1;
}
void tmst_overlay_set_rank_prims(int rank, TmstPrimsBuilder b) {
    if (!b) return;
    int i = 0;
    while (i < RANKS && ranks[i].builder != b) i++;
    if (i == RANKS) {                                   /* a new one: a free slot */
        if (rank < 0 || rank >= 1024) return;
        for (i = 0; i < RANKS && ranks[i].builder; i++) {}
        if (i == RANKS) return;
        if (!ranks[i].packet && setup()) ranks[i].packet = psx_mod_alloc_gpu_dma_memory(RANK_BYTES, 16);
        if (!ranks[i].packet) return;
    }
    if (rank < 0 || rank >= 1024) { ranks[i].builder = NULL; return; }
    ranks[i].builder = b; ranks[i].target = rank;
}
/* The area goes back to what it held: our pixels are no longer there, so
 * the next show uploads them again. */
static void atlas_give_back(void);
void tmst_overlay_show_prims(void) { if (setup()) prims_shown = 1; }
void tmst_overlay_backdrop(int on, uint32_t bgr) { backdrop_on = on; backdrop_bgr = bgr & 0xFFFFFFu; }
static void sheet_give_back(void);
void tmst_overlay_hide(void) {
    shown = 0; prims_shown = 0; yielded = 0; give_back_later();
    atlas_back_due = sheet_back_due = 1;           /* (each gives back only what it holds) */
    dirty = 1;
}
int tmst_overlay_visible(void) { return shown; }
static int drawing(void) { return shown || prims_shown; }

/* ---- the chain's builder ---- */
struct TmstPrims { uint32_t head, at, end; unsigned words; };
static void prims_begin_at(TmstPrims *k, uint32_t buf, uint32_t bytes) { k->head = buf; k->at = buf + 4; k->end = buf + bytes; k->words = 0; }
static void prims_begin(TmstPrims *k) { prims_begin_at(k, packet, BUF_BYTES); }
void tmst_prim_room(TmstPrims *k, unsigned n) {
    if (k->words + n <= NODE_MAX || k->at + 4 + n * 4 > k->end) return;
    psx_mod_write_word(k->head, (uint32_t)k->words << 24 | (k->at & 0x00FFFFFFu));
    k->head = k->at; k->at += 4; k->words = 0;
}
void tmst_prim_word(TmstPrims *k, uint32_t w) {
    if (k->at + 4 >= k->end || k->words >= NODE_MAX) return;   /* the last word stays free */
    psx_mod_write_word(k->at, w); k->at += 4; k->words++;
}
static void prims_finish_to(TmstPrims *k, uint32_t next) {
    psx_mod_write_word(k->head, (uint32_t)k->words << 24 | (next & 0x00FFFFFFu));
}
static void prims_finish(TmstPrims *k) { prims_finish_to(k, 0x00FFFFFFu); }

/* ---- the font's sheet ----
 * The UI font's SCREEN face (tmst_ui_font.h): its glyphs, each with a
 * see-through texel all round (a glyph is drawn from half a screen unit's
 * texel off, see below), on shelves (the tallest glyphs first, each shelf as
 * tall as its first) on texture pages of 256 (u up to 255), 4-bit; the last
 * row holds the palette: 16 levels of cover, the faint ones see-through,
 * the middle ones white with the STP bit (half see-through, B/2 + F/2: a
 * quad of them over the screen is half itself, half the screen), the rest
 * white to grey (opaque). Placed from the half's bottom right corner, where
 * the stage packs leave room (tools/t2_remake_stages.py keeps 192 x 64 there
 * for it where it must: FONT_W, FONT_H); within one row of pages. */
enum { ATLAS_PAGES = 3, ATLAS_W = ATLAS_PAGES * 64, ATLAS_H = 61, ATLAS_GLYPHS = 128 };
static uint16_t atlas[ATLAS_W * ATLAS_H], atlas_kept[ATLAS_W * ATLAS_H];
static struct { uint8_t page, u, v; } cell[ATLAS_GLYPHS];   /* by the face's glyph index */
static int atlas_built, atlas_x = -1, atlas_y, atlas_has_kept, atlas_dirty = 1, text_wanted, text_last, text_ready, atlas_said;
static unsigned atlas_generation, atlas_frame;
#define FACE (&tmst_ui_face_screen)

static void build_atlas(void) {
    memset(atlas, 0, sizeof atlas);
    int n = FACE->count < ATLAS_GLYPHS ? FACE->count : ATLAS_GLYPHS, order[ATLAS_GLYPHS];
    for (int i = 0; i < n; i++) {                       /* by height, the tallest first */
        int j = i;
        while (j > 0 && FACE->glyphs[order[j - 1]].h < FACE->glyphs[i].h) { order[j] = order[j - 1]; j--; }
        order[j] = i;
    }
    enum { SHELVES = 16 };
    struct { int page, v, h, u; } shelf[SHELVES];
    int shelves = 0, page_h[ATLAS_PAGES] = {0};
    for (int k = 0; k < n; k++) {
        int i = order[k];
        const TmstUiGlyph *g = &FACE->glyphs[i];
        if (!g->w || !g->h) continue;
        int w = g->w + 2, h = g->h + 2, s = 0;
        while (s < shelves && (shelf[s].h < h || shelf[s].u + w > 255)) s++;
        if (s == shelves) {
            int p = 0;
            while (p < ATLAS_PAGES && page_h[p] + h > ATLAS_H - 1) p++;
            if (p == ATLAS_PAGES || shelves == SHELVES) { tmst_log("overlay: the font's sheet is full at glyph %d", i); break; }
            shelf[s].page = p; shelf[s].v = page_h[p]; shelf[s].h = h; shelf[s].u = 0;
            page_h[p] += h; shelves++;
        }
        int page = shelf[s].page, u = shelf[s].u, v = shelf[s].v;
        cell[i].page = (uint8_t)page; cell[i].u = (uint8_t)u; cell[i].v = (uint8_t)v;
        shelf[s].u += w;
        int stride = (g->w + 1) / 2;
        for (int r = 0; r < g->h; r++)
            for (int q = 0; q < g->w; q++) {
                uint8_t b = FACE->bits[g->offset + (size_t)r * stride + q / 2];
                unsigned c = q & 1 ? b & 15u : b >> 4;
                int tu = page * 256 + u + 1 + q, tv = v + 1 + r;   /* 4 texels to a halfword, the first lowest */
                atlas[tv * ATLAS_W + tu / 4] |= (uint16_t)(c << (tu % 4 * 4));
            }
    }
    uint16_t *clut = &atlas[(ATLAS_H - 1) * ATLAS_W];
    for (int v = 0; v < 16; v++) {
        unsigned grey = v < 3 ? 0u : v <= 10 ? (unsigned)(v * 62 + 7) / 15 : (unsigned)(v * 31 + 7) / 15;
        if (grey > 31) grey = 31;
        clut[v] = v < 3 ? 0 : (uint16_t)((v <= 10 ? 0x8000u : 0u) | grey << 10 | grey << 5 | grey);
    }
    atlas_built = 1;
}
static void atlas_give_back(void) {
    if (atlas_x >= 0 && atlas_has_kept) vram_up("font back", atlas_x, atlas_y, ATLAS_W, ATLAS_H, atlas_kept);
    atlas_x = -1; atlas_has_kept = 0; atlas_dirty = 1;
}
static int atlas_fits(int x, int y) {
    return (y - 512) / 256 == (y + ATLAS_H - 1 - 512) / 256 && !tmst_compat_high_vram_taken(x, y, ATLAS_W, ATLAS_H) &&
           !tmst_compat_roster_hd_area(x, y, ATLAS_W, ATLAS_H) && !split_overlaps(x, y, ATLAS_W, ATLAS_H);
}
/* This frame: the sheet in place and up to date, or 0. */
static int place_atlas(void) {
    if (!atlas_built) build_atlas();
    unsigned gen = tmst_compat_high_vram_generation();
    if (atlas_x < 0 || !atlas_fits(atlas_x, atlas_y)) {
        atlas_give_back();
        for (int y = 1024 - ATLAS_H; y >= 512 && atlas_x < 0; y -= 8)
            for (int x = 1024 - ATLAS_W; x >= 0; x -= 64)
                if (atlas_fits(x, y)) { atlas_x = x; atlas_y = y; tmst_log("overlay: the font at x %d y %d", x, y); break; }
        if (atlas_x < 0) {
            if (!atlas_said) { atlas_said = 1; tmst_log("overlay: no room for the font in the upper VRAM half"); }
            return 0;
        }
    }
    if (atlas_dirty || gen != atlas_generation || frame - atlas_frame >= 30) {
        if (!atlas_has_kept || gen != atlas_generation)
            atlas_has_kept = tmst_compat_high_vram_read(atlas_x, atlas_y, ATLAS_W, ATLAS_H, atlas_kept);
        vram_up("font", atlas_x, atlas_y, ATLAS_W, ATLAS_H, atlas);
        atlas_dirty = 0; atlas_generation = gen; atlas_frame = frame;
    }
    return 1;
}

/* Floor division by t (1 or 2), for negative numbers too. */
static int floor_div(int a, int t) { return a >= 0 ? a / t : -((-a + t - 1) / t); }

int tmst_ui_prim_width(const char *text, int scale) {
    int t = scale >= 2 ? 1 : 2;                          /* texels to a screen unit */
    return (tmst_ui_width(FACE, text) + t / 2) / t;
}

/* Each glyph a quad on whole screen units: at scale 1 a unit is two texels,
 * so a glyph that starts on an odd texel is drawn from the see-through
 * texel before it (off 1), and the quad is as many units as it then needs
 * (its last texel see-through too, where the glyph ends on an even one). */
int tmst_ui_prim_text(TmstPrims *k, int x, int y, const char *text, int scale, uint32_t tint) {
    int t = scale >= 2 ? 1 : 2;
    text_wanted = 1;
    if (!text_ready || !text) return tmst_ui_prim_width(text, scale);
    if (!atlas_built) build_atlas();
    int base = atlas_y < 768 ? 512 : 768, v_top = atlas_y - base, pen = 0, baseline = t * y + FACE->cap;
    uint32_t clut = 0x8000u | (uint32_t)(atlas_y + ATLAS_H - 1 - 512) << 6 | (uint32_t)(atlas_x / 16);
    const char *p = text;
    unsigned c;
    while ((c = tmst_ui_next_code(&p)) != 0) {
        /* the face's glyph (ASCII, the ellipsis, the icons; an accented
         * letter as its plain one), '?' for any other or one off the sheet */
        const TmstUiGlyph *g = tmst_ui_glyph(FACE, c);
        int i = (int)(g - FACE->glyphs);
        if (i >= ATLAS_GLYPHS) { g = tmst_ui_glyph(FACE, '?'); i = (int)(g - FACE->glyphs); }
        if (g->w && g->h) {
            int gx = t * x + ((pen + 32) >> 6) + g->left, gy = baseline + g->top;
            int qx = floor_div(gx, t), qy = floor_div(gy, t), offx = gx - t * qx, offy = gy - t * qy;
            int qw = (offx + g->w + t - 1) / t, qh = (offy + g->h + t - 1) / t;
            uint32_t u0 = (uint32_t)(cell[i].u + 1 - offx), u1 = u0 + (uint32_t)(t * qw);
            uint32_t v0 = (uint32_t)(v_top + cell[i].v + 1 - offy), v1 = v0 + (uint32_t)(t * qh);
            uint32_t tpage = (uint32_t)(atlas_x / 64 + cell[i].page) | (uint32_t)(base - 512) / 256u << 4 | 1u << 11;
            tmst_prim_room(k, 9);
            tmst_prim_word(k, 0x2E000000u | (tint & 0xFFFFFFu));      /* textured quad, semi-transparent, tinted */
            tmst_prim_word(k, tmst_xy(qx, qy));           tmst_prim_word(k, u0 | v0 << 8 | clut << 16);
            tmst_prim_word(k, tmst_xy(qx + qw, qy));      tmst_prim_word(k, u1 | v0 << 8 | tpage << 16);
            tmst_prim_word(k, tmst_xy(qx, qy + qh));      tmst_prim_word(k, u0 | v1 << 8);
            tmst_prim_word(k, tmst_xy(qx + qw, qy + qh)); tmst_prim_word(k, u1 | v1 << 8);
        }
        pen += g->advance;
    }
    return (((pen + 32) >> 6) + t / 2) / t;
}

/* ---- a mod's small image (tmst_overlay_set_sheet) ----
 * 15-bit texels (texel(): the half see-through ones with the STP bit), up to
 * one texture page (64 x 256); placed as the font's sheet is, right of where
 * the borrowed picture goes (x 832 and on, up to PREVIEW 800 wide) and clear
 * of the font's sheet, kept and given back the same way. */
enum { SHEET_MAX_W = 64, SHEET_MAX_H = 128, SHEET_X0 = 832 };
static uint16_t sheet[SHEET_MAX_W * SHEET_MAX_H], sheet_kept[SHEET_MAX_W * SHEET_MAX_H];
static int sheet_w, sheet_h, sheet_x = -1, sheet_y, sheet_has_kept, sheet_dirty = 1, sheet_wanted, sheet_last, sheet_ready, sheet_said;
static unsigned sheet_generation, sheet_frame;

int tmst_overlay_set_sheet(const uint32_t *argb, int w, int h) {
    if (!argb || w <= 0 || h <= 0 || w > SHEET_MAX_W || h > SHEET_MAX_H) return 0;
    if (w != sheet_w || h != sheet_h) {
        if (sheet_x >= 0 && sheet_has_kept) vram_up("image back", sheet_x, sheet_y, sheet_w, sheet_h, sheet_kept);
        sheet_x = -1; sheet_has_kept = 0;
    }
    for (int i = 0; i < w * h; i++) sheet[i] = texel(argb[i]);
    sheet_w = w; sheet_h = h; sheet_dirty = 1;
    return 1;
}
static void sheet_give_back(void) {
    if (sheet_x >= 0 && sheet_has_kept) vram_up("image back", sheet_x, sheet_y, sheet_w, sheet_h, sheet_kept);
    sheet_x = -1; sheet_has_kept = 0; sheet_dirty = 1;
}
static int sheet_fits(int x, int y) {
    if ((y - 512) / 256 != (y + sheet_h - 1 - 512) / 256 || y + sheet_h > 1024) return 0;
    if (atlas_x >= 0 && x < atlas_x + ATLAS_W && atlas_x < x + sheet_w && y < atlas_y + ATLAS_H && atlas_y < y + sheet_h)
        return 0;
    if (y + sheet_h > 1024 - ATLAS_H && x + sheet_w > 1024 - ATLAS_W) return 0;   /* where the font's sheet goes first */
    return !tmst_compat_high_vram_taken(x, y, sheet_w, sheet_h) && !tmst_compat_roster_hd_area(x, y, sheet_w, sheet_h) &&
           !split_overlaps(x, y, sheet_w, sheet_h);
}
static int place_sheet(void) {
    if (!sheet_w) return 0;
    unsigned gen = tmst_compat_high_vram_generation();
    if (sheet_x < 0 || !sheet_fits(sheet_x, sheet_y)) {
        sheet_give_back();
        for (int y = 1024 - sheet_h; y >= 512 && sheet_x < 0; y -= 8)
            for (int x = 1024 - 64; x >= SHEET_X0; x -= 64)
                if (sheet_fits(x, y)) { sheet_x = x; sheet_y = y; tmst_log("overlay: the image at x %d y %d", x, y); break; }
        if (sheet_x < 0) {
            if (!sheet_said) { sheet_said = 1; tmst_log("overlay: no room for the image in the upper VRAM half"); }
            return 0;
        }
    }
    if (sheet_dirty || gen != sheet_generation || frame - sheet_frame >= 30) {
        if (!sheet_has_kept || gen != sheet_generation)
            sheet_has_kept = tmst_compat_high_vram_read(sheet_x, sheet_y, sheet_w, sheet_h, sheet_kept);
        vram_up("image", sheet_x, sheet_y, sheet_w, sheet_h, sheet);
        sheet_dirty = 0; sheet_generation = gen; sheet_frame = frame;
    }
    return 1;
}

int tmst_prim_sheet(TmstPrims *k, int x, int y, int w, int h) {
    sheet_wanted = 1;
    if (!sheet_ready) return 0;
    int base = sheet_y < 768 ? 512 : 768, v0 = sheet_y - base, v1 = v0 + sheet_h;
    /* half see-through texels as B/2 + F/2; raw texture; the upper half */
    uint32_t tpage = (uint32_t)(sheet_x / 64) | (uint32_t)(base - 512) / 256u << 4 | 2u << 7 | 1u << 11;
    uint32_t u1 = (uint32_t)sheet_w;
    tmst_prim_room(k, 9);
    tmst_prim_word(k, 0x2F808080u);                     /* textured quad, semi-transparent, raw */
    tmst_prim_word(k, tmst_xy(x, y));         tmst_prim_word(k, (uint32_t)v0 << 8);
    tmst_prim_word(k, tmst_xy(x + w, y));     tmst_prim_word(k, tpage << 16 | (uint32_t)v0 << 8 | u1);
    tmst_prim_word(k, tmst_xy(x, y + h));     tmst_prim_word(k, (uint32_t)v1 << 8);
    tmst_prim_word(k, tmst_xy(x + w, y + h)); tmst_prim_word(k, (uint32_t)v1 << 8 | u1);
    return 1;
}

/* The drawing offset (E5: x in bits 0-10, y in 11-21, both signed): the
 * screen's (0, 0) in VRAM, so the frame being drawn is a texture there. */
static int e5_x(void) { return (int)((gpu_e5 & 0x7FFu) ^ 0x400u) - 0x400; }
static int e5_y(void) { return (int)((gpu_e5 >> 11 & 0x7FFu) ^ 0x400u) - 0x400; }

/* The drawing area (E3 / E4: x in bits 0-9, y from bit 10) the screen's
 * whole height, its x as the game's; or the game's again. */
void tmst_prim_whole_height(TmstPrims *k, int on) {
    tmst_prim_room(k, 2);
    if (!on) { tmst_prim_word(k, gpu_e3); tmst_prim_word(k, gpu_e4); return; }
    int y0 = e5_y() < 0 ? 0 : e5_y(), y1 = e5_y() + 479;
    tmst_prim_word(k, (gpu_e3 & 0xFF0003FFu) | (uint32_t)y0 << 10);
    tmst_prim_word(k, (gpu_e4 & 0xFF0003FFu) | (uint32_t)y1 << 10);
}

void tmst_prim_screen_copy(TmstPrims *k, int x, int y, int w, int h, int dx, int dy, uint32_t tint, int blend) {
    int ox = e5_x(), oy = e5_y();
    /* the source within the 368 x 480 screen and the lower VRAM half */
    int x0 = x + dx < 0 ? -dx : x, y0 = y + dy < 0 ? -dy : y;
    int x1 = x + w + dx > 368 ? 368 - dx : x + w, y1 = y + h + dy > 480 ? 480 - dy : y + h;
    if (x0 >= x1 || y0 >= y1 || ox + x0 + dx < 0 || oy + y0 + dy < 0 || ox + x1 + dx > 1024 || oy + y1 + dy > 512) return;
    if (blend >= 0) {
        /* Only texels with the mask bit blend: the source's pixels get it
         * first, unchanged (black added, the mask bit forced). */
        tmst_prim_room(k, 5);
        tmst_prim_word(k, 0xE6000001u);
        tmst_prim_word(k, 0xE1000420u);                         /* B + F, drawing to the display */
        tmst_prim_word(k, 0x62000000u);
        tmst_prim_word(k, tmst_xy(x0 + dx, y0 + dy));
        tmst_prim_word(k, (uint32_t)(y1 - y0) << 16 | (uint32_t)(x1 - x0));
        tmst_prim_room(k, 1);
        tmst_prim_word(k, 0xE6000000u);
    }
    for (int ty = y0; ty < y1;) {
        int vy0 = oy + ty + dy, py = vy0 >= 256 ? 256 : 0, v = vy0 - py;
        int th = y1 - ty < 256 - v ? y1 - ty : 256 - v;
        for (int tx = x0; tx < x1;) {
            int vx0 = ox + tx + dx, px = vx0 & ~63, u = vx0 - px;
            int tw = x1 - tx < 256 - u ? x1 - tx : 256 - u;
            uint32_t tpage = (uint32_t)(px / 64) | (uint32_t)(py / 256) << 4 | 2u << 7 /* 15-bit */ | 1u << 10;
            tmst_prim_room(k, 5);
            tmst_prim_word(k, 0xE1000000u | tpage | (blend >= 0 ? (uint32_t)(blend & 3) << 5 : 0u));
            tmst_prim_word(k, (blend >= 0 ? 0x66000000u : 0x64000000u) | (tint & 0xFFFFFFu));
            tmst_prim_word(k, tmst_xy(tx, ty));
            tmst_prim_word(k, (uint32_t)v << 8 | (uint32_t)u);
            tmst_prim_word(k, (uint32_t)th << 16 | (uint32_t)tw);
            tx += tw;
        }
        ty += th;
    }
    if (blend >= 0) {
        /* The mask bits off again, colours unchanged (black added, drawn
         * without the mask bit), on the source and the copy: a pixel left
         * with it would not be drawn over by a game that checks it (the
         * software renderer kept such pixels from frame to frame). */
        int sy0 = y0 + dy < y0 ? y0 + dy : y0, sy1 = y1 + dy > y1 ? y1 + dy : y1;
        int sx0 = x0 + dx < x0 ? x0 + dx : x0, sx1 = x1 + dx > x1 ? x1 + dx : x1;
        tmst_prim_room(k, 4);
        tmst_prim_word(k, 0xE1000420u);
        tmst_prim_word(k, 0x62000000u);
        tmst_prim_word(k, tmst_xy(sx0, sy0));
        tmst_prim_word(k, (uint32_t)(sy1 - sy0) << 16 | (uint32_t)(sx1 - sx0));
    }
    tmst_prim_room(k, 1);
    tmst_prim_word(k, gpu_e6);
}

/* The picture's tiles: 1:1 as sprites (an E1 for each page), scaled as
 * textured quads (each names its page). Raw texture, the half see-through
 * texels blended. */
static void picture_prims(TmstPrims *k) {
    int one = sw == pw && sh == ph;
    int b[2][4]; bands(b);
    for (int ty = 0; ty * TILE < ph; ty++)
        for (int tx = 0; tx * TILE < pw; tx++) {
            int w = pw - tx * TILE < TILE ? pw - tx * TILE : TILE;
            int h = ph - ty * TILE < TILE ? ph - ty * TILE : TILE;
            /* the tile's texels: its band's place (a split picture's second
             * band starts inside its page row: v from there) */
            int bx = split ? b[ty][0] : vx, by = split ? b[ty][1] : vy + ty * TILE;
            unsigned page = (unsigned)(bx + tx * TILE) / 64u;
            unsigned ybit = (unsigned)(by - 512) / 256u & 1u;
            uint32_t v0 = (uint32_t)(by - 512) % 256u;
            uint32_t tpage = page | ybit << 4 | 2u << 7 /* 15-bit */ | 1u << 11;
            if (one) {
                int x = sx + tx * TILE, y = sy + ty * TILE;
                tmst_prim_room(k, 5);
                tmst_prim_word(k, 0xE1000000u | tpage | 1u << 10);
                tmst_prim_word(k, 0x67808080u);                       /* raw sprite, half see-through texels */
                tmst_prim_word(k, tmst_xy(x, y));
                tmst_prim_word(k, v0 << 8);
                tmst_prim_word(k, (uint32_t)h << 16 | (uint32_t)w);
            } else {
                int x0 = sx + tx * TILE * sw / pw, x1 = sx + (tx * TILE + w) * sw / pw;
                int y0 = sy + ty * TILE * sh / ph, y1 = sy + (ty * TILE + h) * sh / ph;
                uint32_t u1 = (uint32_t)(w > 255 ? 255 : w), v1 = v0 + (uint32_t)h > 255 ? 255 : v0 + (uint32_t)h;
                tmst_prim_room(k, 9);
                tmst_prim_word(k, 0x2E808080u);                       /* textured quad (x 1.0), half see-through texels */
                tmst_prim_word(k, tmst_xy(x0, y0)); tmst_prim_word(k, v0 << 8);
                tmst_prim_word(k, tmst_xy(x1, y0)); tmst_prim_word(k, u1 | v0 << 8 | tpage << 16);
                tmst_prim_word(k, tmst_xy(x0, y1)); tmst_prim_word(k, v1 << 8);
                tmst_prim_word(k, tmst_xy(x1, y1)); tmst_prim_word(k, u1 | v1 << 8);
            }
        }
}

/* A place the stage engine leaves free for the picture, from its first
 * (x 0) on: the area moves there (what the old one held given back). 0 when
 * there is none. */
static int find_place(void) {
    for (int y = ph <= TILE ? 1024 - TILE : 512; y >= 512; y -= TILE) {
        for (int x = 0; x + pw <= 1024; x += 64) {
            if (tmst_compat_high_vram_taken(x, y, pw, ph)) continue;
            if (x != vx || y != vy) { give_back(); vx = x; vy = y; dirty = 1; }
            return 1;
        }
    }
    return 0;
}

/* This frame's chain: empty while hidden; the picture left out while the
 * stage engine leaves it no place (unless borrowed). */
static void write_chain(uint32_t e1_before) {
    TmstPrims k; prims_begin(&k);
    if (!shown) held_n = 0;
    if (drawing()) {
        /* The font's sheet only while text is drawn: asked for in the last
         * frame (the rank builder's, the builders') or this one's rank
         * builder, else its place given back (the stage engine's floor
         * textures were found where it stayed, through the fights). */
        text_ready = (text_wanted || text_last) && place_atlas();
        sheet_ready = (sheet_wanted || sheet_last) && place_sheet();
        int borrow = (show_flags & TMST_OVERLAY_BORROW) != 0;
        if (shown && borrow && (show_flags & TMST_OVERLAY_YIELD) && kept_w && !yielded &&
            tmst_compat_high_vram_generation() != vram_generation) {
            kept_w = 0; yielded = 1;                   /* theirs now: nothing of ours given back over it */
            tmst_log("overlay: the stage engine took the borrowed area, the picture yields");
        }
        int placed = shown && !yielded && (borrow || split || !tmst_compat_high_vram_taken(vx, vy, pw, ph) || find_place());
        if (!placed) held_n = 0;
        int under = (show_flags & TMST_OVERLAY_UNDER) != 0;
        if (placed) {
            unsigned gen = tmst_compat_high_vram_generation();
            /* Again every half second too: an upload of someone else's into
             * the area (an effect, a stage's animated texture) is undone.
             * A borrowed area is kept once: what the stage engine moves
             * into it later is ours over theirs, not theirs to give back.
             * One over TTT Hitspark's (or the wolf's) cells, every frame:
             * they write their images there each frame an effect plays
             * (before the list: ours, written as it is walked, is what is
             * drawn; their effects are under our picture meanwhile). */
            int contested = borrow && !split && tmst_compat_high_vram_effects(vx, vy, pw, ph);
            if (dirty || contested || gen != vram_generation || frame - uploaded_frame >= 30) {
                if (!kept_w || (gen != vram_generation && !borrow)) keep();
                upload_picture();
                dirty = 0; vram_generation = gen; uploaded_frame = frame;
                int b[2][4];
                held_n = bands(b);
                for (int i = 0; i < held_n; i++) {
                    held_r[i].x = b[i][0]; held_r[i].y = b[i][1]; held_r[i].w = pw; held_r[i].h = b[i][3];
                }
                held_vx = vx; held_vy = vy; held_pw = pw; held_ph = ph;
            }
            /* (held while uploaded for this place and size) */
            if (held_vx != vx || held_vy != vy || held_pw != pw || held_ph != ph) held_n = 0;
            if (under) picture_prims(&k);
        }
        for (int i = 0; i < builder_count; i++) builders[i](&k);
        if (!text_wanted && atlas_x >= 0) atlas_give_back();
        text_last = text_wanted; text_wanted = 0;
        if (!sheet_wanted && sheet_x >= 0) sheet_give_back();
        sheet_last = sheet_wanted; sheet_wanted = 0;
        if (placed && !under) picture_prims(&k);
        tmst_prim_room(&k, 1);
        tmst_prim_word(&k, e1_before);
    }
    prims_finish(&k);
}



/* Words of the GP0 command at word i of a node of n words. */
static unsigned command_words(uint32_t node, unsigned i, unsigned n) {
    uint32_t w = psx_mod_read_word(node + 4 + i * 4);
    unsigned op = w >> 24;
    if (op >= 0x20 && op < 0x40) {                      /* polygons */
        unsigned verts = (op & 8) ? 4 : 3, tex = (op & 4) ? 1 : 0, shaded = (op & 0x10) ? 1 : 0;
        unsigned len = 1 + verts * (1 + tex) + shaded * (verts - 1);
        if (tex && i + len <= n) {
            unsigned at = shaded ? 5 : 4;               /* the second vertex's UV word: the texture page */
            uint32_t page = psx_mod_read_word(node + 4 + (i + at) * 4) >> 16;
            gpu_e1 = (gpu_e1 & ~0x9FFu) | (page & 0x9FFu);
        }
        return len;
    }
    if (op >= 0x40 && op < 0x60) {                      /* lines */
        unsigned shaded = (op & 0x10) ? 1 : 0;
        if (!(op & 8)) return 3 + shaded;
        unsigned k = i + 1;                            /* polyline: to its terminator */
        while (k < n && (psx_mod_read_word(node + 4 + k * 4) & 0xF000F000u) != 0x50005000u) k++;
        return k - i + 1;
    }
    if (op >= 0x60 && op < 0x80) return 2 + ((op & 4) ? 1 : 0) + ((op & 0x18) == 0 ? 1 : 0);
    if (op >= 0x80 && op < 0xA0) return 4;
    if (op >= 0xA0 && op < 0xC0) {
        uint32_t size = i + 2 < n ? psx_mod_read_word(node + 4 + (i + 2) * 4) : 0;
        return 3 + (((size & 0xFFFF) * (size >> 16) + 1) / 2);
    }
    if (op >= 0xC0 && op < 0xE0) return 3;
    if (op == 0x02) return 3;
    if (op == 0xE1) gpu_e1 = w;
    if (op == 0xE3) gpu_e3 = w;
    if (op == 0xE4) gpu_e4 = w;
    if (op == 0xE5) gpu_e5 = w;
    if (op == 0xE6) gpu_e6 = w;
    return 1;
}
static void follow(uint32_t node) {
    unsigned n = psx_mod_read_word(node) >> 24;
    for (unsigned i = 0; i < n;) i += command_words(node, i, n);
}

static int ours(uint32_t at) {
    uint32_t lo = packet & 0x00FFFFFFu;
    return at >= lo && at < lo + BUF_BYTES;
}

/* The ordering table's ranks: T3 clears it in reverse (ClearOTagR), so the
 * list starts at its last entry, rank 1023, and each entry is an empty node
 * 4 bytes below the one before. */
enum { OT_SIZE = 1024 };
static uint32_t ot_top;                 /* the walked list's first node (rank OT_SIZE - 1), 0 in another list */
static int rank = -1;
int tmst_overlay_rank(void) { return rank; }
static void note_rank(uint32_t at, uint32_t tag) {
    if (!ot_top || (tag >> 24) != 0 || at > ot_top || ot_top - at >= OT_SIZE * 4u || ((ot_top - at) & 3u)) return;
    rank = OT_SIZE - 1 - (int)((ot_top - at) / 4u);
}

/* tmst_overlay_hide_node's changes, given back before the next list. */
enum { UNDO_MAX = 8192 };
static uint32_t undo_at[UNDO_MAX], undo_word[UNDO_MAX];
static int undo_count;
static void give_back_nodes(void) {
    while (undo_count > 0) { undo_count--; psx_mod_write_word(undo_at[undo_count], undo_word[undo_count]); }
}
static void undo_write(uint32_t at, uint32_t word) {
    if (undo_count >= UNDO_MAX) return;
    undo_at[undo_count] = at; undo_word[undo_count++] = psx_mod_read_word(at);
    psx_mod_write_word(at, word);
}
static void hide_vertex(uint32_t at) { undo_write(at, tmst_xy(-1024, -1024)); }
void tmst_overlay_patch_word(uint32_t at, uint32_t word) { undo_write(at, word); }
void tmst_overlay_hide_node(uint32_t node) {
    unsigned n = psx_mod_read_word(node) >> 24;
    for (unsigned i = 0; i < n;) {
        uint32_t at = node + 4 + i * 4, w = psx_mod_read_word(at);
        unsigned op = w >> 24, len;
        if (op >= 0x20 && op < 0x40) {                  /* polygons: each vertex's position word */
            unsigned verts = (op & 8) ? 4 : 3, tex = (op & 4) ? 1 : 0, shaded = (op & 0x10) ? 1 : 0, k = 1;
            for (unsigned v = 0; v < verts; v++) {
                if (v && shaded) k++;
                if (i + k < n) hide_vertex(at + k * 4);
                k += 1 + tex;
            }
            len = k;
        } else if (op >= 0x40 && op < 0x60) {           /* lines: their points up to the end mark */
            unsigned shaded = (op & 0x10) ? 1 : 0, k = 1;
            for (unsigned v = 0; i + k < n; v++) {
                if (v && shaded) k++;
                if (i + k >= n) break;
                uint32_t pt = psx_mod_read_word(at + k * 4);
                if (v >= 2 && (op & 8) && (pt & 0xF000F000u) == 0x50005000u) break;
                hide_vertex(at + k * 4); k++;
                if (v == 1 && !(op & 8)) break;
            }
            len = k + ((op & 8) ? 1 : 0);
        } else if (op >= 0x60 && op < 0x80) {           /* rectangles and sprites */
            if (i + 1 < n) hide_vertex(at + 4);
            len = 2 + ((op & 4) ? 1 : 0) + ((op & 0x18) == 0 ? 1 : 0);
        } else if (op >= 0x80 && op < 0xA0) {           /* VRAM copies: onto their own source, nothing moves */
            if (i + 2 < n) undo_write(at + 8, psx_mod_read_word(at + 4));
            len = 4;
        } else {
            len = command_words(node, i, n);
        }
        i += len ? len : 1;
    }
}

/* This drawing only (given back before the next list): the backdrop after
 * the table's first entry, a rectangle past the screen's edges (the wide
 * screen's sides too); the rank builder's primitives after the entry of its
 * rank, ending with the drawing mode the game had there. */
static void link_extras(void) {
    if (backdrop_on && backdrop_node && ot_top) {
        uint32_t first = psx_mod_read_word(ot_top);
        psx_mod_write_word(backdrop_node, 3u << 24 | (first & 0x00FFFFFFu));
        psx_mod_write_word(backdrop_node + 4, 0x60000000u | backdrop_bgr);
        psx_mod_write_word(backdrop_node + 8, tmst_xy(-256, 0));
        psx_mod_write_word(backdrop_node + 12, tmst_xy(368 + 512, 512));
        undo_write(ot_top, (first & 0xFF000000u) | (backdrop_node & 0x00FFFFFFu));
    }
    for (int r = 0; r < RANKS; r++) {
        if (!ranks[r].entry || !ranks[r].builder) continue;
        uint32_t rank_entry = ranks[r].entry, rank_e1 = ranks[r].e1, rank_packet = ranks[r].packet;
        uint32_t entry = psx_mod_read_word(rank_entry);
        TmstPrims k; prims_begin_at(&k, rank_packet, RANK_BYTES);
        ranks[r].builder(&k);
        if (k.at == rank_packet + 4 && k.head == rank_packet) continue;   /* nothing to draw: not linked */
        /* The drawing mode back, without the upper half's bit (the stage
         * engine's polygons leave it in what we follow, but this packet is
         * not one the runtime lets read the half: on a PS1 E1 it is the
         * texture-disable bit). */
        tmst_prim_room(&k, 2);
        tmst_prim_word(&k, rank_e1 & ~0x800u);
        tmst_prim_word(&k, gpu_e6);
        prims_finish_to(&k, entry);
        undo_write(rank_entry, (entry & 0xFF000000u) | (rank_packet & 0x00FFFFFFu));
    }
}

/* What follows our chain's last node: 0x00FFFFFF, or packets someone linked
 * after it (the Expanded's MOVESET cards and STAGE line go after the last
 * node of a list that is not cleared, VS BATTLE SELECT's: ours, when our
 * chain is still in it). `relink`: that node's link set to it. */
static uint32_t chain_end(int relink, uint32_t next) {
    uint32_t at = packet & 0x00FFFFFFu;
    for (unsigned n = 0; n < BUF_WORDS; n++) {
        uint32_t tag = psx_mod_read_word(0x80000000u | at), to = tag & 0x00FFFFFFu;
        if (!ours(to)) {
            if (relink) psx_mod_write_word(0x80000000u | at, (tag & 0xFF000000u) | (next & 0x00FFFFFFu));
            return to;
        }
        at = to;
    }
    return 0x00FFFFFFu;
}

/* One walk of the list about to be drawn. Our chain already in it (a list
 * not cleared since: the paused frame's, VS BATTLE SELECT's) gets this
 * frame's content, or goes out when hidden, keeping what others linked after
 * it (the walk, the visitors, go on there); otherwise, shown, the frame's
 * first long list gets it after its last node. */
static void link_into(uint32_t start) {
    uint32_t at = start & 0x00FFFFFFu, last = 0;
    unsigned n = 0;
    int met = 0;                                        /* our chain was in the list */
    ot_top = (psx_mod_read_word(0x80000000u | at) >> 24) == 0 ? 0x80000000u | at : 0;
    rank = -1;
    uint32_t rank_at[RANKS];
    for (int r = 0; r < RANKS; r++) {
        ranks[r].entry = 0;
        rank_at[r] = ot_top && ranks[r].builder ? ot_top - 4u * (uint32_t)(OT_SIZE - 1 - ranks[r].target) : 0;
    }
    while (at != 0x00FFFFFFu && n < MAX_WALK) {
        uint32_t node = 0x80000000u | at;
        note_rank(node, psx_mod_read_word(node));
        if (ours(at)) {
            if (met) return;                            /* round again: never */
            met = 1;
            uint32_t after = chain_end(0, 0);
            if (!drawing()) {                           /* hidden: out of the list */
                if (last) {
                    uint32_t tag = psx_mod_read_word(0x80000000u | last);
                    psx_mod_write_word(0x80000000u | last, (tag & 0xFF000000u) | after);
                }
                psx_mod_write_word(packet, 0x00FFFFFFu);
            } else if (linked_frame != frame) {
                linked_frame = frame;
                link_extras();
                write_chain(gpu_e1);
                if (after != 0x00FFFFFFu && !cut_tail) chain_end(1, after);
            }
            at = after;                                 /* the others' packets after ours */
            continue;
        }
        for (int r = 0; r < RANKS; r++) if (node == rank_at[r]) { ranks[r].entry = node; ranks[r].e1 = gpu_e1; }
        if (drawing()) for (int i = 0; i < visitor_count; i++) visitors[i](node);
        follow(node);
        last = at;
        at = psx_mod_read_word(node) & 0x00FFFFFFu;
        n++;
    }
    if (met || !drawing() || n < (short_lists ? SHORT_LIST : LONG_LIST) || n >= MAX_WALK || linked_frame == frame) return;
    linked_frame = frame;
    link_extras();
    write_chain(gpu_e1);
    uint32_t tag = psx_mod_read_word(0x80000000u | last);
    psx_mod_write_word(0x80000000u | last, (tag & 0xFF000000u) | (packet & 0x00FFFFFFu));
}

extern void __real_gpu_ws_prepass_linked_list(uint32_t start_addr);
void __wrap_gpu_ws_prepass_linked_list(uint32_t start_addr) {
    give_back_nodes();                  /* the list before is drawn */
    give_back_due_now();
    if (ready && psx_mod_game_started()) link_into(start_addr);
    __real_gpu_ws_prepass_linked_list(start_addr);
}

void tmst_overlay_vblank(void) { frame++; give_back_nodes(); }
