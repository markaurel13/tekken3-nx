/* TMST core: a free camera over a paused fight (tmst_camera.h). */
#include "tmst_camera.h"
#include "tmst_core.h"
#include "tmst_compat.h"
#include "cpu_state.h"
#include "mod_plugins.h"
#include <math.h>
#include <string.h>

enum { SLOT0 = 0x800a02a8u, VIEW = 0x800a85c0u, VIEW_PITCH = 0, VIEW_YAW = 4, VIEW_X = 20, VIEW_Y = 24, VIEW_Z = 28,
       FULL_DRAW = 0x800954a4u, FIGHTERS = 0x800adca8u, FIGHTER_BYTES = 16,
       P1 = 0x800a9228u, P2 = 0x800a9228u + 6284u, SHOW_HUD = 4816,
       HUD = 0x8003d6b0u, BEFORE_END = 0x8002b650u, END_RETURN = 0x8002b658u, GTE_H = 26,
       MODE = 0x800afa88u, BALL = 7, BALL_DRAW = 0x800b094cu,   /* Tekken Ball's overlay: the ball, a0 / a1 the fighters */
       TILE = 0x8009e780u, TILES_X = 0x8009e824u, TILES_Z = 0x8009e828u, ARCADE_CELL = 2500, ARCADE_CELLS = 10 };

static TmstCamera ours;
static int free_on, hud_on, ending, fighters_hidden, ball_hud_hidden;          /* ending: frames still drawn the whole way, the game's view back */
static int saved;                            /* the game's values below are kept */
static uint32_t game_view[8], game_flag, game_h;
static int have_h, hud_done, ball_done;

void tmst_camera_game(TmstCamera *c) {
    int32_t *v = &c->x;
    for (int i = 0; i < 6; i++) v[i] = (int32_t)psx_mod_read_word(SLOT0 + 4u * (uint32_t)i);
}

void tmst_camera_fighters(int32_t *x, int32_t *y, int32_t *z) {
    int32_t sx = 0, sy = 0, sz = 0;
    for (uint32_t p = 0; p < 2; p++) {
        sx += (int32_t)psx_mod_read_word(FIGHTERS + p * FIGHTER_BYTES);
        sy += (int32_t)psx_mod_read_word(FIGHTERS + p * FIGHTER_BYTES + 4);
        sz += (int32_t)psx_mod_read_word(FIGHTERS + p * FIGHTER_BYTES + 8);
    }
    *x = sx / 2; *y = sy / 2; *z = sz / 2;
}

/* The PS1 floor (0x80048E58 / 0x80049848) is a grid of TILES_X x TILES_Z
 * tiles of TILE (1 800 in Gon's arena), its centre snapped to a tile; the
 * Expanded's T3 arcade stages draw theirs instead, 10 x 10 cells of 2 500
 * (tekken3_arcade_stages.c, arcade_floor). Both are centred on the
 * fighters' midpoint (or, the camera high and looking down, where it looks;
 * a step back looking up). */
int tmst_camera_floor(int32_t *half_x, int32_t *half_z, int32_t *cell) {
    if (tmst_compat_arcade_stage() >= 0) {
        *half_x = *half_z = ARCADE_CELL * ARCADE_CELLS / 2; *cell = ARCADE_CELL;
        return 1;
    }
    int32_t t = (int32_t)psx_mod_read_word(TILE), nx = (int32_t)psx_mod_read_word(TILES_X),
            nz = (int32_t)psx_mod_read_word(TILES_Z);
    if (t <= 0 || t > 10000 || nx <= 0 || nx > 16 || nz <= 0 || nz > 16) { *half_x = *half_z = *cell = 0; return 0; }
    *half_x = t * nx / 2; *half_z = t * nz / 2; *cell = t;
    return 1;
}

void tmst_camera_free(const TmstCamera *c, int hud) {
    if (!saved) {
        for (int i = 0; i < 8; i++) game_view[i] = psx_mod_read_word(VIEW + 4u * (uint32_t)i);
        game_flag = psx_mod_read_word(FULL_DRAW);
        saved = 1; have_h = 0;
    }
    ours = *c; hud_on = hud; free_on = 1; ending = 0;
    psx_mod_write_word(FULL_DRAW, 1);
}

void tmst_camera_hide_fighters(int on) { fighters_hidden = on; }
void tmst_camera_hide_ball_hud(int on) { ball_hud_hidden = on; }

/* Tekken Ball's overlay (0x800B094C, paused): the power bars (0x800B38DC,
 * 0x800B49A8) and the floating numbers (0x800B3E8C: its list at 0x800B6790,
 * paused only drawn, through the text routine 0x8004CE74) are calls of their
 * own; left out, while our call runs, by a nop in place of the jal (the
 * overlay checked first: its words there). */
static const struct { uint32_t at, jal; } ball_hud_calls[] = {
    {0x800b0a4cu, 0x0c02ce37u},   /* jal 0x800B38DC */
    {0x800b0abcu, 0x0c02d26au},   /* jal 0x800B49A8 */
    {0x800b0af0u, 0x0c02cfa3u},   /* jal 0x800B3E8C: the numbers */
};
static int ball_hud_patched;
static void ball_hud_patch(int on) {
    if (on == ball_hud_patched) return;
    for (size_t i = 0; i < sizeof ball_hud_calls / sizeof *ball_hud_calls; i++) {
        uint32_t want = on ? ball_hud_calls[i].jal : 0;
        if (psx_mod_read_word(ball_hud_calls[i].at) != want) return;   /* not the overlay we know */
    }
    for (size_t i = 0; i < sizeof ball_hud_calls / sizeof *ball_hud_calls; i++)
        psx_mod_write_code_word(ball_hud_calls[i].at, on ? 0 : ball_hud_calls[i].jal);
    ball_hud_patched = on;
}

void tmst_camera_restore(void) {
    if (!saved || !free_on) return;
    free_on = 0; fighters_hidden = 0;
    for (int i = 0; i < 8; i++) psx_mod_write_word(VIEW + 4u * (uint32_t)i, game_view[i]);
    hud_on = 1;
    ending = 2;                             /* the game's view, drawn the whole way once */
}

void tmst_camera_vblank(void) {
    if (!ending || --ending) return;
    psx_mod_write_word(FULL_DRAW, game_flag);
    saved = 0;
}

/* The start of the scene's drawing: our view and H. */
extern void __tmst_chain_func_80048240(CPUState *cpu);
#ifdef TMST_BRIDGE_func_80048240
extern void __real_func_80048240(CPUState *cpu);
void __tmst_chain_func_80048240(CPUState *cpu) { __real_func_80048240(cpu); }
#endif
void __wrap_func_80048240(CPUState *cpu) {
    if (cpu->pc == 0 && saved) {
        if (!have_h) { game_h = cpu->gte_ctrl[GTE_H]; have_h = 1; }
        if (free_on) {
            psx_mod_write_word(VIEW + VIEW_PITCH, (uint32_t)(ours.pitch >> 6));
            psx_mod_write_word(VIEW + VIEW_YAW, (uint32_t)(ours.yaw >> 6));
            psx_mod_write_word(VIEW + VIEW_X, (uint32_t)ours.x);
            psx_mod_write_word(VIEW + VIEW_Y, (uint32_t)ours.y);
            psx_mod_write_word(VIEW + VIEW_Z, (uint32_t)ours.z);
            cpu->gte_ctrl[GTE_H] = (uint32_t)(ours.h > 0 ? ours.h : 1) & 0xffffu;
        } else {
            cpu->gte_ctrl[GTE_H] = game_h;
        }
    }
    __tmst_chain_func_80048240(cpu);
}

/* A fighter's drawing (0x8003A818, a0 its record: from 0x8002BA64, for
 * each fighter), left out while the fighter is behind our eye or nearer
 * than NEAR in front of it: the game never sees a fighter there, and the
 * GTE draws such a model's vertices behind the eye as shards across the
 * screen. */
#define NEAR 900.0
static int behind(uint32_t record) {
    int p = record == P1 ? 0 : record == P2 ? 1 : -1;
    if (p < 0) return 0;
    double fx = (int32_t)psx_mod_read_word(FIGHTERS + (uint32_t)p * FIGHTER_BYTES);
    double fz = (int32_t)psx_mod_read_word(FIGHTERS + (uint32_t)p * FIGHTER_BYTES + 8);
    double a = ours.yaw * 6.283185307179586 / TMST_TURN;
    double depth = (fx - ours.x) * -sin(a) + (fz - ours.z) * cos(a);   /* along the view, on the floor plane */
    return depth < NEAR;
}
extern void __tmst_chain_func_8003A818(CPUState *cpu);
#ifdef TMST_BRIDGE_func_8003A818
extern void __real_func_8003A818(CPUState *cpu);
void __tmst_chain_func_8003A818(CPUState *cpu) { __real_func_8003A818(cpu); }
#endif
/* Tekken 2's Wireframe Mode (src/tekken2/tmst_tekken2_wireframe.c): around a
 * fighter's drawing, entry and return. */
int tmst_t2wf_draw_begin(uint32_t record) __attribute__((weak));
void tmst_t2wf_draw_end(uint32_t record) __attribute__((weak));
void __wrap_func_8003A818(CPUState *cpu) {
    static uint32_t ret, sp, record;
    static int around;
    if ((cpu->pc == 0 || cpu->pc == 0x8003a818u) && free_on && (fighters_hidden || behind(cpu->gpr[4]))) {
        cpu->pc = cpu->gpr[31]; return;
    }
    if (cpu->pc == 0 || cpu->pc == 0x8003a818u) {
        record = cpu->gpr[4]; ret = cpu->gpr[31]; sp = cpu->gpr[29];
        /* (not under our free camera: a photo shows the fighters as they are) */
        around = !free_on && tmst_t2wf_draw_begin && tmst_t2wf_draw_begin(record);
    }
    __tmst_chain_func_8003A818(cpu);
    if (!around || cpu->pc != ret || cpu->gpr[29] != sp) return;
    around = 0;
    tmst_t2wf_draw_end(record);
}

/* The end of the scene's drawing in the whole-screen pause frame (called
 * from 0x8002B650, returning to 0x8002B658): first the HUD routine, as the
 * plain pause frame calls it (both fighters' HUD flag set, a0 / a1 the
 * fighters), coming back to 0x8002B650, which calls this again; then, in
 * Tekken Ball, its overlay's ball (0x800B094C, a0 / a1 the fighters, as at
 * 0x8002B6C8), the same way. */
extern void __tmst_chain_func_8004A6BC(CPUState *cpu);
#ifdef TMST_BRIDGE_func_8004A6BC
extern void __real_func_8004A6BC(CPUState *cpu);
void __tmst_chain_func_8004A6BC(CPUState *cpu) { __real_func_8004A6BC(cpu); }
#endif
void __wrap_func_8004A6BC(CPUState *cpu) {
    if (cpu->pc == 0 && saved && cpu->gpr[31] == END_RETURN) {
        if (hud_on && !hud_done) {
            hud_done = 1;
            psx_mod_write_word(P1 + SHOW_HUD, 1);
            psx_mod_write_word(P2 + SHOW_HUD, 1);
            cpu->gpr[4] = P1; cpu->gpr[5] = P2;
            cpu->gpr[31] = BEFORE_END;
            cpu->pc = HUD;
            return;
        }
        if (!ball_done && psx_mod_read_word(MODE) == BALL) {
            ball_done = 1;
            ball_hud_patch(free_on && ball_hud_hidden);
            cpu->gpr[4] = P1; cpu->gpr[5] = P2;
            cpu->gpr[31] = BEFORE_END;
            cpu->pc = BALL_DRAW;
            return;
        }
        ball_hud_patch(0);
        hud_done = ball_done = 0;
    }
    __tmst_chain_func_8004A6BC(cpu);
}
