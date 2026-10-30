/* Jin's Omen (Tekken Tag Tournament): the Devil mark on his Tekken 3 face.
 *
 * In TTT1 every Omen move (Force, then 2,2,3; tools/ttt1/moves.py
 * GRANTED_PARTNERS) shows face expression 5 for a number of frames (timed
 * property 0x45 once converted): the Devil face, red eye, tattooed forehead
 * and gem. Jin on his TTT1 moves (motion mode 4) keeps his Tekken 3 model,
 * whose face is one 32 x 64 8-bit image the engine's expression table points
 * at (0x800959CC by model, positions 0x8009579C, sizes 0x8009586C; TIPS § 20).
 * While the expression lasts, the mark (Jin-TTT1-omen-mark.bin in the TTT1
 * asset root, tools/ttt1_jin_omen_mark.py) is painted over that image.
 *
 * Colours: the face's 256-colour palette has no red, and the face, its
 * expressions and his ear use every entry. Reds take entries the face and
 * its expressions leave to the ear; for the time of the mark the ear's pixels
 * of those entries take the nearest other colour (one step in 31 apart on
 * this palette). The other colours of the mark take the nearest entry. When
 * the expression ends, face, ear and palette go back to what they were,
 * unless the engine has replaced them meanwhile (an expression copy, a new
 * fight). An engine expression during the Omen (a blink) is painted again.
 * TEKKEN3_JIN_OMEN_MARK overrides the file.
 */
#include "psx_runtime.h"
#include "mod_plugins.h"
#include "gpu_render.h"
#include "tekken3_ttt1_assets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { JIN_ID = 9, DEVIL_EXPRESSION = 5, MARK_W = 32, MARK_H = 64, WORDS = MARK_W / 2 };
enum { ACTOR = 0x800a9228, STRIDE = 0x188c };
enum { FACE_TABLE = 0x800959cc, FACE_POSITIONS = 0x8009579c, FACE_SIZES = 0x8009586c };
/* Jin's other images on that palette, in his page (VRAM words, player 1;
 * measured on the polygons that use it): his expressions above the shown
 * face, and the ear (UV 48..71, 192..223). */
enum { EXPR_X = 384, EXPR_Y = 0, EAR_X = 408, EAR_Y = 192, EAR_W = 12, EAR_H = 32 };

static uint16_t mark[MARK_W * MARK_H];
static int mark_state;                  /* 0 not read yet, 1 read, -1 none */

typedef struct {
    unsigned timer;
    int painted, x, y, clut_y, ear_y;
    uint16_t original[WORDS * MARK_H], shown[WORDS * MARK_H];
    uint16_t palette[256], shown_palette[256];
    uint16_t ear[EAR_W * EAR_H], shown_ear[EAR_W * EAR_H];
} Face;
static Face faces[2];

static void read_mark(void) {
    if (mark_state) return;
    mark_state = -1;
    char path[4096];
    const char *e = getenv("TEKKEN3_JIN_OMEN_MARK"), *root = tekken3_ttt1_asset_root();
    if (e && *e) snprintf(path, sizeof path, "%s", e);
    else if (!root || !*root || snprintf(path, sizeof path, "%s/Jin-TTT1-omen-mark.bin", root) >= (int)sizeof path) return;
    FILE *f = fopen(path, "rb");
    if (!f) return;
    unsigned char head[8], b[sizeof mark];
    if (fread(head, 1, 8, f) == 8 && !memcmp(head, "OMEN", 4) &&
        (head[4] | head[5] << 8) == MARK_W && (head[6] | head[7] << 8) == MARK_H &&
        fread(b, 1, sizeof b, f) == sizeof b) {
        for (unsigned i = 0; i < MARK_W * MARK_H; i++) mark[i] = (uint16_t)(b[2 * i] | b[2 * i + 1] << 8);
        mark_state = 1;
        fprintf(stderr, "Jin Omen: mark read (%s)\n", path);
    }
    fclose(f);
}

static void put(uint16_t *image, int x, int y, int v) {
    uint16_t *w = &image[y * WORDS + x / 2];
    *w = x & 1 ? (uint16_t)((*w & 0x00ff) | v << 8) : (uint16_t)((*w & 0xff00) | v);
}

static int distance(uint16_t a, uint16_t b) {
    int dr = (a & 31) - (b & 31), dg = (a >> 5 & 31) - (b >> 5 & 31), db = (a >> 10 & 31) - (b >> 10 & 31);
    return dr * dr + dg * dg + db * db;
}

static int is_red(uint16_t c) { return (c & 31) > 15 && (c >> 5 & 31) < 13; }

static void mark_used(unsigned char *used, const uint16_t *words, int n) {
    for (int i = 0; i < n; i++) used[words[i] & 255] = used[words[i] >> 8] = 1;
}

/* The shown face of this player's model (16 x 64 VRAM words), or 0. */
static int locate(unsigned player, int *x, int *y) {
    unsigned model = psx_mod_read_half(ACTOR + player * STRIDE + 0x1c);
    if (model > 51) return 0;                          /* guests: model 52, no engine expressions */
    uint32_t entry = psx_mod_read_word(FACE_TABLE + model * 4);
    if ((entry >> 24) == 0xff) return 0;
    uint32_t pos = psx_mod_read_word(FACE_POSITIONS + (entry & 255) * 4);
    uint32_t size = psx_mod_read_word(FACE_SIZES + (entry >> 8 & 255) * 4);
    if ((size & 0xffff) != WORDS || size >> 16 != MARK_H) return 0;
    *x = (int)(pos & 0xffff);
    *y = (int)(pos >> 16) + 256 * (int)player;         /* player 2's page is 256 rows down */
    return 1;
}

/* Puts back what is still ours: the face, the ear, the palette. */
static void restore(unsigned player, Face *f, int log) {
    if (!f->painted) return;
    uint16_t now[WORDS * MARK_H], ear[EAR_W * EAR_H], palette[256];
    gr_vram_transfer_out(f->x, f->y, WORDS, MARK_H, now);
    if (!memcmp(now, f->shown, sizeof now)) gr_vram_transfer_in(f->x, f->y, WORDS, MARK_H, f->original);
    gr_vram_transfer_out(EAR_X, f->ear_y, EAR_W, EAR_H, ear);
    if (!memcmp(ear, f->shown_ear, sizeof ear)) gr_vram_transfer_in(EAR_X, f->ear_y, EAR_W, EAR_H, f->ear);
    gr_vram_transfer_out(0, f->clut_y, 256, 1, palette);
    if (!memcmp(palette, f->shown_palette, sizeof palette)) gr_vram_transfer_in(0, f->clut_y, 256, 1, f->palette);
    f->painted = 0;
    if (log) fprintf(stderr, "Jin Omen: P%u Devil mark off\n", player + 1);
}

static void paint(unsigned player, Face *f) {
    int first = !f->painted;
    restore(player, f, 0);                             /* an engine expression came: from scratch */
    gr_vram_transfer_out(f->x, f->y, WORDS, MARK_H, f->original);
    gr_vram_transfer_out(0, f->clut_y, 256, 1, f->palette);
    gr_vram_transfer_out(EAR_X, f->ear_y, EAR_W, EAR_H, f->ear);
    memcpy(f->shown, f->original, sizeof f->shown);
    memcpy(f->shown_palette, f->palette, sizeof f->palette);
    memcpy(f->shown_ear, f->ear, sizeof f->ear);
    unsigned char face_used[256] = {0}, borrowed[256] = {0};
    uint16_t expressions[WORDS * MARK_H];
    gr_vram_transfer_out(EXPR_X, EXPR_Y + 256 * (int)player, WORDS, MARK_H, expressions);
    mark_used(face_used, f->original, WORDS * MARK_H);
    mark_used(face_used, expressions, WORDS * MARK_H);
    int next = 255, reds = 0;
    for (int y = 0; y < MARK_H; y++)
        for (int x = 0; x < MARK_W; x++) {
            uint16_t c = mark[y * MARK_W + x];
            if (!c) continue;
            c &= 0x7fff;
            int best = -1;
            if (is_red(c)) {
                for (int k = 255; k > next; k--) if (borrowed[k] && f->shown_palette[k] == c) best = k;
                while (best < 0 && next > 0 && face_used[next]) next--;
                if (best < 0 && next > 0) { best = next--; borrowed[best] = 1; f->shown_palette[best] = c; reds++; }
            }
            if (best < 0) {
                int d = 1 << 30;
                for (int k = 0; k < 256; k++)
                    if (!borrowed[k] && distance(f->palette[k], c) < d) { d = distance(f->palette[k], c); best = k; }
            }
            put(f->shown, x, y, best);
        }
    /* The ear's pixels of a borrowed entry: the nearest entry kept. */
    int to[256];
    for (int k = 0; k < 256; k++) {
        to[k] = k;
        if (!borrowed[k]) continue;
        int d = 1 << 30;
        for (int j = 0; j < 256; j++)
            if (!borrowed[j] && distance(f->palette[j], f->palette[k]) < d) { d = distance(f->palette[j], f->palette[k]); to[k] = j; }
    }
    for (int i = 0; i < EAR_W * EAR_H; i++)
        f->shown_ear[i] = (uint16_t)(to[f->ear[i] & 255] | to[f->ear[i] >> 8] << 8);
    gr_vram_transfer_in(EAR_X, f->ear_y, EAR_W, EAR_H, f->shown_ear);
    gr_vram_transfer_in(0, f->clut_y, 256, 1, f->shown_palette);
    gr_vram_transfer_in(f->x, f->y, WORDS, MARK_H, f->shown);
    if (first) fprintf(stderr, "Jin Omen: P%u Devil mark on (VRAM %d,%d, %d red entries)\n", player + 1, f->x, f->y, reds);
    f->painted = 1;
}

/* Each tick of a player on TTT1 moves in motion mode 4 (a native: his own
 * model), after the timed properties of his move were read: expression and
 * duration as for a guest's face (-1: none fired this tick). */
void tekken3_jin_omen_face(unsigned player, unsigned character, int advanced, int expression, unsigned duration) {
    if (player > 1) return;
    Face *f = &faces[player];
    if (character != JIN_ID) { restore(player, f, 1); f->timer = 0; return; }
    if (expression == DEVIL_EXPRESSION) f->timer = duration ? duration : 1;
    else if (expression >= 0) f->timer = 0;
    else if (advanced && f->timer) f->timer--;
    if (!f->timer) { restore(player, f, 1); return; }
    read_mark();
    if (mark_state < 0) return;
    int x, y;
    if (!locate(player, &x, &y)) return;
    if (f->painted && (x != f->x || y != f->y)) restore(player, f, 1);
    if (f->painted) {
        uint16_t now[WORDS * MARK_H];
        gr_vram_transfer_out(f->x, f->y, WORDS, MARK_H, now);
        if (!memcmp(now, f->shown, sizeof now)) return;
    }
    f->x = x; f->y = y;
    f->clut_y = 504 + 4 * (int)player;
    f->ear_y = EAR_Y + 256 * (int)player;
    paint(player, f);
}
