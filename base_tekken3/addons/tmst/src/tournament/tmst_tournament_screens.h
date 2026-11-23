/* tmst.tournament: the screens drawn on the art canvas (tmst_tournament_art.h).
 *
 * TOURNAMENT PROGRESS: the bracket as a pyramid, the 16 fighters along the
 * bottom, each winner one row up, the final boss at the top; winners' lines
 * gold, the losers grey where they fell; the next match marked, a strip at
 * the bottom with it and the buttons. The same pyramid,
 * finished, is TOURNAMENT OVER's, the player's line red where it broke.
 * The match view (the loading screen): both VS portraits, the round. The
 * CHAMPION: the player's portrait, the boss beaten, what is unlocked.
 *
 * Fighters are the caller's roster indices; their faces and names come
 * through ScreenRoster. */
#ifndef TMST_TOURNAMENT_SCREENS_H
#define TMST_TOURNAMENT_SCREENS_H
#include <stdint.h>
#include "tmst_bracket.h"
#include "tmst_tournament_story.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *(*key)(int fighter);
    const char *(*name)(int fighter);
} ScreenRoster;

typedef struct {
    const TmstBracket *bracket;
    const ScreenRoster *roster;
    const char *tourney;                  /* its name */
    uint32_t shown;                       /* matches whose winners are drawn (bit m) */
    uint32_t moving; int moving_t;        /* winners on the way up: their matches (bit m), 0..256 */
    int next;                             /* the match marked, -1 none */
    int blink;                            /* the mark lit this frame */
    int drawn;                            /* slots shown at the opening draw, 16 = all */
    int shuffle;                          /* while drawing: a frame count for the slots still turning */
    int over;                             /* TOURNAMENT OVER's pyramid */
    const char *prompt;                   /* the strip's button line, NULL none */
    int undrawn;                          /* the slots empty: the draw still to come (the story before it) */
} ScreenProgress;

void screen_progress(const ScreenProgress *s);
/* The story before the draw: the pyramid (its slots empty), over it a
 * see-through panel, the story's header, its text `offset` pixels up from
 * under the panel. Returns 1 once the text has come to its end (and stays). */
int screen_story(const ScreenProgress *s, TmstStory *story, int offset);
/* The canvas point a match sits at (its fighters' middle), for the zoom. */
void screen_match_point(int match, int *x, int *y);
/* The match view: the round, both fighters. */
void screen_match(const ScreenProgress *s, int match);
/* The champion: the player, the boss beaten, the unlocked tournament (NULL none). */
void screen_champion(const ScreenProgress *s, const char *unlocked);
/* Tournament over: the finished pyramid, who put the player out and where. */
void screen_over(const ScreenProgress *s);

/* The layout's boxes on the canvas, for the guide pictures. */
void screen_guide(int which);         /* 0 progress, 1 match, 2 champion, 3 over */

#ifdef __cplusplus
}
#endif
#endif
