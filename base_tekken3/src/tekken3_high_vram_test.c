/* Test of the upper VRAM half (Arcade Stages, step E3): hidden package
 * tekken3.test.high-vram, staged only with -DTEKKEN3_HIGH_VRAM_TEST=ON.
 *
 * Activation makes the half (psx_mod_gpu_high_vram_enable) and puts two test
 * images in it: a 15-bit 64 x 64 checkerboard at VRAM (960, 512) and a 4-bit
 * 64 x 64 target at (896, 576) whose 16 colours sit at (0, 1023). Every
 * VBlank, four 64 x 64 quads are linked at the end of the game's ordering
 * table (OT[0] of both buffers, 0x800A6588 / 0x800A7588, drawn last):
 *   A  checkerboard, texpage bit 11            -> upper half: checkerboard
 *   B  target, texpage bit 11 + CLUT bit 15    -> upper half: target
 *   C  A without bit 11                        -> PS1 VRAM (960, 0)
 *   D  A from packet memory not given to       -> PS1 VRAM (960, 0), like C
 *      psx_mod_gpu_high_vram_source
 * so A and B show the test images, C and D whatever the game keeps there. */
#include "mod_plugins.h"
#include <stdio.h>

#define OT0 0x800A6588u
#define OT1 0x800A7588u
#define QUAD_WORDS 9u

static uint32_t s_trusted[2], s_untrusted[2];
static int s_linked_once;

static uint16_t rgb(unsigned r, unsigned g, unsigned b) {
    uint16_t c = (uint16_t)((r & 31u) | (g & 31u) << 5 | (b & 31u) << 10);
    return c ? c : 1u;                      /* 0x0000 is a transparent texel */
}

static void upload_images(void) {
    static uint16_t board[64 * 64], target[16 * 64], clut[16];
    for (unsigned y = 0; y < 64; y++)
        for (unsigned x = 0; x < 64; x++) {
            unsigned edge = x == 0 || y == 0 || x == 63 || y == 63 || x == y;
            board[y * 64 + x] = edge ? rgb(31, 31, 31)
                : ((x / 8 + y / 8) & 1) ? rgb(x / 2, 31 - y / 2, 4) : rgb(2, y / 3, 31 - x / 3);
        }
    for (unsigned y = 0; y < 64; y++)
        for (unsigned x = 0; x < 64; x++) {
            int dx = (int)x - 32, dy = (int)y - 32;
            unsigned index = (unsigned)(dx * dx + dy * dy) / 40u & 15u;
            target[y * 16 + x / 4] |= (uint16_t)(index << (x & 3) * 4);
        }
    for (unsigned i = 0; i < 16; i++) {
        static const uint8_t hue[16][3] = {
            {31,0,0},{31,12,0},{31,24,0},{24,31,0},{12,31,0},{0,31,0},{0,31,12},{0,31,24},
            {0,24,31},{0,12,31},{0,0,31},{12,0,31},{24,0,31},{31,0,24},{31,31,31},{8,8,8}};
        clut[i] = rgb(hue[i][0], hue[i][1], hue[i][2]);
    }
    psx_mod_gpu_high_vram_upload(960, 512, 64, 64, board);
    psx_mod_gpu_high_vram_upload(896, 576, 16, 64, target);
    psx_mod_gpu_high_vram_upload(0, 1023, 16, 1, clut);
}

/* GP0(2Dh): textured opaque quad, raw texels. */
static uint32_t quad(uint32_t o, int x, int y, unsigned tpage, unsigned clut, unsigned v) {
    #define XY(px, py) ((uint32_t)((py) & 0xffff) << 16 | (uint32_t)((px) & 0xffff))
    psx_mod_write_word(o, 0x2D808080u);
    psx_mod_write_word(o + 4, XY(x, y));
    psx_mod_write_word(o + 8, clut << 16 | v << 8);
    psx_mod_write_word(o + 12, XY(x + 64, y));
    psx_mod_write_word(o + 16, tpage << 16 | v << 8 | 64u);
    psx_mod_write_word(o + 20, XY(x, y + 64));
    psx_mod_write_word(o + 24, (v + 64u) << 8);
    psx_mod_write_word(o + 28, XY(x + 64, y + 64));
    psx_mod_write_word(o + 32, (v + 64u) << 8 | 64u);
    #undef XY
    return o + 36;
}

static void build_packets(void) {
    const unsigned direct = 2u << 7, high = 1u << 11;
    for (unsigned b = 0; b < 2; b++) {
        uint32_t o = s_trusted[b] + 4;
        o = quad(o, 16, 64, 15u | direct | high, 0, 0);                /* A */
        o = quad(o, 96, 64, 14u | high, (511u << 6) | 0x8000u, 64);    /* B */
        quad(o, 176, 64, 15u | direct, 0, 0);                          /* C */
        quad(s_untrusted[b] + 4, 256, 64, 15u | direct | high, 0, 0);  /* D */
    }
}

/* Put our two nodes at the head of OT[0]'s chain, once per cleared list. */
static void link(unsigned b, uint32_t ot) {
    uint32_t entry = psx_mod_read_word(ot), mine = s_trusted[b] & 0xffffffu;
    uint32_t next = entry & 0xffffffu;
    for (unsigned n = 0; next != 0xffffffu && n < 256; n++) {
        if (next == mine) return;           /* still linked from an earlier VBlank */
        next = psx_mod_read_word(0x80000000u | next) & 0xffffffu;
    }
    psx_mod_write_word(s_untrusted[b], (entry & 0xffffffu) | QUAD_WORDS << 24);
    psx_mod_write_word(s_trusted[b], (s_untrusted[b] & 0xffffffu) | 3u * QUAD_WORDS << 24);
    psx_mod_write_word(ot, (entry & 0xff000000u) | mine);
    if (!s_linked_once) {
        s_linked_once = 1;
        fprintf(stdout, "high VRAM test: quads linked at OT 0x%08X\n", (unsigned)ot);
        fflush(stdout);
    }
}

static void tekken3_high_vram_test_tick(void) {
    if (!s_trusted[1] || !psx_mod_game_started()) return;
    link(0, OT0);
    link(1, OT1);
}

static void tekken3_high_vram_test_activate(void) {
    if (!psx_mod_gpu_high_vram_enable()) return;
    upload_images();
    for (unsigned b = 0; b < 2; b++) {
        s_trusted[b] = psx_mod_alloc_gpu_dma_memory(4 + 3 * QUAD_WORDS * 4, 16);
        s_untrusted[b] = psx_mod_alloc_gpu_dma_memory(4 + QUAD_WORDS * 4, 16);
        if (!s_trusted[b] || !s_untrusted[b] ||
            !psx_mod_gpu_high_vram_source(s_trusted[b], 4 + 3 * QUAD_WORDS * 4)) {
            s_trusted[0] = s_trusted[1] = 0;
            return;
        }
    }
    build_packets();
    fprintf(stdout, "high VRAM test: upper half made, test images uploaded\n");
    fflush(stdout);
}

PSX_MOD_CONSTRUCTOR(tekken3_register_high_vram_test) {
    (void)psx_mod_register_activation_plugin("tekken3.high-vram-test",
                                             tekken3_high_vram_test_activate);
    (void)psx_mod_register_vblank_plugin("tekken3.high-vram-test",
                                         tekken3_high_vram_test_tick);
}
