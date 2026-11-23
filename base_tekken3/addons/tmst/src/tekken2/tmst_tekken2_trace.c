/* tmst.tekken2.stages, a diagnostic: what of a Tekken 2 stage's frame reaches
 * the screen. On only while the file t2-trace.on is beside the game (the
 * user runs it, 2026-11-05: the black band between the panorama and the
 * floor in the pause, seen only in the user's own runs). Writes t2-trace.log
 * beside the game: for a few frames of the fight and of each of the first
 * pauses, the whole ordering table the frame handed to the GPU, walked as
 * the GPU walks it (from rank 1023 down), every packet with its rank, its
 * command, colour, screen box, texture page and palette.
 *
 * When: at the start of the PS1's stage routine (0x8006D014; src/tekken2/
 * tmst_tekken2_mirror.c calls tmst_tekken2_trace_entry), the table of the
 * frame before (its base kept as that frame's stage routine returned:
 * tmst_tekken2_trace_exit) is whole and drawn; the game clears it only as it
 * starts the frame after this one. */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "mod_plugins.h"
#include "psx_sdl.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tmst_tekken2_drawn(void);


enum { SCREEN = 0x800ae204, PAUSED = 0x80095494, VSYNC = 0x800a08f0, DRAWENV = 0x800a8c54, CAMERA = 0x800a85c0,
       PHASE = 0x800ae224, ARENA = 0x800adc84, FIGHT_DUMPS = 2, PAUSE_DUMPS = 3, PAUSES = 3 };

static int on = -1;
static FILE *out;
static uint32_t prev_base, prev_vsync;
static unsigned fight_frames, pause_frames, pauses, fight_dumped, pause_dumped, was_paused;

/* The upper VRAM half as the fight left it (at its dump), and in the pause:
 * which 16 x 64 cells changed, their rows (an upload over the stage's
 * pictures would show here). */
static uint16_t *vram_fight, *vram_now;
static void vram_snap(uint16_t **into) {
    if (!*into && !(*into = malloc(1024u * 512u * 2u))) return;
    if (!tmst_compat_high_vram_read(0, 512, 1024, 512, *into)) { free(*into); *into = NULL; }
}
static void vram_compare(void) {
    if (!vram_fight) { fprintf(out, "   vram: no snapshot of the fight\n"); return; }
    vram_snap(&vram_now);
    if (!vram_now) { fprintf(out, "   vram: unreadable\n"); return; }
    unsigned cells = 0;
    for (int cy = 0; cy < 8; cy++)
        for (int cx = 0; cx < 64; cx++) {
            int n = 0, y0 = 999, y1 = -1, x0 = 999, x1 = -1;
            for (int y = cy * 64; y < cy * 64 + 64; y++)
                for (int x = cx * 16; x < cx * 16 + 16; x++)
                    if (vram_fight[y * 1024 + x] != vram_now[y * 1024 + x]) {
                        n++;
                        if (y < y0) y0 = y; if (y > y1) y1 = y; if (x < x0) x0 = x; if (x > x1) x1 = x;
                    }
            if (!n) continue;
            cells++;
            fprintf(out, "   vram changed: cell x %d y %d: %d pixels, x %d..%d y %d..%d\n", cx * 16, 512 + cy * 64, n,
                    x0, x1, 512 + y0, 512 + y1);
        }
    fprintf(out, "   vram: %u cells changed since the fight's dump\n", cells);
}

static int trace_on(void) {
    if (on >= 0) return on;
    char path[1100];
    const char *base = SDL_GetBasePath();
    snprintf(path, sizeof path, "%st2-trace.on", base ? base : "");
    FILE *f = fopen(path, "rb");
    on = f != NULL;
    if (f) fclose(f);
    if (on) {
        snprintf(path, sizeof path, "%st2-trace.log", base ? base : "");
        out = fopen(path, "w");
        if (!out) on = 0;
        else tmst_log("tekken2: trace on: %s", path);
    }
#if !defined(PSX_SDL3)
    SDL_free((void *)base);
#endif
    return on;
}

static int16_t lo(uint32_t w) { return (int16_t)(w & 0xffff); }
static int16_t hi(uint32_t w) { return (int16_t)(w >> 16); }

/* One packet: its command words from a (after the tag), n of them. */
static void packet(int rank, uint32_t a, unsigned n) {
    uint32_t w[16];
    unsigned m = n < 16 ? n : 16;
    for (unsigned i = 0; i < m; i++) w[i] = psx_mod_read_word(0x80000000u | (a + 4 + 4 * i));
    unsigned op = w[0] >> 24;
    fprintf(out, "%4d %06x %2u op %02x rgb %06x", rank, a, n, op, w[0] & 0xffffff);
    if (op >= 0x20 && op < 0x40) {                     /* polygons */
        int quad = op & 8, tex = op & 4, gour = op & 0x10, nv = quad ? 4 : 3, at = 1;
        int x0 = 32767, y0 = 32767, x1 = -32768, y1 = -32768;
        uint32_t clut = 0, page = 0;
        for (int v = 0; v < nv && at < (int)m; v++) {
            if (v && gour) at++;                       /* this vertex's colour */
            if (at >= (int)m) break;
            int x = lo(w[at]), y = hi(w[at]); at++;
            if (x < x0) x0 = x; if (x > x1) x1 = x; if (y < y0) y0 = y; if (y > y1) y1 = y;
            if (tex && at < (int)m) { if (v == 0) clut = w[at] >> 16; if (v == 1) page = w[at] >> 16; at++; }
        }
        fprintf(out, " poly%s%s%s box %d,%d..%d,%d", quad ? "4" : "3", tex ? " tex" : "", gour ? " g" : "",
                x0, y0, x1, y1);
        if (tex) fprintf(out, " clut %04x page %04x", clut, page);
        if (op & 2) fprintf(out, " semi");
    } else if (op >= 0x60 && op < 0x80) {             /* rectangles */
        int x = lo(w[1]), y = hi(w[1]), wd = 0, ht = 0, size = (op >> 3) & 3;
        int at = 2;
        if (op & 4) { fprintf(out, " uvclut %08x", m > 2 ? w[2] : 0); at = 3; }
        if (size == 0 && at < (int)m) { wd = lo(w[at]); ht = hi(w[at]); }
        else wd = ht = size == 1 ? 1 : size == 2 ? 8 : 16;
        fprintf(out, " rect box %d,%d %dx%d", x, y, wd, ht);
    } else if (op == 0x02) {
        fprintf(out, " fill %d,%d %dx%d", lo(w[1]), hi(w[1]), m > 2 ? lo(w[2]) : 0, m > 2 ? hi(w[2]) : 0);
    } else if (op >= 0xe1 && op <= 0xe6) {
        fprintf(out, " env");
        for (unsigned i = 0; i < m; i++) fprintf(out, " %08x", w[i]);
    } else if (op >= 0x40 && op < 0x60) {
        fprintf(out, " line");
    }
    fputc('\n', out);
}

static void dump(const char *what, uint32_t base) {
    fprintf(out, "== %s: vsync %u (frame of vsync %u), paused %u, screen %u, phase %d, arena %d, widescreen margin %d,"
            " OT %06x\n", what, psx_mod_read_word(VSYNC), prev_vsync, psx_mod_read_word(PAUSED),
            psx_mod_read_word(SCREEN), (int16_t)psx_mod_read_half(PHASE), (int16_t)psx_mod_read_half(ARENA),
            tmst_compat_widescreen_margin(), base & 0xffffff);
    fprintf(out, "   camera");
    for (int i = 0; i < 8; i++) fprintf(out, " %d", (int)psx_mod_read_word(CAMERA + 4 * i));
    fputc('\n', out);
    uint32_t lo_ot = base & 0xffffff, hi_ot = lo_ot + 4 * 1024;
    uint32_t a = base & 0xffffff;
    a += 4 * 1023;
    int rank = 1023;
    unsigned count = 0;
    for (unsigned guard = 0; a != 0xffffff && guard < 60000; guard++) {
        if (a >= lo_ot && a < hi_ot) rank = (int)((a - lo_ot) / 4);
        uint32_t tag = psx_mod_read_word(0x80000000u | a);
        unsigned n = tag >> 24;
        if (n) { packet(rank, a, n); count++; }
        a = tag & 0xffffff;
    }
    fprintf(out, "== end: %u packets\n\n", count);
    fflush(out);
}

/* Each fight frame (every 4th): the fighters' places and the shadows the
 * frame drew (Tekken 3's: semi-transparent flat polygons, 0x21 / 0x29, at
 * rank 1008; the user: "the shadow is swallowed in a square, further on it
 * shows again", 2026-11-05). */
static void shadow_line(uint32_t base) {
    uint32_t lo_ot = base & 0xffffff, a = (lo_ot + 4 * 1023);
    unsigned n = 0, first = 0, last = 0, other = 0;
    int rank = 1023, rmin = 9999, rmax = -1;
    char where[160]; int wl = 0; where[0] = 0;
    for (unsigned guard = 0; a != 0xffffff && guard < 60000; guard++) {
        if (a >= lo_ot && a < lo_ot + 4 * 1024) rank = (int)((a - lo_ot) / 4);
        uint32_t tag = psx_mod_read_word(0x80000000u | a);
        if (tag >> 24) {
            uint32_t w = psx_mod_read_word(0x80000000u | (a + 4));
            unsigned op = w >> 24;
            if (op == 0x21 || op == 0x29) {
                n++;
                if (rank < rmin) rmin = rank;
                if (rank > rmax) rmax = rank;
            } else if (rank >= 980 && rank <= 1009 && op >= 0x20 && op < 0x40) {
                other++;
            }
        }
        a = tag & 0xffffff;
    }
    (void)first; (void)last; (void)wl;
    fprintf(out, "shadow vs %u p1 %d %d %d p2 %d %d %d axis %d: %u shadow polys at ranks %d..%d, %u polys at 980..1009\n",
            psx_mod_read_word(VSYNC), (int)psx_mod_read_word(0x800aa190u), (int)psx_mod_read_word(0x800aa194u),
            (int)psx_mod_read_word(0x800aa198u), (int)psx_mod_read_word(0x800aba1cu),
            (int)psx_mod_read_word(0x800aba20u), (int)psx_mod_read_word(0x800aba24u),
            (int)psx_mod_read_word(0x800a9188u), n, rmin, rmax, other);
}

/* As the stage routine starts: the frame before, when it is one to keep. */
void tmst_tekken2_trace_entry(void) {
    if (!trace_on() || !out) return;
    if (!tmst_tekken2_drawn() || psx_mod_read_word(SCREEN) != 8) { was_paused = 0; prev_base = 0; return; }
    unsigned paused = psx_mod_read_word(PAUSED) != 0;
    if (paused && !was_paused) { pauses++; pause_frames = 0; pause_dumped = 0; }
    was_paused = paused;
    if (!prev_base) return;
    if (!paused && (int16_t)psx_mod_read_half(PHASE) >= 6) {
        fight_frames++;
        if (fight_frames % 4 == 0) shadow_line(prev_base);
        if (fight_dumped < FIGHT_DUMPS && (fight_frames == 120 || fight_frames == 121)) {
            fight_dumped++;
            dump("fight", prev_base);
            vram_snap(&vram_fight);
        }
    } else if (paused && pauses <= PAUSES) {
        pause_frames++;
        if (pause_dumped < PAUSE_DUMPS && (pause_frames == 30 || pause_frames == 200 || pause_frames == 400)) {
            char what[32];
            snprintf(what, sizeof what, "pause %u", pauses);
            pause_dumped++;
            dump(what, prev_base);
            vram_compare();
            fflush(out);
        }
    }
}

/* As it returns: this frame's table. */
void tmst_tekken2_trace_exit(void) {
    if (on <= 0) return;
    prev_base = psx_mod_read_word(psx_mod_read_word(DRAWENV) + 4);
    prev_vsync = psx_mod_read_word(VSYNC);
}
