/* TMST core: our own drawing over the game's picture, in both renderers and
 * without touching them.
 *
 * Each frame the core links a chain of packets of its own at the end of the
 * frame's display list (drawn last, so on top):
 *   - the mod's native primitives, if it set a builder (tmst_overlay_set_prims):
 *     rectangles, polygons, the game's own font from its VRAM;
 *   - the picture: an ARGB picture of the mod's, uploaded into the upper VRAM
 *     half and drawn 1:1 (sprites) or scaled onto a screen rectangle (textured
 *     quads: a picture with more texels than the rectangle has pixels is sharp
 *     where the renderer draws at a higher resolution, the OpenGL one).
 * One picture at a time, up to TMST_OVERLAY_MAX_W x TMST_OVERLAY_MAX_H, in
 * the game's screen coordinates (368 x 480 in fights and on the selectors).
 * Alpha below 128 is see-through, 128..254 half see-through (the GPU's
 * B/2 + F/2), 255 opaque.
 *
 * Several mods may draw (each on its own screens: the Jukebox on the
 * selectors, Photo Mode in fights): the picture is whoever showed it last,
 * and every mod's builder and visitor runs while a picture is shown, so each
 * returns at once while its mod shows nothing. Every drawing mod calls
 * tmst_overlay_vblank() from its VBlank plugin; more than one call per
 * VBlank only makes the half-second refresh come sooner. */
#ifndef TMST_OVERLAY_H
#define TMST_OVERLAY_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

enum { TMST_OVERLAY_MAX_W = 1024, TMST_OVERLAY_MAX_H = 512 };

/* From the activation of every mod that draws (start-up): the upper VRAM half
 * must exist before the OpenGL renderer starts, or it never samples it. */
int tmst_overlay_prepare(void);
enum {
    /* The area may hold the stage engine's pixels (a stage pack stays loaded
     * after its fight): take it anyway. Only for screens without a stage
     * (menus, selectors). The area's pixels are always kept and given back. */
    TMST_OVERLAY_BORROW = 1,
    /* The picture under the builders' primitives (by default over them). */
    TMST_OVERLAY_UNDER = 2,
    /* With BORROW: the moment the stage engine uploads (its generation
     * changes) the area is theirs: the picture is no longer drawn nor
     * uploaded, and what it kept is not given back, until it is hidden and
     * shown again. For screens a stage may start loading under (a loading
     * screen). */
    TMST_OVERLAY_YIELD = 4,
    /* A picture of the selectors and grids (up to 1024 x 396): where the
     * Expanded draws the TTT1 guests' cells and big portraits from its HD
     * faces (1.5.0 on, the faces installed: tmst_compat_roster_hd_present),
     * the picture leaves their places in the upper half free: it is
     * narrowed to TMST_OVERLAY_ROOM_W texels (its height kept) and put in
     * two bands around them. Without them, as any other picture. On the
     * Expanded 1.5.1 (the stock fighters' HD tiles too: fewer free places)
     * narrowed to TMST_OVERLAY_ROOM_W_151, for the Team Battle grid (no big
     * portraits: the Jukebox's and Tournament Mode's). */
    TMST_OVERLAY_ROOM_FOR_HD = 8
};
enum { TMST_OVERLAY_ROOM_W = 576, TMST_OVERLAY_ROOM_W_151 = 384 };

/* Shows `argb` (w x h texels, copied) from the next frame on, until hidden,
 * on the screen rectangle (x, y, dw, dh); dw x dh = w x h draws it 1:1.
 * The same pixels again cost nothing; changed ones are uploaded once.
 * Returns 0 when the picture cannot be shown (too big, no upper VRAM). */
int tmst_overlay_show_scaled(const uint32_t *argb, int w, int h, int x, int y, int dw, int dh, unsigned flags);
int tmst_overlay_show_ex(const uint32_t *argb, int w, int h, int x, int y, unsigned flags);
int tmst_overlay_show(const uint32_t *argb, int w, int h, int x, int y);
/* The builders' primitives (and the visitors) from the next frame on with
 * no picture, until hidden: for screens where the stage engine leaves no
 * room for a picture (the fights). */
void tmst_overlay_show_prims(void);
/* A backdrop: from the next frame on, while the primitives or the picture
 * are shown, the frame's ordering table starts with a rectangle of `bgr`
 * (the GPU's 0xBBGGRR) over the whole screen, so where the game draws
 * nothing (Tekken 3 never clears the screen) no older frame shows. off = 0
 * takes it away. */
void tmst_overlay_backdrop(int on, uint32_t bgr);
/* Nonzero: the chain also goes into a frame's first list of 32 nodes or
 * more (by default 256: the fights' and selectors' lists; shorter ones are
 * the GPU's set-up lists). The main menu's whole frame is one list of about
 * 160 nodes: a mod that draws on it asks for this while it draws, 0 after. */
void tmst_overlay_short_lists(int on);
/* Nonzero: the chain ends the list again (what others linked after it, the
 * Expanded's selector cards and its CHARACTER SELECT header, is not drawn),
 * for a mod's screen that covers the whole picture; 0 after. */
void tmst_overlay_cut_tail(int on);
/* Moves the shown picture. */
void tmst_overlay_move(int x, int y);
/* The picture hidden from the next frame (its area's pixels given back
 * now), the primitives still drawn. */
void tmst_overlay_hide_picture(void);
/* Nonzero: the borrowed picture found no place clear of TTT Hitspark's (or
 * the wolf's) cells; it is written again every frame over theirs, and an
 * effect drawn over the picture then shows its pixels (a smaller picture
 * may find a place). */
int tmst_overlay_picture_contested(void);
/* Hidden from the next frame (the picture and the primitives); the areas'
 * pixels are given back now. */
void tmst_overlay_hide(void);
int tmst_overlay_visible(void);
/* Once per VBlank, from the VBlank plugin of the mod that shows the picture:
 * the packets are linked once per frame. */
void tmst_overlay_vblank(void);

/* Native primitives, built each frame into the core's packets (several
 * nodes as needed). A builder writes GP0 words; room(n) before a primitive of
 * n words keeps it in one node. */
typedef struct TmstPrims TmstPrims;
void tmst_prim_room(TmstPrims *k, unsigned words);
void tmst_prim_word(TmstPrims *k, uint32_t word);
static inline uint32_t tmst_xy(int x, int y) { return ((uint32_t)(y & 0xffff) << 16) | (uint32_t)(x & 0xffff); }
/* Text in the UI font (tmst_ui_font.h, its SCREEN face: ASCII, Fira Sans,
 * an accented letter as its plain one, the TMST_UI_ICON_* symbols)
 * as textured quads, from a 4-bit sheet of its glyphs (192 x 77 halfwords:
 * three texture pages, its palette in its last row) the core keeps in the
 * upper VRAM half, in a corner the stage engine leaves free (placed, kept
 * and given back as the picture is). (x, y): the capitals' top, screen
 * units of the 368 x 480 screen; scale 1: two texels to a unit (capitals
 * 12 tall), 2: one (24). The glyphs' faint edges blend half (B/2 + F/2),
 * the rest opaque; colour `tint` (0x808080 = white; 0xBBGGRR, the GPU's
 * x/128). Nothing is drawn the first frame, nor while no place is free.
 * Returns the text's width (screen units). */
int tmst_ui_prim_text(TmstPrims *k, int x, int y, const char *text, int scale, uint32_t tint);
int tmst_ui_prim_width(const char *text, int scale);
/* The frame being drawn as a texture: the screen rectangle (x, y, w, h)
 * drawn again from the frame's own pixels (dx, dy) away, each texel times
 * `tint` / 128 (0xBBGGRR, 0x808080 = as it is); `blend` -1 opaque, 0..3
 * the GPU's semi-transparency (B/2 + F/2, B + F, B - F, B + F/4; the
 * source's mask bits are set first, so every texel blends). Within the
 * 368 x 480 screen (the wide screen's sides are not in VRAM). The OpenGL
 * renderer samples the frame at the native resolution: copies over the
 * whole screen soften it. */
void tmst_prim_screen_copy(TmstPrims *k, int x, int y, int w, int h, int dx, int dy, uint32_t tint, int blend);
/* A mod's small image (ARGB, up to 64 x 128; alpha under 128 see-through,
 * under 255 half see-through, as the picture's), kept in the upper VRAM half
 * as the font's sheet is; tmst_prim_sheet draws it scaled to the screen
 * rectangle (x, y, w, h). Nothing is drawn the first frame it is asked for,
 * nor while no place is free (it returns 0 then). */
int tmst_overlay_set_sheet(const uint32_t *argb, int w, int h);
int tmst_prim_sheet(TmstPrims *k, int x, int y, int w, int h);
/* Nonzero: what follows may draw on the screen's whole height (T3 draws
 * only rows 20..467 of its 480, the rest stays black); zero: the game's
 * drawing area again. */
void tmst_prim_whole_height(TmstPrims *k, int on);
/* A builder, run while the picture is shown (before the picture is drawn).
 * Up to 6 builders (one per mod), run in the order added. */
typedef void (*TmstPrimsBuilder)(TmstPrims *k);
void tmst_overlay_set_prims(TmstPrimsBuilder builder);
/* A builder whose primitives go into the frame's ordering table right after
 * the entry of `rank` (drawn after the ranks above it, before its own and
 * those below: rank 1006 = after the stage and the fighters' shadows,
 * before the top HUD and the fighters), for that drawing only, while the
 * primitives or the picture are shown. Up to 2 (one per mod: Photo Mode's,
 * the tournament's), each again to move it; a rank of -1 takes it away. */
void tmst_overlay_set_rank_prims(int rank, TmstPrimsBuilder builder);
/* A visitor of each node of the game's lists (not ours), called while the
 * picture is shown, before the GPU draws the node: a mod may move the game's
 * primitives out of sight. Up to 6 visitors (one per mod). */
typedef void (*TmstNodeVisitor)(uint32_t node);
void tmst_overlay_set_visitor(TmstNodeVisitor visitor);
/* For a visitor: the ordering-table rank the node is linked at (the last OT
 * entry the walk passed: 1023 farthest, drawn first; T3's stage 1010, its
 * HUD 1009..1006), -1 in a list that is not the frame's ordering table. */
int tmst_overlay_rank(void);
/* For a visitor: every primitive of the node off screen for this drawing
 * only (a VRAM copy copies onto its own source). Its words are given back before the next list is drawn, so a packet
 * the game builds once and keeps is never changed for good. */
void tmst_overlay_hide_node(uint32_t node);
/* For a visitor: one word of a node (a primitive's colour, its texture
 * page or palette) changed for this drawing only, given back as
 * tmst_overlay_hide_node's changes are. */
void tmst_overlay_patch_word(uint32_t at, uint32_t word);

#ifdef __cplusplus
}
#endif
#endif
