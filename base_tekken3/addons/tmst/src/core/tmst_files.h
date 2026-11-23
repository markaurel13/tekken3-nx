/* TMST core: where the add-on's files go.
 *
 * The Expanded's folder is the first folder up from the game that holds
 * game.toml (build-release's parent for the player; deeper for a developer
 * build under addons/tmst/dev/build); the add-on's own is addons/tmst in it.
 * The Expanded's setup empties its workspace/, never addons/. */
#ifndef TMST_FILES_H
#define TMST_FILES_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

/* The Expanded's folder, with no slash at the end. */
const char *tmst_files_root(void);
/* `path` and the folders above it made where missing. Returns 1 when it is
 * a folder afterwards. */
int tmst_files_make_dirs(const char *path);
/* Nonzero when something is at `path`. */
int tmst_files_exist(const char *path);
/* The game's own folder (the executable's; the data folder on Android), with
 * a slash at the end. */
const char *tmst_files_game_dir(void);
/* The add-on's file `rel` ("data/photo/t3e_logo.png"), read only:
 * <root>/addons/tmst/<rel> when it is there, else the copy the build stages
 * beside the game, <game>/mods/tmst-data/<rel> (addon.cmake TMST_STAGED: the
 * Expanded's own Android APK holds only the game's mods/). Neither there:
 * the first. */
void tmst_files_addon(char *out, size_t size, const char *rel);
/* Only the staged copy: <game>/mods/tmst-data/<rel>. Returns 0 when it does
 * not fit. */
int tmst_files_staged(char *out, size_t size, const char *rel);

#ifdef __cplusplus
}
#endif
#endif
