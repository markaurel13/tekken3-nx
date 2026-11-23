/* Test stand-in for the Expanded's psx_sdl.h (tests/test_files.py): the
 * game's folder from TMST_TEST_BASE. */
#ifndef PSX_SDL_H
#define PSX_SDL_H
#include <stdlib.h>
#include <string.h>
static inline const char *SDL_GetBasePath(void) {
    const char *base = getenv("TMST_TEST_BASE");
    if (!base) return NULL;
    char *copy = malloc(strlen(base) + 1);
    if (copy) strcpy(copy, base);
    return copy;
}
#define SDL_free free
#endif
