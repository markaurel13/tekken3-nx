/* tmst.vs-plus: GAME OPTION's mode box, the VS lamp lit on the DIFFICULTY
 * row (VS MODE PLUS's CPU takes that difficulty). */
#ifndef TMST_VSPLUS_OPTIONS_H
#define TMST_VSPLUS_OPTIONS_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Every VBlank while the mod is on. */
void tmst_vsplus_options_tick(void);
/* The overlay's visitor: each node of the frame's list. */
void tmst_vsplus_options_visit(uint32_t node);
#ifdef __cplusplus
}
#endif
#endif
