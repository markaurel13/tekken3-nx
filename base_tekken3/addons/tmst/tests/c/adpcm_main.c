/* tests/test_adpcm_pack.py: src/jukebox/tmst_adpcm.c's frames of a pack.
 *   adpcm_main <pack> <out.raw> <frames> <max>   the first frames (through its
 *   loop as often as it takes), asking for at most max frames at a time; an
 *   ADPCM pack or an XA pack (44 100 Hz frames, resampled) */
#include "tmst_adpcm.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 5) return 2;
    FILE *f = fopen(argv[1], "rb"), *out = fopen(argv[2], "wb");
    long want = atol(argv[3]);
    unsigned max = (unsigned)atoi(argv[4]);
    uint8_t head[TMST_ADPCM_HEAD];
    static TmstAdpcm a;
    static TmstXaPack x;
    int16_t *buf = malloc(sizeof(int16_t) * 2 * max);
    if (!f || !out || !buf || fread(head, 1, sizeof head, f) != sizeof head) return 1;
    int xa = !memcmp(head, TMST_XA_MAGIC, 8);
    if (xa ? !tmst_xa_open(&x, f, head) : !tmst_adpcm_open(&a, f, head)) return 1;
    while (want > 0) {
        unsigned first = 0, n = xa ? tmst_xa_fill(&x, buf, max) : tmst_adpcm_fill(&a, buf, max, &first);
        if (!n) return 3;
        long take = (long)(n - first) < want ? (long)(n - first) : want;
        fwrite(buf + first * 2, sizeof(int16_t) * 2, (size_t)take, out);
        want -= take;
    }
    fclose(out); fclose(f); free(buf);
    return 0;
}
