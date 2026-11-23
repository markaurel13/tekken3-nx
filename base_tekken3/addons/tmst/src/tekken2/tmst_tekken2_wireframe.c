/* tmst.tekken2.wireframe: Tekken 2 Content's Wireframe Mode, Tekken 2's
 * hidden mode rebuilt.
 *
 * Tekken 2 (the PS1's hidden mode, watched frame by frame): the player's own
 * fighter is drawn as green edges, no fill, its shadow kept; the camera sits
 * just behind the fighter's head, turned the way the body faces, and follows
 * the body (down when it crouches or falls); the replays and the winner's
 * pose keep the game's camera, the fighter still green.
 *
 * Here not by a button at the selector (Tekken 2: L1 + L2) but GAME
 * OPTION's TEKKEN 2 WIREFRAME MODE row, OFF / ON (the user's choice,
 * 2026-11-06), kept beside the memory card in t2-wireframe.txt
 * ("wireframe on"). Its fights: Arcade, Team Battle (the Tournament is one),
 * Time Attack, Survival and Practice (the user's wish, 2026-11-06: its
 * timer stands, its lives refill, its menu is the pause; measured), with
 * one human (0x800AFAA5's two low bits, one
 * a side each: measured; a challenger in the Arcade or a Team Battle of two
 * players sets both, and that fight keeps the game's own look).
 *
 * - The camera. The fight's camera routine (func_80064E10, the Expanded's
 *   generated copy, chained) builds slot `index` (0x800A02A8 + 24 index:
 *   x, y, z, pitch, yaw, H) from its own slot of the frame before; the view
 *   the frame draws from (0x800A85C0) is then built from slot 0 in the fight
 *   and from slot 1 during a throw (measured). Slot 0 is ours as the routine
 *   returns: the next frame's routine eases from it, so the game's camera
 *   comes back smoothly when ours stops. The yaw given to the routine at its
 *   entry is ours too (the Expanded's panorama takes it there).
 * - The fighter. Its drawing (0x8003A818, a0 its record; hooked from
 *   src/core/tmst_camera.c) links its packets into the ordering table; the
 *   table's heads are kept at the entry, and at the return every packet
 *   linked since is a polygon of this fighter: each becomes a closed green
 *   line strip, in a buffer of ours, linked where the polygon was (the
 *   polygon left as an empty packet).
 * - The pad. The game turns the d-pad's RIGHT into forward or back by the
 *   side of the screen the fighter stands on (its record's +0xC6: 0 RIGHT
 *   forward, 1 RIGHT back; measured), judged from the camera: with ours just
 *   behind the head, the opponent straight ahead, that side swings (18
 *   changes in 160 frames of a fight, against 1 with the game's camera). So
 *   the human's RIGHT and LEFT change places whenever +0xC6 says RIGHT is
 *   back: RIGHT always forward, LEFT always back, the whole round (the throws
 *   and the KO too: the user's wish, 2026-11-10); UP / DOWN (the side-steps)
 *   as the game gives them; not in the pause (its menus).
 *
 * Bench: TMST_T2_WIREFRAME=1 takes the row as ON (the fights' rule kept)
 * and logs, every 120 frames, how much the camera moves against the one
 * that would copy the root bone and the head; TMST_T2_WF_BACK / _UP / _H /
 * _PITCH / _SHADOW / _YAW_FOLLOW / _PLACE_FOLLOW / _DEAD the camera's and
 * the shadow's numbers. */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "tmst_camera.h"
#include "tmst_input.h"
#include "cpu_state.h"
#include "memcard.h"
#include "mod_plugins.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef TMST_HAS_GAME_OPTIONS
#include "tekken3_game_options.h"
#endif

enum { SCREEN = 0x800ae204, PHASE = 0x800ae224, MODE = 0x800afa88, HUMAN = 0x800afa88 + 0x1d,
       LIFE_P1 = 0x800a961e, LIFE_P2 = 0x800aaeaa, TIMER = 0x800adbcc,
       SLOT0 = 0x800a02a8, SLOT_BYTES = 24, RECORD = 0x800a9228, RECORD_STRIDE = 0x188c,
       BONES = 0x8f4, BONE_BYTES = 68, BONE_ROOT = 0, BONE_HEAD = 2,
       DRAWENV = 0x800a8c54, OT_ENTRIES = 1024, OPTIONS = 5, FIGHT = 8, FIGHTING_PHASE = 6,
       SIDE = 0xc6, PAUSED = 0x80095494 };
/* The modes (0x800AFA88; GAME OPTION's lamps the same bits): the Tournament
 * plays as a Team Battle. */
enum { ARCADE = 0, VS = 1, TEAM_BATTLE = 2, TIME_ATTACK = 3, SURVIVAL = 4, PRACTICE = 5,
       ROW_MODES = 1u << ARCADE | 1u << TEAM_BATTLE | 1u << TIME_ATTACK | 1u << SURVIVAL | 1u << PRACTICE };
enum { OFF, ON, VALUES };
static const char *const labels[VALUES] = { "OFF", "ON" };
#define ROW_NAME "TEKKEN 2 WIREFRAME MODE"

/* The camera, behind the head (the game's units: y grows downwards). */
static double back = 700.0, up = 150.0, focal = 380.0;
static int32_t pitch_down = 0;
static uint32_t shadow_rank = 1000;          /* ranks from here on are left as they are (the shadow: 1007) */
static uint32_t colour = 0x00e030u;          /* 0xBBGGRR */

static int part;                             /* the feature, at activation */
static int wanted = OFF, loaded;             /* GAME OPTION's row; t2-wireframe.txt read */
static int bench = -1;
static int active;                           /* this frame: our camera and the green fighter */
static int human = -1;                       /* the record's index (0 / 1) */
static int throw_seen;                       /* slot 1 built this frame: the game's throw camera */
static int was_active;                       /* our camera the frame before */
static double yaw_follow, place_follow, dead_degrees;
static unsigned frame;

int tmst_tekken2_on(void);                   /* src/tekken2/tmst_tekken2.c: the package's switch */

static int bench_on(void) {
    if (bench < 0) {
        const char *e = getenv("TMST_T2_WIREFRAME");
        bench = e && *e == '1';
        const char *v;
        if ((v = getenv("TMST_T2_WF_BACK"))) back = atof(v);
        if ((v = getenv("TMST_T2_WF_UP"))) up = atof(v);
        if ((v = getenv("TMST_T2_WF_H"))) focal = atof(v);
        if ((v = getenv("TMST_T2_WF_PITCH"))) pitch_down = atoi(v);
        if ((v = getenv("TMST_T2_WF_SHADOW"))) shadow_rank = (uint32_t)atoi(v);
        if ((v = getenv("TMST_T2_WF_YAW_FOLLOW"))) yaw_follow = atof(v);
        if ((v = getenv("TMST_T2_WF_PLACE_FOLLOW"))) place_follow = atof(v);
        if ((v = getenv("TMST_T2_WF_DEAD"))) dead_degrees = atof(v);
        if (bench) tmst_log("tekken2: Wireframe Mode bench on (back %.0f, up %.0f, H %.0f, pitch %d)",
                            back, up, focal, (int)pitch_down);
    }
    return bench;
}

/* ---- the row, kept beside the memory card (as the Expanded's
 * stage-select.txt) ---- */

static int setting_path(char *out, size_t size) {
    const char *card = NULL; out[0] = 0;
    if (memcard_debug_info(0, &card, NULL, NULL, NULL) || !card || !*card) return 0;
    const char *slash = strrchr(card, '/'), *bs = strrchr(card, '\\');
    if (bs && (!slash || bs > slash)) slash = bs;
    int n = slash ? (int)(slash - card) : 1;
    snprintf(out, size, "%.*s/t2-wireframe.txt", n, slash ? card : ".");
    return 1;
}
static void save(void) {
    char path[4096];
    FILE *f = setting_path(path, sizeof path) ? fopen(path, "wb") : NULL;
    if (!f) return;
    fprintf(f, "wireframe %s\n", wanted == ON ? "on" : "off");
    fclose(f);
}
/* Once, as soon as the memory card's place is known. */
static void load(void) {
    char path[4096];
    if (loaded || !setting_path(path, sizeof path)) return;
    loaded = 1;
    FILE *f = fopen(path, "rb");
    if (!f) return;
    char key[16], value[16], line[64];
    while (fgets(line, sizeof line, f))
        if (sscanf(line, "%15s %15s", key, value) == 2 && !strcmp(key, "wireframe"))
            wanted = !strcmp(value, "on") ? ON : OFF;
    fclose(f);
    if (wanted == ON) tmst_log("tekken2: " ROW_NAME " is ON (t2-wireframe.txt)");
}

/* GAME OPTION's row, added at the first visit of the Options screen. */
static void menu(void) {
#ifdef TMST_HAS_GAME_OPTIONS
    static int row = -2, shown;
    if (row == -2) {
        row = tekken3_game_options_add(ROW_NAME, labels, VALUES, (unsigned)wanted, OFF, ROW_MODES);
        shown = wanted;
    }
    if (row < 0) return;
    int v = tekken3_game_options_value(row);
    if (v >= 0 && v < VALUES && v != shown) {
        shown = wanted = v;
        save();
        tmst_log("tekken2: " ROW_NAME " set to %s", labels[v]);
    }
#endif
}

/* ---- which fights ---- */

static int16_t s16(uint32_t a) { return (int16_t)psx_mod_read_half(a); }

/* The part on and the row ON. */
static int enabled(void) { return part && tmst_tekken2_on() && (wanted == ON || bench_on()); }

/* Arcade, Team Battle (the Tournament), Time Attack, Survival, Practice; one human. */
static int our_fight(void) {
    uint32_t m = psx_mod_read_word(MODE);
    if (m > 31 || !(ROW_MODES >> m & 1u)) return 0;
    unsigned h = psx_mod_read_byte(HUMAN) & 3u;
    return h == 1 || h == 2;
}

/* Whose fighter is green: the human's side (bit 0 set: player 1). */
static int human_record(void) { return (psx_mod_read_byte(HUMAN) & 1u) ? 0 : 1; }

static int fighting(void) {
    if (psx_mod_read_word(SCREEN) != FIGHT || psx_mod_read_half(PHASE) < FIGHTING_PHASE) return 0;
    /* Not the pause (0x80095494; Practice's menus are one): paused, the game
     * computes no camera and draws no fighter, the screen stays the last
     * frame, which is to be ours, the fighter green (src/core/tmst_camera.h;
     * Photo Mode's free camera draws them solid). */
    if (s16(LIFE_P1) <= 0 || s16(LIFE_P2) <= 0 || psx_mod_read_half(TIMER) == 0) return 0;
    return 1;
}

/* RIGHT forward, LEFT back (see the top). */
static uint16_t pad(int player, uint16_t buttons) {
    if (!enabled() || !psx_mod_game_started() || psx_mod_read_word(SCREEN) != FIGHT || psx_mod_read_word(PAUSED) ||
        !our_fight()) return buttons;
    int p = human_record();
    if (player != p || !psx_mod_read_half(RECORD + (uint32_t)p * RECORD_STRIDE + SIDE)) return buttons;
    uint16_t lr = buttons & (TMST_PAD_LEFT | TMST_PAD_RIGHT);
    if (lr == TMST_PAD_LEFT || lr == TMST_PAD_RIGHT) buttons ^= TMST_PAD_LEFT | TMST_PAD_RIGHT;
    return buttons;
}

static void tick(void) {
    if (!part || !psx_mod_game_started()) return;
    load();
    if (psx_mod_read_word(SCREEN) == OPTIONS && tmst_tekken2_on()) menu();
}

static void activate(void) {
    part = tmst_guard("tmst.tekken2.wireframe", TMST_NEED_ACTIVATION | TMST_NEED_VBLANK | TMST_NEED_GAME_OPTIONS);
    if (part) tmst_input_add_filter(pad);
    if (part) tmst_log("tekken2: Wireframe Mode ready (GAME OPTION's " ROW_NAME " row)");
    (void)bench_on();                        /* its numbers, whatever the row says */
}

PSX_MOD_CONSTRUCTOR(tmst_register_tekken2_wireframe) {
    (void)psx_mod_register_activation_plugin("tmst.tekken2.wireframe", activate);
    (void)psx_mod_register_vblank_plugin("tmst.tekken2.wireframe", tick);
}

/* ---- the camera ---- */

/* The camera follows, it does not copy: the direction is the one the game
 * gives the fighter (its record's +0x2C, 0x10000 a turn, forward = sin,
 * cos; a TTT1 guest's too), not its root bone's (each move's own swing, and
 * a guest's root has its axes the other way round), and the head and that
 * direction are followed a part of the way each frame, the last few degrees
 * left alone: a combo's every blow turns the fighter 5 to 15 degrees and
 * puts its head forward and back (measured), which the camera need not
 * show. `fresh`: our camera starts (the round, after a throw): no follow. */
static double yaw_follow = 0.10, place_follow = 0.20, dead_degrees = 4.0;   /* bench: TMST_T2_WF_YAW_FOLLOW ... */
static double at_x, at_y, at_z, at_yaw;                  /* the head and the direction followed */
static const double TAU = 6.283185307179586;

static void head_camera(int p, TmstCamera *c, int fresh) {
    uint32_t record = RECORD + (uint32_t)p * RECORD_STRIDE, head = record + BONES + BONE_HEAD * BONE_BYTES;
    double hx = (int32_t)psx_mod_read_word(head + 0x14), hy = (int32_t)psx_mod_read_word(head + 0x18),
           hz = (int32_t)psx_mod_read_word(head + 0x1c);
    double a = psx_mod_read_half(record + 0x2c) * TAU / 65536.0;
    if (fresh) {
        at_x = hx; at_y = hy; at_z = hz; at_yaw = a;
    } else {
        double e = remainder(a - at_yaw, TAU), dead = dead_degrees * TAU / 360.0;
        e = e > dead ? e - dead : e < -dead ? e + dead : 0.0;
        at_yaw += e * yaw_follow;
        at_x += (hx - at_x) * place_follow;
        at_y += (hy - at_y) * place_follow;
        at_z += (hz - at_z) * place_follow;
    }
    double fx = sin(at_yaw), fz = cos(at_yaw);
    c->x = (int32_t)lround(at_x - fx * back);
    c->y = (int32_t)lround(at_y - up);
    c->z = (int32_t)lround(at_z - fz * back);
    double y = atan2(-fx, fz);                       /* the camera's: forward = (-sin yaw, cos yaw) */
    c->yaw = (int32_t)lround(y * TMST_TURN / TAU) & (TMST_TURN - 1);
    c->pitch = pitch_down;
    c->h = (int32_t)focal;
}

/* bench: the camera as phase 1 had it (the root bone's direction and the
 * head, each frame as they are), to measure against. */
static void bone_camera(int p, TmstCamera *c) {
    uint32_t r = RECORD + (uint32_t)p * RECORD_STRIDE + BONES;
    uint32_t head = r + BONE_HEAD * BONE_BYTES, root = r + BONE_ROOT * BONE_BYTES;
    double hx = (int32_t)psx_mod_read_word(head + 0x14), hy = (int32_t)psx_mod_read_word(head + 0x18),
           hz = (int32_t)psx_mod_read_word(head + 0x1c);
    double fx = s16(root + 6 * 2), fz = -s16(root + 0 * 2), n = sqrt(fx * fx + fz * fz);
    if (n < 1) { fx = 0; fz = 1; } else { fx /= n; fz /= n; }
    c->x = (int32_t)lround(hx - fx * back);
    c->y = (int32_t)lround(hy - up);
    c->z = (int32_t)lround(hz - fz * back);
    c->yaw = (int32_t)lround(atan2(-fx, fz) * TMST_TURN / TAU) & (TMST_TURN - 1);
    c->h = (int32_t)focal;
}
typedef struct { TmstCamera last; double turned, moved, turn_jerk, move_jerk, d, dx, dy, dz; unsigned counted, away; } Moves;
static Moves moves[2];
static void measure(Moves *m, const TmstCamera *c, int going) {
    if (!going) { memset(m, 0, sizeof *m); m->last = *c; return; }
    int32_t t = ((c->yaw - m->last.yaw) % TMST_TURN + TMST_TURN + TMST_TURN / 2) % TMST_TURN - TMST_TURN / 2;
    double d = t * 360.0 / TMST_TURN, dx = c->x - m->last.x, dy = c->y - m->last.y, dz = c->z - m->last.z;
    m->turned += fabs(d);
    m->moved += sqrt(dx * dx + dy * dy + dz * dz);
    if (m->counted) {
        m->turn_jerk += fabs(d - m->d);
        m->move_jerk += sqrt((dx - m->dx) * (dx - m->dx) + (dy - m->dy) * (dy - m->dy) + (dz - m->dz) * (dz - m->dz));
    }
    m->d = d; m->dx = dx; m->dy = dy; m->dz = dz; m->last = *c;
    m->counted++;
    /* the opponent (its record's x, z) out of the picture: past the screen's
     * half width (160) at the camera's H */
    uint32_t o = RECORD + (uint32_t)(1 - human) * RECORD_STRIDE;
    double ox = (int32_t)psx_mod_read_word(o) - c->x, oz = (int32_t)psx_mod_read_word(o + 8) - c->z;
    double y = c->yaw * TAU / TMST_TURN, ahead = -sin(y) * ox + cos(y) * oz, side = cos(y) * ox + sin(y) * oz;
    if (ahead <= 0 || fabs(side) * c->h > 160.0 * ahead) m->away++;
}

extern void __tmst_chain_func_80064E10(CPUState *cpu);
#ifdef TMST_BRIDGE_func_80064E10
extern void __real_func_80064E10(CPUState *cpu);
void __tmst_chain_func_80064E10(CPUState *cpu) { __real_func_80064E10(cpu); }
#endif
void __wrap_func_80064E10(CPUState *cpu) {
    static uint32_t ret, sp;
    static int ours;
    static TmstCamera cam;
    if (cpu->pc == 0 || cpu->pc == 0x80064e10u) {
        ours = 0;
        if (enabled()) {
            uint32_t index = psx_mod_read_word(cpu->gpr[29] + 0x18);     /* the slot it builds */
            if (index == 1) throw_seen = 1;
            if (index == 0) {
                frame++;
                active = fighting() && our_fight() && !throw_seen;
                throw_seen = 0;
                was_active = active && human == human_record() ? was_active : 0;
                human = human_record();
                if (active) {
                    head_camera(human, &cam, !was_active);
                    /* the routine's yaw keeps the game's turn count (the panorama takes it here) */
                    int32_t game = (int32_t)psx_mod_read_word(SLOT0 + 16);
                    int32_t d = ((cam.yaw - game) % TMST_TURN + TMST_TURN + TMST_TURN / 2) % TMST_TURN - TMST_TURN / 2;
                    cam.yaw = game + d;
                    cpu->gpr[7] = (uint32_t)cam.yaw;
                    cpu->gpr[6] = (uint32_t)cam.pitch;
                    ours = 1;
                }
            }
        }
        ret = cpu->gpr[31]; sp = cpu->gpr[29];
    }
    __tmst_chain_func_80064E10(cpu);
    if (!ours || cpu->pc != ret || cpu->gpr[29] != sp) return;
    ours = 0;
    const int32_t *v = &cam.x;
    for (uint32_t i = 0; i < 6; i++) psx_mod_write_word(SLOT0 + 4u * i, (uint32_t)v[i]);
    /* bench: how much it moves, against the camera that would copy the
     * root bone and the head each frame (the first one), over 120 frames */
    if (bench_on()) {
        static TmstCamera raw;
        bone_camera(human, &raw);
        measure(&moves[0], &raw, was_active);
        measure(&moves[1], &cam, was_active);
        if (moves[1].counted == 120) {
            tmst_log("tekken2: wireframe camera P%d, a frame: copied %.2f degrees %.1f units, jerk %.2f %.1f, "
                     "opponent out %u%%; followed %.2f %.1f, jerk %.2f %.1f, out %u%%", human + 1,
                     moves[0].turned / 120, moves[0].moved / 120, moves[0].turn_jerk / 119, moves[0].move_jerk / 119,
                     moves[0].away * 100 / 120, moves[1].turned / 120, moves[1].moved / 120,
                     moves[1].turn_jerk / 119, moves[1].move_jerk / 119, moves[1].away * 100 / 120);
            for (int i = 0; i < 2; i++)
                moves[i].turned = moves[i].moved = moves[i].turn_jerk = moves[i].move_jerk = 0, moves[i].away = 0,
                moves[i].counted = 1;
        }
    }
    was_active = 1;
}

/* ---- the green fighter ---- */

static uint32_t heads[OT_ENTRIES], ot_base;
/* The line strips: two buffers, one a drawing in turn (not a camera
 * frame: Practice's menus draw the fighters with no camera computed). */
static uint32_t bufs[2], buf_at, buf_end, drawings;
enum { BUF_BYTES = 96 * 1024, TERMINATOR = 0x55555555u };

/* src/core/tmst_camera.c, around the fighter's drawing (0x8003A818). */
int tmst_t2wf_draw_begin(uint32_t record) {
    if (!enabled() || !active || human < 0 || record != RECORD + (uint32_t)human * RECORD_STRIDE) return 0;
    ot_base = psx_mod_read_word(psx_mod_read_word(DRAWENV) + 4);
    if (!ot_base) return 0;
    for (uint32_t r = 0; r < OT_ENTRIES; r++) heads[r] = psx_mod_read_word(ot_base + 4 * r) & 0xffffffu;
    drawings++;
    buf_at = bufs[drawings & 1]; buf_end = buf_at + BUF_BYTES;
    return 1;
}

static uint32_t take(uint32_t words) {
    if (!bufs[0]) {
        bufs[0] = psx_mod_alloc_gpu_dma_memory(BUF_BYTES, 16);
        bufs[1] = psx_mod_alloc_gpu_dma_memory(BUF_BYTES, 16);
        if (!bufs[0] || !bufs[1]) return 0;
        buf_at = bufs[drawings & 1]; buf_end = buf_at + BUF_BYTES;
    }
    if (buf_at + 4 * words > buf_end) return 0;
    uint32_t a = buf_at; buf_at += 4 * words;
    return a;
}

/* A polygon packet at `p` (its tag's length `len`) into a closed line strip. */
static int outline(uint32_t p, uint32_t len) {
    uint32_t w1 = psx_mod_read_word(p + 4), code = w1 >> 24;
    if ((code & 0xe0) != 0x20) return 0;
    int quad = (code & 0x08) != 0, tex = (code & 0x04) != 0, gour = (code & 0x10) != 0;
    int n = quad ? 4 : 3, stride = 1 + tex + gour;
    if (len < (uint32_t)(1 + n * stride - gour)) return 0;
    uint32_t xy[4];
    for (int i = 0; i < n; i++) xy[i] = psx_mod_read_word(p + 4u * (uint32_t)(2 + i * stride));
    static const int order3[4] = {0, 1, 2, 0}, order4[5] = {0, 1, 3, 2, 0};
    const int *ord = quad ? order4 : order3;
    int pts = quad ? 5 : 4;
    uint32_t words = 1 + 1 + (uint32_t)pts + 1, q = take(words);
    if (!q) return 0;
    uint32_t next = psx_mod_read_word(p) & 0xffffffu;
    psx_mod_write_word(q, ((words - 1) << 24) | next);
    psx_mod_write_word(q + 4, 0x48000000u | colour);
    for (int i = 0; i < pts; i++) psx_mod_write_word(q + 8 + 4u * (uint32_t)i, xy[ord[i]]);
    psx_mod_write_word(q + 8 + 4u * (uint32_t)pts, TERMINATOR);
    psx_mod_write_word(p, q & 0xffffffu);                  /* the polygon: an empty packet on to ours */
    return 1;
}

void tmst_t2wf_draw_end(uint32_t record) {
    (void)record;
    unsigned packets = 0, lines = 0;
    for (uint32_t r = 0; r < OT_ENTRIES && r < shadow_rank; r++) {
        uint32_t p = psx_mod_read_word(ot_base + 4 * r) & 0xffffffu;
        for (int guard = 0; p != heads[r] && guard < 4096; guard++) {
            uint32_t a = 0x80000000u | p, tag = psx_mod_read_word(a), len = tag >> 24;
            if (len) { packets++; lines += (uint32_t)outline(a, len); }
            p = tag & 0xffffffu;
        }
    }
    if (bench_on() && frame % 120 == 1)
        tmst_log("tekken2: wireframe P%d: %u packets, %u outlined", human + 1, packets, lines);
}
