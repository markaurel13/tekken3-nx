/* tests/test_png.py: writes the pattern it names with the core's PNG writer. */
#include "tmst_png.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc == 4 && argv[1][0] == 'r') {              /* r in.png out.raw: the reader's ARGB, w and h first */
        uint32_t *px; int w, h;
        if (!tmst_png_read(argv[2], &px, &w, &h)) return 1;
        FILE *f = fopen(argv[3], "wb");
        if (!f) return 3;
        uint32_t head[2] = {(uint32_t)w, (uint32_t)h};
        fwrite(head, sizeof head, 1, f); fwrite(px, sizeof(uint32_t), (size_t)w * (size_t)h, f);
        fclose(f); free(px);
        return 0;
    }
    if (argc != 5) return 2;
    int w = atoi(argv[2]), h = atoi(argv[3]), kind = atoi(argv[4]);
    uint32_t *px = malloc(sizeof(uint32_t) * (size_t)w * (size_t)h);
    if (!px) return 3;
    uint32_t seed = 12345;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            uint32_t v;
            if (kind == 0) v = (uint32_t)(x * 255 / (w > 1 ? w - 1 : 1)) << 16 | (uint32_t)(y * 255 / (h > 1 ? h - 1 : 1)) << 8 | (uint32_t)((x ^ y) & 255);
            else if (kind == 1) { seed = seed * 1103515245u + 12345u; v = seed >> 8; }
            else v = ((x / 8 + y / 8) & 1) ? 0xFFFFFF : 0x102030;
            px[(size_t)y * (size_t)w + (size_t)x] = 0xFF000000u | v;
        }
    int ok = tmst_png_write(argv[1], px, w, h, w);
    free(px);
    return ok ? 0 : 1;
}
