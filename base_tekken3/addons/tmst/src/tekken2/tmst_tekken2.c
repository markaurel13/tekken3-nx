/* tmst.tekken2: Tekken 2 Content, what the add-on takes from the player's own
 * Tekken 2 PlayStation disc (the add-on's setup: setup/tmst_setup.py), in one
 * package. Its feature "tekken2" is the package's switch; each part is a
 * feature of its own that requires it (the manifest's requires_feature: the
 * Expanded switches a part off with it and the switch on with a part), shown
 * in the Mods tab's next group, TMST / Tekken 2 Content.
 *
 * Parts:
 *   Music  tools/tekken2_music.py makes the .bgm files of
 *          addons/tmst/workspace/t2-music/{console,arcade}, which the Jukebox
 *          lists as its TEKKEN 1 and TEKKEN 2 categories (and plays in
 *          fights) only while this part is on (tmst_tekken2_music_active,
 *          asked when it reads its lists, so the order of the activations
 *          does not matter). The setup makes the music whether it is on or
 *          not: it cannot know, and the music is there as soon as it is on.
 *   Stages src/tekken2/tmst_tekken2_stages.c: Tekken 2's stages, drawn by
 *          the Expanded's stage engine, picked on the STAGE line of VS and
 *          Practice (tmst_tekken2_on: this switch, for that part).
 *   Wireframe Mode src/tekken2/tmst_tekken2_wireframe.c: Tekken 2's hidden
 *          mode, the player's fighter in green lines and the camera behind
 *          its head, GAME OPTION's TEKKEN 2 WIREFRAME MODE row (asks
 *          tmst_tekken2_on too). */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "mod_plugins.h"

static int on, music;                  /* the features, at activation */

int tmst_tekken2_music_active(void) { return on && music; }
int tmst_tekken2_on(void) { return on; }

static void activate(void) {
    on = tmst_guard("tmst.tekken2", TMST_NEED_ACTIVATION);
}

static void activate_music(void) {
    music = tmst_guard("tmst.tekken2.music", TMST_NEED_ACTIVATION);
    if (music) tmst_log("tekken2: Tekken 1 and 2 music in the Jukebox");
}

PSX_MOD_CONSTRUCTOR(tmst_register_tekken2) {
    (void)psx_mod_register_activation_plugin("tmst.tekken2", activate);
    (void)psx_mod_register_activation_plugin("tmst.tekken2.music", activate_music);
}
