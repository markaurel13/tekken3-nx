/* TMST core: our pad filters, chained in front of the Expanded's (tmst_input.h). */
#include "tmst_input.h"

enum { MAX_FILTERS = 6 };
static TmstInputFilter filters[MAX_FILTERS];
static int count;

int tmst_input_add_filter(TmstInputFilter f) {
    for (int i = 0; i < count; i++) if (filters[i] == f) return 1;
    if (!f || count >= MAX_FILTERS) return 0;
    filters[count++] = f;
    return 1;
}

extern uint16_t __tmst_chain_tekken3_outfits_input(int player, uint16_t buttons);
#ifdef TMST_BRIDGE_tekken3_outfits_input
extern uint16_t __real_tekken3_outfits_input(int player, uint16_t buttons);
uint16_t __tmst_chain_tekken3_outfits_input(int player, uint16_t buttons) {
    return __real_tekken3_outfits_input(player, buttons);
}
#endif

uint16_t __wrap_tekken3_outfits_input(int player, uint16_t buttons) {
    for (int i = 0; i < count; i++) buttons = filters[i](player, buttons);
    return __tmst_chain_tekken3_outfits_input(player, buttons);
}

uint16_t tmst_input_run(int player, uint16_t buttons) { return __wrap_tekken3_outfits_input(player, buttons); }
