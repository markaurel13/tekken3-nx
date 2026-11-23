/* tests/test_files.py: src/core/tmst_files.c's lookups, printed.
 *   files_main <rel>   ->  root, game folder, tmst_files_addon(rel) */
#include "tmst_files.h"
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc < 2) return 2;
    char path[4300];
    tmst_files_addon(path, sizeof path, argv[1]);
    printf("%s\n%s\n%s\n", tmst_files_root(), tmst_files_game_dir(), path);
    return 0;
}
