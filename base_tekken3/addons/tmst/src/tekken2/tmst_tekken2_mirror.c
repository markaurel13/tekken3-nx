/* Tekken 2 Content's Stages: Mirror Darkness's mirror (the remake's, the old
 * fork's tekken3_mirror_arcade, src/tekken3_ttt1_stage.c of
 * stage-engine.zip; Classic: its panels only, no frame, glow, fog or
 * feathers), drawn while src/tekken2/tmst_tekken2_stages.c says its pack is
 * the one the engine draws (T2MR).
 *
 * As Tekken 2 draws it (its GPU lists in MAME): the mirror shows the last
 * frame, on upright panels half see-through, the picture squeezed but not
 * flipped. Tekken 3 draws into its one frame buffer (368 x 480 at 0, 0): at
 * each frame's end a copy of the screen under the gauges goes into the
 * arena's VRAM (x 576..831; the PS1 arena's own pictures, which the stage
 * engine never draws), and the next frame's panels show it, column k on
 * panel k, a frame late as in Tekken 2: the mirror seen in it is Tekken 2's
 * endless row of mirrors. The copy is linked at the end of the ordering
 * table's rank 9 (after the fighters, before the captions at rank 8), the
 * panels at rank 1022 (after the far background, under the floor: only the
 * fighters come in front of them), once the engine has drawn the stage
 * (0x8006D014 returns: chained, the engine's wrapper runs inside ours).
 *
 * The panels stand on an arc round the camera, fixed there as Tekken 2's
 * hangs in its sky: it turns with the camera only (T2MR: radius, centre
 * angle in degrees, width, top, bottom, panels). The camera's view is the
 * PS1's own for its stage (0x8006D014): its rotation from 0x800A85C0, the
 * GTE's screen. */
#include "tmst_core.h"
#include <time.h>

uint64_t g_t2_stage_wrap_ns = 0;
uint64_t g_t2_arcade_stages_ns = 0;

static inline uint64_t t2m_now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}
#include "tmst_compat.h"
#include "cpu_state.h"
#include "mod_plugins.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

/* src/tekken2/tmst_tekken2_stages.c: nonzero, with the mirror's place, when
 * the engine draws a Tekken 2 pack with one. */
int tmst_tekken2_mirror(int32_t at[6]);
/* ... and its style (the Remake's MIRR), 0: none (the Classic's). */
int tmst_tekken2_mirror_style(uint8_t style[20]);
/* ... and, as the stage routine starts, its pack switch (when one is due). */
void tmst_tekken2_stage_entry(void);
int tmst_tekken2_ground(uint8_t rgb[3]);
int tmst_tekken2_drawn(void);
void tmst_tekken2_trace_entry(void);       /* src/tekken2/tmst_tekken2_trace.c */
void tmst_tekken2_trace_exit(void);
int tmst_tekken2_anchor_begin(void);       /* src/tekken2/tmst_tekken2_anchor.c */
void tmst_tekken2_anchor_end(void);
void tmst_tekken2_anchor_step(uint32_t v0);
/* ... whether the Remake's extras are drawn this frame, and they
 * (src/tekken2/tmst_tekken2_extras.c), once the engine has drawn the stage. */
int tmst_tekken2_extras_wanted(void);
void tmst_tekken2_extras_draw(const CPUState *cpu);

enum { CAMERA = 0x800a85c0, SIN_PS1 = 0x8001e8c4, COS_PS1 = 0x8001f0c4, DRAWENV = 0x800a8c54,
       FRAME_PARITY = 0x800adefc, CAMERA_Y0 = 0x80025138, CAMERA_Y0_AT = 0x800b0460 };
enum { MIRROR_TOP = 96, MIRROR_BOTTOM = 468, MIRROR_RANK = 9, PANEL_RANK = 1022, MIRROR_X = 576, MIRROR_W = 256,
       MIRROR_ROWS = 368, MIRROR_SPLIT = 256, MIRROR_LOW = 480 - MIRROR_SPLIT,
       COPY_PIECES = 8, COPY_ENV_WORDS = 7, COPY_BACK_WORDS = 4, COPY_PIECE_WORDS = 10,
       COPY_BYTES = (COPY_ENV_WORDS + COPY_BACK_WORDS + COPY_PIECES * COPY_PIECE_WORDS) * 4,
       PANEL_WORDS = 10, SHADED_WORDS = 9, PANELS_MAX = 15,
       /* a frame's: the panels, the glow and its draw mode, the rails, the posts, the black under it all */
       PANEL_BYTES = (2 * PANELS_MAX * PANEL_WORDS + PANELS_MAX * SHADED_WORDS + 2 + 2 * PANELS_MAX * SHADED_WORDS
                      + 2 * SHADED_WORDS + 4) * 4 };

/* ---- the PS1's view of its stage (0x8003A490, RTPS) ---- */

typedef struct { int32_t r[3][3]; int32_t t[3]; int32_t ofx, ofy; uint32_t h; } Gte;
static int16_t s_sin[4096], s_cos[4096];
static uint8_t s_unr[0x101];
static int s_tables;

static void tables(void) {
    if (s_tables) return;
    for (unsigned i = 0; i < 4096; i++) {
        s_sin[i] = (int16_t)psx_mod_read_half(SIN_PS1 + 2 * i);
        s_cos[i] = (int16_t)psx_mod_read_half(COS_PS1 + 2 * i);
    }
    for (int i = 0; i < 0x101; i++) {
        int v = (0x40000 / (i + 0x100) + 1) / 2 - 0x101;
        s_unr[i] = (uint8_t)(v < 0 ? 0 : v);
    }
    s_tables = 1;
}
static int16_t sat16(int64_t v) { return (int16_t)(v < -0x8000 ? -0x8000 : v > 0x7FFF ? 0x7FFF : v); }
static int gpf(int ir0, int v) { return sat16(((int64_t)ir0 * v) >> 12); }
static void rot_ps1(int ax, int ay, int az, int32_t m[3][3]) {
    int sx = s_sin[ax & 0xFFF], cx = s_cos[ax & 0xFFF];
    int sy = s_sin[ay & 0xFFF], cy = s_cos[ay & 0xFFF];
    int sz = s_sin[az & 0xFFF], cz = s_cos[az & 0xFFF];
    int a1 = gpf(sx, cz), a2 = gpf(sx, sz), a3 = gpf(sx, cy);
    int i6 = (cx * cz) >> 12;
    int b1 = gpf(sy, a1), b2 = gpf(sy, a2), b3 = gpf(sy, i6);
    int i9 = (cx * sz) >> 12;
    int c1 = gpf(cy, cz), c2 = gpf(cy, -sz), c3 = gpf(cy, cx);
    m[0][0] = (int16_t)c1; m[0][1] = (int16_t)c2; m[0][2] = (int16_t)sy;
    m[1][0] = (int16_t)(b1 + i9); m[1][1] = (int16_t)(i6 - b2); m[1][2] = (int16_t)-a3;
    m[2][0] = (int16_t)(a2 - b3); m[2][1] = (int16_t)(((i9 * sy) >> 12) + a1); m[2][2] = (int16_t)c3;
}
static uint32_t gte_divide(uint32_t h, uint32_t sz3) {
    if (h >= sz3 * 2u) return 0x1FFFF;
    int z = 0;
    while (z < 15 && !(sz3 << z & 0x8000u)) z++;
    uint64_t n = (uint64_t)h << z;
    int64_t d = (int64_t)(sz3 << z);
    int64_t u = s_unr[(d - 0x7FC0) >> 7] + 0x101;
    d = (0x2000080 - d * u) >> 8;
    d = (0x0000080 + d * u) >> 8;
    uint64_t q = (n * (uint64_t)d + 0x8000) >> 16;
    return q > 0x1FFFF ? 0x1FFFF : (uint32_t)q;
}
/* World point x, y, z round the camera on the screen; 0 when behind it. */
static int camera_point(const CPUState *cpu, int x, int y, int z, int *sx, int *sy) {
    tables();
    Gte g;
    int pitch = (int)psx_mod_read_word(CAMERA), yaw = (int)psx_mod_read_word(CAMERA + 4);
    int roll = (int)psx_mod_read_word(CAMERA + 8), camy = (int)psx_mod_read_word(CAMERA + 0x18);
    rot_ps1(pitch, yaw, -roll, g.r);
    for (int k = 0; k < 3; k++) g.r[1][k] = (int16_t)((g.r[1][k] * 0x14) / 0xC);
    int y0 = (int16_t)psx_mod_read_half(CAMERA_Y0 + 4 * psx_mod_read_word(CAMERA_Y0_AT));
    int vy = (int16_t)((int16_t)(y0 - camy) >> 3);
    for (int i = 0; i < 3; i++) g.t[i] = (int32_t)(((int64_t)g.r[i][1] * vy) >> 12);
    g.ofx = (int32_t)cpu->gte_ctrl[24]; g.ofy = (int32_t)cpu->gte_ctrl[25]; g.h = cpu->gte_ctrl[26] & 0xFFFF;
    int64_t mac[3];
    for (int i = 0; i < 3; i++)
        mac[i] = (((int64_t)g.t[i] << 12) + (int64_t)g.r[i][0] * x + (int64_t)g.r[i][1] * y + (int64_t)g.r[i][2] * z) >> 12;
    int64_t ir0 = mac[0] < -0x8000 ? -0x8000 : mac[0] > 0x7FFF ? 0x7FFF : mac[0];
    int64_t ir1 = mac[1] < -0x8000 ? -0x8000 : mac[1] > 0x7FFF ? 0x7FFF : mac[1];
    uint32_t sz3 = (uint32_t)(mac[2] < 0 ? 0 : mac[2] > 0xFFFF ? 0xFFFF : mac[2]);
    int64_t q = gte_divide(g.h, sz3);
    int64_t vx = (q * ir0 + g.ofx) >> 16, vyy = (q * ir1 + g.ofy) >> 16;
    *sx = (int)(vx < -0x400 ? -0x400 : vx > 0x3FF ? 0x3FF : vx);
    *sy = (int)(vyy < -0x400 ? -0x400 : vyy > 0x3FF ? 0x3FF : vyy);
    return sz3 > 0;
}

/* ---- the copy: this frame's picture, for the next frame's panels ---- */

/* The picture: the screen's rectangle it shows, drawn MIRROR_W across, its
 * upper MIRROR_SPLIT / MIRROR_ROWS in h0 rows (page rows 0..), the rest in
 * h1 (256..). */
typedef struct { double x, y, w, h; int h0, h1; } Part;
static Part s_part;
static uint32_t s_block;                 /* GPU memory: 2 copies, 2 panel buffers */

static void copy_env(uint32_t pk, int x0, int y0, int x1, int y1, int ox, int oy) {
    psx_mod_write_word(pk + 4, 0xe3000000u | (uint32_t)(y0 & 0x3ff) << 10 | (uint32_t)(x0 & 0x3ff));
    psx_mod_write_word(pk + 8, 0xe4000000u | (uint32_t)(y1 & 0x3ff) << 10 | (uint32_t)(x1 & 0x3ff));
    psx_mod_write_word(pk + 12, 0xe5000000u | (uint32_t)(oy & 0x7ff) << 11 | (uint32_t)(ox & 0x7ff));
}
/* The quads: what of the rectangle is on the screen (below MIRROR_TOP),
 * cut where the frame buffer's 15-bit pages end, scaled into its place. */
static void copy_quads(uint32_t pk, const Part *m) {
    uint32_t q = pk + COPY_ENV_WORDS * 4;
    unsigned n = 0;
    double split = m->y + m->h * MIRROR_SPLIT / MIRROR_ROWS;
    for (int part = 0; part < 2; part++) {
        double r0 = part ? split : m->y, r1 = part ? m->y + m->h : split;
        int rows = part ? m->h1 : m->h0, base = part ? MIRROR_SPLIT : 0;
        int ya = (int)(r0 < MIRROR_TOP ? MIRROR_TOP : r0 + 0.999), yend = (int)(r1 > MIRROR_BOTTOM ? MIRROR_BOTTOM : r1);
        int xs = (int)(m->x < 0 ? 0 : m->x + 0.999), xend = (int)(m->x + m->w > 368 ? 368 : m->x + m->w);
        if (r1 <= r0) continue;
        while (ya < yend) {
            int yb = (ya / 256 + 1) * 256;
            if (yb > yend) yb = yend;
            for (int xa = xs; xa < xend;) {
                int xb = (xa / 256 + 1) * 256;
                if (xb > xend) xb = xend;
                if (n < COPY_PIECES) {
                    int px = xa / 256 * 256, py = ya / 256 * 256;
                    int dx0 = MIRROR_X + (int)((xa - m->x) * MIRROR_W / m->w), dx1 = MIRROR_X + (int)((xb - m->x) * MIRROR_W / m->w + 0.5);
                    int dy0 = base + (int)((ya - r0) * rows / (r1 - r0)), dy1 = base + (int)((yb - r0) * rows / (r1 - r0) + 0.5);
                    unsigned u0 = (unsigned)(xa - px), u1 = (unsigned)(xb - px), v0 = (unsigned)(ya - py), v1 = (unsigned)(yb - py);
                    if (u1 > 255) u1 = 255;
                    if (v1 > 255) v1 = 255;
                    uint32_t page = 0x100u | (uint32_t)(px / 64) | (py ? 0x10u : 0);
                    uint32_t w = q + n * COPY_PIECE_WORDS * 4;
                    psx_mod_write_word(w, (uint32_t)(COPY_PIECE_WORDS - 1) << 24 | ((w + COPY_PIECE_WORDS * 4) & 0xffffffu));
                    psx_mod_write_word(w + 4, 0x2d808080u);
                    psx_mod_write_word(w + 8, (uint32_t)(uint16_t)dy0 << 16 | (uint16_t)dx0);
                    psx_mod_write_word(w + 12, v0 << 8 | u0);
                    psx_mod_write_word(w + 16, (uint32_t)(uint16_t)dy0 << 16 | (uint16_t)dx1);
                    psx_mod_write_word(w + 20, page << 16 | v0 << 8 | u1);
                    psx_mod_write_word(w + 24, (uint32_t)(uint16_t)dy1 << 16 | (uint16_t)dx0);
                    psx_mod_write_word(w + 28, v1 << 8 | u0);
                    psx_mod_write_word(w + 32, (uint32_t)(uint16_t)dy1 << 16 | (uint16_t)dx1);
                    psx_mod_write_word(w + 36, v1 << 8 | u1);
                    n++;
                }
                xa = xb;
            }
            ya = yb;
        }
    }
    for (; n < COPY_PIECES; n++) {           /* the rest: links with nothing to draw */
        uint32_t w = q + n * COPY_PIECE_WORDS * 4;
        psx_mod_write_word(w, (w + COPY_PIECE_WORDS * 4) & 0xffffffu);
    }
}
/* Back to the game's drawing area after the copy: its own packet at rank
 * 1023 (E3, E4), else the whole screen; the offset its DRAWENV's. */
static void game_env(uint32_t base, uint32_t pk) {
    uint32_t e3 = 0xe3000000u, e4 = 0xe4000000u | 479u << 10 | 367u;
    uint32_t link = base + 4 * 1023, end = (base + 4 * 1022) & 0xffffffu;
    for (unsigned guard = 0; guard < 4096; guard++) {
        uint32_t tag = psx_mod_read_word(link), next = tag & 0xffffffu;
        if (next == end || next == 0xffffffu) break;
        link = 0x80000000u | next;
        uint32_t w = psx_mod_read_word(link + 4);
        if (psx_mod_read_word(link) >> 24 >= 2 && w >> 24 == 0xe3 && psx_mod_read_word(link + 8) >> 24 == 0xe4) {
            e3 = w; e4 = psx_mod_read_word(link + 8); break;
        }
    }
    int ox = (int16_t)psx_mod_read_half(0x800a8f14u + 8), oy = (int16_t)psx_mod_read_half(0x800a8f14u + 10);
    psx_mod_write_word(pk + 4, e3);
    psx_mod_write_word(pk + 8, e4);
    psx_mod_write_word(pk + 12, 0xe5000000u | (uint32_t)(oy & 0x7ff) << 11 | (uint32_t)(ox & 0x7ff));
}
/* This frame's copy at the end of rank MIRROR_RANK, before the next. */
static void mirror_copy(uint32_t base, unsigned parity) {
    uint32_t pk = s_block + parity * COPY_BYTES;
    uint32_t link = base + 4 * MIRROR_RANK, rank0 = (base + 4 * (MIRROR_RANK - 1)) & 0xffffffu;
    for (unsigned guard = 0;; guard++) {
        uint32_t next = psx_mod_read_word(link) & 0xffffffu;
        if (next == (pk & 0xffffffu)) return;                    /* linked this frame already */
        if (next == rank0) break;
        if (next == 0xffffffu || guard >= 1u << 16) return;     /* not the chain expected */
        link = 0x80000000u | next;
    }
    uint32_t e = psx_mod_read_word(link);
    uint32_t back = pk + (COPY_ENV_WORDS + COPY_PIECES * COPY_PIECE_WORDS) * 4;
    psx_mod_write_word(pk, (uint32_t)(COPY_ENV_WORDS - 1) << 24 | ((pk + COPY_ENV_WORDS * 4) & 0xffffffu));
    copy_env(pk, MIRROR_X, 0, MIRROR_X + MIRROR_W - 1, 479, 0, 0);
    psx_mod_write_word(pk + 16, 0x02000000u);                      /* black first */
    psx_mod_write_word(pk + 20, (uint32_t)MIRROR_X);
    psx_mod_write_word(pk + 24, 480u << 16 | MIRROR_W);
    copy_quads(pk, &s_part);
    psx_mod_write_word(back, (uint32_t)(COPY_BACK_WORDS - 1) << 24 | (e & 0xffffffu));
    game_env(base, back);
    psx_mod_write_word(link, (e & 0xff000000u) | (pk & 0xffffffu));
}

/* ---- the panels ---- */

/* The four corners (angle a0..a1 of the arc, y0 over y1) on the screen; 0
 * when one is behind the camera or the whole lies off the screen. */
static int arc_quad(const CPUState *cpu, double radius, double a0, double a1, double y0, double y1, int *x, int *y) {
    int margin = tmst_compat_widescreen_margin();
    for (int c = 0; c < 4; c++) {
        double a = c & 1 ? a1 : a0;
        if (!camera_point(cpu, (int)(radius * sin(a)), (int)(c >> 1 ? y1 : y0), (int)(radius * cos(a)), &x[c], &y[c]))
            return 0;
    }
    int left = 1, right = 1, above = 1, below = 1;
    for (int c = 0; c < 4; c++) {
        if (x[c] >= -margin) left = 0;
        if (x[c] < 368 + margin) right = 0;
        if (y[c] >= 0) above = 0;
        if (y[c] < 480) below = 0;
    }
    return !(left || right || above || below);
}
/* The packets of this frame's block, each put at the head of the rank:
 * the last put is drawn first. */
static uint32_t s_cursor, s_end, s_ot;
static uint32_t take(unsigned words) {
    if (!s_cursor || s_cursor + 4 * words > s_end) return 0;
    uint32_t pk = s_cursor;
    s_cursor += 4 * words;
    return pk;
}
static void put(uint32_t pk, unsigned words) {
    uint32_t e = psx_mod_read_word(s_ot);
    psx_mod_write_word(pk, (uint32_t)(words - 1) << 24 | (e & 0xffffffu));
    psx_mod_write_word(s_ot, (e & 0xff000000u) | (pk & 0xffffffu));
}

/* The Remake's style (the old fork's MirrorStyle, MIRR after the place): a
 * frame round the panels (posts at its ends, rails over and under, shaded
 * from hi to lo across; none between the panels: they cut up the picture)
 * and a glow rising from its foot, added to what is behind, breathing over
 * glow_period frames. */
typedef struct { int16_t frame_w, rail_h, glow_h, glow_period; uint8_t hi[3], lo[3], glow[3]; } Style;

/* A Gouraud quad (0x38, or 0x3A added to what is behind), corners' colours c. */
static void shaded_quad(const int *x, const int *y, const uint32_t *c, int added) {
    uint32_t pk = take(SHADED_WORDS);
    if (!pk) return;
    for (int k = 0; k < 4; k++) {
        psx_mod_write_word(pk + 4 + k * 8, (k ? 0 : (added ? 0x3au : 0x38u) << 24) | (c[k] & 0xffffffu));
        psx_mod_write_word(pk + 8 + k * 8, (uint16_t)x[k] | (uint32_t)(uint16_t)y[k] << 16);
    }
    put(pk, SHADED_WORDS);
}
static uint32_t rgb_times(const uint8_t *c, double k) {
    uint32_t v = 0;
    for (int i = 0; i < 3; i++) { double t = c[i] * k; v |= (uint32_t)(t > 255 ? 255 : t < 0 ? 0 : t + 0.5) << (8 * i); }
    return v;
}

/* The frame and the glow (the old fork's mirror_draw before its panels):
 * the glow put first (drawn last, over the panels' foot), its draw mode
 * (additive) before it, then the rails and the posts. */
static void style_draw(const CPUState *cpu, const int32_t *m, const Style *st) {
    int n = m[5] < 1 ? 1 : m[5] > PANELS_MAX ? PANELS_MAX : m[5];
    double radius = m[0], half = m[2] / (double)m[0] / 2, centre = m[1] * 3.14159265358979 / 180;
    int x[4], y[4];
    if (st->glow_h > 0) {
        /* bright at the foot, nothing glow_h over it */
        static unsigned clock;
        double breath = st->glow_period > 0 ? 0.65 + 0.35 * sin(2 * 3.14159265358979 *
                        (clock++ % (unsigned)st->glow_period) / st->glow_period) : 1;
        uint32_t foot = rgb_times(st->glow, breath);
        const uint32_t c[4] = { 0, 0, foot, foot };
        for (int k = 0; k < n; k++)
            if (arc_quad(cpu, radius, centre - half + 2 * half * k / n, centre - half + 2 * half * (k + 1) / n,
                         m[4] - st->glow_h, m[4], x, y))
                shaded_quad(x, y, c, 1);
        /* added: the draw mode's blending (texture page bits 5-6) 1, drawn
         * before the glow; the next textured primitive sets its own */
        uint32_t pk = take(2);
        if (pk) { psx_mod_write_word(pk + 4, 0xe1000620u); put(pk, 2); }
    }
    if (st->frame_w > 0 || st->rail_h > 0) {
        uint32_t hi = rgb_times(st->hi, 1), lo = rgb_times(st->lo, 1);
        if (st->rail_h > 0)
            for (int k = 0; k < n; k++) {
                double a0 = centre - half + 2 * half * k / n, a1 = centre - half + 2 * half * (k + 1) / n;
                const uint32_t c[4] = { hi, hi, lo, lo };
                if (arc_quad(cpu, radius, a0, a1, m[3] - st->rail_h, m[3], x, y)) shaded_quad(x, y, c, 0);
                if (arc_quad(cpu, radius, a0, a1, m[4], m[4] + st->rail_h, x, y)) shaded_quad(x, y, c, 0);
            }
        if (st->frame_w > 0)
            for (int k = 0; k <= n; k += n) {
                double a = centre - half + 2 * half * k / n, w = st->frame_w / radius / 2;
                const uint32_t c[4] = { hi, lo, hi, lo };
                if (arc_quad(cpu, radius, a - w, a + w, m[3] - st->rail_h, m[4] + st->rail_h, x, y))
                    shaded_quad(x, y, c, 0);
            }
    }
}

static void panels(const CPUState *cpu, const int32_t *m) {
    int n = m[5] < 1 ? 1 : m[5] > PANELS_MAX ? PANELS_MAX : m[5];
    double radius = m[0], half = m[2] / (double)m[0] / 2, centre = m[1] * 3.14159265358979 / 180;
    double split = m[3] + (m[4] - m[3]) * (double)MIRROR_SPLIT / MIRROR_ROWS;
    int x[4], y[4];
    for (int k = 0; k < n; k++)
        for (int part = 0; part < 2; part++) {
            if (!arc_quad(cpu, radius, centre - half + 2 * half * k / n, centre - half + 2 * half * (k + 1) / n,
                          part ? split : m[3], part ? m[4] : split, x, y))
                continue;
            if ((int64_t)(x[1] - x[0]) * (y[2] - y[0]) - (int64_t)(x[2] - x[0]) * (y[1] - y[0]) <= 0) continue;
            unsigned u0 = MIRROR_W * (unsigned)k / (unsigned)n, u1 = MIRROR_W * (unsigned)(k + 1) / (unsigned)n - 1;
            unsigned v1 = (unsigned)(part ? s_part.h1 : s_part.h0) - 1;
            const unsigned us[4] = { u0, u1, u0, u1 }, vs[4] = { 0, 0, v1, v1 };
            /* 15-bit, half the picture and half what is behind */
            uint32_t page = 0x100u | (part ? 0x10u : 0) | MIRROR_X / 64;
            uint32_t pk = take(PANEL_WORDS);
            if (!pk) return;
            psx_mod_write_word(pk + 4, 0x2fu << 24 | 0x808080u);
            for (int c = 0; c < 4; c++) {
                uint32_t uv = us[c] | vs[c] << 8;
                if (c == 1) uv |= page << 16;
                psx_mod_write_word(pk + 8 + c * 8, (uint16_t)x[c] | (uint32_t)(uint16_t)y[c] << 16);
                psx_mod_write_word(pk + 12 + c * 8, uv);
            }
            put(pk, PANEL_WORDS);
        }
}

/* Black under all of the stage (first of rank 1023, the far background's):
 * Mirror Darkness's sky leaves the screen's top undrawn, and Tekken 3 never
 * clears its frame buffer: an older screen (the select's, the Practice's
 * menu) stayed there over the mirror. As the old fork's TTT1 packs clear it. */
static void clear_black(uint32_t base) {
    int margin = tmst_compat_widescreen_margin();
    if (margin < 0) margin = 0;
    uint32_t pk = take(4);
    if (!pk) return;
    psx_mod_write_word(pk + 4, 0x60000000u);
    psx_mod_write_word(pk + 8, (uint32_t)(uint16_t)(-margin));
    psx_mod_write_word(pk + 12, 480u << 16 | (uint32_t)(368 + 2 * margin));
    s_ot = base + 4u * 1023;
    put(pk, 4);
}

/* The stage's ground colour under all of it (first of rank 1023, as
 * clear_black): wherever nothing of the stage covers the screen (below the
 * engine's bottom band, 75 % down; the user's black band between the
 * panorama and the floor in the pause, 2026-11-05), the ground shows, not an
 * older frame nor black. Its own packet a frame (by parity), every Tekken 2
 * stage that has a bottom colour, Classic and Remake. */
static uint32_t s_ground_block;
static void ground_fill(uint32_t base, unsigned parity, const uint8_t rgb[3]) {
    if (!s_ground_block && !(s_ground_block = psx_mod_alloc_gpu_dma_memory(2 * 16, 16))) return;
    int margin = tmst_compat_widescreen_margin();
    if (margin < 0) margin = 0;
    uint32_t pk = s_ground_block + 16u * parity, rank = base + 4u * 1023;
    psx_mod_write_word(pk + 4, 0x60000000u | (uint32_t)rgb[2] << 16 | (uint32_t)rgb[1] << 8 | rgb[0]);
    psx_mod_write_word(pk + 8, (uint32_t)(uint16_t)(-margin));
    psx_mod_write_word(pk + 12, 480u << 16 | (uint32_t)(368 + 2 * margin));
    uint32_t e = psx_mod_read_word(rank);
    psx_mod_write_word(pk, 3u << 24 | (e & 0xffffffu));
    psx_mod_write_word(rank, (e & 0xff000000u) | (pk & 0xffffffu));
}

/* The pause's drawing area, given back to the stage. In the pause Tekken 3
 * draws the stage only below its menu: it puts a drawing area (E3) at the
 * head of rank 1023 after the stage routine, y from 234 under the pause
 * menu, from 358 under Practice's 1P FREESTYLE menu (the user's trace,
 * 2026-11-05), and gives the fighters and the HUD theirs at rank 161 (y 20
 * .. 467). Above it the screen keeps an older frame: with a Tekken 2 stage a
 * black band showed there, between the panorama and the floor, and flashed
 * as the menu closed. Our packet, put at the head of rank 1023 now, comes
 * after Tekken 3's (put later, at the head): the stage is drawn on the area
 * the fighters get, whole each frame, the menu over it. In the pause only,
 * every Tekken 2 stage. */
enum { PAUSED = 0x80095494, AREA_TOP_LEFT = 0xe3005000u, AREA_BOTTOM_RIGHT = 0xe4074d6fu };
static uint32_t s_area_block, s_area_last_ot;
static int s_area_seen;
/* As the stage routine starts: did Tekken 3 cut the stage's area in the
 * frame before (an E3 of its own at the head of rank 1023)? The menus open
 * and close over a second with the pause flag still (or already) clear (the
 * user: "for a second it still pops up", 2026-11-05). That frame's table is
 * whole now (the game clears it only as it starts the frame after this). */
static void area_look(void) {
    s_area_seen = 0;
    uint32_t base = s_area_last_ot & 0xffffffu;
    if (!base) return;
    uint32_t a = psx_mod_read_word(0x80000000u | (base + 4u * 1023)) & 0xffffffu;
    for (unsigned k = 0; k < 8 && a != 0xffffffu; k++) {
        if (a >= base && a < base + 4u * 1024) break;             /* the next rank */
        uint32_t tag = psx_mod_read_word(0x80000000u | a);
        if (tag >> 24) {
            uint32_t w = psx_mod_read_word(0x80000000u | (a + 4));
            if (w >> 24 == 0xe3 && w != AREA_TOP_LEFT) { s_area_seen = 1; return; }
        }
        a = tag & 0xffffffu;
    }
}
static void pause_area(uint32_t base, unsigned parity) {
    s_area_last_ot = base;
    if (!psx_mod_read_word(PAUSED) && !s_area_seen) return;
    if (!s_area_block && !(s_area_block = psx_mod_alloc_gpu_dma_memory(2 * 12, 16))) return;
    uint32_t pk = s_area_block + 12u * parity, rank = base + 4u * 1023;
    psx_mod_write_word(pk + 4, AREA_TOP_LEFT);
    psx_mod_write_word(pk + 8, AREA_BOTTOM_RIGHT);
    uint32_t e = psx_mod_read_word(rank);
    psx_mod_write_word(pk, 2u << 24 | (e & 0xffffffu));
    psx_mod_write_word(rank, (e & 0xff000000u) | (pk & 0xffffffu));
}

/* The picture: the whole screen below the gauges, as many rows as the VRAM
 * holds, at most one a screen row. */
static void part_init(void) {
    Part m;
    m.x = 0; m.y = MIRROR_TOP; m.w = 368; m.h = MIRROR_BOTTOM - MIRROR_TOP;
    double up = m.h * MIRROR_SPLIT / MIRROR_ROWS, low = m.h - up;
    m.h0 = (int)(up < MIRROR_SPLIT ? up + 0.5 : MIRROR_SPLIT);
    m.h1 = (int)(low < MIRROR_LOW ? low + 0.5 : MIRROR_LOW);
    if (m.h0 < 1) m.h0 = 1;
    if (m.h1 < 1) m.h1 = 1;
    s_part = m;
}

/* 0x8006D014, the PS1's stage (the engine's wrapper draws the arcade stage
 * as it returns): before, the pack switch; once it has, the Remake's extras
 * and the mirror. Chained. */
extern void __tmst_chain_func_8006D014(CPUState *cpu);
#ifdef TMST_BRIDGE_func_8006D014
extern void __real_func_8006D014(CPUState *cpu);
void __tmst_chain_func_8006D014(CPUState *cpu) { __real_func_8006D014(cpu); }
#endif
void __wrap_func_8006D014(CPUState *cpu) {
    uint64_t _tw0 = t2m_now_ns();
    static uint32_t ret, sp;
    static int running, extras, grounded, traced, anchored;
    static int32_t at[6];
    static uint8_t ground[3];
    if (cpu->pc == 0 || cpu->pc == 0x8006d014u) {
        tmst_tekken2_trace_entry();
        area_look();
        traced = tmst_tekken2_drawn();
        anchored = tmst_tekken2_anchor_begin();     /* the fighters at the scenery's anchor (the extras step there too) */
        tmst_tekken2_stage_entry();
        running = tmst_tekken2_mirror(at);
        extras = tmst_tekken2_extras_wanted();
        grounded = !running && tmst_tekken2_ground(ground);       /* the mirror clears to black itself */
        ret = cpu->gpr[31]; sp = cpu->gpr[29];
    }
    if (cpu->pc == 0x8006d110u) tmst_tekken2_anchor_step(cpu->gpr[2]);     /* back from the axis step */
    uint64_t _ta0 = t2m_now_ns();
    __tmst_chain_func_8006D014(cpu);
    uint64_t _ta1 = t2m_now_ns();
    g_t2_arcade_stages_ns += (_ta1 - _ta0);
    if ((!running && !extras && !grounded && !traced && !anchored) || cpu->pc != ret || cpu->gpr[29] != sp) {
        g_t2_stage_wrap_ns += (_ta1 - _tw0);
        return;
    }
    if (traced) { traced = 0; tmst_tekken2_trace_exit(); }
    if (extras) { extras = 0; tmst_tekken2_extras_draw(cpu); }     /* round the same anchor */
    if (anchored) { anchored = 0; tmst_tekken2_anchor_end(); }
    if (grounded) {
        grounded = 0;
        uint32_t base = psx_mod_read_word(psx_mod_read_word(DRAWENV) + 4);
        if (base) {
            ground_fill(base, psx_mod_read_word(FRAME_PARITY) & 1u, ground);
            pause_area(base, psx_mod_read_word(FRAME_PARITY) & 1u);     /* before the fill, after Tekken 3's */
        }
    }
    if (!running) { g_t2_stage_wrap_ns += (t2m_now_ns() - _tw0); return; }
    running = 0;
    if (!s_block && !(s_block = psx_mod_alloc_gpu_dma_memory(2 * COPY_BYTES + 2 * PANEL_BYTES, 16))) { g_t2_stage_wrap_ns += (t2m_now_ns() - _tw0); return; }
    if (!s_part.h0) part_init();
    uint32_t base = psx_mod_read_word(psx_mod_read_word(DRAWENV) + 4);
    if (!base) { g_t2_stage_wrap_ns += (t2m_now_ns() - _tw0); return; }
    unsigned parity = psx_mod_read_word(FRAME_PARITY) & 1u;
    mirror_copy(base, parity);
    s_cursor = s_block + 2 * COPY_BYTES + parity * PANEL_BYTES;
    s_end = s_cursor + PANEL_BYTES;
    s_ot = base + 4u * PANEL_RANK;
    uint8_t raw[20];
    if (tmst_tekken2_mirror_style(raw)) {
        Style st;
        memcpy(&st.frame_w, raw, 2); memcpy(&st.rail_h, raw + 2, 2); memcpy(&st.glow_h, raw + 4, 2);
        memcpy(&st.glow_period, raw + 6, 2);
        memcpy(st.hi, raw + 8, 3); memcpy(st.lo, raw + 11, 3); memcpy(st.glow, raw + 14, 3);
        style_draw(cpu, at, &st);
    }
    panels(cpu, at);
    clear_black(base);
    pause_area(base, parity);
    g_t2_stage_wrap_ns += (t2m_now_ns() - _tw0);
}

/* 0x80036CAC, a model's drawing (the Expanded's src/tekken3_ttt1_renderer.c
 * wraps it: after a guest's head, Devil's and Angel's laser, its sparks and
 * Unknown's blade, put at the head of rank 1 of the ordering table it is
 * given, in packets of the GPU memory). With the mirror, as the old fork
 * did: what it put there this call goes to the head of rank MIRROR_RANK,
 * before the mirror's copy at that rank's end, so the mirror shows it (in
 * front of the fighters still, after the stage; the captions after it).
 * Chained. */
enum { APERTURE = 0x00e00000u };         /* PSX_MOD_GPU_DMA_APERTURE_BASE: packets of the GPU memory */
extern void __tmst_chain_func_80036CAC(CPUState *cpu);
#ifdef TMST_BRIDGE_func_80036CAC
extern void __real_func_80036CAC(CPUState *cpu);
void __tmst_chain_func_80036CAC(CPUState *cpu) { __real_func_80036CAC(cpu); }
#endif
void __wrap_func_80036CAC(CPUState *cpu) {
    int32_t at[6];
    uint32_t ot = cpu->gpr[6];
    if (!tmst_tekken2_mirror(at)) { __tmst_chain_func_80036CAC(cpu); return; }
    uint32_t base = psx_mod_read_word(psx_mod_read_word(DRAWENV) + 4);
    uint32_t slot = ot + 4, before = base && ot == base ? psx_mod_read_word(slot) & 0xffffffu : 0xffffffffu;
    __tmst_chain_func_80036CAC(cpu);
    if (before == 0xffffffffu) return;
    uint32_t head = psx_mod_read_word(slot) & 0xffffffu;
    if (head == before) return;
    /* the run of new packets of the GPU memory at the rank's head */
    uint32_t last = 0, p = head;
    for (unsigned guard = 0; p != before && p >= APERTURE && p != 0xffffffu && guard < 64; guard++) {
        last = p;
        p = psx_mod_read_word(0x80000000u | p) & 0xffffffu;
    }
    if (!last) return;
    uint32_t rank = base + 4u * MIRROR_RANK, e = psx_mod_read_word(rank);
    psx_mod_write_word(slot, (psx_mod_read_word(slot) & 0xff000000u) | p);       /* rank 1 leads on as before */
    uint32_t lt = psx_mod_read_word(0x80000000u | last);
    psx_mod_write_word(0x80000000u | last, (lt & 0xff000000u) | (e & 0xffffffu));
    psx_mod_write_word(rank, (e & 0xff000000u) | head);
}
