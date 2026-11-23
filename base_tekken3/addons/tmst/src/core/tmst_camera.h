/* TMST core: a free camera over a paused fight.
 *
 * Paused, Tekken 3 draws little: its pause frame redraws the band around its
 * menu (the drawing area of the list's start, y 20..181) and the HUD
 * (0x8003D6B0), and leaves the rest of the screen as the last fight frame;
 * the fighters are not drawn, nor any camera computed (neither the fight's
 * camera routine, func_80064E10, nor the matrix builder, 0x80064080, runs).
 * The same pause frame has a second way, taken while 0x800954A4 is set (the
 * replay's: 0x8003CB84's answer, computed each frame the fight runs, never
 * while paused): the whole scene, stage and fighters, drawn over the whole
 * screen from the frame's view at 0x800A85C0 (pitch >> 6, yaw >> 6, eye x,
 * y, z), projected at the GTE's H; no HUD.
 *
 * Free, the pause frame takes that way (0x800954A4 set; nothing the paused
 * fight runs reads it otherwise), the view is ours (written at the start of
 * each drawing, func_80048240: the Expanded wraps it, chained) and H the GTE's
 * from ours; the HUD routine is called as the plain pause frame calls it,
 * before 0x8004A6BC (chained) at the end of the scene's drawing, unless
 * hidden. In Tekken Ball the plain pause frame also calls its overlay's
 * 0x800B094C (the ball, its power bars and floating numbers; the whole-
 * screen way does not): called there too, after the HUD routine, its HUD's
 * three calls left out while hidden (tmst_camera_hide_ball_hud). A fighter
 * behind the eye, or too near in front, is not drawn
 * (0x8003A818, chained): the GTE would draw it as shards. Out, the view, H
 * and the flag are the game's again; one frame more
 * is drawn the whole way, from the game's own view, so the frozen screen
 * under the pause menu is the fight as it was.
 *
 * Camera units: the game's (y grows downwards; a fighter is about 1 000
 * tall), angles TMST_TURN a turn, H the projection's distance (500). */
#ifndef TMST_CAMERA_H
#define TMST_CAMERA_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct { int32_t x, y, z, pitch, yaw, h; } TmstCamera;
enum { TMST_TURN = 0x40000 };              /* a whole turn in the camera's angles */

/* The fight's camera now (its slot 0 at 0x800A02A8: as the paused frame
 * shows it). */
void tmst_camera_game(TmstCamera *out);
/* Free on: the paused frame draws the whole scene from `camera` (each call
 * changes it); `hud`: the game's HUD drawn too. */
void tmst_camera_free(const TmstCamera *camera, int hud);
/* Free on: nonzero leaves both fighters undrawn (a frame of the stage
 * alone). */
void tmst_camera_hide_fighters(int on);
/* Free on, Tekken Ball: nonzero leaves out the ball's own HUD (the power
 * bars at the bottom, the ball's damage number); the ball stays. */
void tmst_camera_hide_ball_hud(int on);
/* Free off: everything the game's again (see above). */
void tmst_camera_restore(void);
/* Every VBlank, from the VBlank plugin of the mod that uses it. */
void tmst_camera_vblank(void);
/* The fighters' midpoint (x, z) and their height (y). */
void tmst_camera_fighters(int32_t *x, int32_t *y, int32_t *z);
/* The floor drawn around the fighters' midpoint: half its width along x and
 * z, and its cell (the grid snaps its centre to a cell). 0: unknown. */
int tmst_camera_floor(int32_t *half_x, int32_t *half_z, int32_t *cell);

#ifdef __cplusplus
}
#endif
#endif
