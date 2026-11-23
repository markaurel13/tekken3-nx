/* tmst.tournament: TOURNAMENT in GAME OPTION's mode box.
 *
 * OPTION -> GAME OPTION shows, in a box at the top right, the modes the row
 * under the cursor affects (ARCADE ... PRACTICE, lit or dim). With the
 * tournament on, the box is a line taller and its ninth line is
 * TOURNAMENT, old gold where the row affects it, dim where not (the
 * tournament plays as Team Battle: it follows Team Battle's lamp, but for
 * the rows it sets itself). Without the tournament the box is the game's. */
#ifndef TMST_TOURNAMENT_OPTIONS_H
#define TMST_TOURNAMENT_OPTIONS_H
#include <stdint.h>

/* Every VBlank, from the tournament's. */
void tmst_tournament_options_tick(void);
/* From the tournament's visitor: the box's primitives moved and grown. */
void tmst_tournament_options_visit(uint32_t node);
#endif
