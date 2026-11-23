/* Builds that emulate -Wl,--wrap=fopen (macOS: ld64 has no --wrap;
 * dev/tmst_wraps.cmake, addon.cmake's TMST_APPLE_CALLERS): put before the
 * first line of an Expanded source whose fopen calls go to the add-on's
 * wrapper (-include), as --wrap sends them on the other systems. stdio.h
 * first, so that its own declaration keeps the name. */
#ifndef TMST_REDIRECT_FOPEN_H
#define TMST_REDIRECT_FOPEN_H
#include <stdio.h>
FILE *__wrap_fopen(const char *path, const char *mode);
#define fopen __wrap_fopen
#endif
