/* tmst.tekken2.stages, the Remake: its scenery half fixed to the ground.
 *
 * The Expanded's stage engine lays an arcade stage's tiles (the 3D scenery)
 * round the fighters' midpoint, each frame (tile_matrix, visible_tiles: the
 * point the stage follows, from P1/P2's x and z and, for a fighter in its
 * 'S' state, its last four places). Tekken 3's scenery stands far off, so
 * it never shows; the Remake's stands close round the fight, and slid over
 * the floor (which is the world's, fixed) as the fighters moved: "the
 * objects move too fast and too visibly with the camera, they do not look
 * set down on the stage; not fixed, but less" (the user, 2026-11-05, D16).
 *
 * So the scenery follows an anchor of ours instead: a point of the world.
 * D16 let it catch up slowly (1/240 a stage frame, 3 m at most behind);
 * it then slid on over the floor for seconds after a sidestep: "the
 * objects slide with the floor when the characters sidestep; as on Paul's
 * stage, make them bound" (the user, 2026-11-08, "A: fixed + leash"). Now
 * it stands: only when the fighters' point is more than LEASH from it is it
 * pushed along, by just as much (as Paul's walls follow, the whole way;
 * Tekken 3 hides it on an even gravel floor); at a round's start, or a jump
 * (a new fight), it is the point itself. And, as the Church's, the scenery
 * is not turned by the fight's axis (below).
 *
 * How: for the call of the PS1's stage routine (0x8006D014, src/tekken2/
 * tmst_tekken2_mirror.c; the engine draws the arcade stage as it returns),
 * both fighters' x and z and their four places of history are moved by the
 * same (anchor - point), and given back after it (and after the Remake's
 * extras, drawn in the same frame). The routine reads them only to keep the
 * history (written moved, given back moved back) and to place the PS1's own
 * tiles (the engine drops them); its axis step (0x8006E1B8) reads the camera
 * and 0x800ADC10 / 0x800ADC18, not the fighters. The floor is drawn by the
 * floor routine, outside this call: the world's as before. The Remake only
 * (the Classic has no scenery); not while paused (the anchor stays). */
#include "tmst_core.h"
#include "mod_plugins.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

int tmst_tekken2_remake_drawn(void);
int32_t tmst_tekken2_fixed_period(void);
const int16_t *tmst_tekken2_walls(int *n, int *radius);

enum { P1_X = 0x800aa190, P1_Z = 0x800aa198, P2_X = 0x800aba1c, P2_Z = 0x800aba24, HISTORY_P1 = 0x800a08f8,
       HISTORY_P2 = 0x800a0938, FIXED_STAGE = 0x800afaa1, PAUSED = 0x80095494, PHASE = 0x800ae224,
       SCREEN = 0x800ae204, AXIS = 0x800a9188 };
static const double LEASH = 4000.0;        /* world units (1000 a metre): the point this far, the scenery stands
                                              (the Church's measure: 4 m off the middle, none of it dropped) */
static const double JUMP = 6000.0;         /* farther: a new fight or round, the anchor is the point */

static double ax, az;
static int anchored, moved, last_phase = -1;
static int32_t dx, dz;
/* The axis (both kinds): */
static int frozen, axis_saved, axis_step;
/* A stage fixed to the floor (FIXW: the Church, the user's, 2026-11-05: "the
 * fixed position, upstream-independent, on this stage with its objects"),
 * as the old fork's stage_anchor: its anchor the multiple of the floor's
 * period (its pattern the same at each) nearest the fighters' midpoint,
 * moved a whole period only when the midpoint is 0.6 of one away; and its
 * scenery unturned by the fighters' axis (Tekken 3 turns the stage, the
 * sky and the tiles, by it as the fighters circle; the floor it does not).
 * The axis: 0 as the engine's wrapper starts (its far background's
 * yaw - axis), then, as the routine comes back from its axis step
 * (0x8006E1B8, v0 the step, at 0x8006D110: tmst_tekken2_anchor_step), minus
 * the step, so that the routine's add leaves it 0 for the drawing; given
 * back after, the step added (the fight's axis goes on as it would).
 * The Remake's other stages the same (frozen): standing in the world, they
 * must not turn round the fighters either. */
static int fixed;

/* The point the engine's stage follows (tile_matrix's pick). */
static void engine_point(int *x, int *z) {
    int p1x = (int)psx_mod_read_word(P1_X), p1z = (int)psx_mod_read_word(P1_Z);
    int p2x = (int)psx_mod_read_word(P2_X), p2z = (int)psx_mod_read_word(P2_Z);
    int pick = -1;
    if (psx_mod_read_half(0x800a923eu) == 4 && psx_mod_read_byte(psx_mod_read_word(0x800a927cu) + 0x24) == 'S') pick = 0;
    else if (psx_mod_read_half(0x800aaacau) == 4 && psx_mod_read_byte(psx_mod_read_word(0x800aab08u) + 0x24) == 'S') pick = 1;
    if (pick >= 0) {
        uint32_t h = pick ? HISTORY_P2 : HISTORY_P1;
        int hx = 0, hz = 0;
        for (unsigned k = 0; k < 4; k++) { hx += (int)psx_mod_read_word(h + 16 * k); hz += (int)psx_mod_read_word(h + 8 + 16 * k); }
        int ox = pick ? p1x : p2x, oz = pick ? p1z : p2z;
        *x = ((hx >> 2) + ox) >> 1; *z = ((hz >> 2) + oz) >> 1;
    } else {
        *x = (p2x + p1x) >> 1; *z = (p2z + p1z) >> 1;
    }
}

static void add(uint32_t a, int32_t d) { psx_mod_write_word(a, (uint32_t)((int32_t)psx_mod_read_word(a) + d)); }
static void shift(int32_t sx, int32_t sz) {
    add(P1_X, sx); add(P1_Z, sz); add(P2_X, sx); add(P2_Z, sz);
    for (unsigned k = 0; k < 4; k++) {
        add(HISTORY_P1 + 16 * k, sx); add(HISTORY_P1 + 8 + 16 * k, sz);
        add(HISTORY_P2 + 16 * k, sx); add(HISTORY_P2 + 8 + 16 * k, sz);
    }
}

static void freeze(void) {
    axis_saved = (int)psx_mod_read_word(AXIS); axis_step = 0;
    psx_mod_write_word(AXIS, 0);
    frozen = 1;
    moved = 1;
}

/* As the stage routine starts: the anchor a step on, the fighters moved to it. */
int tmst_tekken2_anchor_begin(void) {
    static int off = -1;        /* bench: TMST_T2_ANCHOR=0, the engine's own point (the before of a before / after) */
    if (off < 0) { const char *e = getenv("TMST_T2_ANCHOR"); off = e && *e == '0'; }
    moved = 0;
    if (off) return 0;
    if (!tmst_tekken2_remake_drawn() || psx_mod_read_byte(FIXED_STAGE) || psx_mod_read_word(SCREEN) != 8) {
        anchored = 0;
        return 0;
    }
    int px, pz;
    engine_point(&px, &pz);
    int phase = (int16_t)psx_mod_read_half(PHASE);
    int32_t P = tmst_tekken2_fixed_period();
    fixed = P > 0;
    if (fixed) {
        int m[2] = { px, pz };
        double *a[2] = { &ax, &az };
        for (int k = 0; k < 2; k++)
            if (!anchored || fabs(m[k] - *a[k]) > P / 10 * 6) *a[k] = floor((double)m[k] / P + 0.5) * P;
        anchored = 1;
        last_phase = phase;
        dx = (int32_t)lround(ax) - px; dz = (int32_t)lround(az) - pz;
        shift(dx, dz);
        freeze();
        return 1;
    }
    double ex = px - ax, ez = pz - az, d = sqrt(ex * ex + ez * ez);
    if (!anchored || d > JUMP || (phase < 6 && last_phase >= 6)) {
        ax = px; az = pz; anchored = 1;
    } else if (!psx_mod_read_word(PAUSED) && d > LEASH) {
        ax = px - ex * LEASH / d; az = pz - ez * LEASH / d;
    }
    last_phase = phase;
    dx = (int32_t)lround(ax) - px; dz = (int32_t)lround(az) - pz;
    shift(dx, dz);
    freeze();
    return 1;
}

/* The scenery as walls (COLL, on a stage fixed to the floor: the Church),
 * the old fork's collide() (the user's, 2026-11-05: "the Church as I made
 * it in the other project: fixed assets, collision detection"): each
 * fighter's own place (its root, 0x800A9228, P2's 0x188C on; 0x800AA190
 * and the rest are copies the game makes again from it) pushed out of
 * every shape (0 a circle: x, z, radius; 1 a box: x, z, half width, half
 * depth, turn of 4096; cm in the scenery's frame, here unturned: the
 * world's, from the anchor), the fighters' radius round it, the shortest
 * way, once a stage frame: they stop at a pew, a column, the altar as at a
 * wall. */
enum { ROOT_P1 = 0x800a9228, ROOT_NEXT = 0x188c };
static void collide(void) {
    int n = 0, radius = 0;
    const int16_t *c0 = tmst_tekken2_walls(&n, &radius);
    if (!c0 || n <= 0 || psx_mod_read_word(SCREEN) != 8) return;
    double fr = 10.0 * radius;
    for (int f = 0; f < 2; f++) {
        uint32_t root = ROOT_P1 + ROOT_NEXT * (uint32_t)f;
        double x = (int32_t)psx_mod_read_word(root) - ax, z = (int32_t)psx_mod_read_word(root + 8) - az;
        int pushed = 0;
        for (int i = 0; i < n; i++) {
            const int16_t *c = c0 + 6 * i;
            double cx = 10.0 * c[1], cz = 10.0 * c[2], ex = x - cx, ez = z - cz;
            if (c[0] == 0) {
                double r = 10.0 * c[3] + fr, d = sqrt(ex * ex + ez * ez);
                if (d >= r) continue;
                if (d < 1) { ex = 1; ez = 0; d = 1; }
                x = cx + ex * r / d; z = cz + ez * r / d; pushed = 1;
            } else {
                double a = c[5] * (6.283185307179586 / 4096.0), ux = cos(a), uz = -sin(a);   /* the box's x */
                double u = ex * ux + ez * uz, w = -ex * uz + ez * ux;                        /* its z: (-uz, ux) */
                double hu = 10.0 * c[3] + fr, hw = 10.0 * c[4] + fr;
                if (fabs(u) >= hu || fabs(w) >= hw) continue;
                if (hu - fabs(u) < hw - fabs(w)) u = u < 0 ? -hu : hu;
                else w = w < 0 ? -hw : hw;
                x = cx + u * ux - w * uz; z = cz + u * uz + w * ux; pushed = 1;
            }
        }
        if (pushed) {
            psx_mod_write_word(root, (uint32_t)(int32_t)lround(x + ax));
            psx_mod_write_word(root + 8, (uint32_t)(int32_t)lround(z + az));
        }
    }
}

/* Once the stage (and the Remake's extras) is drawn: the fighters back. */
void tmst_tekken2_anchor_end(void) {
    if (!moved) return;
    moved = 0;
    shift(-dx, -dz);
    if (frozen) {       /* the axis back, its step on (0 now if the step came through us) */
        int now = (int)psx_mod_read_word(AXIS);
        psx_mod_write_word(AXIS, (uint32_t)((axis_saved + axis_step + now) & 0xFFF));
        frozen = 0;
    }
    if (fixed) {
        fixed = 0;
        if (!psx_mod_read_word(PAUSED)) collide();
    }
}

/* The stage routine back from its axis step (0x8006D110, v0 the step): the
 * axis made 0 by the routine's own add. */
void tmst_tekken2_anchor_step(uint32_t v0) {
    if (!moved || !frozen) return;
    axis_step = (int)v0;
    psx_mod_write_word(AXIS, (uint32_t)(-(int)v0 & 0xFFF));
}
