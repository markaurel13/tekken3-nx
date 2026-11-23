/* tmst.tournament: the boss intros, a short cinematic before a boss's second
 * form (Kazuya, then Devil: the user's request, 2026-11-07). It plays in the
 * Expanded's cinematic engine (src/tekken3_cine.c, a .cine file); this file
 * writes that file, so it builds and is tested alone.
 *
 * Each boss has an entry: the parts it plays one after another (clips of
 * TTT1 combat packs: its own, or another fighter's of the same body, as the
 * player's install holds them in mods/ttt1; each from a frame of its clip,
 * at a speed, blended into from the last), the character it starts as and
 * the frame it becomes the boss (the engine's `swap`), a white flash round
 * that frame, the lightning aura, where it stands, the camera's keys and the
 * fades. The parts' own sounds (their records' sound lists) are cues the
 * game side plays from the fighters' TTT1 sound packs. Nothing is taken from
 * elsewhere. A boss without an entry gets the fallback: its stance, the
 * camera coming closer. The player's fighter is hidden. */
#ifndef TMST_TOURNAMENT_CINE_H
#define TMST_TOURNAMENT_CINE_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

/* A camera key: from `frame` on, eased to the next (y grows downwards). */
typedef struct { int frame; int16_t eye[3], target[3]; } TmstCineKey;
/* A part: a clip (record `clip` of the pack `pack`, a file stem such as
 * "Kazuya-TTT1"; NULL the boss's own; clip -1 its neutral, the stance)
 * from its frame `from`, looped at `speed` percent, for `frames` (60 a
 * second); its first `blend` frames eased from the last part's last pose. */
typedef struct { const char *pack; int clip, from, speed, frames, blend; } TmstCinePart;
enum { TMST_AURA_BONES = 8 };
typedef struct {
    const char *key;            /* the boss's fighter key; NULL: the fallback */
    const TmstCinePart *parts; int part_count;
    int frames;                 /* the length (past the parts: the last pose held) */
    int32_t place[4];           /* x, y, z where it stands, facing (0x10000 a turn; 0x8000: towards -z) */
    int fade_in, fade_out;      /* frames from black, to black */
    const TmstCineKey *cam; int cam_count;
    const char *start;          /* the character it starts as (a key: "kazuya"); NULL the boss all along */
    int swap;                   /* with `start`: the frame it becomes the boss */
    int flash;                  /* with `start`: a white flash, up for these frames to the swap and down after */
    int aura_from, aura_to, aura_step;          /* the lightning (its own hit effect) on these bones in turn */
    uint8_t aura_bones[TMST_AURA_BONES]; int aura_bone_count;
    const char *own;            /* the boss's own pack's stem ("Heihachi-TTT1": a native's TTT1 pack, its moves
                                   in the fight not TTT1); NULL: P2's TTT1 moves' (a guest's) */
} TmstBossIntro;

/* The entry for `key` (the fallback when it has none; never NULL). */
const TmstBossIntro *tmst_boss_intro(const char *key);

/* A pack's bytes by its stem (NULL: the boss's own, an entry without `own`),
 * NULL when missing. A part with no pack is the boss's: `own`, or NULL. */
typedef const uint8_t *(*TmstPackOf)(const char *stem, size_t *size, void *ctx);

/* The .cine for entry e into `out`, the poses from the packs `pack_of`
 * gives: actor 0 the player (ID, hidden), actor 1 the boss (ID, costume;
 * e->start until e->swap). A clip a pack does not hold (or a pack missing)
 * is the boss's neutral instead. Returns 1 when written. */
int tmst_boss_intro_write(FILE *out, const TmstBossIntro *e, TmstPackOf pack_of, void *ctx, unsigned player_id,
                          unsigned player_costume, unsigned boss_id, unsigned boss_costume);

/* A sound of the intro: at its frame, the TTT1 sound `index` of the pack
 * `stem`'s fighter (NULL: the boss's; its <stem>-sfx.jus). */
typedef struct { int frame; const char *stem; unsigned index; } TmstCineSound;
/* The parts' own sounds (each record's sound list: two sound events and
 * the clip's frame they play at; a TTT1 sound is the event's low 11 bits),
 * in order, at most `max`; returns how many. */
int tmst_boss_intro_sounds(const TmstBossIntro *e, TmstPackOf pack_of, void *ctx, TmstCineSound *out, int max);

#ifdef __cplusplus
}
#endif
#endif
