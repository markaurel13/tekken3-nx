/* tmst.tournament: the story told before a tournament's draw.
 *
 * addons/tmst/data/tournament/story/<id>.txt (UTF-8; the T1, T2 and T3
 * tournaments have one, TEKKEN TAG and FREE none): its first line is the
 * panel's header (in the screens' title font: capitals and digits), the rest
 * the text, paragraphs between empty lines (a paragraph's lines are one).
 * The screens cut it into lines as wide as the panel (tmst_story_wrap). */
#ifndef TMST_TOURNAMENT_STORY_H
#define TMST_TOURNAMENT_STORY_H
#ifdef __cplusplus
extern "C" {
#endif

enum { TMST_STORY_TEXT = 8192, TMST_STORY_LINES = 200 };
typedef struct {
    char title[64];
    char body[TMST_STORY_TEXT];          /* the paragraphs, '\n' between them */
    char out[TMST_STORY_TEXT];           /* the lines cut, NUL after each; "" a paragraph's gap */
    int line[TMST_STORY_LINES];          /* each line's start in out */
    int lines;
    int wrapped_w;                       /* the width cut at, 0 not cut */
} TmstStory;

/* The file's text read into s: 1 when it has a header or a text. */
int tmst_story_parse(TmstStory *s, const char *text);
/* The body cut into lines no wider than max_w (a word wider stays alone on
 * its line) as `width` measures them; one empty line between paragraphs. */
void tmst_story_wrap(TmstStory *s, int max_w, int (*width)(const char *text, void *ctx), void *ctx);

#ifdef __cplusplus
}
#endif
#endif
