/* TMST core: rows of our own in Tekken 3's main menu: at the end of the list
 * (after every row the game has and every row the Expanded adds, its QUIT
 * GAME, now or in a later release), or right after a native row.
 *
 * The native menu reads a working list (count at +4, rows of 8 bytes from
 * +16: the label's address, then the mode byte and its flags). The Expanded
 * relocates that list and patches the menu's LUI/ORI (0x800DB5D0/D8) to its
 * copy; we read the list's address from those two instructions, so we never
 * need the Expanded's variables. The Expanded's menu code runs in its wrapper
 * of 0x80028CD4; ours is chained in front of it: our rows are taken out
 * before it runs (it always sees its own list, the highlight counted in it)
 * and put back after it, for the native menu that runs next. */
#ifndef TMST_MENU_H
#define TMST_MENU_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Called once when the row's choice is confirmed (the frame the game
 * dispatches its mode). */
typedef void (*TmstMenuChosen)(void);

/* A row labelled `label` (upper case, the menu's font) that dispatches the
 * mode of the first native row whose mode byte is `like_mode` (its look too):
 * Team Battle (2) for the Jukebox, at the end of the list. Up to 4 rows.
 * Returns the row's handle, -1 when full. */
int tmst_menu_add_row(const char *label, unsigned like_mode, TmstMenuChosen chosen);
/* The same, right after the native row whose mode byte is `after_mode`
 * (Tekken Force, 8); at the end of the list when there is none. */
int tmst_menu_add_row_after(const char *label, unsigned like_mode, unsigned after_mode, TmstMenuChosen chosen);
/* The menu comes back with this row highlighted (e.g. after its screen). */
void tmst_menu_select_next_time(int row);
/* Nonzero while the menu waits for a choice with this row highlighted. */
int tmst_menu_highlighted(int row);
/* On the menu: how many rows this row is from the highlighted one (the list
 * turns round: the shorter way), -1 when it is not in the list. */
int tmst_menu_distance(int row);
/* The row's letters in a colour of its own instead of its native row's:
 * `rgb` (0xRRGGBB) as it shows on the screen on the highlighted row, darker
 * away from it as the native rows are, and in the frames the menu goes on
 * being drawn after a choice (the screen already the next one). */
void tmst_menu_set_colour(int row, uint32_t rgb);
/* A native row taken over: the native row whose mode byte is `mode` reads
 * `label` (the menu's font: 1..9, A..Z, !) and has no controller
 * restriction (the VS row asks for two pads: the menu skips it with one),
 * so it can be chosen whatever is connected. The Expanded's menu code still
 * sees the row as it was (taken back before it runs, given again after).
 * One row. */
void tmst_menu_native_row(unsigned mode, const char *label);
/* Nonzero while the menu waits for a choice with that native row
 * highlighted. */
int tmst_menu_native_highlighted(unsigned mode);

/* Every VBlank, from the VBlank plugin of the mod that owns a row. */
void tmst_menu_vblank(void);

#ifdef __cplusplus
}
#endif
#endif
