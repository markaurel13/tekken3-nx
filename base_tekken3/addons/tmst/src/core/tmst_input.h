/* TMST core: the pads, before the game and the Expanded see them.
 *
 * The runtime passes every pad read through tekken3_outfits_input(player,
 * buttons) (the costume panel's filter, which the Expanded's TTT1 selector
 * wraps); ours is chained in front of both. A filter returns the buttons to
 * pass on: set a bit (active low) to swallow that button. */
#ifndef TMST_INPUT_H
#define TMST_INPUT_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t (*TmstInputFilter)(int player, uint16_t buttons);
/* Up to 6 filters, run in the order added. */
int tmst_input_add_filter(TmstInputFilter filter);
/* The whole chain for `player` (ours, then the Expanded's), as the runtime
 * runs it for a pad it reads: for buttons handed to a slot without a pad
 * (tmst_compat_pad_set), which the runtime never filters. */
uint16_t tmst_input_run(int player, uint16_t buttons);

/* Pad bits, active low (0 = pressed). */
enum { TMST_PAD_SELECT = 0x0001, TMST_PAD_START = 0x0008, TMST_PAD_UP = 0x0010, TMST_PAD_RIGHT = 0x0020,
       TMST_PAD_DOWN = 0x0040, TMST_PAD_LEFT = 0x0080, TMST_PAD_L2 = 0x0100, TMST_PAD_R2 = 0x0200,
       TMST_PAD_L1 = 0x0400, TMST_PAD_R1 = 0x0800, TMST_PAD_TRIANGLE = 0x1000, TMST_PAD_CIRCLE = 0x2000,
       TMST_PAD_CROSS = 0x4000, TMST_PAD_SQUARE = 0x8000 };

#ifdef __cplusplus
}
#endif
#endif
