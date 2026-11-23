/* Tekken 2 Content's Stages, Remake: the old fork's runtime extras of its
 * Tekken 2 stages (src/tekken3_arcade_stages.c of
 * C:\Games\UltimateTekken3\tekken3-expanded, the version stage-engine.zip
 * made the packs with), which the Expanded's stage engine does not read:
 *
 * - ANI2, ANI3, ANI4: animated palettes (New York's and Hong Kong's
 *   windows, the Pagoda's eyes and candles), their frames in the pack's own
 *   VRAM rectangles (grab_frames); mode 1: lit one by one, held until the
 *   next round;
 * - DUST: puffs where a fighter hits the ground;
 * - FFLY: fireflies, butterflies (kind 1), glowing bugs (2), dragonflies (3);
 * - FTHR: feathers (shape 0), petals falling (1), petals strewn on the floor
 *   (2), New York's newspapers (3), Serene Forest's leaves (4: strewn as 2,
 *   two falling from the trees), the Great Wall's dust in the wind (5);
 * - RAIN, DIYA: the Taj Mahal's rain and oil lamps;
 * - the objects (T2OB models, T2RT, T2OA the machine): Grand Canyon's eagle
 *   (200, EAGL), the grass (201, GRAS), Akihabara's ship (202, SHIP), the
 *   birds, crabs, rabbits and pigeons (203, GULL), Hong Kong's helicopter
 *   (11: Tekken 3's stage 11 machine, the whole fight as the old fork's).
 * FDIM (the fighters' tint and flicker) is src/tekken2/tmst_tekken2_stages.c's
 * (SetBackColor). The lightning comes later (phase 5).
 *
 * A port, 1:1: the old numbers, its random numbers (one generator for them
 * all, as snow_rand), its timing. Each frame of the stage, once the engine
 * has drawn it (0x8006D014 returns; src/tekken2/tmst_tekken2_mirror.c calls
 * tmst_tekken2_extras_draw): simulated (not in the pause), projected through
 * the tiles' GTE (the engine's tile_matrix, copied: they stand where its
 * scenery does), each a quad in the main ordering table: rank 40 in front of
 * the fighters, 1009 behind them (the depth decides, as the old), 980 lying
 * on the floor. Its own GPU memory, one half a frame (the OT buffer's
 * parity). A round's start (decided, then not): all anew, as the old
 * anim_round_start (there only on the stages with objects; here on every
 * one). The screen's edges: the wide screen's margin added. */
#include "tmst_core.h"
#include "tmst_compat.h"
#include "cpu_state.h"
#include "mod_plugins.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

uint64_t g_t2_extras_draw_ns = 0;
uint64_t g_t2_own_tiles_ns = 0;
uint64_t g_t2_objects_ns = 0;
uint64_t g_t2_fx_ns = 0;

static inline uint64_t t2_now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}

void tmst_tekken2_extras_load(const uint8_t *pack, long size);
void tmst_tekken2_extras_clear(void);
void tmst_tekken2_extras_tick(void);
int tmst_tekken2_extras_on(void);
void tmst_tekken2_extras_draw(const CPUState *cpu);
static void glass_load(const uint8_t *b, long size);     /* T2GL, below */
static void glass_clear(void);
static int glass_on(void);
static void own_tiles_load(const uint8_t *b, long size);  /* T2TL, below */
static void own_tiles_clear(void);
static int own_tiles_on(void);

enum { SCREEN_W = 368 };
#define CAMERA       0x800A85C0u
#define AXIS         0x800A9188u
#define OT_BUFFER    0x800ADEFCu
#define DRAWENV      0x800A8C54u
#define FIXED_STAGE  0x800AFAA1u
#define P1_X 0x800AA190u
#define P1_Y 0x800AA194u
#define P1_Z 0x800AA198u
#define P2_X 0x800ABA1Cu
#define P2_Y 0x800ABA20u
#define P2_Z 0x800ABA24u
#define HISTORY_P1 0x800A08F8u          /* 4 x (x, -, z, -) */
#define HISTORY_P2 0x800A0938u
#define STAGE_Y  50                     /* arcade 0x8017C454, every stage */
#define SIN_PS1  0x8001E8C4u
#define COS_PS1  0x8001F0C4u
#define PAUSED   0x80095494u            /* 1 while the pause menu is open */
#define SCREEN   0x800AE204u
#define PHASE    0x800AE224u
#define LIFE_P1  0x800A961Eu
#define LIFE_P2  0x800AAEAAu
#define TIMER    0x800ADBCCu
#define BUFFER_BYTES 0x30000u           /* packets of one OT buffer (objects, particles, a fixed stage's tiles) */

/* ---- the pack's sections ---- */

enum { XANIMS = 3 };
typedef struct {
    uint16_t x, y, w, h, period, frames, mode;
    uint16_t *pix;                      /* frames x w x h */
    unsigned count, frame;
} XAnim;
static struct {
    int loaded;
    int dust_on;
    uint16_t dust_tpage, dust_clut;
    uint8_t dust_u, dust_v;
    int16_t feather[6];
    uint8_t feather_rgb[3];
    int feather_shape, feather_on;
    uint16_t paper_tpage, paper_clut;
    uint8_t paper_u, paper_v, paper_w, paper_h;
    /* FTHR shape 6 (Nowhere Desert): the dust devil's puff (its u, v, width,
     * height on the tumbleweeds' page), its colour, devils (0 or 1) */
    uint8_t devil_u, devil_v, devil_w, devil_h, devil_rgb[3], devils;
    int ffly_on;
    int16_t ffly[6];
    uint16_t ffly_tpage, ffly_clut;
    uint8_t ffly_u, ffly_v, ffly_rgb[3], ffly_kind;
    int rain_on;
    int16_t rain[7];
    uint16_t rain_tpage, rain_clut;
    uint8_t rain_u, rain_v, rain_rgb[3];
    int16_t diya[5];
    uint16_t diya_tpage, diya_clut;
    uint8_t diya_uv[4];
    int16_t *diya_at;
    int16_t diya_pool, diya_level;
    XAnim xanim[XANIMS];
    /* FLIT (Eternal Darkness): the lightning's light on the floor, a
     * palette's VRAM x, y and its 16 colours lit; black while no flash */
    int flit_on, flit_was;
    uint16_t flit_x, flit_y, flit_lit[16];
} s_pack;

/* The objects' sections: T2OB (the old OBJS: u32 models, each u16 vertices,
 * primitives, then the vertices (Vert) and primitives (Prim); u32 drawing
 * calls (Layout)), T2RT (the old ROTT: RotMatrix's table, sine | cosine <<
 * 16), T2OA (the old OBJA: which machine moves them), and the machines'
 * own: EAGL, GRAS, SHIP, GULL. TRIG's sine for stage 11's circles. */
typedef struct { int16_t x, y, z, pad; } Vert;
typedef struct {
    uint32_t header;
    uint8_t kind, code, cull, ncol, idx[4], uv[4][2];
    uint16_t clut, tpage;
    uint8_t rgb[4][3], win[4];
} Prim;
typedef struct { uint8_t model, pad; uint16_t rank, size, pad2; } Layout;
typedef struct { const Vert *v; const Prim *p; unsigned nv, np; } Model;
_Static_assert(sizeof(Vert) == 8 && sizeof(Prim) == 40 && sizeof(Layout) == 8, "the pack's object records");
enum { MODELS_MAX = 6, LAYOUTS_MAX = 12, EAGLE_MAX = 8 };
static struct {
    uint8_t *data;                      /* T2OB's copy */
    Model models[MODELS_MAX];
    unsigned nmodels, nlayout;
    const Layout *layout;
    uint32_t rot[4096];
    int16_t sin[4096];
    int ostage;
    int16_t eagle_n, eagle_radius, eagle_height, eagle_perch[4 * EAGLE_MAX];
    int grass_period;
    int ship_on;
    int16_t ship[7];
    /* GULL: ... then the critters' kind (0 crabs, 1 rabbits, 2 pigeons, 3
     * Nowhere Desert's lizards, 4 Eternal Darkness's spiders); then the
     * critters of them, the last, that hang on a thread (the 5th model) and
     * the lowest they come down to (cm, up negative) */
    int16_t gull[16];
} s_obj;

/* The lightning (FARV the lit skies, LGHT the flashes, THND the thunder;
 * Taj Mahal, Church Sanctuary): the far background's own (FARB) to draw
 * the lit sky again over the engine's. */
enum { BOLTS_MAX = 8, SOUNDS_MAX = 8 };
static struct {
    int nbolt, npat;
    int fcols, frows, fring, fyoff, fdegrees, fpitch;
    uint8_t fcolours[12];
    uint8_t *far;                       /* FARB's kinds, FARV's tables: a copy */
    const uint8_t *fkinds;
    const uint16_t *bdesc[BOLTS_MAX], *bcluts[BOLTS_MAX];
    int16_t lthr[5];
    uint16_t lforce[2];
    uint8_t lforce_acc[2], plen[BOLTS_MAX], pnext[BOLTS_MAX], pat[BOLTS_MAX][64];
    uint32_t light[BOLTS_MAX];
    unsigned light_frames;
    uint8_t thunder_of[SOUNDS_MAX];
} s_storm;
/* THND: kept for the session once read (the audio thread plays from it) */
static struct { uint8_t *data; uint32_t size; unsigned n; uint32_t frames[SOUNDS_MAX]; const int16_t *pcm[SOUNDS_MAX]; } s_thnd;

static const uint8_t *section(const uint8_t *b, long size, const char *id, uint32_t *len) {
    uint32_t n; memcpy(&n, b + 8, 4);
    for (uint32_t i = 0; i < n && 16 + 12 * (long)i + 12 <= size; i++) {
        if (memcmp(b + 16 + 12 * i, id, 4)) continue;
        uint32_t o, l; memcpy(&o, b + 20 + 12 * i, 4); memcpy(&l, b + 24 + 12 * i, 4);
        if ((long)o + (long)l > size) return NULL;
        *len = l;
        return b + o;
    }
    return NULL;
}

/* the parts of an animation's frames (at frames: x, y each) a VRAM
 * rectangle r (its pixels o) holds, into pix (n x w x h) */
static void grab_frames(uint16_t *pix, const uint8_t *frames, unsigned n, int w, int h, const uint16_t r[4],
                        const uint8_t *o) {
    if (!pix) return;
    for (unsigned f = 0; f < n; f++) {
        uint16_t fxy[2]; memcpy(fxy, frames + 4 * f, 4);
        int fx = fxy[0], fy = fxy[1];
        for (int y = 0; y < h; y++) {
            int vy = fy + y - r[1];
            if (vy < 0 || vy >= r[3]) continue;
            for (int x = 0; x < w; x++) {
                int vx = fx + x - r[0];
                if (vx >= 0 && vx < r[2])
                    memcpy(&pix[((size_t)f * h + y) * w + x], o + 2u * ((unsigned)vy * r[2] + vx), 2);
            }
        }
    }
}

static void round_start(void);
static void fight_start(void);

void tmst_tekken2_extras_clear(void) {
    for (int i = 0; i < XANIMS; i++) free(s_pack.xanim[i].pix);
    free(s_pack.diya_at);
    memset(&s_pack, 0, sizeof s_pack);
    free(s_obj.data);
    memset(&s_obj, 0, sizeof s_obj);
    free(s_storm.far);
    memset(&s_storm, 0, sizeof s_storm);
    glass_clear();
    own_tiles_clear();
}

/* The lightning, as the old fork's pack_load reads it (FARV and LGHT both,
 * else none; THND when there). */
static void storm_load(const uint8_t *b, long size) {
    uint32_t flen = 0, vlen = 0, llen = 0, tlen = 0;
    const uint8_t *fb = section(b, size, "FARB", &flen), *fv = section(b, size, "FARV", &vlen);
    const uint8_t *lg = section(b, size, "LGHT", &llen), *th = section(b, size, "THND", &tlen);
    if (!fb || !fv || !lg || flen < 24 || vlen < 4 || llen < 18) return;
    int16_t h[6]; memcpy(h, fb, 12);
    if (h[0] <= 0 || h[1] < 0 || h[2] <= 0 || h[0] > 64 || h[1] > 32 || h[2] > 64) return;
    unsigned cells = (unsigned)h[0] * (unsigned)h[1];
    if (flen < 24u + 4u * cells + (unsigned)h[1] * (unsigned)h[2]) return;
    uint16_t nv; memcpy(&nv, fv, 2);
    unsigned nbolt = nv > BOLTS_MAX ? BOLTS_MAX : nv;
    if (vlen < 4u + 4u * cells * nbolt) return;
    size_t kinds = (size_t)h[1] * (size_t)h[2], tables = 4u * cells * nbolt;
    if (!(s_storm.far = malloc(kinds + tables + 2))) return;
    memcpy(s_storm.far, fb + 24 + 4u * cells, kinds);
    memcpy(s_storm.far + ((kinds + 1) & ~(size_t)1), fv + 4, tables);
    s_storm.fkinds = s_storm.far;
    const uint16_t *t = (const uint16_t *)(s_storm.far + ((kinds + 1) & ~(size_t)1));
    for (unsigned i = 0; i < nbolt; i++) { s_storm.bdesc[i] = t; t += cells; s_storm.bcluts[i] = t; t += cells; }
    s_storm.fcols = h[0]; s_storm.frows = h[1]; s_storm.fring = h[2]; s_storm.fyoff = h[3];
    s_storm.fdegrees = h[4]; s_storm.fpitch = h[5];
    memcpy(s_storm.fcolours, fb + 12, 12);
    /* LGHT */
    memcpy(s_storm.lthr, lg, 10); memcpy(s_storm.lforce, lg + 10, 4);
    s_storm.lforce_acc[0] = lg[14]; s_storm.lforce_acc[1] = lg[15];
    unsigned npat = lg[16] > BOLTS_MAX ? BOLTS_MAX : lg[16];
    if (llen < 18u + 2u * lg[16]) { free(s_storm.far); memset(&s_storm, 0, sizeof s_storm); return; }
    memcpy(s_storm.plen, lg + 18, npat); memcpy(s_storm.pnext, lg + 18 + lg[16], npat);
    size_t q = 18u + 2u * lg[16];
    for (unsigned i = 0; i < lg[16]; i++) {
        unsigned n = lg[18 + i];
        if (q + n > llen) { free(s_storm.far); memset(&s_storm, 0, sizeof s_storm); return; }
        if (i < npat) {
            if (n > sizeof s_storm.pat[0]) n = s_storm.plen[i] = sizeof s_storm.pat[0];
            memcpy(s_storm.pat[i], lg + q, n);
        }
        q += lg[18 + i];
    }
    size_t at = (q + 3) & ~(size_t)3;
    if (at + 4 <= llen) {
        uint16_t nl[2]; memcpy(nl, lg + at, 4);
        for (unsigned i = 0; i < nl[0] && i < BOLTS_MAX && at + 8 + 4 * i <= llen; i++) memcpy(&s_storm.light[i], lg + at + 4 + 4 * i, 4);
        s_storm.light_frames = nl[1];
    }
    s_storm.nbolt = (int)nbolt;
    s_storm.npat = (int)(npat > nbolt ? nbolt : npat);
    memset(s_storm.thunder_of, 0xFF, sizeof s_storm.thunder_of);
    if (!th || tlen < 4) return;
    uint16_t c[2]; memcpy(c, th, 4);
    memcpy(s_storm.thunder_of, th + 4, c[1] < SOUNDS_MAX ? c[1] : SOUNDS_MAX);
    /* the sounds: the same as the last pack's (Taj Mahal's and the Church's
     * are), or read anew; the old kept for the session, never freed under
     * the audio thread */
    if (s_thnd.data && s_thnd.size == tlen && !memcmp(s_thnd.data, th, tlen)) return;
    uint8_t *copy = malloc(tlen);
    if (!copy) return;
    memcpy(copy, th, tlen);
    unsigned n = c[0] < SOUNDS_MAX ? c[0] : SOUNDS_MAX;
    uint32_t frames[SOUNDS_MAX] = {0};
    const int16_t *pcm[SOUNDS_MAX] = {0};
    size_t o = 4u + ((c[1] + 3u) & ~3u);
    for (unsigned i = 0; i < n; i++) {
        if (o + 4 > tlen) { n = i; break; }
        memcpy(&frames[i], copy + o, 4); o += 4;
        if (o + 4u * frames[i] > tlen) { n = i; break; }
        pcm[i] = (const int16_t *)(copy + o); o += 4u * frames[i];
    }
    s_thnd.n = 0;                       /* none while the tables change */
    s_thnd.data = copy; s_thnd.size = tlen;
    memcpy(s_thnd.frames, frames, sizeof frames); memcpy(s_thnd.pcm, pcm, sizeof pcm);
    s_thnd.n = n;
}

/* The objects' sections, checked as the old fork's pack_load (at most 6
 * models, 12 drawing calls, 256 vertices a model), else none. */
static void objects_load(const uint8_t *b, long size) {
    uint32_t len = 0, rlen = 0, tlen = 0, alen = 0;
    const uint8_t *o = section(b, size, "T2OB", &len), *rot = section(b, size, "T2RT", &rlen);
    const uint8_t *trig = section(b, size, "TRIG", &tlen), *oa = section(b, size, "T2OA", &alen);
    if (!o || !rot || rlen < sizeof s_obj.rot || !trig || tlen < sizeof s_obj.sin || !oa || alen < 2 || len < 8) return;
    if (!(s_obj.data = malloc(len))) return;
    memcpy(s_obj.data, o, len);
    const uint8_t *p = s_obj.data, *end = s_obj.data + len;
    uint32_t nm; memcpy(&nm, p, 4); p += 4;
    int good = nm <= MODELS_MAX;
    for (uint32_t i = 0; good && i < nm; i++) {
        uint16_t c[2];
        if (p + 4 > end) { good = 0; break; }
        memcpy(c, p, 4); p += 4;
        Model *m = &s_obj.models[s_obj.nmodels++];
        m->nv = c[0]; m->np = c[1];
        m->v = (const Vert *)p; p += 8u * m->nv;
        m->p = (const Prim *)p; p += sizeof(Prim) * m->np;
        if (p > end || m->nv > 0x100) good = 0;
    }
    if (good && p + 4 <= end) {
        uint32_t nl; memcpy(&nl, p, 4);
        s_obj.layout = (const Layout *)(p + 4);
        good = nl <= LAYOUTS_MAX && p + 4 + sizeof(Layout) * nl <= end;
        for (uint32_t i = 0; good && i < nl; i++) {
            Layout l; memcpy(&l, s_obj.layout + i, sizeof l);
            if (l.model >= s_obj.nmodels) good = 0;
        }
        if (good) s_obj.nlayout = nl;
    }
    if (!good || !s_obj.nlayout) {
        tmst_log("tekken2: remake extras: the pack's objects are not as expected, none drawn");
        free(s_obj.data);
        memset(&s_obj, 0, sizeof s_obj);
        return;
    }
    memcpy(s_obj.rot, rot, sizeof s_obj.rot);
    memcpy(s_obj.sin, trig, sizeof s_obj.sin);
    uint16_t os; memcpy(&os, oa, 2); s_obj.ostage = os;
    if ((o = section(b, size, "EAGL", &len)) && len >= 8) {
        int16_t e[4]; memcpy(e, o, 8);
        s_obj.eagle_n = e[0] < 0 ? 0 : e[0] > EAGLE_MAX ? EAGLE_MAX : e[0];
        if (len < 8u + 8u * (unsigned)s_obj.eagle_n) s_obj.eagle_n = (int16_t)((len - 8) / 8);
        s_obj.eagle_radius = e[1]; s_obj.eagle_height = e[2];
        memcpy(s_obj.eagle_perch, o + 8, 8u * (unsigned)s_obj.eagle_n);
    }
    if ((o = section(b, size, "GRAS", &len)) && len >= 2) { int16_t g; memcpy(&g, o, 2); s_obj.grass_period = g > 0 ? g : 0; }
    if ((o = section(b, size, "SHIP", &len)) && len >= sizeof s_obj.ship) {
        memcpy(s_obj.ship, o, sizeof s_obj.ship);
        s_obj.ship_on = s_obj.ship[3] > 0 && s_obj.ship[1] != s_obj.ship[2];
    }
    if ((o = section(b, size, "GULL", &len)) && len >= 16) {
        memcpy(s_obj.gull, o, len >= 32 ? 32 : len >= 28 ? 28 : len >= 26 ? 26 : len >= 24 ? 24 : len >= 18 ? 18 : 16);
        if (s_obj.gull[0] < 0 || s_obj.gull[0] > 8) s_obj.gull[0] = 0;
        if (s_obj.gull[9] < 0 || s_obj.gull[0] + s_obj.gull[9] > 8) s_obj.gull[9] = 0;
        if (s_obj.gull[14] < 0 || s_obj.gull[14] > s_obj.gull[9]) s_obj.gull[14] = 0;
    }
}

/* The fight's pack (src/tekken2/tmst_tekken2_stages.c, fight_extras):
 * what of it the extras need, copied (the pack is freed after). */
void tmst_tekken2_extras_load(const uint8_t *b, long size) {
    tmst_tekken2_extras_clear();
    uint32_t len = 0;
    const uint8_t *o;
    if ((o = section(b, size, "DUST", &len)) && len >= 6) {
        memcpy(&s_pack.dust_tpage, o, 2); memcpy(&s_pack.dust_clut, o + 2, 2);
        s_pack.dust_u = o[4]; s_pack.dust_v = o[5]; s_pack.dust_on = 1;
    }
    if ((o = section(b, size, "FTHR", &len)) && len >= 15) {
        memcpy(s_pack.feather, o, 12); memcpy(s_pack.feather_rgb, o + 12, 3);
        if (len >= 18) { int16_t sh; memcpy(&sh, o + 16, 2); s_pack.feather_shape = sh; }
        if ((s_pack.feather_shape == 3 || s_pack.feather_shape == 5 || s_pack.feather_shape == 6 ||
             s_pack.feather_shape == 7) && len >= 26) {
            memcpy(&s_pack.paper_tpage, o + 18, 2); memcpy(&s_pack.paper_clut, o + 20, 2);
            s_pack.paper_u = o[22]; s_pack.paper_v = o[23]; s_pack.paper_w = o[24]; s_pack.paper_h = o[25];
        }
        if (s_pack.feather_shape == 6 && len >= 34) {
            s_pack.devil_u = o[26]; s_pack.devil_v = o[27]; s_pack.devil_w = o[28]; s_pack.devil_h = o[29];
            memcpy(s_pack.devil_rgb, o + 30, 3); s_pack.devils = o[33];
        }
        s_pack.feather_on = 1;
    }
    if ((o = section(b, size, "FFLY", &len)) && len >= 21) {
        memcpy(s_pack.ffly, o, 12); memcpy(&s_pack.ffly_tpage, o + 12, 2); memcpy(&s_pack.ffly_clut, o + 14, 2);
        s_pack.ffly_u = o[16]; s_pack.ffly_v = o[17]; memcpy(s_pack.ffly_rgb, o + 18, 3);
        s_pack.ffly_kind = len >= 22 ? o[21] : 0;
        s_pack.ffly_on = s_pack.ffly[0] > 0 && s_pack.ffly[2] > s_pack.ffly[1];
    }
    if ((o = section(b, size, "RAIN", &len)) && len >= 23) {
        memcpy(s_pack.rain, o, 14); memcpy(&s_pack.rain_tpage, o + 14, 2); memcpy(&s_pack.rain_clut, o + 16, 2);
        s_pack.rain_u = o[18]; s_pack.rain_v = o[19]; memcpy(s_pack.rain_rgb, o + 20, 3);
        s_pack.rain_on = s_pack.rain[0] > 0 && s_pack.rain[1] > 0 && s_pack.rain[6] > s_pack.rain[5];
    }
    if ((o = section(b, size, "DIYA", &len)) && len >= 22) {
        memcpy(s_pack.diya, o, 10); memcpy(&s_pack.diya_tpage, o + 10, 2); memcpy(&s_pack.diya_clut, o + 12, 2);
        memcpy(s_pack.diya_uv, o + 14, 4);
        memcpy(&s_pack.diya_pool, o + 18, 2); memcpy(&s_pack.diya_level, o + 20, 2);
        if (s_pack.diya[0] < 0 || len < 22u + 6u * (unsigned)s_pack.diya[0]) s_pack.diya[0] = 0;
        if (s_pack.diya[0] > 0 && (s_pack.diya_at = malloc(6u * (unsigned)s_pack.diya[0])) != NULL)
            memcpy(s_pack.diya_at, o + 22, 6u * (unsigned)s_pack.diya[0]);
        else s_pack.diya[0] = 0;
    }
    /* the animated palettes: their frames from the pack's VRAM rectangles */
    const uint8_t *xframes[XANIMS] = { NULL, NULL, NULL };
    for (int i = 0; i < XANIMS; i++) {
        static const char *const ids[XANIMS] = { "ANI2", "ANI3", "ANI4" };
        XAnim *xa = &s_pack.xanim[i];
        if (!(o = section(b, size, ids[i], &len)) || len < 12) continue;
        uint16_t h[6]; memcpy(h, o, 12);
        xa->x = h[0]; xa->y = h[1]; xa->w = h[2]; xa->h = h[3]; xa->period = h[4]; xa->frames = h[5];
        if (len < 12u + 4u * xa->frames) { xa->frames = 0; continue; }
        xframes[i] = o + 12;
        if (len >= 14u + 4u * xa->frames) memcpy(&xa->mode, o + 12 + 4 * xa->frames, 2);
        xa->pix = xa->frames && xa->period && xa->w && xa->h ? calloc((size_t)xa->frames * xa->w * xa->h, 2) : NULL;
        if (!xa->pix) xa->frames = 0;
    }
    if ((o = section(b, size, "VRAM", &len)) && len >= 4) {
        const uint8_t *end = o + len;
        uint32_t n; memcpy(&n, o, 4); o += 4;
        for (uint32_t i = 0; i < n && o + 8 <= end; i++) {
            uint16_t r[4]; memcpy(r, o, 8); o += 8;
            if (o + 2u * r[2] * r[3] > end) break;
            for (int k = 0; k < XANIMS; k++)
                if (s_pack.xanim[k].frames)
                    grab_frames(s_pack.xanim[k].pix, xframes[k], s_pack.xanim[k].frames, s_pack.xanim[k].w,
                                s_pack.xanim[k].h, r, o);
            o += 2u * r[2] * r[3];
        }
    }
    if ((o = section(b, size, "FLIT", &len)) && len >= 36) {
        memcpy(&s_pack.flit_x, o, 2); memcpy(&s_pack.flit_y, o + 2, 2); memcpy(s_pack.flit_lit, o + 4, 32);
        s_pack.flit_on = 1; s_pack.flit_was = -1;
    }
    objects_load(b, size);
    storm_load(b, size);
    glass_load(b, size);
    own_tiles_load(b, size);
    s_pack.loaded = s_pack.flit_on || s_pack.dust_on || s_pack.feather_on || s_pack.ffly_on || s_pack.rain_on || s_pack.diya[0] > 0 ||
                    s_pack.xanim[0].frames || s_pack.xanim[1].frames || s_pack.xanim[2].frames || s_obj.nlayout ||
                    s_storm.npat || glass_on() || own_tiles_on();
    if (s_pack.loaded) {
        fight_start();
        round_start();
        tmst_log("tekken2: remake extras:%s%s%s%s%s%s%s%s", s_storm.npat ? " lightning" : "",
                 s_obj.nlayout ? " objects" : "", s_pack.dust_on ? " dust" : "",
                 s_pack.feather_on ? " feathers" : "", s_pack.ffly_on ? " fireflies" : "", s_pack.rain_on ? " rain" : "",
                 s_pack.diya[0] > 0 ? " lamps" : "",
                 s_pack.xanim[0].frames || s_pack.xanim[1].frames || s_pack.xanim[2].frames ? " palettes" : "");
    }
}

int tmst_tekken2_extras_on(void) { return s_pack.loaded; }

/* ---- the GTE: RTPS (sf 1, lm 0), as the engine's ---- */

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
static uint32_t xy(int x, int y) { return (uint32_t)(y & 0xFFFF) << 16 | (uint32_t)(x & 0xFFFF); }
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
static void mul_matrix2(const int32_t a[3][3], int32_t b[3][3]) {      /* b = a x b */
    int32_t out[3][3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            out[i][j] = sat16(((int64_t)a[i][0] * b[0][j] + (int64_t)a[i][1] * b[1][j]
                               + (int64_t)a[i][2] * b[2][j]) >> 12);
    memcpy(b, out, sizeof out);
}
static void apply_matrix(const int32_t m[3][3], int x, int y, int z, int32_t out[3]) {
    for (int i = 0; i < 3; i++)
        out[i] = (int32_t)(((int64_t)m[i][0] * x + (int64_t)m[i][1] * y + (int64_t)m[i][2] * z) >> 12);
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
/* px, py (may be null): the 16.16 screen point before rounding, NO_PRECISE
 * when the projection saturates (IR, divide or screen clamp). */
#define NO_PRECISE INT32_MIN
static void rtps_p(const Gte *g, int x, int y, int z, int *sx, int *sy, int *sz, int32_t *px, int32_t *py) {
    int64_t mac[3];
    for (int i = 0; i < 3; i++)
        mac[i] = (((int64_t)g->t[i] << 12) + (int64_t)g->r[i][0] * x + (int64_t)g->r[i][1] * y
                  + (int64_t)g->r[i][2] * z) >> 12;
    int64_t ir0 = mac[0] < -0x8000 ? -0x8000 : mac[0] > 0x7FFF ? 0x7FFF : mac[0];
    int64_t ir1 = mac[1] < -0x8000 ? -0x8000 : mac[1] > 0x7FFF ? 0x7FFF : mac[1];
    uint32_t sz3 = (uint32_t)(mac[2] < 0 ? 0 : mac[2] > 0xFFFF ? 0xFFFF : mac[2]);
    int64_t q = gte_divide(g->h, sz3);
    int64_t vx = (q * ir0 + g->ofx) >> 16, vy = (q * ir1 + g->ofy) >> 16;
    *sx = (int)(vx < -0x400 ? -0x400 : vx > 0x3FF ? 0x3FF : vx);
    *sy = (int)(vy < -0x400 ? -0x400 : vy > 0x3FF ? 0x3FF : vy);
    *sz = (int)sz3;
    if (px) {
        int ok = ir0 == mac[0] && ir1 == mac[1] && q < 0x1FFFF && vx == *sx && vy == *sy;
        *px = ok ? (int32_t)(q * ir0 + g->ofx) : NO_PRECISE;
        *py = ok ? (int32_t)(q * ir1 + g->ofy) : NO_PRECISE;
    }
}
static void rtps(const Gte *g, int x, int y, int z, int *sx, int *sy, int *sz) {
    int64_t mac[3];
    for (int i = 0; i < 3; i++)
        mac[i] = (((int64_t)g->t[i] << 12) + (int64_t)g->r[i][0] * x + (int64_t)g->r[i][1] * y
                  + (int64_t)g->r[i][2] * z) >> 12;
    int64_t ir0 = mac[0] < -0x8000 ? -0x8000 : mac[0] > 0x7FFF ? 0x7FFF : mac[0];
    int64_t ir1 = mac[1] < -0x8000 ? -0x8000 : mac[1] > 0x7FFF ? 0x7FFF : mac[1];
    uint32_t sz3 = (uint32_t)(mac[2] < 0 ? 0 : mac[2] > 0xFFFF ? 0xFFFF : mac[2]);
    int64_t q = gte_divide(g->h, sz3);
    int64_t vx = (q * ir0 + g->ofx) >> 16, vy = (q * ir1 + g->ofy) >> 16;
    *sx = (int)(vx < -0x400 ? -0x400 : vx > 0x3FF ? 0x3FF : vx);
    *sy = (int)(vy < -0x400 ? -0x400 : vy > 0x3FF ? 0x3FF : vy);
    *sz = (int)sz3;
}

/* The tiles' GTE state, as the engine's tile_matrix (src/tekken3_arcade_stages.c)
 * builds it for a Tekken 3 pack (axis0 0): 0x8006D014's for the PS1 stage,
 * with the arcade stage's units (/ 10, not >> 3) and origin height. */
static void tile_matrix(Gte *g) {
    int32_t cam[3][3], axis[3][3];
    int pitch = (int)psx_mod_read_word(CAMERA), yaw = (int)psx_mod_read_word(CAMERA + 4);
    int roll = (int)psx_mod_read_word(CAMERA + 8);
    int camx = (int)psx_mod_read_word(CAMERA + 0x14), camy = (int)psx_mod_read_word(CAMERA + 0x18);
    int camz = (int)psx_mod_read_word(CAMERA + 0x1C);
    rot_ps1(pitch, yaw, -roll, cam);
    for (int k = 0; k < 3; k++) cam[1][k] = (int16_t)((cam[1][k] * 0x14) / 0xC);
    rot_ps1(0, -(int)psx_mod_read_word(AXIS), 0, axis);
    /* the point the stage follows (0x80062B04 picks the smoothed fighter) */
    int dx, dz;
    if (psx_mod_read_byte(FIXED_STAGE)) {
        dx = -camx; dz = -camz;
    } else {
        int p1x = (int)psx_mod_read_word(P1_X), p1z = (int)psx_mod_read_word(P1_Z);
        int p2x = (int)psx_mod_read_word(P2_X), p2z = (int)psx_mod_read_word(P2_Z);
        int pick = -1;
        if (psx_mod_read_half(0x800A923Eu) == 4 && psx_mod_read_byte(psx_mod_read_word(0x800A927Cu) + 0x24) == 'S')
            pick = 0;
        else if (psx_mod_read_half(0x800AAACAu) == 4 && psx_mod_read_byte(psx_mod_read_word(0x800AAB08u) + 0x24) == 'S')
            pick = 1;
        if (pick >= 0) {
            uint32_t hist = pick ? HISTORY_P2 : HISTORY_P1;
            int hx = 0, hz = 0;
            for (unsigned k = 0; k < 4; k++) {
                hx += (int)psx_mod_read_word(hist + 16 * k);
                hz += (int)psx_mod_read_word(hist + 8 + 16 * k);
            }
            dx = (((hx >> 2) + (pick ? p1x : p2x)) >> 1) - camx;
            dz = (((hz >> 2) + (pick ? p1z : p2z)) >> 1) - camz;
        } else {
            dx = ((p2x + p1x) >> 1) - camx;
            dz = ((p2z + p1z) >> 1) - camz;
        }
    }
    int vx = (int16_t)dx / 10, vz = (int16_t)dz / 10, vy = (int16_t)(STAGE_Y - camy) / 10;
    apply_matrix(cam, vx, vy, vz, g->t);
    memcpy(g->r, axis, sizeof axis);
    mul_matrix2(cam, g->r);
}

/* ---- packets ---- */

static uint32_t s_memory, s_cursor, s_end;
static int s_margin;                    /* the wide screen's, each side */

static uint32_t alloc_words(unsigned words) {
    uint32_t a = s_cursor;
    if (!a || a + 4u * words > s_end) return 0;
    s_cursor += 4u * words;
    return a;
}
static void add_prim(uint32_t ot, uint32_t p, unsigned words) {
    uint32_t e = psx_mod_read_word(ot);
    psx_mod_write_word(p, (uint32_t)words << 24 | (e & 0xFFFFFFu));
    psx_mod_write_word(ot, (e & 0xFF000000u) | (p & 0xFFFFFFu));
}
static uint32_t main_ot(unsigned rank) {
    return psx_mod_read_word(psx_mod_read_word(DRAWENV) + 4) + 4u * rank;
}
/* On the screen, as the old's bounds (x within -m..SCREEN_W + m), the wide
 * screen's sides added. */
static int off_x(int x, int m) { return x < -m - s_margin || x > SCREEN_W + m + s_margin; }

/* A 16 x 16 picture's quad (code: 0x2C solid, 0x2D raw, 0x2E half and half). */
static void tex_quad(uint32_t ot, uint32_t code_col, const int x[4], const int y[4], unsigned clut, unsigned tpage,
                     unsigned u0, unsigned v0, unsigned u1, unsigned v1) {
    uint32_t q = alloc_words(10);
    if (!q) return;
    psx_mod_write_word(q + 4, code_col);
    psx_mod_write_word(q + 8, xy(x[0], y[0]));
    psx_mod_write_word(q + 12, (uint32_t)clut << 16 | v0 << 8 | u0);
    psx_mod_write_word(q + 16, xy(x[1], y[1]));
    psx_mod_write_word(q + 20, (uint32_t)tpage << 16 | v0 << 8 | u1);
    psx_mod_write_word(q + 24, xy(x[2], y[2]));
    psx_mod_write_word(q + 28, v1 << 8 | u0);
    psx_mod_write_word(q + 32, xy(x[3], y[3]));
    psx_mod_write_word(q + 36, v1 << 8 | u1);
    add_prim(ot, q, 9);
}
/* The same, turned: the centre sx, sy, its half axes (cx, cy) and (dx, dy). */
static void turned_quad(uint32_t ot, uint32_t code_col, int sx, int sy, int cx, int cy, int dx, int dy, unsigned clut,
                        unsigned tpage, unsigned u0, unsigned v0) {
    const int x[4] = { sx - cx - dx, sx + cx - dx, sx - cx + dx, sx + cx + dx };
    const int y[4] = { sy - cy - dy, sy + cy - dy, sy - cy + dy, sy + cy + dy };
    tex_quad(ot, code_col, x, y, clut, tpage, u0, v0, u0 + 15, v0 + 15);
}

static uint32_t s_rand = 0x2545F491u;  /* the old's snow_rand, for all of them */
static int rnd(void) {
    s_rand = s_rand * 1103515245u + 12345u;
    return (int)(s_rand >> 16 & 0x7FFF);
}
static int paused(void) { return psx_mod_read_word(PAUSED) != 0; }

/* World (x, z) to the tiles' frame (the world turned by the fight's axis
 * round the fighters' midpoint, or the world itself on a fixed stage): the
 * puffs keep world places. (The old's fixed stages, FIXW: phase 5.) */
static void world_tile(int wx, int wz, int *x, int *z) {
    int vx = wx, vz = wz;
    if (!psx_mod_read_byte(FIXED_STAGE)) {
        int p1x = (int)psx_mod_read_word(P1_X), p1z = (int)psx_mod_read_word(P1_Z);
        int p2x = (int)psx_mod_read_word(P2_X), p2z = (int)psx_mod_read_word(P2_Z);
        vx = wx - (p1x + (p2x - p1x) / 2); vz = wz - (p1z + (p2z - p1z) / 2);
    }
    int32_t m[3][3], v[3];
    rot_ps1(0, (int)psx_mod_read_word(AXIS), 0, m);
    apply_matrix(m, vx, 0, vz, v);
    *x = v[0]; *z = v[2];
}

/* ---- DUST: where a fighter hits the ground (its hips, s32 y at +4 from x,
 * falling fast and stopped low) puffs fly up round it, drift out, fall back
 * quickly and shrink away, kept at their world place; more and wider for a
 * body than for feet. World units (mm). ---- */
#define DUST_MAX 96
typedef struct { int x, y, z, vx, vy, vz, age, life, size; } Mote;   /* position mm x 16, speed mm/frame x 16 */
static Mote s_mote[DUST_MAX];
static int s_nmote, s_dust_seen, s_dust_prev[2], s_dust_prev_v[2], s_dust_cool[2];

static void dust_burst(int p, int speed, int y) {
    int fx = (int)psx_mod_read_word(p ? P2_X : P1_X), fz = (int)psx_mod_read_word(p ? P2_Z : P1_Z);   /* world */
    int body = y > -600;                        /* the hips low: the body hit, not the feet */
    int n = speed / 2;
    if (n < 8) n = 8;
    if (n > 12) n = 12;
    if (body) n = n * 4 / 3;
    for (int i = 0; i < n && s_nmote < DUST_MAX; i++) {
        Mote *m = &s_mote[s_nmote++];
        double a = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
        int r = rnd() % (body ? 700 : 320);
        m->x = (fx + (int)(r * cos(a))) * 16; m->z = (fz + (int)(r * sin(a))) * 16; m->y = -20 * 16;
        int out = 24 + rnd() % (body ? 150 : 100);           /* outwards, mm/frame x 16 */
        m->vx = (int)(out * cos(a)); m->vz = (int)(out * sin(a));
        int up = 160 + rnd() % 260;
        if (speed < 40) up = up * (20 + speed) / 60;
        m->vy = -up;
        m->size = 40 + rnd() % (body ? 80 : 50);
        m->life = 30 + rnd() % 30;
        m->age = 0;
    }
}

static void dust(const Gte *g) {
    if (!s_pack.dust_on) return;
    if (!paused()) {
        for (int p = 0; p < 2; p++) {
            int y = (int)psx_mod_read_word(p ? P2_Y : P1_Y);
            if (!s_dust_seen) { s_dust_prev[p] = y; s_dust_prev_v[p] = 0; s_dust_cool[p] = 0; }
            int v = y - s_dust_prev[p];
            if (v > 400 || v < -400) v = 0;     /* placed anew (a round's start) */
            if (s_dust_cool[p] > 0) s_dust_cool[p]--;
            if (s_dust_prev_v[p] > 14 && v < s_dust_prev_v[p] / 3 && y > -1300 && !s_dust_cool[p]) {
                dust_burst(p, s_dust_prev_v[p], y);
                s_dust_cool[p] = 10;
            }
            s_dust_prev[p] = y; s_dust_prev_v[p] = v;
        }
        s_dust_seen = 1;
        for (int i = 0; i < s_nmote; ) {
            Mote *m = &s_mote[i];
            m->x += m->vx; m->y += m->vy; m->z += m->vz;
            m->vx = m->vx * 30 / 32; m->vz = m->vz * 30 / 32;
            m->vy += 22;                        /* gravity: up and down again quickly */
            int floor_y = -m->size * 16 / 3;
            if (m->y > floor_y && m->vy > 0) { m->y = floor_y; m->vy = 0; m->vx = m->vx * 3 / 4; m->vz = m->vz * 3 / 4; }
            if (++m->age >= m->life) { s_mote[i] = s_mote[--s_nmote]; continue; }
            i++;
        }
    }
    int near_z = g->t[2];
    for (int i = 0; i < s_nmote; i++) {
        const Mote *m = &s_mote[i];
        int sx, sy, sz, tx, tz;
        world_tile(m->x / 16, m->z / 16, &tx, &tz);
        rtps(g, tx / 10, m->y / 160, tz / 10, &sx, &sy, &sz);
        if (sz < 32 || off_x(sx, 64) || sy < -64 || sy > 512) continue;
        /* it swells as it spreads; at its end it shrinks away (half and half
         * blending has no fading) */
        int half = m->size * (2 * m->life + m->age) / (4 * m->life);
        int left = m->life - m->age, tail = m->life / 4;
        if (left < tail) half = half * left / tail;
        if (m->age < 4) half = half * (m->age + 1) / 5;
        int hx = (int)((half * (int64_t)g->h) / (10 * (int64_t)sz)), hy = hx * 20 / 12;
        if (hx < 1 || hy < 1) continue;
        /* turned a little, each its own way and slowly (a screen-square quad
         * is taken for the interface by the wide-screen layer, which then
         * reads the PS1's VRAM, not the upper half) */
        double turn = (m->size * 37 % 360) * (3.14159265 / 180.0) + m->age * 0.01;
        int cx = (int)(hx * cos(turn)), cy = (int)(hx * sin(turn) * 20 / 12);
        int dx = (int)(-hy * sin(turn) * 12 / 20), dy = (int)(hy * cos(turn));
        turned_quad(main_ot(sz < near_z ? 40 : 1009), 0x2E808080u, sx, sy, cx, cy, dx, dy, s_pack.dust_clut,
                    s_pack.dust_tpage, s_pack.dust_u, s_pack.dust_v);
    }
}

/* ---- FTHR shapes 0, 1: feathers (Mirror Darkness: white, as if from
 * Angel), Kyoto's petals: a few float down round the fight, rocking side to
 * side and turning over as they fall, kept at their world place; at the
 * floor a new one starts high up. Each a long diamond, shaded white to grey
 * from its tip (no texture); a petal pink, small and round, spinning as it
 * flutters, drifting a little with the breeze. World units (mm, x 16). ---- */
#define FEATHER_MAX 48
typedef struct { int x, y, z, vy; double sway, sway_rate, turn, turn_rate, spin, spin_rate; } Feather;
static Feather s_feather[FEATHER_MAX];
static int s_nfeather, s_feather_seen;

static void feather_spawn(Feather *f, int strewn) {
    const int16_t *c = s_pack.feather;
    int p1x = (int)psx_mod_read_word(P1_X), p1z = (int)psx_mod_read_word(P1_Z);
    int p2x = (int)psx_mod_read_word(P2_X), p2z = (int)psx_mod_read_word(P2_Z);
    double a = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    int r = c[1] + (c[2] > c[1] ? rnd() % (c[2] - c[1]) : 0);
    f->x = (p1x + (p2x - p1x) / 2 + (int)(r * cos(a))) * 16;
    f->z = (p1z + (p2z - p1z) / 2 + (int)(r * sin(a))) * 16;
    f->y = -(c[3] - (strewn ? rnd() % (c[3] - 200) : 0)) * 16;
    f->vy = c[5] * 3 / 4 + rnd() % (c[5] / 2 + 1);
    f->sway = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->sway_rate = 0.018 + (rnd() & 0xFF) / 255.0 * 0.014;
    f->turn = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->turn_rate = 0.02 + (rnd() & 0xFF) / 255.0 * 0.03;
    f->spin = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->spin_rate = s_pack.feather_shape ? ((rnd() & 0xFF) / 255.0 - 0.5) * 0.08 : 0;
    if (s_pack.feather_shape) f->turn_rate *= 2.2;
}

static void strewn_petals(const Gte *g);
static void newspapers(const Gte *g);
static void wind_dust(const Gte *g);
static void desert_wind(const Gte *g);
static void dandelion_seeds(const Gte *g);

static void feathers(const Gte *g) {
    if (!s_pack.feather_on) return;
    if (s_pack.feather_shape == 2 || s_pack.feather_shape == 4) { strewn_petals(g); return; }
    if (s_pack.feather_shape == 3) { newspapers(g); return; }
    if (s_pack.feather_shape == 5) { wind_dust(g); return; }
    if (s_pack.feather_shape == 6) { desert_wind(g); return; }
    if (s_pack.feather_shape == 7) { dandelion_seeds(g); return; }
    const int16_t *c = s_pack.feather;
    int n = c[0] < 0 ? 0 : c[0] > FEATHER_MAX ? FEATHER_MAX : c[0];
    if (!n || c[3] <= 200) return;
    if (!s_feather_seen) {                      /* the round's start: strewn all the way down */
        for (s_nfeather = 0; s_nfeather < n; s_nfeather++) feather_spawn(&s_feather[s_nfeather], 1);
        s_feather_seen = 1;
    }
    if (!paused())
        for (int i = 0; i < s_nfeather; i++) {
            Feather *f = &s_feather[i];
            /* it falls slower while it rocks out to a side, faster through the middle */
            f->sway += f->sway_rate; f->turn += f->turn_rate;
            f->y += (int)(f->vy * (0.55 + 0.45 * fabs(cos(f->sway))));
            f->x += (int)(4000 * cos(f->sway) * f->sway_rate);   /* about 25 cm each way */
            if (s_pack.feather_shape) { f->spin += f->spin_rate; f->x += 40; f->z += 12; }   /* the breeze */
            if (f->y > -c[4] * 16 / 8) feather_spawn(f, 0);
        }
    int near_z = g->t[2];
    for (int i = 0; i < s_nfeather; i++) {
        const Feather *f = &s_feather[i];
        int sx, sy, sz, tx, tz;
        world_tile(f->x / 16, f->z / 16, &tx, &tz);
        rtps(g, tx / 10, f->y / 160, tz / 10, &sx, &sy, &sz);
        if (sz < 32 || off_x(sx, 64) || sy < -64 || sy > 512) continue;
        /* along: the quill's way on the screen (rocking with the sway);
         * across: its vane, narrowed as the feather turns over */
        double len = (c[4] / 2.0 * g->h) / (10.0 * sz), wid = len * 0.3 * (0.25 + 0.75 * fabs(cos(f->turn)));
        if (len < 1) continue;
        double ang = f->spin + 0.6 * sin(f->sway);
        double ax = sin(ang) * len, ay = cos(ang) * len * 20 / 12;
        double bx = cos(ang) * wid, by = -sin(ang) * wid * 20 / 12;
        /* its outline from the tip (t 1) to the quill's end (t -1): the
         * vane widest a little before the middle, the quill bare at the end,
         * the whole a little bent; a Gouraud quad between each two levels */
        static const double T[5] = {1, 0.4, -0.35, -0.72, -1}, W[5] = {0, 1, 0.78, 0.1, 0.06};
        /* a petal: round at its outer end (t 1), narrowing to its foot,
         * hardly bent; its foot a deeper pink */
        static const double PT[5] = {1, 0.75, 0.2, -0.5, -1}, PW[5] = {0.55, 0.95, 1, 0.62, 0.12};
        int petal = s_pack.feather_shape == 1;
        const double *TT = petal ? PT : T, *WW = petal ? PW : W;
        if (petal) { wid = len * 0.62 * (0.2 + 0.8 * fabs(cos(f->turn))); bx = cos(ang) * wid; by = -sin(ang) * wid * 20 / 12; }
        double lit = 0.72 + 0.28 * fabs(sin(f->turn));    /* the face turned up brighter */
        const uint8_t *rgb = s_pack.feather_rgb;
        int lx[5], ly[5], rx[5], ry[5];
        uint32_t lc[5], rc[5];
        for (int k = 0; k < 5; k++) {
            double bend = (petal ? 0.1 : 0.35) * (1 - TT[k] * TT[k]);
            double cx = sx + TT[k] * ax + bend * bx, cy = sy + TT[k] * ay + bend * by;
            lx[k] = (int)(cx - WW[k] * bx); ly[k] = (int)(cy - WW[k] * by);
            rx[k] = (int)(cx + WW[k] * bx); ry[k] = (int)(cy + WW[k] * by);
            double shade = lit * (0.78 + 0.22 * (TT[k] + 1) / 2);
            lc[k] = rc[k] = 0;
            for (int j = 0; j < 3; j++) {
                /* a petal's foot deeper (its green and blue less) */
                double deep = petal && j ? 0.8 + 0.2 * (TT[k] + 1) / 2 : 1;
                int a = (int)(rgb[j] * shade * 0.88 * deep), b2 = (int)(rgb[j] * shade * deep);
                lc[k] |= (uint32_t)(a > 255 ? 255 : a) << (8 * j);
                rc[k] |= (uint32_t)(b2 > 255 ? 255 : b2) << (8 * j);
            }
        }
        uint32_t ot = main_ot(sz < near_z ? 40 : 1009);
        for (int k = 0; k < 4; k++) {
            uint32_t q = alloc_words(9);
            if (!q) return;
            const int qx[4] = {lx[k], rx[k], lx[k + 1], rx[k + 1]}, qy[4] = {ly[k], ry[k], ly[k + 1], ry[k + 1]};
            const uint32_t col[4] = {lc[k], rc[k], lc[k + 1], rc[k + 1]};
            for (int v = 0; v < 4; v++) {
                psx_mod_write_word(q + 4 + 8 * v, (v ? 0 : 0x38000000u) | col[v]);
                psx_mod_write_word(q + 8 + 8 * v, xy(qx[v], qy[v]));
            }
            add_prim(ot, q, 8);
        }
    }
}

/* ---- DIYA: the Taj Mahal's oil lamps. Their flames, standing on their
 * bowls (the lamp picture's flame, its flicker the ANIM's), go out one by
 * one as the round goes on, as if the rain put them out: each at a frame of
 * its own, the flame sinking and guttering a while, then only a glow on the
 * wick fading; the floor round each lit while it burns. All lit again at a
 * round's start. Tiles' frame (cm). ---- */
#define DIYA_MAX 32
#define DIYA_GUTTER 40          /* frames the flame takes to die */
#define DIYA_EMBER 150          /* frames the wick glows after */
static int s_diya_out[DIYA_MAX];
static unsigned s_diya_frame;

static void diya_reset(void) {
    const int16_t *c = s_pack.diya;
    s_diya_frame = 0;
    for (int i = 0; i < DIYA_MAX; i++)
        s_diya_out[i] = c[3] + (c[4] > c[3] ? (int)(rnd() % (unsigned)(c[4] - c[3])) : 0);
}

static void diya_quad(uint32_t ot, int code, uint32_t col, const int x[4], const int y[4], unsigned tpage,
                      unsigned u0, unsigned v0, unsigned u1, unsigned v1) {
    tex_quad(ot, (uint32_t)code << 24 | col, x, y, s_pack.diya_clut, tpage, u0, v0, u1, v1);
}

static void diyas(const Gte *g) {
    const int16_t *c = s_pack.diya;
    int n = c[0] > DIYA_MAX ? DIYA_MAX : c[0];
    if (n <= 0 || !s_pack.diya_at) return;
    if (!paused() && (int16_t)psx_mod_read_half(SCREEN) == 8) s_diya_frame++;
    int near_z = g->t[2];
    for (int i = 0; i < n; i++) {
        const int16_t *at = s_pack.diya_at + 3 * i;
        int left = s_diya_out[i] - (int)s_diya_frame;          /* frames till it is out */
        if (left < -DIYA_EMBER) continue;
        int bx, by, bz, tx, ty, tz;
        rtps(g, at[0], at[1], at[2], &bx, &by, &bz);
        if (bz < 32 || off_x(bx, 64) || by < -64 || by > 540) continue;
        uint32_t ot = main_ot(bz < near_z ? 40 : 1009);
        double k = 1.0;                                       /* the flame's height */
        if (left > 0) {
            /* its light on the floor round it (the lamp picture's floor
             * light), as bright as the flame is high */
            double lv = left < DIYA_GUTTER ? (double)left / DIYA_GUTTER : 1.0;
            int r = s_pack.diya_pool / 2, sx[4], sy[4], sz;
            static const int cx[4] = {-1, 1, -1, 1}, cz[4] = {-1, -1, 1, 1};
            int ok = 1;
            for (int j = 0; j < 4 && ok; j++) {
                rtps(g, at[0] + cx[j] * r, -1, at[2] + cz[j] * r, &sx[j], &sy[j], &sz);
                ok = sz >= 32;
            }
            if (ok) {
                int lvl = (int)(s_pack.diya_level * lv * (0.9 + 0.1 * sin(s_diya_frame * 0.31 + i)));
                const uint8_t *uv = s_pack.diya_uv;
                /* under the rest: the floor's rank */
                diya_quad(main_ot(1009), 0x2E, (uint32_t)lvl << 16 | (uint32_t)lvl << 8 | (uint32_t)lvl, sx, sy,
                          s_pack.diya_tpage | 0x20u, uv[0], uv[1] + 16u, uv[0] + 63u, uv[1] + 47u);
            }
        }
        if (left < DIYA_GUTTER) {
            double t = left > 0 ? (double)left / DIYA_GUTTER : 0;
            /* guttering: sinking, ducking and flaring now and then */
            k = t * (0.75 + 0.25 * sin(s_diya_frame * 0.9 + i)) * (((s_diya_frame + i) & 7) == 0 ? 0.4 : 1.0);
        }
        if (k > 0.05) {
            rtps(g, at[0], at[1] - (int)(c[1] * k), at[2], &tx, &ty, &tz);
            int hw = (int)(c[2] / 2.0 * g->h / bz);
            if (hw < 1) hw = 1;
            /* the tip a pixel aside: never a screen-square quad (dust()) */
            const int x[4] = {tx - hw + 1, tx + hw + 1, bx - hw, bx + hw}, y[4] = {ty, ty, by, by};
            const uint8_t *uv = s_pack.diya_uv;
            unsigned v0 = uv[1] + (unsigned)(15 * (1 - k));         /* the flame's top cut off as it sinks */
            diya_quad(ot, 0x2D, 0x808080u, x, y, s_pack.diya_tpage, uv[0], v0, uv[0] + 31u, uv[1] + 15u);
        }
        if (left < DIYA_GUTTER / 2) {
            /* the wick's glow, fading after the flame */
            double e = left > 0 ? 1.0 : 1.0 + (double)left / DIYA_EMBER;
            int col = (int)(0x50 * e);
            int hw = (int)(c[2] * 0.4 * g->h / bz);
            if (hw < 1) hw = 1;
            const int x[4] = {bx - hw, bx + hw, bx - hw + 1, bx + hw + 1}, y[4] = {by - hw, by - hw, by + hw, by + hw};
            const uint8_t *uv = s_pack.diya_uv;
            diya_quad(ot, 0x2E, (uint32_t)(col / 3) << 16 | (uint32_t)(col / 2) << 8 | (uint32_t)col, x, y,
                      s_pack.diya_tpage | 0x20u, uv[2], uv[3], uv[2] + 15u, uv[3] + 15u);
        }
    }
}

/* ---- RAIN: the Taj Mahal's storm. Drops before the camera, falling fast
 * and drifting with the wind; each a short streak from where it was to
 * where it is, the drop picture along it: a round head, a tail. Tiles'
 * frame (mm). ---- */
#define RAIN_MAX 160
typedef struct { int x, y, z; } Drop;
static Drop s_drop[RAIN_MAX];
static int s_ndrop, s_rain_seen;

/* the camera in the tiles' frame (g maps tile points to view space; its
 * y row is scaled by 20 / 12) */
static void tile_camera(const Gte *g, int64_t cam[3], int64_t fwd[3]) {
    for (int j = 0; j < 3; j++) {
        int64_t r1 = (int64_t)g->r[1][j] * 12 / 20;
        cam[j] = -((int64_t)g->r[0][j] * g->t[0] + r1 * g->t[1] + (int64_t)g->r[2][j] * g->t[2]) / 4096;
        fwd[j] = g->r[2][j];
    }
}

static void rain_spawn(const Gte *g, Drop *r, int strewn) {
    const int16_t *c = s_pack.rain;
    int64_t cam[3], fwd[3];
    tile_camera(g, cam, fwd);
    double fx = (double)fwd[0], fz = (double)fwd[2], n = sqrt(fx * fx + fz * fz);
    if (n < 1) { fx = 0; fz = 1; } else { fx /= n; fz /= n; }
    double a = ((rnd() & 0xFFF) / 4096.0 - 0.5) * 1.5;     /* within the view */
    double dx = fx * cos(a) - fz * sin(a), dz = fx * sin(a) + fz * cos(a);
    int d = c[5] + rnd() % (c[6] - c[5]);
    int64_t top = cam[1] * 10 - 4000 - (rnd() & 0x7FF);
    r->x = (int)(cam[0] * 10 + (int64_t)(dx * d));
    r->z = (int)(cam[2] * 10 + (int64_t)(dz * d));
    r->y = strewn ? (int)(top * (rnd() & 0x7FFF) / 0x8000) : (int)top;
}

static void rain(const Gte *g) {
    if (!s_pack.rain_on) return;
    const int16_t *c = s_pack.rain;
    int n = c[0] > RAIN_MAX ? RAIN_MAX : c[0];
    if (!s_rain_seen) {
        for (s_ndrop = 0; s_ndrop < n; s_ndrop++) rain_spawn(g, &s_drop[s_ndrop], 1);
        s_rain_seen = 1;
    }
    if (!paused())
        for (int i = 0; i < s_ndrop; i++) {
            Drop *r = &s_drop[i];
            r->x += c[3]; r->y += c[1]; r->z += c[4];
            if (r->y > 0) rain_spawn(g, r, 0);
        }
    int near_z = g->t[2];
    uint32_t col = 0;
    for (int j = 0; j < 3; j++) col |= (uint32_t)s_pack.rain_rgb[j] << (8 * j);
    for (int i = 0; i < s_ndrop; i++) {
        const Drop *r = &s_drop[i];
        /* the streak: its foot where the drop is, its top a length back
         * along its way */
        double k = (double)c[2] / c[1];
        int ax, ay, az, bx, by, bz;
        rtps(g, r->x / 10, r->y / 10, r->z / 10, &ax, &ay, &az);
        rtps(g, (int)(r->x - c[3] * k) / 10, (int)(r->y - c[2]) / 10, (int)(r->z - c[4] * k) / 10, &bx, &by, &bz);
        if (az < 64 || bz < 64 || off_x(ax, 32) || ay < -64 || ay > 540) continue;
        double lx = ax - bx, ly = ay - by, len = sqrt(lx * lx + ly * ly);
        if (len < 2) continue;
        /* two or three pixels across (the drop's round head), nearer wider */
        double w = (14.0 * g->h) / (10.0 * az);
        if (w < 1.0) w = 1.0;
        if (w > 1.8) w = 1.8;
        int px = (int)(-ly / len * w), py = (int)(lx / len * w);
        if (!px && !py) px = 1;
        /* the drop picture stretched along it, its head at the foot */
        const int x[4] = { bx - px, bx + px, ax - px, ax + px }, y[4] = { by - py, by + py, ay - py, ay + py };
        tex_quad(main_ot(az < near_z ? 40 : 1009), 0x2E000000u | col, x, y, s_pack.rain_clut, s_pack.rain_tpage,
                 s_pack.rain_u, s_pack.rain_v, s_pack.rain_u + 15u, s_pack.rain_v + 15u);
    }
}

/* ---- FFLY: fireflies low over the ground round the fight, each wandering
 * slowly about a spot of its own, glowing up and fading away again (the
 * glow picture times its colour); dark, one far off the fight starts at a
 * new spot. World units (mm). ---- */
#define FIREFLY_MAX 32
typedef struct { int hx, hy, hz; double ph[4], w[4]; } Firefly;
static Firefly s_ffly[FIREFLY_MAX];
static int s_nffly, s_ffly_seen;
static unsigned s_ffly_t;

static void firefly_spawn(Firefly *f) {
    const int16_t *c = s_pack.ffly;
    int p1x = (int)psx_mod_read_word(P1_X), p1z = (int)psx_mod_read_word(P1_Z);
    int p2x = (int)psx_mod_read_word(P2_X), p2z = (int)psx_mod_read_word(P2_Z);
    double a = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    int r = c[1] + rnd() % (c[2] - c[1]);
    f->hx = p1x + (p2x - p1x) / 2 + (int)(r * cos(a));
    f->hz = p1z + (p2z - p1z) / 2 + (int)(r * sin(a));
    f->hy = c[3] + (c[4] > c[3] ? rnd() % (c[4] - c[3]) : 0);
    for (int k = 0; k < 4; k++) f->ph[k] = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->w[0] = 0.006 + (rnd() & 0xFF) / 255.0 * 0.008;
    f->w[1] = 0.005 + (rnd() & 0xFF) / 255.0 * 0.008;
    f->w[2] = 0.011 + (rnd() & 0xFF) / 255.0 * 0.01;
    f->w[3] = 0.025 + (rnd() & 0xFF) / 255.0 * 0.035;   /* the glow: a second or two */
}

static uint32_t tinted(const double tint[3], double b) {
    uint32_t col = 0;
    for (int j = 0; j < 3; j++) {
        int v = (int)(s_pack.ffly_rgb[j] * b * tint[j]);
        col |= (uint32_t)(v > 255 ? 255 : v) << (8 * j);
    }
    return col;
}

/* Szechwan's butterflies (kind 1): the fireflies' wandering, wider and
 * quicker, always seen; each the butterfly picture (solid, tinted its
 * colour: yellow, white or orange in turn), its wings beating (the picture
 * narrowed across and back), now and then gliding a while. One far off the
 * fight starts anew where it is not seen. */
static void butterflies(const Gte *g, int mx, int mz, int still) {
    const int16_t *c = s_pack.ffly;
    int near_z = g->t[2];
    static const double TINT[3][3] = {{1, 1, 1}, {1.0, 1.07, 2.2}, {1.1, 0.7, 0.4}};
    for (int i = 0; i < s_nffly; i++) {
        Firefly *f = &s_ffly[i];
        double t = s_ffly_t;
        int x = f->hx + (int)(700 * sin(t * f->w[0] * 2.2 + f->ph[0]) + 250 * sin(t * f->w[2] * 3.1 + f->ph[3]));
        int z = f->hz + (int)(700 * cos(t * f->w[1] * 2.2 + f->ph[1]));
        int y = f->hy + (int)(220 * sin(t * f->w[2] * 2.0 + f->ph[2]) + 60 * sin(t * 0.21 + f->ph[3]));
        int sx, sy, sz, tx, tz;
        world_tile(x, z, &tx, &tz);
        rtps(g, tx / 10, y / 10, tz / 10, &sx, &sy, &sz);
        int seen = sz >= 400 && !off_x(sx, 32) && sy >= -32 && sy <= 512;
        double d = hypot(f->hx - mx, f->hz - mz);
        if (!seen) {
            if (!still && (d > c[2] * 1.4 || d < c[1] * 0.6)) firefly_spawn(f);
            continue;
        }
        /* the wings: beating fast, a glide now and then (open, still) */
        double glide = sin(t * 0.013 + f->ph[2] * 3);
        double beat = glide > 0.75 ? 0.9 : fabs(cos(t * 0.42 + f->ph[0] * 5));
        int hx = (int)((c[5] * (int64_t)g->h) / (10 * (int64_t)sz)), hy = hx * 20 / 12;
        if (hy < 2) hy = 2;
        hx = (int)(hx * (0.15 + 0.85 * beat));
        if (hx < 1) hx = 1;
        /* tilted a little, rocking (never screen-square: dust()) */
        double turn = 0.2 * sin(t * 0.05 + f->ph[1]) + 0.05;
        int cx = (int)(hx * cos(turn)), cy = (int)(hx * sin(turn) * 20 / 12);
        int dx = (int)(-hy * sin(turn) * 12 / 20), dy = (int)(hy * cos(turn));
        turned_quad(main_ot(sz < near_z ? 40 : 1009), 0x2C000000u | tinted(TINT[i % 3], 1), sx, sy, cx, cy, dx, dy,
                    s_pack.ffly_clut, s_pack.ffly_tpage, s_pack.ffly_u, s_pack.ffly_v);
    }
}

/* The Great Wall's dragonflies (kind 3): each hovers at a spot, trembling a
 * little, then darts off a metre or so in a blink and hovers again, its
 * body along its way; the dragonfly picture (solid, tinted blue, green or
 * red in turn), its wings a shimmer (the picture narrowed every other
 * frame). One far off the fight starts anew where it is not seen. World
 * units (mm). */
typedef struct { double x, y, z, fx, fy, fz, tx, ty, tz, head; int t, dur, hover; } Dragonfly;
static Dragonfly s_dfly[FIREFLY_MAX];

static void dragonfly_dart(Dragonfly *d, const Firefly *f, int start) {
    const int16_t *c = s_pack.ffly;
    if (start) { d->x = d->tx = f->hx; d->y = d->ty = f->hy; d->z = d->tz = f->hz; }
    d->fx = d->x; d->fy = d->y; d->fz = d->z;
    double a = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    int r = 300 + rnd() % 900;
    d->tx = f->hx + r * sin(a); d->tz = f->hz + r * cos(a);
    d->ty = c[3] + (c[4] > c[3] ? rnd() % (c[4] - c[3]) : 0);
    d->head = atan2(d->tx - d->fx, d->tz - d->fz);
    d->t = 0;
    d->dur = start ? 0 : 9 + rnd() % 8;
    d->hover = 50 + rnd() % 130;
}

static void dragonflies(const Gte *g, int mx, int mz, int still) {
    const int16_t *c = s_pack.ffly;
    int near_z = g->t[2];
    static const double TINT[3][3] = {{1, 1, 1}, {0.9, 1.25, 0.5}, {2.1, 0.6, 0.4}};
    if (s_ffly_t <= 1)
        for (int i = 0; i < s_nffly; i++) dragonfly_dart(&s_dfly[i], &s_ffly[i], 1);
    for (int i = 0; i < s_nffly; i++) {
        Firefly *f = &s_ffly[i];
        Dragonfly *d = &s_dfly[i];
        if (!still) {
            if (d->t < d->dur) {
                double s = (d->t + 1.0) / d->dur, e = s * s * (3 - 2 * s);
                d->x = d->fx + (d->tx - d->fx) * e; d->y = d->fy + (d->ty - d->fy) * e; d->z = d->fz + (d->tz - d->fz) * e;
            } else {
                /* hovering: a tremble round the spot */
                double t = s_ffly_t;
                d->x = d->tx + 25 * sin(t * 0.21 + f->ph[0]); d->y = d->ty + 18 * sin(t * 0.17 + f->ph[1]);
                d->z = d->tz + 25 * cos(t * 0.19 + f->ph[2]);
            }
            if (++d->t >= d->dur + d->hover) dragonfly_dart(d, f, 0);
        }
        int sx, sy, sz, tx, tz, hx2, hy2, hz2;
        world_tile((int)d->x, (int)d->z, &tx, &tz);
        rtps(g, tx / 10, (int)d->y / 10, tz / 10, &sx, &sy, &sz);
        int seen = sz >= 400 && !off_x(sx, 32) && sy >= -32 && sy <= 512;
        double dd = hypot(f->hx - mx, f->hz - mz);
        if (!seen) {
            if (!still && (dd > c[2] * 1.4 || dd < c[1] * 0.6)) { firefly_spawn(f); dragonfly_dart(d, f, 1); }
            continue;
        }
        /* its way on the screen (the pixels' aspect taken out) */
        world_tile((int)(d->x + 100 * sin(d->head)), (int)(d->z + 100 * cos(d->head)), &tx, &tz);
        rtps(g, tx / 10, (int)d->y / 10, tz / 10, &hx2, &hy2, &hz2);
        double ax = hx2 - sx, ay = (hy2 - sy) * 12.0 / 20.0, al = hypot(ax, ay);
        if (al < 0.5) { ax = 1; ay = 0; al = 1; }
        ax /= al; ay /= al;
        double len = (c[5] * (double)g->h) / (10.0 * sz);
        if (len < 2) len = 2;
        double wid = len * ((s_ffly_t & 1) ? 0.82 : 1.0);
        /* the head's end and the tail's along its way, the wings across */
        double hxo = ax * len / 2, hyo = ay * len / 2 * 20 / 12, exo = -ay * wid / 2, eyo = ax * wid / 2 * 20 / 12;
        const int x[4] = { (int)(sx + hxo - exo), (int)(sx + hxo + exo), (int)(sx - hxo - exo), (int)(sx - hxo + exo) };
        const int y[4] = { (int)(sy + hyo - eyo), (int)(sy + hyo + eyo), (int)(sy - hyo - eyo), (int)(sy - hyo + eyo) };
        tex_quad(main_ot(sz < near_z ? 40 : 1009), 0x2C000000u | tinted(TINT[i % 3], 1), x, y, s_pack.ffly_clut,
                 s_pack.ffly_tpage, s_pack.ffly_u, s_pack.ffly_v, s_pack.ffly_u + 15u, s_pack.ffly_v + 15u);
    }
}

static void fireflies(const Gte *g) {
    if (!s_pack.ffly_on) return;
    const int16_t *c = s_pack.ffly;
    int n = c[0] > FIREFLY_MAX ? FIREFLY_MAX : c[0];
    if (!s_ffly_seen) {
        for (s_nffly = 0; s_nffly < n; s_nffly++) firefly_spawn(&s_ffly[s_nffly]);
        s_ffly_seen = 1;
    }
    int still = paused();
    if (!still) s_ffly_t++;
    int mx = ((int)psx_mod_read_word(P1_X) + (int)psx_mod_read_word(P2_X)) / 2;
    int mz = ((int)psx_mod_read_word(P1_Z) + (int)psx_mod_read_word(P2_Z)) / 2;
    int near_z = g->t[2];
    if (s_pack.ffly_kind == 1) { butterflies(g, mx, mz, still); return; }
    if (s_pack.ffly_kind == 3) { dragonflies(g, mx, mz, still); return; }
    for (int i = 0; i < s_nffly; i++) {
        Firefly *f = &s_ffly[i];
        double t = s_ffly_t;
        double pulse = sin(t * f->w[3] + f->ph[3]);
        if (pulse <= 0.05) {
            /* dark: one strayed off the fight (the fighters walked) starts anew */
            double d = hypot(f->hx - mx, f->hz - mz);
            if (!still && (d > c[2] * 1.4 || d < c[1] * 0.6)) firefly_spawn(f);
            continue;
        }
        double b = pulse * pulse;
        int x = f->hx + (int)(350 * sin(t * f->w[0] + f->ph[0]));
        int z = f->hz + (int)(350 * cos(t * f->w[1] + f->ph[1]));
        int y = f->hy + (int)(150 * sin(t * f->w[2] + f->ph[2]));
        int sx, sy, sz, tx, tz;
        world_tile(x, z, &tx, &tz);
        rtps(g, tx / 10, y / 10, tz / 10, &sx, &sy, &sz);
        if (sz < 32 || off_x(sx, 32) || sy < -32 || sy > 512) continue;
        int hx = (int)((c[5] * (int64_t)g->h) / (10 * (int64_t)sz)), hy = hx * 20 / 12;
        if (hx < 2) hx = 2;
        if (hy < 2) hy = 2;
        /* kind 2: each bug its own colour (green, sky blue, pink, amber) */
        static const double BUG[4][3] = {{1, 1, 1}, {0.45, 1.0, 1.9}, {1.5, 0.6, 1.3}, {1.6, 0.95, 0.3}};
        static const double ONE[3] = {1, 1, 1};
        uint32_t col = tinted(s_pack.ffly_kind == 2 ? BUG[i % 4] : ONE, b);
        /* turned a little (a screen-square quad is the interface's to the
         * wide-screen layer: dust()) */
        double turn = 0.3 + i * 0.37;
        int cx = (int)(hx * cos(turn)), cy = (int)(hx * sin(turn) * 20 / 12);
        int dx = (int)(-hy * sin(turn) * 12 / 20), dy = (int)(hy * cos(turn));
        turned_quad(main_ot(sz < near_z ? 40 : 1009), 0x2E000000u | col, sx, sy, cx, cy, dx, dy, s_pack.ffly_clut,
                    s_pack.ffly_tpage, s_pack.ffly_u, s_pack.ffly_v);
    }
}

/* ---- FTHR shapes 2, 4: the Taj Mahal's rose petals, Serene Forest's
 * leaves: strewn on the floor round the fight, lying flat; a fighter walking
 * through them kicks the near ones up a little, one falling to the ground
 * throws all round it up as from a heap of leaves (its hips stopped low
 * after a fast fall, as dust()); up, they flutter and spin down slowly and
 * lie again where they land. Kept at their world places (mm, x 16 for the
 * speeds); one lying far from the fight is laid anew near it, unseen. The
 * leaves: two always coming loose high up near the fight now and then. ---- */
#define PETAL_MAX 96
typedef struct { int x, y, z, vx, vy, vz, up; double rest, spin, spin_rate, turn, turn_rate; } Petal;
static Petal s_petal[PETAL_MAX];
static int s_npetal, s_petal_seen, s_petal_prev[2], s_petal_prev_v[2], s_petal_cool[2], s_petal_px[2], s_petal_pz[2];

static void petal_lay(Petal *f, int mx, int mz) {
    const int16_t *c = s_pack.feather;
    double a = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    int r = c[1] + (c[2] > c[1] ? rnd() % (c[2] - c[1]) : 0);
    f->x = (mx + (int)(r * cos(a))) * 16; f->z = (mz + (int)(r * sin(a))) * 16; f->y = 0;
    f->vx = f->vy = f->vz = 0; f->up = 0;
    f->rest = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
}

static void petal_kick(Petal *f, int fx, int fz, int strength) {
    /* away from the foot, up: strength 1 a step, 3 a body on the ground */
    double dx = f->x / 16.0 - fx, dz = f->z / 16.0 - fz, d = sqrt(dx * dx + dz * dz);
    if (d < 1) { dx = 1; d = 1; }
    int out = (8 + rnd() % 14) * strength * 16, up = (14 + rnd() % 18) * strength * 16;
    f->vx = (int)(out * dx / d); f->vz = (int)(out * dz / d); f->vy = -up;
    f->up = 1;
    f->spin = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->spin_rate = ((rnd() & 0xFF) / 255.0 - 0.5) * 0.25;
    f->turn = 0; f->turn_rate = 0.08 + (rnd() & 0xFF) / 255.0 * 0.1;
}

static void leaf_drop(Petal *f, int mx, int mz) {
    const int16_t *c = s_pack.feather;
    double a = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    int r = 300 + rnd() % (c[2] > 300 ? c[2] - 300 : 1);
    f->x = (mx + (int)(r * cos(a))) * 16; f->z = (mz + (int)(r * sin(a))) * 16;
    f->y = -c[3] * 16;
    f->vx = f->vz = 0; f->vy = 0; f->up = 1;
    f->spin = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->spin_rate = ((rnd() & 0xFF) / 255.0 - 0.5) * 0.12;
    f->turn = 0; f->turn_rate = 0.05 + (rnd() & 0xFF) / 255.0 * 0.06;
}

static void strewn_petals(const Gte *g) {
    const int16_t *c = s_pack.feather;
    int n = c[0] < 0 ? 0 : c[0] > PETAL_MAX ? PETAL_MAX : c[0];
    if (!n) return;
    int fx[2] = {(int)psx_mod_read_word(P1_X), (int)psx_mod_read_word(P2_X)};
    int fz[2] = {(int)psx_mod_read_word(P1_Z), (int)psx_mod_read_word(P2_Z)};
    int mx = fx[0] + (fx[1] - fx[0]) / 2, mz = fz[0] + (fz[1] - fz[0]) / 2;
    if (!s_petal_seen) {
        for (s_npetal = 0; s_npetal < n; s_npetal++) petal_lay(&s_petal[s_npetal], mx, mz);
        for (int p = 0; p < 2; p++) {
            s_petal_prev[p] = (int)psx_mod_read_word(p ? P2_Y : P1_Y); s_petal_prev_v[p] = 0; s_petal_cool[p] = 0;
            s_petal_px[p] = fx[p]; s_petal_pz[p] = fz[p];
        }
        s_petal_seen = 1;
    }
    if (!paused()) {
        for (int p = 0; p < 2; p++) {
            /* a fall: the hips stopped low after falling fast (dust()) */
            int y = (int)psx_mod_read_word(p ? P2_Y : P1_Y), v = y - s_petal_prev[p];
            if (v > 400 || v < -400) v = 0;
            if (s_petal_cool[p] > 0) s_petal_cool[p]--;
            int fell = s_petal_prev_v[p] > 14 && v < s_petal_prev_v[p] / 3 && y > -1300 && !s_petal_cool[p];
            if (fell) s_petal_cool[p] = 10;
            s_petal_prev[p] = y; s_petal_prev_v[p] = v;
            /* a step: the fighter moving on the floor, its feet down */
            int sx = fx[p] - s_petal_px[p], sz = fz[p] - s_petal_pz[p];
            if (sx > 300 || sx < -300 || sz > 300 || sz < -300) sx = sz = 0;      /* placed anew */
            int stepping = y > -1200 && (sx * sx + sz * sz) > 6 * 6;
            s_petal_px[p] = fx[p]; s_petal_pz[p] = fz[p];
            if (!fell && !stepping) continue;
            int reach = fell ? (y > -600 ? 1500 : 900) : 450;
            for (int i = 0; i < s_npetal; i++) {
                Petal *f = &s_petal[i];
                if (f->up && !fell) continue;
                double dx = f->x / 16.0 - fx[p], dz = f->z / 16.0 - fz[p], d2 = dx * dx + dz * dz;
                if (d2 > (double)reach * reach) continue;
                if (!fell && (rnd() & 3)) continue;          /* a step lifts a few */
                petal_kick(f, fx[p], fz[p], fell ? (d2 < reach * reach / 4.0 ? 3 : 2) : 1);
            }
        }
        for (int i = 0; i < s_npetal; i++) {
            Petal *f = &s_petal[i];
            if (!f->up) {
                double dx = f->x / 16.0 - mx, dz = f->z / 16.0 - mz;
                if (dx * dx + dz * dz > 2.25 * c[2] * c[2]) petal_lay(f, mx, mz);
                /* the leaves: the first two fall from the trees now and then */
                if (s_pack.feather_shape == 4 && i < 2 && !(rnd() % 150)) leaf_drop(f, mx, mz);
                continue;
            }
            /* the air holds it: slow to fall, quick to stop sideways */
            f->x += f->vx; f->y += f->vy; f->z += f->vz;
            f->vx = f->vx * 30 / 32; f->vz = f->vz * 30 / 32;
            f->vy += 24;
            int fall = c[5] * 16 / 3;
            if (f->vy > fall) f->vy = fall;
            f->spin += f->spin_rate; f->turn += f->turn_rate;
            if (f->vy > 0) f->x += (int)(160 * sin(f->turn * 0.7));             /* rocking down */
            if (f->y >= 0) { f->y = 0; f->up = 0; f->rest = f->spin; }
        }
    }
    int near_z = g->t[2];
    const uint8_t *rgb = s_pack.feather_rgb;
    for (int i = 0; i < s_npetal; i++) {
        const Petal *f = &s_petal[i];
        /* its outline in the world: along a (its length), across b, the
         * levels of feathers()' petal; lying, both flat on the floor */
        static const double PT[5] = {1, 0.75, 0.2, -0.5, -1}, PW[5] = {0.55, 0.95, 1, 0.62, 0.12};
        static const double LW[5] = {0.04, 0.72, 1, 0.7, 0.08};              /* a leaf: pointed both ends */
        int leaf = s_pack.feather_shape == 4;
        const double *pw = leaf ? LW : PW;
        double L = c[4] / 2.0, W = c[4] * (leaf ? 0.26 : 0.31), ax, ay, az, bx, by, bz;
        if (!f->up) {
            ax = cos(f->rest); ay = 0; az = sin(f->rest);
            bx = -az; by = 0; bz = ax;
        } else {
            double tilt = cos(f->turn);                                        /* turning over */
            ax = cos(f->spin); ay = 0.5 * sin(f->turn); az = sin(f->spin);
            bx = -az * tilt; by = sin(f->turn) * 0.8; bz = ax * tilt;
        }
        int px[10], py[10], ok = 1, depth = 0;
        for (int k = 0; k < 5 && ok; k++)
            for (int s = 0; s < 2 && ok; s++) {
                double w = (s ? 1 : -1) * pw[k] * W;
                int wx = f->x / 16 + (int)(PT[k] * L * ax + w * bx), wz = f->z / 16 + (int)(PT[k] * L * az + w * bz);
                int wy = f->y / 16 + (int)(PT[k] * L * ay + w * by) - (f->up ? 0 : 6);
                int tx, tz, sz;
                world_tile(wx, wz, &tx, &tz);
                rtps(g, tx / 10, wy / 10, tz / 10, &px[2 * k + s], &py[2 * k + s], &sz);
                if (sz < 32 || off_x(px[2 * k + s], 64) || py[2 * k + s] < -64 || py[2 * k + s] > 540) ok = 0;
                depth = sz;
            }
        if (!ok) continue;
        /* lying: on the floor, under the fighters; up: in front or behind */
        uint32_t ot = main_ot(!f->up ? 980 : depth < near_z ? 40 : 1009);
        double lit = f->up ? 0.75 + 0.25 * fabs(cos(f->turn)) : 0.8;
        /* the leaves: yellow, orange, golden brown, brown */
        static const double AUTUMN[4][3] = {{1, 1, 1}, {1.05, 0.72, 0.6}, {0.85, 0.72, 0.6}, {0.62, 0.45, 0.4}};
        const double *tint = leaf ? AUTUMN[i % 4] : AUTUMN[0];
        for (int k = 0; k < 4; k++) {
            uint32_t q = alloc_words(9);
            if (!q) return;
            for (int v = 0; v < 4; v++) {
                int idx = 2 * (k + (v >> 1)) + (v & 1);
                double t = PT[k + (v >> 1)], shade = lit * (0.78 + 0.22 * (t + 1) / 2);
                uint32_t col = 0;
                for (int j = 0; j < 3; j++) {
                    double deep = j ? 0.8 + 0.2 * (t + 1) / 2 : 1;
                    int a = (int)(rgb[j] * tint[j] * shade * deep * ((v & 1) ? 1 : 0.88));
                    col |= (uint32_t)(a > 255 ? 255 : a) << (8 * j);
                }
                psx_mod_write_word(q + 4 + 8 * v, (v ? 0 : 0x38000000u) | col);
                psx_mod_write_word(q + 8 + 8 * v, xy(px[idx], py[idx]));
            }
            add_prim(ot, q, 8);
        }
    }
}

/* ---- FTHR shape 3: New York's newspapers (then the picture: texture page,
 * palette, u, v, width, height): a page or two folded once, the wind
 * carrying them round the fight in gusts, tumbling and flapping at the
 * fold, settling on the floor and sliding till a gust lifts them again; a
 * fighter stepping by kicks one up. Gone too far: a new one blows in from
 * upwind. World units (mm); the picture's left half one side of the fold,
 * its right half the other. ---- */
#define PAPER_MAX 4
typedef struct { double x, y, z, vx, vy, vz, yaw, tumble, tumble_rate, roll, fold, phase; int up; } Paper;
static Paper s_paper[PAPER_MAX];
static int s_npaper, s_paper_seen;
static unsigned s_paper_t;
static double s_wind;

static void paper_blow_in(Paper *f, int mx, int mz, int anywhere) {
    const int16_t *c = s_pack.feather;
    double a = anywhere ? (rnd() & 0xFFF) * (6.2831853 / 4096.0) : s_wind + 3.14159265
        + ((rnd() & 0xFF) / 255.0 - 0.5) * 1.6;
    int r = anywhere ? c[1] + (c[2] > c[1] ? rnd() % (c[2] - c[1]) : 0) : c[2];
    f->x = mx + r * sin(a); f->z = mz + r * cos(a);
    f->y = -(400 + rnd() % (c[3] > 600 ? c[3] - 400 : 200));
    f->vx = f->vz = 0; f->vy = 0; f->up = 1;
    f->yaw = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->tumble = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    f->tumble_rate = 0.03 + (rnd() & 0xFF) / 255.0 * 0.05;
    f->roll = 0; f->fold = 0.5;
    f->phase = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
}

static void newspapers(const Gte *g) {
    const int16_t *c = s_pack.feather;
    int n = c[0] < 0 ? 0 : c[0] > PAPER_MAX ? PAPER_MAX : c[0];
    if (!n || !s_pack.paper_w) return;
    int fx[2] = {(int)psx_mod_read_word(P1_X), (int)psx_mod_read_word(P2_X)};
    int fz[2] = {(int)psx_mod_read_word(P1_Z), (int)psx_mod_read_word(P2_Z)};
    int mx = fx[0] + (fx[1] - fx[0]) / 2, mz = fz[0] + (fz[1] - fz[0]) / 2;
    if (!s_paper_seen) {
        s_wind = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
        for (s_npaper = 0; s_npaper < n; s_npaper++) paper_blow_in(&s_paper[s_npaper], mx, mz, 1);
        s_paper_seen = 1;
    }
    if (!paused()) {
        s_paper_t++;
        s_wind += 0.0006 * sin(s_paper_t * 0.002);                    /* the wind turning slowly */
        double wx = sin(s_wind), wz = cos(s_wind);
        for (int i = 0; i < s_npaper; i++) {
            Paper *f = &s_paper[i];
            /* gusts: each paper its own, now and then a strong one */
            double gust = 0.5 + 0.32 * sin(s_paper_t * 0.011 + f->phase) + 0.18 * sin(s_paper_t * 0.029 + 2 * f->phase);
            for (int p = 0; p < 2; p++) {
                int y = (int)psx_mod_read_word(p ? P2_Y : P1_Y);
                double dx = f->x - fx[p], dz = f->z - fz[p], d = sqrt(dx * dx + dz * dz);
                if (d < 650 && y > -1200 && (f->up == 0 || f->y > -300)) {
                    if (d < 1) { dx = 1; d = 1; }
                    f->vx = 9 * dx / d; f->vz = 9 * dz / d; f->vy = -14 - rnd() % 8; f->up = 1;
                    f->tumble_rate = 0.06 + (rnd() & 0xFF) / 255.0 * 0.06;
                }
            }
            if (f->up) {
                double speed = 3 + 13 * gust;
                f->vx += (wx * speed - f->vx) * 0.03 + 0.6 * sin(f->tumble);
                f->vz += (wz * speed - f->vz) * 0.03 + 0.6 * cos(f->tumble * 0.7);
                double lift = gust > 0.72 ? 12 * (gust - 0.72) / 0.28 : 0;
                f->vy += (4.5 - lift - f->vy) * 0.04;
                if (f->y < -2600 && f->vy < 0) f->vy = 0;
                f->x += f->vx; f->y += f->vy; f->z += f->vz;
                f->tumble += f->tumble_rate;
                f->roll = 0.5 * sin(f->tumble * 0.6 + f->phase);
                f->yaw += 0.01 * sin(f->tumble * 0.3);
                f->fold = 0.55 + 0.4 * sin(f->tumble * 2.3);
                if (f->y >= 0) { f->y = 0; f->up = 0; f->vy = 0; }
            } else {
                /* lying: flattening out, sliding before the stronger gusts */
                f->tumble = 0; f->roll = 0; f->fold += (0.12 - f->fold) * 0.1;
                if (gust > 0.6) { f->x += wx * (gust - 0.6) * 10; f->z += wz * (gust - 0.6) * 10; f->fold = 0.12 + 0.1 * sin(s_paper_t * 0.4); }
                if (gust > 0.88 && !(rnd() & 15)) { f->up = 1; f->vy = -6 - rnd() % 6; f->vx = wx * 6; f->vz = wz * 6; }
            }
            double dx = f->x - mx, dz = f->z - mz;
            if (dx * dx + dz * dz > 2.25 * c[2] * c[2]) paper_blow_in(f, mx, mz, 0);
        }
    }
    int near_z = g->t[2];
    double L = c[4] / 2.0, W = L * 0.72;
    for (int i = 0; i < s_npaper; i++) {
        const Paper *f = &s_paper[i];
        /* its frame: a along it, b across (the fold), n its face */
        double cy = cos(f->yaw), sy = sin(f->yaw), ct = cos(f->tumble), st = sin(f->tumble);
        double a0[3] = {cy, 0, sy}, b0[3] = {-sy, 0, cy}, n0[3] = {0, -1, 0}, a[3], n1[3], b[3], nn[3];
        for (int k = 0; k < 3; k++) { a[k] = a0[k] * ct + n0[k] * st; n1[k] = -a0[k] * st + n0[k] * ct; }
        double cr = cos(f->roll), sr = sin(f->roll);
        for (int k = 0; k < 3; k++) { b[k] = b0[k] * cr + n1[k] * sr; nn[k] = -b0[k] * sr + n1[k] * cr; }
        double cf = cos(f->fold), sf = sin(f->fold);
        /* the six corners: the fold's two ends, each half's outer two */
        double P[6][3];
        for (int s = 0; s < 2; s++)
            for (int k = 0; k < 3; k++) {
                double crease = (s ? W : -W) * b[k];
                P[s][k] = crease;
                P[2 + s][k] = crease + L * (a[k] * cf + nn[k] * sf);
                P[4 + s][k] = crease + L * (-a[k] * cf + nn[k] * sf);
            }
        int px[6], py[6], ok = 1, depth = 0;
        double lift = f->up ? 0 : -5;
        for (int v = 0; v < 6 && ok; v++) {
            int tx, tz, sz;
            world_tile((int)(f->x + P[v][0]), (int)(f->z + P[v][2]), &tx, &tz);
            double wy = f->y + P[v][1] + lift;
            if (!f->up && wy > -2) wy = -2;
            rtps(g, tx / 10, (int)(wy / 10), tz / 10, &px[v], &py[v], &sz);
            if (sz < 32 || off_x(px[v], 96) || py[v] < -96 || py[v] > 560) ok = 0;
            depth = sz;
        }
        if (!ok) continue;
        uint32_t ot = main_ot(!f->up ? 980 : depth < near_z ? 40 : 1009);
        int lit = (int)(0x58 + 0x30 * fabs(nn[1]));
        unsigned u0 = s_pack.paper_u, v0 = s_pack.paper_v, um = u0 + s_pack.paper_w / 2, u1 = u0 + s_pack.paper_w - 1;
        unsigned v1 = v0 + s_pack.paper_h - 1;
        for (int h = 0; h < 2; h++) {
            /* crease end, outer end; at -W, then at +W */
            const int idx[4] = {0, h ? 4 : 2, 1, h ? 5 : 3};
            unsigned uc = um, uo = h ? u0 : u1;
            uint32_t q = alloc_words(10);
            if (!q) return;
            psx_mod_write_word(q + 4, 0x2C000000u | (uint32_t)lit << 16 | (uint32_t)lit << 8 | (uint32_t)lit);
            psx_mod_write_word(q + 8, xy(px[idx[0]], py[idx[0]]));
            psx_mod_write_word(q + 12, (uint32_t)s_pack.paper_clut << 16 | v0 << 8 | uc);
            psx_mod_write_word(q + 16, xy(px[idx[1]], py[idx[1]]));
            psx_mod_write_word(q + 20, (uint32_t)s_pack.paper_tpage << 16 | v0 << 8 | uo);
            psx_mod_write_word(q + 24, xy(px[idx[2]], py[idx[2]]));
            psx_mod_write_word(q + 28, v1 << 8 | uc);
            psx_mod_write_word(q + 32, xy(px[idx[3]], py[idx[3]]));
            psx_mod_write_word(q + 36, v1 << 8 | uo);
            add_prim(ot, q, 9);
        }
    }
}

/* ---- FTHR shape 5: the Great Wall's dust (then the picture as shape
 * 3's): wisps the wind drives along the ground round the fight, in gusts,
 * each growing and rising a little as it goes, showing and fading away
 * again (the dust picture times the colour); a new one rises upwind. World
 * units (mm). ---- */
#define WISP_MAX 16
typedef struct { double x, y, z, vy, size, swirl; int age, life; } Wisp;
static Wisp s_wisp[WISP_MAX];
static int s_nwisp, s_wisp_seen;
static unsigned s_wisp_t;

static void wisp_spawn(Wisp *w, int mx, int mz, int anywhere) {
    const int16_t *c = s_pack.feather;
    double a = anywhere ? (rnd() & 0xFFF) * (6.2831853 / 4096.0)
                        : s_wind + 3.14159265 + ((rnd() & 0xFF) / 255.0 - 0.5) * 1.8;
    int r = c[1] + (c[2] > c[1] ? rnd() % (c[2] - c[1]) : 0);
    w->x = mx + r * sin(a); w->z = mz + r * cos(a);
    w->y = -(double)(rnd() % (c[3] / 4 + 1));
    w->vy = -(0.3 + (rnd() & 0xFF) / 255.0 * 0.9);
    w->size = c[4] * (0.5 + (rnd() & 0xFF) / 255.0 * 0.4);
    w->swirl = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    w->life = 140 + rnd() % 120;
    w->age = anywhere ? (int)(rnd() % (unsigned)w->life) : 0;
}

static void wind_dust(const Gte *g) {
    const int16_t *c = s_pack.feather;
    int n = c[0] < 0 ? 0 : c[0] > WISP_MAX ? WISP_MAX : c[0];
    if (!n || !s_pack.paper_w) return;
    int mx = ((int)psx_mod_read_word(P1_X) + (int)psx_mod_read_word(P2_X)) / 2;
    int mz = ((int)psx_mod_read_word(P1_Z) + (int)psx_mod_read_word(P2_Z)) / 2;
    if (!s_wisp_seen) {
        s_wind = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
        for (s_nwisp = 0; s_nwisp < n; s_nwisp++) wisp_spawn(&s_wisp[s_nwisp], mx, mz, 1);
        s_wisp_seen = 1;
    }
    double gust = 0.55 + 0.3 * sin(s_wisp_t * 0.013) + 0.15 * sin(s_wisp_t * 0.031);
    double wx = sin(s_wind), wz = cos(s_wind);
    if (!paused()) {
        s_wisp_t++;
        s_wind += 0.0006 * sin(s_wisp_t * 0.002);                     /* the wind turning slowly */
        for (int i = 0; i < s_nwisp; i++) {
            Wisp *w = &s_wisp[i];
            double speed = 10 + 22 * gust, side = 4 * sin(s_wisp_t * 0.05 + w->swirl);
            w->x += wx * speed + wz * side; w->z += wz * speed - wx * side;
            w->y += w->vy;
            if (w->y < -c[3]) w->y = -c[3];
            w->size *= 1.004;
            double dx = w->x - mx, dz = w->z - mz;
            if (++w->age >= w->life || dx * dx + dz * dz > 2.25 * c[2] * c[2]) wisp_spawn(w, mx, mz, 0);
        }
    }
    int near_z = g->t[2];
    for (int i = 0; i < s_nwisp; i++) {
        const Wisp *w = &s_wisp[i];
        int sx, sy, sz, tx, tz, ex, ey, ez;
        world_tile((int)w->x, (int)w->z, &tx, &tz);
        rtps(g, tx / 10, (int)w->y / 10, tz / 10, &sx, &sy, &sz);
        if (sz < 64 || off_x(sx, 128) || sy < -128 || sy > 560) continue;
        double env = sin(3.14159265 * w->age / w->life) * (0.6 + 0.4 * gust);
        if (env < 0.05) continue;
        /* drawn long along the wind's way on the screen */
        world_tile((int)(w->x + 500 * wx), (int)(w->z + 500 * wz), &tx, &tz);
        rtps(g, tx / 10, (int)w->y / 10, tz / 10, &ex, &ey, &ez);
        double ax = ex - sx, ay = (ey - sy) * 12.0 / 20.0, al = hypot(ax, ay);
        if (al < 0.5) { ax = 1; ay = 0; al = 1; }
        ax /= al; ay /= al;
        double len = (w->size * (double)g->h) / (10.0 * sz);
        if (len < 2) continue;
        double hxo = ax * len * 1.1, hyo = ay * len * 1.1 * 20 / 12, exo = -ay * len * 0.45, eyo = ax * len * 0.45 * 20 / 12;
        uint32_t col = 0;
        for (int j = 0; j < 3; j++) {
            int v = (int)(s_pack.feather_rgb[j] * env);
            col |= (uint32_t)(v > 255 ? 255 : v) << (8 * j);
        }
        const int x[4] = { (int)(sx - hxo - exo), (int)(sx + hxo - exo), (int)(sx - hxo + exo), (int)(sx + hxo + exo) };
        const int y[4] = { (int)(sy - hyo - eyo), (int)(sy + hyo - eyo), (int)(sy - hyo + eyo), (int)(sy + hyo + eyo) };
        tex_quad(main_ot(sz < near_z ? 40 : 1009), 0x2E000000u | col, x, y, s_pack.paper_clut, s_pack.paper_tpage,
                 s_pack.paper_u, s_pack.paper_v, s_pack.paper_u + s_pack.paper_w - 1u, s_pack.paper_v + s_pack.paper_h - 1u);
    }
}

/* ---- FTHR shape 7: the Hills' dandelion seeds (count, distance from the
 * fighters' midpoint min and max, the highest they fly, size, sink (1/16
 * mm a frame); colour, halved (0x80 the picture's own); their picture as
 * shape 3's): drifting across the fight on the wind in gusts, bobbing up
 * and down as they go, sinking slowly, rocking a little under their
 * parachutes; one blown too far downwind (or down at the grass) comes in
 * again from upwind. World units (mm, x 16). The old fork's e454a7e. ---- */
static void dandelion_seed_in(Feather *f, int mx, int mz) {
    const int16_t *c = s_pack.feather;
    const double wx = 0.94, wz = 0.34;
    double across = ((rnd() & 0xFFF) / 4095.0 - 0.5) * 1.6 * c[2];
    feather_spawn(f, 1);
    f->x = (int)((mx - wx * c[2] * 0.95 - wz * across) * 16);
    f->z = (int)((mz - wz * c[2] * 0.95 + wx * across) * 16);
    f->y = -(400 + (c[3] > 500 ? rnd() % (c[3] - 400) : 0)) * 16;
}

static void dandelion_seeds(const Gte *g) {
    const int16_t *c = s_pack.feather;
    int n = c[0] < 0 ? 0 : c[0] > FEATHER_MAX ? FEATHER_MAX : c[0];
    if (!n || !s_pack.paper_w || c[3] <= 500) return;
    const double wx = 0.94, wz = 0.34;               /* the wind's way */
    int mx = ((int)psx_mod_read_word(P1_X) + (int)psx_mod_read_word(P2_X)) / 2;
    int mz = ((int)psx_mod_read_word(P1_Z) + (int)psx_mod_read_word(P2_Z)) / 2;
    if (!s_feather_seen) {                           /* the round's start: all about already */
        for (s_nfeather = 0; s_nfeather < n; s_nfeather++) feather_spawn(&s_feather[s_nfeather], 1);
        s_feather_seen = 1;
    }
    if (!paused())
        for (int i = 0; i < s_nfeather; i++) {
            Feather *f = &s_feather[i];
            f->sway += f->sway_rate; f->turn += f->turn_rate * 0.25; f->spin += f->spin_rate;
            double gust = 0.55 + 0.45 * sin(f->turn * 0.6 + i);
            f->x += (int)(16 * (4 + 9 * gust) * wx); f->z += (int)(16 * (4 + 9 * gust) * wz);
            f->y += (int)(f->vy * (0.3 + 1.2 * sin(f->sway)) - 16 * 3 * (gust > 0.9 ? gust - 0.9 : 0) * 10);
            if (f->y < -c[3] * 16) f->y = -c[3] * 16;
            double dx = f->x / 16.0 - mx, dz = f->z / 16.0 - mz;
            if (f->y > -60 * 16 || dx * wx + dz * wz > c[2] || dx * dx + dz * dz > 1.7 * c[2] * c[2]) dandelion_seed_in(f, mx, mz);
        }
    int near_z = g->t[2];
    uint32_t col = 0;
    for (int j = 0; j < 3; j++) col |= (uint32_t)(s_pack.feather_rgb[j] / 2) << (8 * j);
    for (int i = 0; i < s_nfeather; i++) {
        const Feather *f = &s_feather[i];
        int sx, sy, sz, tx, tz;
        world_tile(f->x / 16, f->z / 16, &tx, &tz);
        rtps(g, tx / 10, f->y / 160, tz / 10, &sx, &sy, &sz);
        if (sz < 32 || off_x(sx, 32) || sy < -32 || sy > 512) continue;
        double hx = (c[4] / 2.0 * g->h) / (10.0 * sz);
        if (hx < 0.8) continue;
        if (hx < 1.5) hx = 1.5;                       /* far off: a speck, still there */
        double turn = 0.3 * sin(f->spin * 3);
        int hy = (int)(hx * 20 / 12);
        int cx = (int)(hx * cos(turn)), cy = (int)(hx * sin(turn) * 20 / 12);
        int ddx = (int)(-hy * sin(turn) * 12 / 20), ddy = (int)(hy * cos(turn));
        const int x[4] = { sx - cx - ddx, sx + cx - ddx, sx - cx + ddx, sx + cx + ddx };
        const int y[4] = { sy - cy - ddy, sy + cy - ddy, sy - cy + ddy, sy + cy + ddy };
        tex_quad(main_ot(sz < near_z ? 40 : 1009), 0x2C000000u | col, x, y, s_pack.paper_clut, s_pack.paper_tpage,
                 s_pack.paper_u, s_pack.paper_v, s_pack.paper_u + s_pack.paper_w - 1u, s_pack.paper_v + s_pack.paper_h - 1u);
    }
}

/* ---- FTHR shape 6: Nowhere Desert's tumbleweeds and dust devil (count,
 * distance min and max, the devil's height, a tumbleweed's size, the
 * devil's puff size (mm); the tumbleweeds' colour; their picture as shape
 * 3's, then the puff's u, v, width, height, the devil's colour, devils).
 * The tumbleweeds roll before the wind round the fight in gusts, bouncing
 * now and then, turning as they roll; a fighter stepping by kicks one off,
 * gone too far one blows in from upwind. Now and then a dust devil rises
 * further off (puffs of dust whirling up a leaning funnel, added), drifts
 * with the wind, a tumbleweed it catches whirled round and lifted, and
 * dies down again. World units (mm). The old fork's e454a7e. ---- */
#define WEED_MAX 6
typedef struct { double x, y, z, vx, vy, vz, roll, size, phase; } Weed;
static Weed s_weed[WEED_MAX];
static int s_nweed, s_weed_seen;
static unsigned s_weed_t;
static struct { double x, z, phase; int age, life, wait, on; } s_devil;

static double devil_env(void) {
    if (!s_devil.on) return 0;
    double in = s_devil.age / 150.0, out = (s_devil.life - s_devil.age) / 200.0;
    double e = in < out ? in : out;
    return e < 0 ? 0 : e > 1 ? 1 : e;
}

static void weed_blow_in(Weed *w, int mx, int mz, int anywhere) {
    const int16_t *c = s_pack.feather;
    double a = anywhere ? (rnd() & 0xFFF) * (6.2831853 / 4096.0)
                        : s_wind + 3.14159265 + ((rnd() & 0xFF) / 255.0 - 0.5) * 1.4;
    int r = anywhere ? c[1] + (c[2] > c[1] ? rnd() % (c[2] - c[1]) : 0) : c[2];
    w->x = mx + r * sin(a); w->z = mz + r * cos(a);
    w->y = 0; w->vx = w->vz = 0; w->vy = 0;
    w->size = c[4] * (0.75 + (rnd() & 0xFF) / 255.0 * 0.5);
    w->roll = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    w->phase = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
}

static void devil_rise(int mx, int mz) {
    const int16_t *c = s_pack.feather;
    double a = (rnd() & 0xFFF) * (6.2831853 / 4096.0), r = c[2] * (0.8 + (rnd() & 0xFF) / 255.0 * 0.7);
    s_devil.x = mx + r * sin(a); s_devil.z = mz + r * cos(a);
    s_devil.phase = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
    s_devil.age = 0; s_devil.life = 900 + rnd() % 700; s_devil.on = 1;
}

/* a puff of the picture at a screen point, turned, sized (pixels), added */
static void devil_puff(int sx, int sy, int hx, double turn, double level, int ot) {
    int hy = hx * 20 / 12;
    uint32_t col = 0;
    for (int j = 0; j < 3; j++) {
        int v = (int)(s_pack.devil_rgb[j] * level);
        col |= (uint32_t)(v > 255 ? 255 : v < 0 ? 0 : v) << (8 * j);
    }
    int cx = (int)(hx * cos(turn)), cy = (int)(hx * sin(turn) * 20 / 12);
    int dx = (int)(-hy * sin(turn) * 12 / 20), dy = (int)(hy * cos(turn));
    const int x[4] = { sx - cx - dx, sx + cx - dx, sx - cx + dx, sx + cx + dx };
    const int y[4] = { sy - cy - dy, sy + cy - dy, sy - cy + dy, sy + cy + dy };
    tex_quad(main_ot((unsigned)ot), 0x2E000000u | col, x, y, s_pack.paper_clut, s_pack.paper_tpage | 0x20u,
             s_pack.devil_u, s_pack.devil_v, s_pack.devil_u + s_pack.devil_w - 1u, s_pack.devil_v + s_pack.devil_h - 1u);
}

static void desert_wind(const Gte *g) {
    const int16_t *c = s_pack.feather;
    int n = c[0] < 0 ? 0 : c[0] > WEED_MAX ? WEED_MAX : c[0];
    if (!s_pack.paper_w) return;
    int fx[2] = {(int)psx_mod_read_word(P1_X), (int)psx_mod_read_word(P2_X)};
    int fz[2] = {(int)psx_mod_read_word(P1_Z), (int)psx_mod_read_word(P2_Z)};
    int mx = fx[0] + (fx[1] - fx[0]) / 2, mz = fz[0] + (fz[1] - fz[0]) / 2;
    if (!s_weed_seen) {
        s_wind = (rnd() & 0xFFF) * (6.2831853 / 4096.0);
        for (s_nweed = 0; s_nweed < n; s_nweed++) weed_blow_in(&s_weed[s_nweed], mx, mz, 1);
        memset(&s_devil, 0, sizeof s_devil);
        s_devil.wait = 200 + rnd() % 600;
        s_weed_seen = 1;
    }
    double wx = sin(s_wind), wz = cos(s_wind);
    if (!paused()) {
        s_weed_t++;
        s_wind += 0.0006 * sin(s_weed_t * 0.002);                     /* the wind turning slowly */
        if (s_pack.devils) {
            if (s_devil.on) {
                /* drifting with the wind, meandering */
                s_devil.x += wx * 7 + wz * 3 * sin(s_weed_t * 0.004 + s_devil.phase);
                s_devil.z += wz * 7 - wx * 3 * sin(s_weed_t * 0.004 + s_devil.phase);
                if (++s_devil.age >= s_devil.life) { s_devil.on = 0; s_devil.wait = 600 + rnd() % 1200; }
            } else if (--s_devil.wait <= 0) devil_rise(mx, mz);
        }
        double env = devil_env();
        for (int i = 0; i < s_nweed; i++) {
            Weed *w = &s_weed[i];
            double gust = 0.55 + 0.3 * sin(s_weed_t * 0.009 + w->phase) + 0.15 * sin(s_weed_t * 0.027 + 2 * w->phase);
            double speed = 6 + 30 * gust;
            w->vx += (wx * speed - w->vx) * 0.02;
            w->vz += (wz * speed - w->vz) * 0.02;
            for (int q = 0; q < 2; q++) {
                int y = (int)psx_mod_read_word(q ? P2_Y : P1_Y);
                double dx = w->x - fx[q], dz = w->z - fz[q], d = sqrt(dx * dx + dz * dz);
                if (d < 600 && y > -1200 && w->y > -300) {
                    if (d < 1) { dx = 1; d = 1; }
                    w->vx += 14 * dx / d; w->vz += 14 * dz / d; w->vy = -(16 + rnd() % 10);
                }
            }
            if (env > 0.2) {
                /* caught by the devil: whirled round it, drawn in and lifted */
                double dx = w->x - s_devil.x, dz = w->z - s_devil.z, d = sqrt(dx * dx + dz * dz);
                if (d < 1400 && d > 1) {
                    double k = env * (1 - d / 1400);
                    w->vx += k * (3.2 * -dz / d - 1.2 * dx / d);
                    w->vz += k * (3.2 * dx / d - 1.2 * dz / d);
                    if (w->y > -c[3] * 0.4) w->vy -= 2.0 * k;
                }
            }
            w->vy += 1.0;                                             /* falling */
            w->x += w->vx; w->y += w->vy; w->z += w->vz;
            if (w->y >= 0) {
                w->y = 0;
                w->vy = w->vy > 5 ? -w->vy * 0.45 : 0;                 /* a bounce */
                if (speed > 22 && !(rnd() % 50)) w->vy = -(6 + rnd() % 12);
            }
            w->roll += hypot(w->vx, w->vz) / (w->size * 0.5);
            double dx = w->x - mx, dz = w->z - mz;
            if (dx * dx + dz * dz > 2.25 * c[2] * c[2]) weed_blow_in(w, mx, mz, 0);
        }
    }
    /* the camera looking steeply down (the continue screen's): none drawn,
     * they would stand up off the floor under it */
    if (g->r[2][1] > 0x800 || g->r[2][1] < -0x800) return;
    int near_z = g->t[2];
    uint32_t wcol = (uint32_t)s_pack.feather_rgb[0] | (uint32_t)s_pack.feather_rgb[1] << 8 |
                    (uint32_t)s_pack.feather_rgb[2] << 16;
    for (int i = 0; i < s_nweed; i++) {
        const Weed *w = &s_weed[i];
        int sx, sy, sz, tx, tz, ex, ey, ez;
        world_tile((int)w->x, (int)w->z, &tx, &tz);
        rtps(g, tx / 10, (int)(w->y - w->size * 0.45) / 10, tz / 10, &sx, &sy, &sz);
        if (sz < 300 || off_x(sx, 96) || sy < -96 || sy > 560) continue;   /* none right by the camera */
        int hx = (int)((w->size * 0.5 * (double)g->h) / (10.0 * sz)), hy = hx * 20 / 12;
        if (hx < 2) continue;
        /* turning the way it rolls across the screen */
        world_tile((int)(w->x + 40 * w->vx), (int)(w->z + 40 * w->vz), &tx, &tz);
        rtps(g, tx / 10, (int)(w->y - w->size * 0.45) / 10, tz / 10, &ex, &ey, &ez);
        double turn = (ex >= sx ? 1 : -1) * w->roll + 0.3;
        int cx = (int)(hx * cos(turn)), cy = (int)(hx * sin(turn) * 20 / 12);
        int dx = (int)(-hy * sin(turn) * 12 / 20), dy = (int)(hy * cos(turn));
        const int x[4] = { sx - cx - dx, sx + cx - dx, sx - cx + dx, sx + cx + dx };
        const int y[4] = { sy - cy - dy, sy + cy - dy, sy - cy + dy, sy + cy + dy };
        tex_quad(main_ot(sz < near_z ? 40 : 1009), 0x2C000000u | wcol, x, y, s_pack.paper_clut, s_pack.paper_tpage,
                 s_pack.paper_u, s_pack.paper_v, s_pack.paper_u + s_pack.paper_w - 1u, s_pack.paper_v + s_pack.paper_h - 1u);
    }
    double env = devil_env();
    if (env < 0.03 || !s_pack.devil_w) return;
    /* the devil: puffs whirling up a funnel, narrow at the foot, wide and
     * faint at the top, its axis swaying; a skirt of dust whirling low */
    const int levels = 14;
    double H = c[3], t = s_weed_t;
    for (int k = -4; k < levels; k++) {
        double sl = k < 0 ? 0.02 : (double)k / (levels - 1);
        double sway = H * 0.1 * sl * sin(t * 0.011 + s_devil.phase + 2.2 * sl);
        double ax = s_devil.x + wz * sway + wx * H * 0.06 * sl, az = s_devil.z - wx * sway + wz * H * 0.06 * sl;
        double r = k < 0 ? H * 0.11 : 60 + H * 0.2 * pow(sl, 1.5);
        double th = t * (k < 0 ? 0.2 : 0.13 * (1.3 - 0.6 * sl)) + (k + 4) * 2.4;
        double x = ax + r * cos(th), z = az + r * sin(th), y = -H * sl - (k < 0 ? 120 : 0);
        int sx, sy, sz, tx, tz;
        world_tile((int)x, (int)z, &tx, &tz);
        rtps(g, tx / 10, (int)y / 10, tz / 10, &sx, &sy, &sz);
        if (sz < 64 || off_x(sx, 160) || sy < -160 || sy > 600) continue;
        double size = c[5] * (k < 0 ? 1.3 : 0.45 + 1.3 * sl);
        int hx = (int)((size * (double)g->h) / (10.0 * sz));
        if (hx < 2) continue;
        double level = env * (k < 0 ? 0.8 : 1.0 - 0.6 * sl) * (0.75 + 0.25 * sin(th * 1.7));
        devil_puff(sx, sy, hx, th * 0.5 + k * 0.7 + 0.3, level, sz < near_z ? 40 : 1009);
    }
}

/* ==== the objects (T2OB, T2RT, T2OA: the old fork's OBJS, ROTT, OBJA) ==== */

/* the arcade's memory as s16 halves, by address, so the port reads as the
 * reference (stage 11's helicopter); 12 objects' angles and places */
static int16_t s_st[0x40], s_ang[48], s_pos[48];
#define ST(a)  s_st[((a) - 0x8021F9F8u) >> 1]      /* 0x8021F9F8..0x8021FA78 */
#define ANG(a) s_ang[((a) - 0x802FECE4u) >> 1]     /* 4 SVECTORs: angles */
#define POS(a) s_pos[((a) - 0x8031E134u) >> 1]     /* 4 SVECTORs: positions */
#define U16(v) ((int)(uint16_t)(v))

/* RotMatrix (0x801F0D64) and RotMatrixYXZ (0x801F0034), table 0x8021688C
 * (T2RT): a negative angle reads entry -a and negates the sine */
static void rot_sc(int a, int *s, int *c) {
    a = (int16_t)a;
    uint32_t w = s_obj.rot[(a < 0 ? -a : a) & 0xFFF];
    int sv = (int16_t)(w & 0xFFFF), cv = (int16_t)(w >> 16);
    *s = a < 0 ? -sv : sv;
    *c = cv;
}

static void rot_matrix(const int16_t *ang, int yxz, int32_t m[3][3]) {
    int sx, cx, sy, cy, sz, cz, t;
    rot_sc(ang[0], &sx, &cx); rot_sc(ang[1], &sy, &cy); rot_sc(ang[2], &sz, &cz);
    if (!yxz) {
        m[0][2] = (int16_t)sy;
        m[1][2] = (int16_t)(-(cy * sx) >> 12);
        m[2][2] = (int16_t)((cy * cx) >> 12);
        m[0][0] = (int16_t)((cz * cy) >> 12);
        m[0][1] = (int16_t)(-(sz * cy) >> 12);
        t = (cz * -sy) >> 12;
        m[1][0] = (int16_t)((int16_t)((sz * cx) >> 12) - (int16_t)((t * sx) >> 12));
        m[2][0] = (int16_t)((int16_t)((sz * sx) >> 12) + (int16_t)((t * cx) >> 12));
        t = (sz * -sy) >> 12;
        m[1][1] = (int16_t)((int16_t)((cz * cx) >> 12) + (int16_t)((t * sx) >> 12));
        m[2][1] = (int16_t)((int16_t)((cz * sx) >> 12) - (int16_t)((t * cx) >> 12));
    } else {
        m[1][2] = (int16_t)-sx;
        m[0][2] = (int16_t)((sy * cx) >> 12);
        m[2][2] = (int16_t)((cy * cx) >> 12);
        m[1][0] = (int16_t)((sz * cx) >> 12);
        m[1][1] = (int16_t)((cz * cx) >> 12);
        t = (sy * sx) >> 12;
        m[0][0] = (int16_t)((int16_t)((cy * cz) >> 12) + (int16_t)((t * sz) >> 12));
        m[0][1] = (int16_t)((int16_t)((t * cz) >> 12) - (int16_t)((cy * sz) >> 12));
        t = (cy * sx) >> 12;
        m[2][1] = (int16_t)((int16_t)((sy * sz) >> 12) + (int16_t)((t * cz) >> 12));
        m[2][0] = (int16_t)((int16_t)((t * sz) >> 12) - (int16_t)((sy * cz) >> 12));
    }
}

/* ---- stage 11's helicopter (OBJA 11: Hong Kong's, King's from
 * tekken3.zip; 0x801E3F68 / 0x801E40C0 and its three flights). It flies the
 * whole fight, as the old fork's (the user's choice there, 2026-10-17). ---- */

/* 0x801E3DE0; mode: the vsync count & 3 in the arcade */
static void stage11_init(int mode) {
    ANG(0x802FECE4u) = -0x9F; ANG(0x802FECE6u) = 0x71C;
    POS(0x8031E136u) = -0x190; POS(0x8031E13Eu) = -0x12C; POS(0x8031E146u) = 0x118; POS(0x8031E148u) = -0x4CE;
    ST(0x8021FA04u) = 0x80; ST(0x8021FA14u) = -2;
    ANG(0x802FECE8u) = 0; ANG(0x802FECECu) = 0; ANG(0x802FECEEu) = 0; ANG(0x802FECF0u) = 0;
    ANG(0x802FECF4u) = 0; ANG(0x802FECF6u) = 0; ANG(0x802FECF8u) = 0;
    POS(0x8031E134u) = 0; POS(0x8031E138u) = 0x1068; POS(0x8031E13Cu) = 0; POS(0x8031E140u) = 0; POS(0x8031E144u) = 0;
    ST(0x8021FA1Cu) = 0; ST(0x8021FA4Cu) = 0; ST(0x8021FA00u) = -1; ST(0x8021FA6Cu) = 0x20; ST(0x8021FA08u) = -1;
    ST(0x8021FA0Cu) = 0x20; ST(0x8021FA10u) = -1; ST(0x8021FA18u) = 0; ST(0x8021FA20u) = 0; ST(0x8021FA24u) = 0;
    ST(0x8021FA28u) = 0; ST(0x8021FA54u) = 0; ST(0x8021FA34u) = 0; ST(0x8021FA48u) = 0; ST(0x8021FA5Cu) = -0x46;
    ST(0x8021FA60u) = 0x46; ST(0x8021FA50u) = 0; ST(0x8021FA64u) = 1; ST(0x8021FA30u) = 1; ST(0x8021FA70u) = 0x1068;
    ST(0x8021F9FEu) = 1;
    ST(0x8021FA44u) = (int16_t)(mode & 3);
}

/* x, z on the circle: sine[-angle], sine[-angle + 0x400] x radius */
static void circle(int angle, int radius) {
    const int16_t *sine = s_obj.sin;
    int a = -angle;
    int v = sine[a & 0xFFF] * radius;
    POS(0x8031E134u) = (int16_t)((v < 0 ? v + 0xFFF : v) >> 12);
    v = sine[(a + 0x400) & 0xFFF] * radius;
    POS(0x8031E138u) = (int16_t)((v < 0 ? v + 0xFFF : v) >> 12);
}

static void f3f68(int yaw) {
    ANG(0x802FECE6u) = (int16_t)(U16(ANG(0x802FECE6u)) & 0xFFF);
    ST(0x8021FA20u) = (int16_t)(U16(ST(0x8021FA20u)) & 0xFFF);
    int mode = ST(0x8021FA44u);
    if (mode >= 1 && mode <= 3) {
        ST(0x8021FA50u) += 1;
        ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) | 0x1000);
        return;
    }
    if (mode != 0) return;
    yaw = (int16_t)yaw;
    ST(0x8021FA58u) = (int16_t)yaw;
    if (ST(0x8021FA1Cu) & 2) return;
    int last = ST(0x8021FA54u), d, bits;
    if (last < yaw) { d = yaw - last; bits = 7; } else { d = last - yaw; bits = 0x23; }
    if (d > 0x17C) {
        uint32_t table = d > 0x352 ? 0x802045F4u : 0x802045ECu;   /* read by 0x801E42DC only: never in mode 0 */
        ST(0x8021F9F8u) = (int16_t)(table & 0xFFFF); ST(0x8021F9FAu) = (int16_t)(table >> 16);
        ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) | bits);
        ST(0x8021FA54u) = (int16_t)yaw;
        ST(0x8021FA20u) = ANG(0x802FECE6u);
        ST(0x8021FA18u) = 0;
        return;
    }
    ST(0x8021FA50u) += 1;
}

/* mode 1 (0x801E4A0C) */
static void f4a0c(void) {
    int t = ST(0x8021FA50u), st = ST(0x8021FA4Cu);
    switch (st) {
    case 0:
        if (t < 200) { ANG(0x802FECE6u) += 4; return; }
        break;
    case 1:
        if (t < 0x118) { ANG(0x802FECE6u) += 4; return; }
        break;
    case 2: {
        ST(0x8021FA38u) = (int16_t)((ST(0x8021FA34u) >> 3) | 1);
        if (t == 500) ST(0x8021F9FCu) = 0;
        if (ST(0x8021F9FCu) == 0) {
            if (t < 0x212) ANG(0x802FECE8u) += (int16_t)(ST(0x8021FA30u) * 6);
            if (t < 0x230) ANG(0x802FECE4u) += 4;
            ST(0x8021FA34u) += ST(0x8021F9FEu) == 0 ? 1 : -1;
            if (ST(0x8021FA34u) == 0) { ST(0x8021FA4Cu) = 5; ST(0x8021FA50u) = 0; }
        } else {
            if (t < 0x1F) ANG(0x802FECE8u) -= (int16_t)(ST(0x8021FA30u) * 6);
            if (t < 0x3D) ANG(0x802FECE4u) -= 1;
            if (abs(ST(0x8021FA38u)) < 8) ST(0x8021FA34u) += ST(0x8021F9FEu) != 0 ? 1 : -1;
            else ST(0x8021FA38u) = ST(0x8021F9FEu) != 0 ? 8 : -8;
        }
        int step = ST(0x8021FA38u), angle = (int16_t)(ST(0x8021FA24u) + step);
        ST(0x8021FA24u) = (int16_t)angle;
        circle(angle, 0x1068);
        ANG(0x802FECE6u) -= (int16_t)step;
        return;
    }
    case 4:
        if (t < 0x118) { ANG(0x802FECE6u) -= 4; return; }
        break;
    case 5:
        if (t < 0x1E) return;
        if (t < 0xD2) ANG(0x802FECE4u) -= 1;
        if (t > 0x137) {
            if (ST(0x8021FA34u) == 0) ST(0x8021F9FEu) = (int16_t)(1 - ST(0x8021F9FEu));
            ST(0x8021F9FCu) = 1;
            ST(0x8021FA30u) = (int16_t)-ST(0x8021FA30u);
            ST(0x8021FA4Cu) = 2;
            ST(0x8021FA50u) = 0;
            return;
        }
        ANG(0x802FECE6u) += ST(0x8021F9FEu) != 0 ? -4 : 4;
        return;
    default:
        return;
    }
    ST(0x8021FA4Cu) = 2; ST(0x8021FA50u) = 0; ST(0x8021F9FCu) = 1;
}

/* mode 2 (0x801E4E04): radius in 0x8021FA70 */
static void radius_circle(int a) {
    const int16_t *sine = s_obj.sin;
    int r = ST(0x8021FA70u);
    int v = r * sine[a & 0xFFF];
    POS(0x8031E134u) = (int16_t)((v < 0 ? v + 0xFFF : v) >> 12);
    v = r * sine[(a + 0x400) & 0xFFF];
    POS(0x8031E138u) = (int16_t)((v < 0 ? v + 0xFFF : v) >> 12);
}

static void f4e04(void) {
    int st = ST(0x8021FA4Cu), t = ST(0x8021FA50u), next;
    switch (st) {
    case 0:
        if (t < 0x1E) ANG(0x802FECE8u) -= 8;
        if ((uint32_t)(U16(ST(0x8021FA50u)) - 0x82) < 0x1Eu) ANG(0x802FECE8u) += 8;
        if (t < 0xA0) ANG(0x802FECE6u) -= 0xB;
        if (t == 0x8C) ST(0x8021F9FCu) = 1;
        if (t < 0x8D) return;
        ST(0x8021FA38u) = ST(0x8021FA34u) > 1 ? 0x19 : 5;
        if (t == 0x15E) ST(0x8021F9FCu) = 0;
        if (ST(0x8021F9FCu) == 0) {
            ST(0x8021FA34u) -= 1;
            if (ST(0x8021FA34u) == 0) { ST(0x8021FA4Cu) = 2; ST(0x8021FA50u) = 0; }
        } else {
            if (ST(0x8021FA38u) > 200) ST(0x8021FA38u) = 200;
            ST(0x8021FA34u) += 1;
        }
        ST(0x8021FA70u) += ST(0x8021FA38u);
        radius_circle(-U16(ST(0x8021FA24u)));
        break;
    case 2:
        if (t < 0x50) { ANG(0x802FECE6u) -= 0xB; ANG(0x802FECE8u) -= 3; }
        else {
            ST(0x8021F9FCu) = 1; ST(0x8021FA4Cu) = 3; ST(0x8021FA50u) = 0; ST(0x8021FA24u) = 0; ST(0x8021FA34u) = 0;
            ANG(0x802FECE6u) -= 0xB; ANG(0x802FECE8u) -= 5;
        }
        break;
    case 3: {
        if (t < 0x14) { ANG(0x802FECE6u) -= 0x16; ANG(0x802FECE8u) -= 5; }
        else ANG(0x802FECE6u) -= ST(0x8021FA38u);
        ST(0x8021FA38u) = (int16_t)((ST(0x8021FA34u) >> 3) | 1);
        if (ST(0x8021FA50u) == 0x578) ST(0x8021F9FCu) = 0;
        if (ST(0x8021F9FCu) == 0) {
            next = ST(0x8021FA34u) - 1;
            if (ST(0x8021FA34u) == 1) { ST(0x8021FA4Cu) = 4; ST(0x8021FA50u) = 0; }
        } else {
            next = ST(0x8021FA34u) + 1;
            if (ST(0x8021FA38u) > 8) { ST(0x8021FA38u) = 8; next = ST(0x8021FA34u); }
        }
        ST(0x8021FA34u) = (int16_t)next;
        int old = U16(ST(0x8021FA24u));
        ST(0x8021FA70u) -= 10;
        ST(0x8021FA24u) = (int16_t)(old + ST(0x8021FA38u));
        int a = -(old + U16(ST(0x8021FA38u)));
        if (ST(0x8021FA70u) < 0x1068) ST(0x8021FA70u) = 0x1068;
        radius_circle(a);
        break;
    }
    case 4:
        if (t < 100) ANG(0x802FECE8u) += 3;
        if (t > 0xA0) { ST(0x8021FA50u) = 0; ST(0x8021FA4Cu) = 5; }
        break;
    case 5:
        if (t < 0x15E) ANG(0x802FECE6u) -= 5;
        else if (t < 0x1D6) ANG(0x802FECE8u) += 2;
        if (t < 0x3C) ANG(0x802FECE4u) += 1;
        if (t < 0x78) ANG(0x802FECE8u) -= 2;
        if (t > 0x208) { ST(0x8021FA4Cu) = 6; ST(0x8021FA50u) = 0; ST(0x8021F9FCu) = 1; }
        break;
    case 6: {
        if (t < 0x1E) ANG(0x802FECE4u) -= 1;
        if (t < 0x50) ANG(0x802FECE8u) += 4;
        ST(0x8021FA38u) = (int16_t)((ST(0x8021FA34u) >> 3) | 1);
        if (ST(0x8021F9FCu) == 0) {
            next = ST(0x8021FA34u) - 1;
            if (ST(0x8021FA34u) == 1) { ST(0x8021FA4Cu) = 7; ST(0x8021FA50u) = 0; }
        } else {
            next = ST(0x8021FA34u) + 1;
            if (ST(0x8021FA38u) > 8) { ST(0x8021FA38u) = 8; next = ST(0x8021FA34u); }
        }
        ST(0x8021FA34u) = (int16_t)next;
        int angle = (int16_t)(ST(0x8021FA24u) - ST(0x8021FA38u));
        ST(0x8021FA24u) = (int16_t)angle;
        circle(angle, 0x1068);
        ANG(0x802FECE6u) += ST(0x8021FA38u);
        break;
    }
    default:
        break;
    }
}

/* mode 3 (0x801E54B4) */
static void f54b4(void) {
    int st = ST(0x8021FA4Cu), t = ST(0x8021FA50u), next;
    switch (st) {
    case 0:
        if (t < 0x1E) ANG(0x802FECE4u) -= 1;
        if (t < 0x10E) POS(0x8031E136u) += 10;
        else {
            ST(0x8021FA4Cu) = 1; ST(0x8021FA50u) = 0;
            ST(0x8021FA68u) = ST(0x8021FA64u) ? ST(0x8021FA5Cu) : ST(0x8021FA60u);
        }
        break;
    case 1:
        if (t < 0x79) {
            if (ST(0x8021FA64u)) POS(0x8031E138u) += ST(0x8021FA68u);
            else POS(0x8031E134u) += ST(0x8021FA68u);
        } else {
            ST(0x8021FA50u) = 0;
            if (ST(0x8021FA64u)) ST(0x8021FA5Cu) = (int16_t)-ST(0x8021FA5Cu);
            else ST(0x8021FA60u) = (int16_t)-ST(0x8021FA60u);
            ST(0x8021FA4Cu) = 2;
            ST(0x8021FA64u) = (int16_t)(1 - ST(0x8021FA64u));
        }
        break;
    case 2:
        if (t < 0x41) ANG(0x802FECE6u) += 0x20;
        else { ST(0x8021FA4Cu) = 3; ST(0x8021FA50u) = 0; ST(0x8021FA24u) += 0x800; }
        break;
    case 3:
        if ((uint32_t)(U16(ST(0x8021FA50u)) - 0xF1) < 0x1Du) ANG(0x802FECE4u) += 1;
        if (t < 0x10E) POS(0x8031E136u) -= 10;
        else { ST(0x8021FA4Cu) = 4; ST(0x8021FA50u) = 0; ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) & 0xFFFD); }
        break;
    case 4:
        if (t < 0x79) return;
        ST(0x8021FA4Cu) = 5; ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) | 2); ST(0x8021FA50u) = 0;
        break;
    case 5:
        if (t < 0x32) { ANG(0x802FECE4u) += 3; ANG(0x802FECE6u) -= 1; POS(0x8031E136u) -= 1; return; }
        if (t < 0x118) { ANG(0x802FECE6u) -= 5; POS(0x8031E136u) -= 6; return; }
        ST(0x8021FA4Cu) = 6; ST(0x8021F9FCu) = 1;
        ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) & 0xFFFD); ST(0x8021FA50u) = 0;
        break;
    case 6: {
        int d = (ST(0x8021FA34u) >> 4) * 2 + 1;
        ST(0x8021FA38u) = (int16_t)d;
        if (t == 0x66) ST(0x8021F9FCu) = 0;
        if (ST(0x8021F9FCu) == 0) {
            next = ST(0x8021FA34u) - 1;
            if (ST(0x8021FA34u) == 1) {
                ST(0x8021FA4Cu) = 7; ST(0x8021FA50u) = 0;
                ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) & 0xFFFD); ST(0x8021FA38u) = 5;
            }
        } else {
            next = ST(0x8021FA34u) + 1;
            if (d > 10) { ST(0x8021FA38u) = 10; next = ST(0x8021FA34u); }
        }
        ST(0x8021FA34u) = (int16_t)next;
        int angle = (int16_t)(ST(0x8021FA24u) - ST(0x8021FA38u));
        ST(0x8021FA24u) = (int16_t)angle;
        circle(angle, 0x1068);
        ANG(0x802FECE6u) += ST(0x8021FA38u);
        break;
    }
    case 7:
        if (t < 300) { ANG(0x802FECE6u) += 4; if (t < 0x11E) POS(0x8031E136u) += 5; }
        if ((uint32_t)(U16(ST(0x8021FA50u)) - 0xC9) < 0x95u) ANG(0x802FECE4u) -= 1;
        if (t < 0x15F) return;
        ST(0x8021FA4Cu) = 8; ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) & 0xFFFD); ST(0x8021FA50u) = 0;
        break;
    case 8:
        if (t < 0x79) return;
        ST(0x8021FA4Cu) = 0; ST(0x8021FA1Cu) = (int16_t)(U16(ST(0x8021FA1Cu)) | 2); ST(0x8021FA50u) = 0;
        break;
    default:
        break;
    }
}

static void f40c0(void) {
    ANG(0x802FECEEu) -= 0x29F;
    ANG(0x802FECF4u) += 0x29F;
    int flags = U16(ST(0x8021FA1Cu)), mode = ST(0x8021FA44u);
    if (!(flags & 1) || mode != 2) {
        int v = U16(ST(0x8021FA04u));
        if (v & 2) ANG(0x802FECE6u) += ST(0x8021FA08u);
        ST(0x8021FA04u) = (int16_t)(v + 1);
        if (ST(0x8021FA04u) == 0x100) { ST(0x8021FA04u) = 0; ST(0x8021FA08u) = (int16_t)-ST(0x8021FA08u); }
    }
    /* else 0x801E42DC / 0x801E4674: bit 0 is only set in mode 0, never with mode 2 */
    flags = U16(ST(0x8021FA1Cu));
    if (!(flags & 1) || !(flags & 0x1000)) {
        POS(0x8031E136u) += ST(0x8021FA00u) > 0 ? 1 : -1;
        ST(0x8021FA6Cu) += 1;
        if (ST(0x8021FA6Cu) == 0x40) { ST(0x8021FA6Cu) = 0; ST(0x8021FA00u) = (int16_t)-ST(0x8021FA00u); }
        ANG(0x802FECE8u) += ST(0x8021FA10u);
        ST(0x8021FA0Cu) += 1;
        if (ST(0x8021FA0Cu) == 0x40) { ST(0x8021FA10u) = (int16_t)-ST(0x8021FA10u); ST(0x8021FA0Cu) = 0; }
        if (!(U16(ST(0x8021FA1Cu)) & 0x1000)) return;
    }
    switch (ST(0x8021FA44u)) {
    case 1: f4a0c(); break;
    case 2: f4e04(); break;
    case 3: f54b4(); break;
    default: break;
    }
}

/* 0x801E3D0C for stage 11 */
static void stage11_step(int yaw) {
    if (ST(0x8021FA48u) > 0x78) f3f68(yaw);
    else ST(0x8021FA48u) += 1;
    f40c0();
}

/* ---- OBJA 200, EAGL: Grand Canyon's eagle. It circles the canyon, glides
 * down onto a butte's top, sits, takes off again. Models: 0 gliding, 1
 * wings up, 2 wings down, 3 perched; one is drawn (s_eagle_model). Tiles'
 * units (cm), y down; facing as RotMatrix's y angle (4096 a turn; +z its
 * beak at 0). ---- */
enum { EAGLE_CIRCLE, EAGLE_DESCEND, EAGLE_PERCH, EAGLE_TAKEOFF };
static struct { int mode, t, dur, perch; double x, y, z, ang, sx, sy, sz, tx, ty, tz, tang; } s_eagle;
static int s_eagle_model;
static uint32_t s_eagle_rand = 0x1234567u;
static int eagle_rand(void) { s_eagle_rand = s_eagle_rand * 1103515245u + 12345u; return (int)(s_eagle_rand >> 16 & 0x7FFF); }

static void eagle_place(double yaw, double roll) {
    for (int k = 0; k < 4; k++) {
        s_pos[4 * k] = (int16_t)s_eagle.x; s_pos[4 * k + 1] = (int16_t)s_eagle.y; s_pos[4 * k + 2] = (int16_t)s_eagle.z;
        s_ang[4 * k] = 0; s_ang[4 * k + 1] = (int16_t)((int)(yaw * 4096.0 / 6.2831853) & 0xFFF);
        s_ang[4 * k + 2] = (int16_t)(int)(roll * 4096.0 / 6.2831853);
    }
}

static void eagle_circle_at(double ang) {
    double r = s_obj.eagle_radius, h = s_obj.eagle_height;
    s_eagle.x = r * sin(ang); s_eagle.z = r * cos(ang);
    s_eagle.y = h + 80.0 * sin(s_eagle.t * 0.02);
}

static void eagle_init(void) {
    memset(&s_eagle, 0, sizeof s_eagle);
    s_eagle.mode = EAGLE_CIRCLE;
    s_eagle.ang = (eagle_rand() & 0xFFF) * (6.2831853 / 4096.0);
    s_eagle.dur = 420 + eagle_rand() % 360;           /* the first landing soon */
    eagle_circle_at(s_eagle.ang);
    s_eagle_model = 0;
    eagle_place(s_eagle.ang + 1.5707963, -0.25);
}

/* flapping: the two wing models in turn every `rate` frames */
static int eagle_flap(int rate) { return (s_eagle.t / rate) % 2 ? 1 : 2; }

static void eagle_step(void) {
    if (!s_obj.eagle_n || !s_obj.eagle_radius) return;
    const int16_t *pr;
    double speed = 17.0, dx, dz, s;
    s_eagle.t++;
    switch (s_eagle.mode) {
    case EAGLE_CIRCLE:
        s_eagle.ang += speed / s_obj.eagle_radius;
        eagle_circle_at(s_eagle.ang);
        s_eagle_model = s_eagle.t % 240 < 90 ? eagle_flap(8) : 0;
        eagle_place(s_eagle.ang + 1.5707963, -0.25);
        if (s_eagle.t >= s_eagle.dur) {
            s_eagle.perch = eagle_rand() % s_obj.eagle_n;
            pr = s_obj.eagle_perch + 4 * s_eagle.perch;
            s_eagle.sx = s_eagle.x; s_eagle.sy = s_eagle.y; s_eagle.sz = s_eagle.z;
            s_eagle.tx = pr[0]; s_eagle.ty = pr[1]; s_eagle.tz = pr[2];
            double d = sqrt((s_eagle.tx - s_eagle.sx) * (s_eagle.tx - s_eagle.sx) + (s_eagle.ty - s_eagle.sy) * (s_eagle.ty - s_eagle.sy)
                            + (s_eagle.tz - s_eagle.sz) * (s_eagle.tz - s_eagle.sz));
            s_eagle.mode = EAGLE_DESCEND; s_eagle.t = 0; s_eagle.dur = (int)(d / 15.0) + 30;
        }
        break;
    case EAGLE_DESCEND:
        s = (double)s_eagle.t / s_eagle.dur;
        if (s > 1) s = 1;
        dx = s_eagle.tx - s_eagle.sx; dz = s_eagle.tz - s_eagle.sz;
        s_eagle.x = s_eagle.sx + dx * s; s_eagle.z = s_eagle.sz + dz * s;
        s_eagle.y = s_eagle.sy + (s_eagle.ty - s_eagle.sy) * (s < 0.85 ? s / 0.85 * 0.9 : 0.9 + (s - 0.85) / 0.15 * 0.1);
        s_eagle_model = s < 0.8 ? 0 : eagle_flap(4);
        eagle_place(atan2(dx, dz), 0);
        if (s >= 1) {
            pr = s_obj.eagle_perch + 4 * s_eagle.perch;
            s_eagle.mode = EAGLE_PERCH; s_eagle.t = 0; s_eagle.dur = 480 + eagle_rand() % 420;
            s_eagle_model = 3;
            eagle_place(pr[3] * (6.2831853 / 4096.0), 0);
        }
        break;
    case EAGLE_PERCH:
        s_eagle_model = 3;
        if (s_eagle.t >= s_eagle.dur) {
            s_eagle.sx = s_eagle.x; s_eagle.sy = s_eagle.y; s_eagle.sz = s_eagle.z;
            s_eagle.tang = atan2(s_eagle.x, s_eagle.z) + 0.6;
            s_eagle.tx = s_obj.eagle_radius * sin(s_eagle.tang); s_eagle.tz = s_obj.eagle_radius * cos(s_eagle.tang);
            s_eagle.ty = s_obj.eagle_height;
            s_eagle.mode = EAGLE_TAKEOFF; s_eagle.t = 0; s_eagle.dur = 300;
        }
        break;
    case EAGLE_TAKEOFF:
        s = (double)s_eagle.t / s_eagle.dur;
        if (s > 1) s = 1;
        dx = s_eagle.tx - s_eagle.sx; dz = s_eagle.tz - s_eagle.sz;
        s_eagle.x = s_eagle.sx + dx * s; s_eagle.z = s_eagle.sz + dz * s;
        s_eagle.y = s_eagle.sy + (s_eagle.ty - s_eagle.sy) * sqrt(s);
        s_eagle_model = s < 0.6 ? eagle_flap(5) : 0;
        eagle_place(atan2(dx, dz), 0);
        if (s >= 1) {
            s_eagle.mode = EAGLE_CIRCLE; s_eagle.t = 0; s_eagle.dur = 900 + eagle_rand() % 900;
            s_eagle.ang = s_eagle.tang;
        }
        break;
    }
}

/* ---- OBJA 202, SHIP: Akihabara's ship. Its models (hull, house, the
 * rest) go together along a lane across the bay, slowly, in the tiles'
 * frame as the far background and the tiles are; out at the end, a wait,
 * then back the other way. It goes on over the rounds; a fight's start
 * starts it somewhere along the lane. ---- */
static struct { int started, dir, wait; double x; } s_ship;
static uint32_t s_ship_rand = 0x2468ACEu;
static int ship_rand(void) { s_ship_rand = s_ship_rand * 1103515245u + 12345u; return (int)(s_ship_rand >> 16 & 0x7FFF); }

static void ship_place(void) {
    const int16_t *sh = s_obj.ship;
    for (unsigned k = 0; k < 4; k++) {
        s_pos[4 * k] = (int16_t)s_ship.x; s_pos[4 * k + 1] = sh[6]; s_pos[4 * k + 2] = sh[0];
        s_ang[4 * k] = 0; s_ang[4 * k + 1] = (int16_t)(s_ship.dir > 0 ? 0 : 2048); s_ang[4 * k + 2] = 0;
    }
}

static void ship_init(void) {
    const int16_t *sh = s_obj.ship;
    if (!s_obj.ship_on || s_ship.started) { if (s_obj.ship_on) ship_place(); return; }
    s_ship.started = 1;
    s_ship.dir = ship_rand() & 1 ? 1 : -1;
    s_ship.wait = 0;
    s_ship.x = sh[1] + (sh[2] - sh[1]) * (0.2 + 0.4 * (ship_rand() % 1000) / 1000.0);
    if (s_ship.dir < 0) s_ship.x = sh[1] + sh[2] - s_ship.x;
    ship_place();
}

static void ship_step(void) {
    const int16_t *sh = s_obj.ship;
    if (!s_obj.ship_on) return;
    if (s_ship.wait > 0) {
        if (--s_ship.wait == 0) { s_ship.dir = -s_ship.dir; s_ship.x = s_ship.dir > 0 ? sh[1] : sh[2]; }
    } else {
        s_ship.x += s_ship.dir * sh[3] / 16.0;
        if ((s_ship.dir > 0 && s_ship.x >= sh[2]) || (s_ship.dir < 0 && s_ship.x <= sh[1])) {
            int lo = sh[4] > 0 ? sh[4] : 1, hi = sh[5] > lo ? sh[5] : lo + 1;
            s_ship.wait = lo + ship_rand() % (hi - lo);
            s_ship.x = s_ship.dir > 0 ? sh[2] : sh[1];
        }
    }
    ship_place();
}

/* ---- OBJA 203, GULL: the birds. Each on a circle of its own round a point
 * near the fight, high up, its radius breathing; it glides banked into the
 * turn, flaps a while now and then (models 0 gliding, 1 wings up, 2 half
 * way, 3 down), rising and falling a little; a flock (GULL's 9th value)
 * flies in a V behind the first, a bat (13th) darts. Then the critters
 * (10th: how many; 14th: their kind): the beach's crabs, Morning Fields'
 * rabbits (1), New York's pigeons (2). Tiles' frame, as the eagle. ---- */
typedef struct { double ang, r, h, cx, cz, dir, speed; int t, flap, glide; } Gull;
static Gull s_gull[8];
static int s_gull_pose[8];
static uint32_t s_gull_rand = 0x13579BDu;
static int gull_rand(void) { s_gull_rand = s_gull_rand * 1103515245u + 12345u; return (int)(s_gull_rand >> 16 & 0x7FFF); }

/* The beach's crabs: each keeps to a spot on the sand round the fight,
 * facing it, and scuttles sideways now and then, a short run one way or the
 * other, then waits; its body bobs and swings a little as it runs. */
typedef struct { double hx, hz, yaw, off; int dir, run, rest, t; } Crab;
static Crab s_crab[8];

/* a bird's flapping poses: with crabs model 2 is the crab's, no half way */
static int flap_pose(int i) {
    static const int cycle[4] = {1, 2, 3, 2}, short_cycle[4] = {1, 0, 2, 0};
    return (s_obj.gull[9] > 0 ? short_cycle : cycle)[i & 3];
}

static void crab_init(void) {
    const int16_t *g = s_obj.gull;
    memset(s_crab, 0, sizeof s_crab);
    for (int k = 0; k < g[9]; k++) {
        Crab *c = &s_crab[k];
        double a = (gull_rand() & 0xFFF) * (6.2831853 / 4096.0);
        double r = g[10] + (g[11] > g[10] ? gull_rand() % (g[11] - g[10]) : 0);
        c->hx = r * sin(a); c->hz = r * cos(a);
        c->yaw = a + 3.14159265;                 /* facing the fight */
        c->rest = gull_rand() % 240;
        c->dir = gull_rand() & 1 ? 1 : -1;
    }
}

static void crab_step(int k, int slot) {
    Crab *c = &s_crab[k];
    c->t++;
    double bob = 0, swing = 0;
    if (c->run > 0) {
        c->run--;
        c->off += c->dir * 2.6;
        if (c->off > 160 || c->off < -160) { c->dir = -c->dir; c->off += c->dir * 2.6; }
        bob = -1.5 * fabs(sin(c->t * 0.9));
        swing = 0.08 * sin(c->t * 0.45);
        if (!c->run) c->rest = 60 + gull_rand() % 360;
    } else if (--c->rest <= 0) {
        c->run = 20 + gull_rand() % 70;
        if (gull_rand() % 3 == 0) c->dir = -c->dir;
    }
    /* sideways: across the way it faces */
    double x = c->hx + cos(c->yaw) * c->off, z = c->hz - sin(c->yaw) * c->off;
    s_pos[4 * slot] = (int16_t)x; s_pos[4 * slot + 1] = (int16_t)bob; s_pos[4 * slot + 2] = (int16_t)z;
    s_ang[4 * slot] = 0; s_ang[4 * slot + 1] = (int16_t)((int)((c->yaw + swing) * 4096.0 / 6.2831853) & 0xFFF);
    s_ang[4 * slot + 2] = 0;
    s_gull_pose[slot] = 3;
}

/* Morning Fields' rabbits (kind 1): each sits a while, now and then turning
 * or nibbling, then hops off a few times the way it faces (each hop an arc,
 * nose up as it leaves the ground, down as it lands), keeping between the
 * critters' distances round the fight; one a fighter comes near bolts away
 * from them. New York's pigeons (kind 2) the same way, but walking (short
 * low steps, the head nodding), pecking often, and fluttering off low and
 * far. Tiles' frame, on the ground. */
typedef struct { double x, z, yaw, want; int hops, f, rest, t, fast; } Rabbit;
static Rabbit s_rabbit[8];
#define HOP_FRAMES 16

static void rabbit_init(void) {
    const int16_t *g = s_obj.gull;
    memset(s_rabbit, 0, sizeof s_rabbit);
    for (int k = 0; k < g[9]; k++) {
        Rabbit *r = &s_rabbit[k];
        double a = (gull_rand() & 0xFFF) * (6.2831853 / 4096.0);
        double d = g[10] + (g[11] > g[10] ? gull_rand() % (g[11] - g[10]) : 0);
        r->x = d * sin(a); r->z = d * cos(a);
        r->yaw = r->want = (gull_rand() & 0xFFF) * (6.2831853 / 4096.0);
        r->rest = gull_rand() % 200;
    }
}

static void rabbit_step(int k, int slot) {
    const int16_t *g = s_obj.gull;
    Rabbit *r = &s_rabbit[k];
    r->t++;
    /* the fighters, in the tiles' frame (cm) */
    int pigeon = g[13] == 2, spider = g[13] == 4, lizard = g[13] == 3 || spider;
    int fx[2], fz[2];
    world_tile((int)psx_mod_read_word(P1_X), (int)psx_mod_read_word(P1_Z), &fx[0], &fz[0]);
    world_tile((int)psx_mod_read_word(P2_X), (int)psx_mod_read_word(P2_Z), &fx[1], &fz[1]);
    for (int i = 0; i < 2; i++) {
        double dx = r->x - fx[i] / 10.0, dz = r->z - fz[i] / 10.0;
        if (dx * dx + dz * dz < 280.0 * 280.0 && !r->fast) {
            r->want = atan2(dx, dz);
            r->hops = pigeon || lizard ? 2 + gull_rand() % 2 : 4 + gull_rand() % 3; r->fast = 1; r->rest = 0;
            if (!r->f) r->f = 1;
        }
    }
    /* keep between the distances: turn back toward the ring's middle */
    double d = hypot(r->x, r->z);
    if (!r->f && (d < g[10] || d > g[11])) {
        double out = atan2(r->x, r->z);
        r->want = d < g[10] ? out + ((gull_rand() % 100) - 50) * 0.01 : out + 3.14159265 + ((gull_rand() % 100) - 50) * 0.01;
        if (!r->hops) r->hops = 2 + gull_rand() % 3;
    }
    /* turn toward where it wants to go (quickly, before hopping) */
    double turn = remainder(r->want - r->yaw, 6.2831853);
    r->yaw += turn * 0.25;
    double y = 0, pitch = 0, wiggle = 0;
    if (r->f) {
        double t = (double)r->f / HOP_FRAMES;
        double step = spider ? (r->fast ? 7.0 : 2.6) : lizard ? (r->fast ? 8.0 : 4.0) : pigeon ? (r->fast ? 9.0 : 1.1)
                    : r->fast ? 4.4 : 2.8;
        r->x += sin(r->yaw) * step; r->z += cos(r->yaw) * step;
        if (spider) {
            wiggle = 0.07 * sin(r->t * 2.3);                             /* scuttling: eight legs' jitter */
            y = -0.6 * fabs(sin(r->t * 1.7));
        } else if (lizard) {
            wiggle = 0.3 * sin(4 * 3.14159265 * t);                      /* the body swinging as it runs */
        } else if (pigeon && !r->fast) {
            y = -1.5 * fabs(sin(2 * 3.14159265 * t));                    /* two steps */
            pitch = 0.12 * sin(4 * 3.14159265 * t);                      /* the head nodding */
        } else {
            y = -(pigeon ? 70.0 : r->fast ? 26.0 : 18.0) * sin(3.14159265 * t);
            pitch = -0.28 * cos(3.14159265 * t);
        }
        if (++r->f > HOP_FRAMES) {
            r->f = 0;
            if (--r->hops > 0) r->f = 1;
            else { r->fast = 0; r->rest = pigeon ? 30 + gull_rand() % 200 : spider ? 40 + gull_rand() % 260
                                                   : lizard ? 200 + gull_rand() % 600 : 90 + gull_rand() % 420; }
        }
    } else if (r->hops > 0 && fabs(turn) < 0.2) {
        r->f = 1;
    } else if (--r->rest <= 0) {
        if (gull_rand() % 3 == 0) {                       /* only looks round */
            r->want = r->yaw + ((gull_rand() % 200) - 100) * 0.012;
            r->rest = 60 + gull_rand() % 240;
        } else {
            r->want = r->yaw + ((gull_rand() % 200) - 100) * 0.008;
            r->hops = pigeon ? 2 + gull_rand() % 6 : spider ? 2 + gull_rand() % 5 : 1 + gull_rand() % 4;
        }
    } else if (spider) {
        if ((r->t / 30 + k) % 4 == 0) wiggle = 0.05 * sin(r->t * 0.6);  /* feeling about, still */
    } else if (lizard) {
        if ((r->t / 40 + k) % 6 == 0) pitch = -0.12 * fabs(sin(r->t * 0.25));   /* push-ups, its head up */
    } else if ((r->t / 50 + k) % 5 == 0) {
        pitch = 0.18 + 0.05 * sin(r->t * 0.9);             /* nibbling the grass */
    } else if (pigeon && (r->t / 30 + k) % 3 == 0) {
        pitch = 0.2 + 0.25 * fabs(sin(r->t * 0.35));      /* pecking at crumbs */
    }
    s_pos[4 * slot] = (int16_t)r->x; s_pos[4 * slot + 1] = (int16_t)y; s_pos[4 * slot + 2] = (int16_t)r->z;
    s_ang[4 * slot] = (int16_t)(int)(pitch * 4096.0 / 6.2831853);
    s_ang[4 * slot + 1] = (int16_t)((int)((r->yaw + wiggle) * 4096.0 / 6.2831853) & 0xFFF);
    s_ang[4 * slot + 2] = 0;
    s_gull_pose[slot] = 3;
}

/* Eternal Darkness's spiders on their threads (GULL's hanging critters,
 * the last of them; the 5th model, its thread up out of sight): each comes
 * down slowly somewhere round the fight, hangs a while turning on the
 * thread, then climbs back up and waits above before the next. The old
 * fork's e454a7e. */
typedef struct { double x, z, y, yaw; int mode, wait; } Dangler;
static Dangler s_dangle[8];
#define DANGLE_TOP -1600.0

static void dangle_place(Dangler *d) {
    const int16_t *g = s_obj.gull;
    double a = ((int)(gull_rand() % 1600) - 800) * (6.2831853 / 4096.0);    /* beyond the fight (the camera on -z) */
    double r = g[10] + (g[11] > g[10] ? gull_rand() % (g[11] - g[10]) : 0);
    d->x = r * sin(a); d->z = r * cos(a);
    d->y = DANGLE_TOP; d->mode = 0; d->wait = 120 + gull_rand() % 600;
    d->yaw = (gull_rand() & 0xFFF) * (6.2831853 / 4096.0);
}

static void dangle_step(int k, int slot) {
    const int16_t *g = s_obj.gull;
    Dangler *d = &s_dangle[k];
    double low = g[15] < 0 ? g[15] : -90;
    switch (d->mode) {
    case 0: if (--d->wait <= 0) d->mode = 1; break;                       /* above, out of sight */
    case 1: d->y += 2.2 + 0.4 * sin(d->y * 0.05);                        /* coming down */
            if (d->y >= low) { d->y = low; d->mode = 2; d->wait = 200 + gull_rand() % 500; } break;
    case 2: d->yaw += 0.006 * sin(d->wait * 0.02);                        /* hanging, turning a little */
            if (--d->wait <= 0) d->mode = 3; break;
    default: d->y -= 3.5;                                                 /* climbing back up */
            if (d->y <= DANGLE_TOP) dangle_place(d);
    }
    double bob = d->mode == 2 ? 1.5 * sin(d->wait * 0.07) : 0;
    s_pos[4 * slot] = (int16_t)d->x; s_pos[4 * slot + 1] = (int16_t)(d->y + bob); s_pos[4 * slot + 2] = (int16_t)d->z;
    s_ang[4 * slot] = 0; s_ang[4 * slot + 1] = (int16_t)((int)(d->yaw * 4096.0 / 6.2831853) & 0xFFF); s_ang[4 * slot + 2] = 0;
    s_gull_pose[slot] = 4;
}

static int danglers(void) { return s_obj.nmodels >= 5 && !s_obj.grass_period ? s_obj.gull[14] : 0; }

static void gull_init(void) {
    const int16_t *g = s_obj.gull;
    memset(s_gull, 0, sizeof s_gull); memset(s_gull_pose, 0, sizeof s_gull_pose);
    if (g[13] >= 1) rabbit_init(); else crab_init();
    for (int k = 0; k < danglers(); k++) { dangle_place(&s_dangle[k]); s_dangle[k].wait = 60 + gull_rand() % 400; }
    for (int k = 0; k < g[0]; k++) {
        s_gull[k].ang = (gull_rand() & 0xFFF) * (6.2831853 / 4096.0);
        s_gull[k].r = g[1] + (g[2] > g[1] ? gull_rand() % (g[2] - g[1]) : 0);
        s_gull[k].h = g[3] + (g[4] > g[3] ? gull_rand() % (g[4] - g[3]) : 0);
        int spread = g[7] > 0 ? g[7] : 1;      /* round a point before the camera (on the tiles' -z side) */
        s_gull[k].cx = gull_rand() % (2 * spread) - spread; s_gull[k].cz = g[6] + gull_rand() % (2 * spread) - spread;
        s_gull[k].dir = gull_rand() & 1 ? 1 : -1;
        s_gull[k].speed = g[5] / 16.0 * (0.8 + 0.4 * (gull_rand() % 1000) / 1000.0);
        s_gull[k].t = gull_rand() % 1000;
        s_gull[k].glide = gull_rand() % 200;
    }
}

static void gull_step(void) {
    int flock = s_obj.gull[8];
    int walking = s_obj.gull[9] - danglers();
    for (int k = 0; k < walking; k++)
        if (s_obj.gull[13] >= 1) rabbit_step(k, s_obj.gull[0] + k); else crab_step(k, s_obj.gull[0] + k);
    for (int k = 0; k < danglers(); k++) dangle_step(k, s_obj.gull[0] + walking + k);
    for (int k = 0; k < s_obj.gull[0]; k++) {
        Gull *b = &s_gull[k];
        if (flock > 0 && k > 0) {
            /* a flock: the first bird leads, the others in a V behind it,
             * a row of two every flock cm, each flapping a little after
             * the one before (Kyoto at Sunset's crows) */
            const Gull *l = &s_gull[0];
            int row = (k + 1) / 2, side = k & 1 ? -1 : 1;
            double r = l->r + 0.12 * l->r * sin(l->t * 0.0031);
            double lx = l->cx + r * sin(l->ang), lz = l->cz + r * cos(l->ang);
            double fx = l->dir * cos(l->ang), fz = -l->dir * sin(l->ang);    /* its way */
            double back = row * flock, out = side * row * flock * 0.8;
            double x = lx - fx * back + fz * out, z = lz - fz * back - fx * out;
            b->t++;
            double y = l->h + 30.0 * row + 25.0 * sin(b->t * 0.021 + 1.7 * k) - (l->flap > 0 ? 20.0 : 0.0);
            s_gull_pose[k] = l->flap > 0 ? flap_pose((l->t + 3 * k) / 5) : 0;
            double yaw = atan2(fx, fz);
            s_pos[4 * k] = (int16_t)x; s_pos[4 * k + 1] = (int16_t)y; s_pos[4 * k + 2] = (int16_t)z;
            s_ang[4 * k] = 0; s_ang[4 * k + 1] = (int16_t)((int)(yaw * 4096.0 / 6.2831853) & 0xFFF);
            s_ang[4 * k + 2] = (int16_t)(int)(-0.3 * l->dir * 4096.0 / 6.2831853);
            continue;
        }
        b->t++;
        int flutter = s_obj.gull[12];
        double r = b->r + 0.12 * b->r * sin(b->t * 0.0031 + k);
        if (flutter > 0)        /* a bat: darting in and out of its circle, never gliding */
            r += flutter * sin(b->t * 0.083 + 1.9 * k) + 0.5 * flutter * sin(b->t * 0.21 + k);
        b->ang += b->dir * b->speed / r;
        if (flutter > 0) {
            s_gull_pose[k] = flap_pose(b->t / 3);
        } else if (b->flap > 0) {
            s_gull_pose[k] = flap_pose(b->t / 5);
            if (--b->flap == 0) b->glide = 150 + gull_rand() % 300;
        } else {
            s_gull_pose[k] = 0;
            if (--b->glide <= 0) b->flap = 40 + gull_rand() % 80;
        }
        double x = b->cx + r * sin(b->ang), z = b->cz + r * cos(b->ang);
        double y = b->h + 70.0 * sin(b->t * 0.017 + 2 * k) - (b->flap > 0 ? 20.0 : 0.0)
                 + (flutter > 0 ? 0.6 * flutter * sin(b->t * 0.13 + 2.3 * k) : 0.0);
        double yaw = atan2(b->dir * cos(b->ang), -b->dir * sin(b->ang));
        s_pos[4 * k] = (int16_t)x; s_pos[4 * k + 1] = (int16_t)y; s_pos[4 * k + 2] = (int16_t)z;
        s_ang[4 * k] = 0; s_ang[4 * k + 1] = (int16_t)((int)(yaw * 4096.0 / 6.2831853) & 0xFFF);
        s_ang[4 * k + 2] = (int16_t)(int)(-0.3 * b->dir * 4096.0 / 6.2831853);
    }
}

/* ---- the drawing (0x801E2A68, scene.object_commands) ---- */

/* 0x801E2C64 (stage 11: model 1 hangs from model 0) and 0x801E2E80: each
 * model's GTE from the tiles' */
static void object_gte_at(const Gte *tile, const int16_t *ang, const int16_t *pos, Gte *out) {
    int32_t r[3][3], v[3];
    *out = *tile;
    rot_matrix(ang, 0, r);
    memcpy(out->r, r, sizeof r);
    mul_matrix2(tile->r, out->r);
    apply_matrix(tile->r, pos[0], pos[1], pos[2], v);
    for (int i = 0; i < 3; i++) out->t[i] = tile->t[i] + v[i];
}
static void object_gte(const Gte *tile, unsigned k, Gte *out) {
    const int16_t *ang = &s_ang[4 * k], *pos = &s_pos[4 * k];
    int32_t r[3][3], v[3];
    *out = *tile;
    if (s_obj.ostage == 11 && k == 1) {
        Gte parent;
        object_gte(tile, 0, &parent);
        rot_matrix(ang, 1, r);
        memcpy(out->r, r, sizeof r);
        mul_matrix2(parent.r, out->r);
        apply_matrix(parent.r, pos[0], pos[1], pos[2], v);
        for (int i = 0; i < 3; i++) out->t[i] = parent.t[i] + v[i];
        return;
    }
    rot_matrix(ang, s_obj.ostage == 11, r);
    memcpy(out->r, r, sizeof r);
    mul_matrix2(tile->r, out->r);
    apply_matrix(tile->r, pos[0], pos[1], pos[2], v);
    for (int i = 0; i < 3; i++) out->t[i] = tile->t[i] + v[i];
}

/* OBJA 201, GRAS: the dry grass (Serene Forest, Arizona; Morning Fields'
 * after its critters). Each model is the same square of tufts, P world
 * units a side, built in the world's frame (centimetres from its corner),
 * laid on the squares of the world's grid of P, brought into the tiles'
 * frame as the dust is: the grass stays on the ground where it grows,
 * whatever the fighters do, and goes on wherever they go. The old fork laid
 * four, on the 2 x 2 squares nearest the fighters' midpoint: as the
 * midpoint crossed a line of the grid, a square in sight went and another
 * came (the user's report, Serene Forest, 2026-11-05); 3 x 3 squares were
 * not enough either: their edge stood in the background, and stepped P at
 * a time (the user's, the same day). Now: the tufts within GRASS_REACH / 2
 * squares of the midpoint (a round, following the midpoint smoothly; each
 * tuft cut on its own, objects()), on the squares that round touches. One
 * of the layouts draws them all (grass_n), the model being the same. */
enum { GRASS_SIDE = 7, GRASS_SQUARES = GRASS_SIDE * GRASS_SIDE, GRASS_REACH = 5 };
static int16_t s_grass_pos[GRASS_SQUARES][4], s_grass_ang[4];
static int32_t s_grass_at[GRASS_SQUARES][2], s_grass_mid[2], s_grass_r2;   /* centimetres */
static unsigned s_grass_n;
static void grass_place(void) {
    int P = s_obj.grass_period, mx = 0, mz = 0;
    s_grass_n = 0;
    if (!P) return;
    if (!psx_mod_read_byte(FIXED_STAGE)) {
        int p1x = (int)psx_mod_read_word(P1_X), p1z = (int)psx_mod_read_word(P1_Z);
        int p2x = (int)psx_mod_read_word(P2_X), p2z = (int)psx_mod_read_word(P2_Z);
        mx = p1x + (p2x - p1x) / 2; mz = p1z + (p2z - p1z) / 2;
    }
    double reach = (double)P * GRASS_REACH / 2;
    s_grass_mid[0] = mx / 10; s_grass_mid[1] = mz / 10;
    s_grass_r2 = (int32_t)((reach / 10) * (reach / 10));
    int i0 = (int)floor((double)mx / P) - GRASS_SIDE / 2, j0 = (int)floor((double)mz / P) - GRASS_SIDE / 2;
    s_grass_ang[0] = 0; s_grass_ang[1] = (int16_t)((int)psx_mod_read_word(AXIS) & 0xFFF); s_grass_ang[2] = 0;
    for (unsigned k = 0; k < GRASS_SQUARES; k++) {
        int x0 = (i0 + (int)(k % GRASS_SIDE)) * P, z0 = (j0 + (int)(k / GRASS_SIDE)) * P, tx, tz;
        /* the square's nearest point to the midpoint: within the round? */
        double nx = mx < x0 ? x0 : mx > x0 + P ? x0 + P : mx, nz = mz < z0 ? z0 : mz > z0 + P ? z0 + P : mz;
        if ((nx - mx) * (nx - mx) + (nz - mz) * (nz - mz) > reach * reach) continue;
        world_tile(x0, z0, &tx, &tz);
        unsigned n = s_grass_n++;
        s_grass_pos[n][0] = (int16_t)(tx / 10); s_grass_pos[n][1] = 0; s_grass_pos[n][2] = (int16_t)(tz / 10);
        s_grass_at[n][0] = x0 / 10; s_grass_at[n][1] = z0 / 10;
    }
}

static const unsigned PRIM_WORDS[7] = {7, 9, 5, 9, 12, 6, 8};   /* FT3 FT4 F4 GT3 GT4 G3 G4 */
static const unsigned PRIM_VERTS[7] = {3, 4, 4, 3, 4, 3, 4};

static uint32_t tex_window(const uint8_t w[4]) {
    unsigned x = w[0], y = w[1], ww = w[2], hh = w[3];
    if (!(ww || hh || x || y)) return 0xE2000000u;
    return 0xE2000000u | ((~(ww - 1u) & 0xFFu) >> 3) | ((~(hh - 1u) & 0xFFu) >> 3) << 5
         | (x >> 3) << 10 | (y >> 3) << 15;
}

/* Sub-pixel precision ([video] geometry_correction / perspective_texturing):
 * beside each projected position word, the 16.16 point it was rounded from
 * (the engine's put_xy, without its T-junction pass: the objects stand
 * alone). */
static int s_precise;
static int32_t in_pixel(int64_t p16, int v) {
    int64_t lo = (int64_t)v << 16;
    return (int32_t)(p16 < lo ? lo : p16 > lo + 0xFFFF ? lo + 0xFFFF : p16);
}
static void put_xy(uint32_t a, int x, int y, int32_t px, int32_t py, int z) {
    uint32_t w = xy(x, y);
    psx_mod_write_word(a, w);
    if (s_precise && px != NO_PRECISE && py != NO_PRECISE)
        psx_mod_precise_word(a, w, in_pixel(px, x), in_pixel(py, y), (uint32_t)(z < 1 ? 1 : z > 0xFFFF ? 0xFFFF : z));
}

static void emit_prim(uint32_t *ot_entry, const Prim *pr, const int *sx, const int *sy, const int32_t *px,
                      const int32_t *py, const int *sz) {
    unsigned kind = pr->kind, n = PRIM_VERTS[kind], words = PRIM_WORDS[kind];
    int win = (pr->header & 0x80000000u) != 0;
    uint32_t q = alloc_words(1 + words + 2 * win);
    if (!q) return;
    uint32_t o = q + 4;
    if (win) { psx_mod_write_word(o, tex_window(pr->win)); o += 4; }
    int gouraud = kind >= 3, textured = kind != 2 && kind != 5 && kind != 6;
    for (unsigned k = 0; k < n; k++) {
        unsigned i = pr->idx[k];
        if (k == 0 || gouraud) {
            const uint8_t *c3 = pr->rgb[gouraud ? k : 0];
            uint32_t c = (uint32_t)c3[0] | (uint32_t)c3[1] << 8 | (uint32_t)c3[2] << 16;
            psx_mod_write_word(o, k == 0 ? (uint32_t)pr->code << 24 | c : c); o += 4;
        }
        put_xy(o, sx[i], sy[i], px[i], py[i], sz[i]); o += 4;
        if (textured) {
            uint32_t w = (uint32_t)pr->uv[k][1] << 8 | pr->uv[k][0];
            if (k == 0) w |= (uint32_t)pr->clut << 16;
            if (k == 1) w |= (uint32_t)pr->tpage << 16;
            psx_mod_write_word(o, w); o += 4;
        }
    }
    if (win) psx_mod_write_word(o, 0xE2000000u);
    uint32_t e = *ot_entry;
    psx_mod_write_word(q, ((uint32_t)(words + 2 * win) << 24) | (e & 0xFFFFFFu));
    *ot_entry = (e & 0xFF000000u) | (q & 0xFFFFFFu);
}

/* Each drawing call's model through its GTE, the primitives facing the
 * camera (vertices 0, 1, 2) sorted by AVSZ (ZSF4 0x100, ZSF3 0x155 of SZ >>
 * 5) into a private OT linked at its main rank: its slots, slot 0
 * overwritten by the link (lost), or (size 1) one slot before the link. */
static void objects(const Gte *tile) {
    static int sx[256], sy[256], sz[256];
    static int32_t px[256], py[256];
    uint32_t ots[8] = {0};
    uint32_t ot_shadow[8][128];
    unsigned sizes[8] = {0}, ranks[8] = {0}, nots = 0;
    if (!s_obj.nlayout) return;
    /* the gulls' birds and critters first, then any grass squares */
    unsigned critters = s_obj.ostage == 203 ? (unsigned)(s_obj.gull[0] + s_obj.gull[9]) : 0;
    unsigned grass_from = s_obj.ostage == 201 ? 0 : s_obj.ostage == 203 && s_obj.grass_period ? critters : ~0u;
    if (grass_from != ~0u) grass_place();
    for (unsigned c = 0; c < s_obj.nlayout; c++) {
        const Layout *l = &s_obj.layout[c];
        int grass = c >= grass_from;
        if (grass && (c != grass_from || !s_grass_n)) continue;              /* the grass: its first layout, every square */
        if (s_obj.ostage == 200 && l->model != s_eagle_model) continue;     /* the eagle: one of its poses */
        if (s_obj.ostage == 202 && s_ship.wait > 0) continue;               /* the ship: gone, round the bay */
        /* a gull: the drawing call's bird (up to 8), its model its pose */
        unsigned at = s_obj.ostage == 203 ? c : l->model;
        unsigned mi = s_obj.ostage == 203 && c < critters ? (unsigned)s_gull_pose[c & 7] : l->model;
        if (mi >= s_obj.nmodels || at >= 12) continue;
        const Model *m = &s_obj.models[mi];
        unsigned o = 0;
        while (o < nots && ranks[o] != l->rank) o++;
        if (o == nots) {        /* ClearOTagR: each entry leads to the one before */
            unsigned n = l->size > 1 ? l->size : 2;
            if (l->size > 128 || nots == 8 || l->rank > 1023 || !(ots[o] = alloc_words(n))) return;
            for (unsigned i = 0; i < n; i++)
                ot_shadow[o][i] = i ? (ots[o] + 4 * (i - 1)) & 0xFFFFFFu : 0xFFFFFFu;
            sizes[o] = l->size; ranks[o] = l->rank; nots++;
        }
        for (unsigned inst = 0; inst < (grass ? s_grass_n : 1u); inst++) {
            Gte g;
            if (grass) {
                object_gte_at(tile, s_grass_ang, s_grass_pos[inst], &g);
            } else {
                object_gte(tile, at, &g);
            }
            for (unsigned i = 0; i < m->nv; i++) {
                Vert v; memcpy(&v, m->v + i, sizeof v);
                rtps_p(&g, v.x, v.y, v.z, &sx[i], &sy[i], &sz[i], &px[i], &py[i]);
            }
            if (s_obj.ostage == 203 && s_obj.gull[13] >= 1 && (int)c >= s_obj.gull[0] && c < critters && m->nv && sz[0] < 300)
                continue;           /* a rabbit right by the camera: not drawn (it would fill the screen) */
            for (unsigned k = 0; k < m->np; k++) {
                Prim pr; memcpy(&pr, m->p + k, sizeof pr);
                if (pr.kind > 6) continue;
                unsigned nvp = PRIM_VERTS[pr.kind], a = pr.idx[0], b = pr.idx[1], c2 = pr.idx[2], bad = 0;
                for (unsigned q = 0; q < nvp; q++) bad |= pr.idx[q] >= m->nv;
                if (bad) continue;
                if (grass) {        /* a tuft out of the round round the midpoint: not drawn */
                    Vert v; memcpy(&v, m->v + a, sizeof v);
                    int32_t dx = s_grass_at[inst][0] + v.x - s_grass_mid[0], dz = s_grass_at[inst][1] + v.z - s_grass_mid[1];
                    if (dx * dx + dz * dz > s_grass_r2) continue;
                }
                int cross = (int16_t)(sx[b] - sx[a]) * (int16_t)(sy[c2] - sy[b])
                          - (int16_t)(sy[b] - sy[a]) * (int16_t)(sx[c2] - sx[b]);
                if (cross <= 0) continue;
                unsigned slot = 1;
                if (sizes[o] > 1) {
                    int z = 0;
                    for (unsigned q = 0; q < nvp; q++) z += sz[pr.idx[q]] >> 5;
                    slot = (unsigned)((z * (nvp == 4 ? 0x100 : 0x155)) >> 12);
                    if (slot > 0xFFFF) slot = 0xFFFF;
                    if (slot >= sizes[o] || slot == 0) continue;      /* outside the table, or lost with slot 0 */
                }
                int behind = 0;         /* not in the arcade, as the tiles' guard */
                for (unsigned q = 0; q < nvp; q++) if (sz[pr.idx[q]] < 16) behind = 1;
                if (behind) continue;
                int minx = sx[a], maxx = sx[a], miny = sy[a], maxy = sy[a];
                for (unsigned q = 1; q < nvp; q++) {
                    int vx = sx[pr.idx[q]], vy = sy[pr.idx[q]];
                    if (vx < minx) minx = vx; if (vx > maxx) maxx = vx;
                    if (vy < miny) miny = vy; if (vy > maxy) maxy = vy;
                }
                if (maxx < -s_margin - 64 || minx >= 368 + s_margin + 64 || maxy < -32 || miny >= 512) continue;
                emit_prim(&ot_shadow[o][slot], &pr, sx, sy, px, py, sz);
            }
        }
    }
    for (unsigned o = 0; o < nots; o++) {     /* entry 0 to the main rank, the main rank to the last entry */
        unsigned n = sizes[o] > 1 ? sizes[o] : 2;
        unsigned last = sizes[o] > 1 ? sizes[o] - 1 : 1;
        uint32_t r = main_ot(ranks[o]);
        uint32_t re = psx_mod_read_word(r);
        ot_shadow[o][0] = (ot_shadow[o][0] & 0xFF000000u) | (re & 0xFFFFFFu);
        for (unsigned i = 0; i < n; i++) psx_mod_write_word(ots[o] + 4u * i, ot_shadow[o][i]);
        psx_mod_write_word(r, (re & 0xFF000000u) | ((ots[o] + 4 * last) & 0xFFFFFFu));
    }
}

/* The objects anew (the arcade starts them again at each round, 0x801E3D94). */
static void objects_round_start(void) {
    memset(s_st, 0, sizeof s_st); memset(s_ang, 0, sizeof s_ang); memset(s_pos, 0, sizeof s_pos);
    if (!s_obj.nlayout) return;
    if (s_obj.ostage == 11) stage11_init((int)psx_mod_read_word(0x800A08F0u));   /* as the vsync count */
    else if (s_obj.ostage == 200) eagle_init();
    else if (s_obj.ostage == 202) ship_init();
    else if (s_obj.ostage == 203) { gull_init(); gull_step(); }
}

/* Once a frame of the stage, not in the pause. */
static void objects_step(void) {
    if (!s_obj.nlayout) return;
    if (s_obj.ostage == 11) stage11_step((int)psx_mod_read_word(CAMERA + 4));
    else if (s_obj.ostage == 200) eagle_step();
    else if (s_obj.ostage == 202) ship_step();
    else if (s_obj.ostage == 203) gull_step();
}

/* ==== the lightning (FARV, LGHT, THND) ==== */

/* TTT1's lightning (0x8015C2A8 / 0x8015BFA0): five counters grow by rand &
 * 15 a frame of the fight; the first past its threshold starts its flash
 * (counter cleared; the strong ones, 3 and 4, also take a quarter off every
 * counter). A flash walks its pattern a frame at a time: 1 shows its bolt
 * sky, 0 the plain one; at the end it may chain to another (4 to 5). Frames
 * 60 and 130 of a round force counters 4 and 3. The flash lights the
 * fighters (fading over light_frames) and starts its thunder. */
static int s_bolt = -1, s_bolt_step, s_bolt_on;
static volatile int s_light_left;
static volatile uint32_t s_light_rgb;
static volatile int s_thunder_want = -1;
static int32_t s_bolt_acc[5];
static unsigned s_bolt_frame, s_bolt_vsync, s_bolt_seed = 0x2545F491u;
static int s_axis_before;               /* the fight's axis as the stage routine starts */
static unsigned bolt_rand(void) { s_bolt_seed = s_bolt_seed * 1103515245u + 12345u; return s_bolt_seed >> 16; }

static void lightning_tick(void) {
    s_bolt_on = 0;
    if (!s_storm.npat) return;
    unsigned vs = psx_mod_read_word(0x800A08F0u);
    int fighting = (int16_t)psx_mod_read_half(SCREEN) == 8 && psx_mod_read_half(PHASE) >= 6;
    if (!fighting) { s_bolt = -1; s_bolt_frame = 0; memset(s_bolt_acc, 0, sizeof s_bolt_acc); s_bolt_vsync = vs; return; }
    if (vs == s_bolt_vsync) {
        if (s_bolt >= 0 && s_bolt_step < s_storm.plen[s_bolt]) s_bolt_on = s_storm.pat[s_bolt][s_bolt_step];
        return;
    }
    s_bolt_vsync = vs;
    s_bolt_frame++;
    if (s_light_left > 0) s_light_left--;
    for (int k = 0; k < 5; k++) s_bolt_acc[k] += bolt_rand() & 15;
    for (int i = 0; i < 2; i++)
        if (s_bolt_frame == s_storm.lforce[i] && s_storm.lforce_acc[i] < 5) s_bolt_acc[s_storm.lforce_acc[i]] = 0x7FFF;
    if (s_bolt < 0) {
        for (int k = 0; k < 5 && k < s_storm.npat; k++) {
            if (s_bolt_acc[k] <= s_storm.lthr[k]) continue;
            s_bolt = k; s_bolt_step = 0; s_bolt_acc[k] = 0;
            if (s_storm.light_frames) { s_light_rgb = s_storm.light[k]; s_light_left = (int)s_storm.light_frames; }
            if (s_storm.thunder_of[k] < s_thnd.n) s_thunder_want = s_storm.thunder_of[k];
            if (k >= 3) for (int j = 0; j < 5; j++) s_bolt_acc[j] -= s_bolt_acc[j] >> 2;
            tmst_log("tekken2: remake extras: lightning %d at frame %u", k, s_bolt_frame);
            break;
        }
        if (s_bolt < 0) return;
    } else if (++s_bolt_step >= s_storm.plen[s_bolt]) {
        /* the chained bolt (4 then 5) lights the fighters again */
        int n = s_storm.pnext[s_bolt];
        if (n < s_storm.npat) {
            s_bolt = n; s_bolt_step = 0;
            if (s_storm.light_frames) { s_light_rgb = s_storm.light[n]; s_light_left = (int)s_storm.light_frames; }
        } else { s_bolt = -1; return; }
    }
    s_bolt_on = s_storm.pat[s_bolt][s_bolt_step];
}

/* For src/tekken2/tmst_tekken2_stages.c (SetBackColor): the flash's light
 * on the fighters, rgb times left / frames added; 0: none. */
int tmst_tekken2_extras_light(uint32_t *rgb, int *left, int *frames) {
    int l = s_light_left;
    if (!s_pack.loaded || !s_storm.light_frames || l <= 0) return 0;
    *rgb = s_light_rgb; *left = l; *frames = (int)s_storm.light_frames;
    return 1;
}

/* The far background (0x801A7688), arcade 512 x 240 mapped on the PS1
 * screen at the same angle per pixel: the engine's far_background_cells for
 * a pack of no stage of its own, drawn again with the bolt's lit sky. */
typedef struct { int ofx, ofy, h, margin; } Screen;
static int tdiv(int64_t a, int64_t b) { return (int)(a / b); }
static int tmod(int64_t a, int64_t b) { return (int)(a % b); }
static int pmod(int a, int b) { int r = a % b; return r < 0 ? r + b : r; }
static int s12(int a) { a &= 0xFFF; return a >= 0x800 ? a - 0x1000 : a; }
static int map_x(const Screen *s, int x) { return s->ofx + (int)(((int64_t)(x - 256) * s->h) / 600); }
static int map_y(const Screen *s, int y) { return s->ofy + (int)(((int64_t)(y - 120) * s->h) / 225); }
static int per_degree(int degrees) {
    int64_t v = (int64_t)degrees << 12;
    int64_t hi = (v * ((int64_t)0xB60B60B7 - ((int64_t)1 << 32))) >> 32;
    return (int)(((hi + v) >> 8) - (v >> 31));
}
static void tile_rect(uint32_t ot, int x, int y, int w, int h, const uint8_t c[3]) {
    if (w <= 0 || h <= 0) return;
    uint32_t p = alloc_words(4);
    if (!p) return;
    psx_mod_write_word(p + 4, 0x60000000u | (uint32_t)c[0] | (uint32_t)c[1] << 8 | (uint32_t)c[2] << 16);
    psx_mod_write_word(p + 8, xy(x, y));
    psx_mod_write_word(p + 12, (uint32_t)h << 16 | (uint32_t)w);
    add_prim(ot, p, 3);
}
static void band(uint32_t ot, const Screen *s, int y0, int y1, const uint8_t c[3]) {
    int a = map_y(s, y0), b = map_y(s, y1);
    if (a < 0) a = 0;
    tile_rect(ot, -s->margin, a, SCREEN_W + 2 * s->margin, b - a, c);
}

static int far_cell(uint32_t ot, const uint16_t *fdesc, const uint16_t *fcluts, int r, int ci, int k,
                    int xa, int ya, int xb, int yb) {
    int kind = s_storm.fkinds[r * s_storm.fring + ci];
    if (kind != 0x31 && kind != 0x74 && kind != 0x64) return 0;
    if (!ot) return 1;
    if (kind == 0x74 || kind == 0x64) {
        tile_rect(ot, xa, ya, xb - xa, yb - ya, kind == 0x74 ? s_storm.fcolours : s_storm.fcolours + 7);
        return 1;
    }
    uint16_t d = fdesc[r * s_storm.fcols + k];
    int tx = (d >> 2) & 0x3C0, ty = (d >> 4) & 0x300;
    uint32_t tp = (uint32_t)(((ty & 0x100) >> 4 | tx >> 6) | (ty & 0x200) << 2);
    int u = (d & 3) << 6, v = (d << 4) & 0xC0;
    int v0 = v, v1 = v + 0x3F;
    uint32_t clut = fcluts[r * s_storm.fcols + k];
    uint32_t q = alloc_words(10);
    if (q) {
        psx_mod_write_word(q + 4, 0x2C808080u);
        psx_mod_write_word(q + 8, xy(xa, ya));
        psx_mod_write_word(q + 12, clut << 16 | (uint32_t)v0 << 8 | (uint32_t)u);
        psx_mod_write_word(q + 16, xy(xb, ya));
        psx_mod_write_word(q + 20, tp << 16 | (uint32_t)v0 << 8 | (uint32_t)(u + 0x3F));
        psx_mod_write_word(q + 24, xy(xa, yb));
        psx_mod_write_word(q + 28, (uint32_t)v1 << 8 | (uint32_t)u);
        psx_mod_write_word(q + 32, xy(xb, yb));
        psx_mod_write_word(q + 36, (uint32_t)v1 << 8 | (uint32_t)(u + 0x3F));
        add_prim(ot, q, 9);
    }
    return 1;
}

static void bolt_sky(const Screen *s, uint32_t ot, int yaw, int pitch) {
    const uint16_t *fdesc = s_storm.bdesc[s_bolt], *fcluts = s_storm.bcluts[s_bolt];
    const uint8_t *cell_rgb = s_storm.fcolours, *top_rgb = s_storm.fcolours + 3, *bottom_rgb = s_storm.fcolours + 7;
    int top_on = s_storm.fcolours[6], bottom_on = s_storm.fcolours[10];
    const int H = 0xF0, step = 0x20;
    int a = tdiv((int64_t)((-yaw) & 0xFFF) << 9, per_degree(s_storm.fdegrees));
    int scroll = tmod(a, s_storm.fcols << 6);
    int col0 = tdiv(scroll, 64), kind0 = tdiv(tmod(a, s_storm.fring << 6), 64);
    int y = tdiv((int64_t)-s12(pitch) * s_storm.fpitch, 256) + s_storm.fyoff;
    if (y > 0 && top_on) band(ot, s, 0, y, top_rgb);
    /* columns: the arcade's 9 from the left edge, more for the 16:9 margins */
    int left = (int)(((int64_t)(s->margin + s->ofx) * 600) / s->h) - 256;
    int extra = left > 0 ? (left + 63) / 64 : 0;
    int count = 9 + 2 * extra;
    for (int r = 0; r < s_storm.frows; r++) {
        int x = tdiv(scroll, 64) * 64 - scroll - 64 * extra;
        for (int c = -extra; c < count - extra; c++) {
            if (!(y < -0x40 || y > H)) {
                int ci = pmod(kind0 + c, s_storm.fring), k = pmod(col0 + c, s_storm.fcols);
                int xa = map_x(s, x), xb = map_x(s, x + 0x40), ya = map_y(s, y), yb = map_y(s, y + step);
                if (!far_cell(ot, fdesc, fcluts, r, ci, k, xa, ya, xb, yb)) {
                    /* an empty cell: the nearest above in its column, else the top colour */
                    int up = r - 1;
                    while (up >= 0 && !far_cell(0, fdesc, fcluts, up, ci, k, 0, 0, 0, 0)) up--;
                    if (up >= 0) far_cell(ot, fdesc, fcluts, up, ci, k, xa, ya, xb, yb);
                    else tile_rect(ot, xa, ya, xb - xa, yb - ya, top_on ? top_rgb : cell_rgb);
                }
            }
            x += 0x40;
        }
        y += step;
    }
    if (y < H && bottom_on) band(ot, s, y < 0 ? 0 : y, H * 75 / 100, bottom_rgb);
}

/* The lit sky over the engine's: its own list, spliced in at the end of
 * rank 1023 (the far background's), before rank 1022. */
static void lightning(const CPUState *cpu) {
    lightning_tick();
    if (!s_bolt_on || s_bolt < 0 || s_bolt >= s_storm.nbolt) return;
    Screen s;
    s.ofx = (int)cpu->gte_ctrl[24] >> 16; s.ofy = (int)cpu->gte_ctrl[25] >> 16; s.h = (int)(cpu->gte_ctrl[26] & 0xFFFF);
    s.margin = s_margin;
    if (s.h <= 0) return;
    uint32_t r1022 = main_ot(1022) & 0xFFFFFFu, head = alloc_words(1);
    if (!head) return;
    psx_mod_write_word(head, r1022);                           /* an empty packet leading on to rank 1022 */
    bolt_sky(&s, head, (int)psx_mod_read_word(CAMERA + 4) - s_axis_before, (int)psx_mod_read_word(CAMERA));
    uint32_t link = main_ot(1023);
    for (unsigned guard = 0;; guard++) {
        uint32_t next = psx_mod_read_word(link) & 0xFFFFFFu;
        if (next == r1022) break;
        if (next == 0xFFFFFFu || guard >= 1u << 16) return;      /* not the chain expected */
        link = 0x80000000u | next;
    }
    psx_mod_write_word(link, (psx_mod_read_word(link) & 0xFF000000u) | (head & 0xFFFFFFu));
}

/* The thunder, mixed into the host's output after the engine's (TTT's
 * stage 4) by the voices' spu_render wrapper (src/tekken3_ttt1_voices.c), as
 * the old fork mixed it: at the sounds' gain / 4, both sides. */
static int s_thunder = -1;
static uint32_t s_thunder_pos;
static void thunder_mix(int32_t *out32, int frames, int audible, const int16_t main_volume[2], int gain) {
    int want = s_thunder_want;
    if (want >= 0) { s_thunder_want = -1; if ((unsigned)want < s_thnd.n) { s_thunder = want; s_thunder_pos = 0; } }
    if (s_thunder < 0 || (unsigned)s_thunder >= s_thnd.n || (int16_t)psx_mod_read_half(SCREEN) != 8) { s_thunder = -1; return; }
    const int16_t *pcm = s_thnd.pcm[s_thunder];
    uint32_t n = s_thnd.frames[s_thunder];
    for (int f = 0; f < frames && s_thunder_pos < n; f++, s_thunder_pos++)
        for (int ch = 0; ch < 2; ch++) {
            int32_t value = audible ? pcm[2 * s_thunder_pos + ch] * gain / 100 / 4 : 0;
            out32[2 * f + ch] += value * main_volume[ch] / 32768;
        }
    if (s_thunder_pos >= n) s_thunder = -1;
}

#ifdef TMST_HAS_STAGE_ENGINE
void __real_tekken3_arcade_stages_thunder_mix(int32_t *out32, int frames, int audible, const int16_t main_volume[2], int gain);
void __real_tekken3_arcade_stages_thunder_stop(void);
void __wrap_tekken3_arcade_stages_thunder_mix(int32_t *out32, int frames, int audible, const int16_t main_volume[2], int gain) {
    __real_tekken3_arcade_stages_thunder_mix(out32, frames, audible, main_volume, gain);
    thunder_mix(out32, frames, audible, main_volume, gain);
}
/* a reset of the SPU or a save state loaded: ours stops too */
void __wrap_tekken3_arcade_stages_thunder_stop(void) {
    s_thunder = -1; s_thunder_want = -1;
    __real_tekken3_arcade_stages_thunder_stop();
}
#endif

/* ---- the round ---- */

/* The round is decided: a fighter's life at 0 (KO, until the next round
 * gives it back) or the timer at 0; or the match is over (phase past 8). */
static int round_over(void) {
    int phase = psx_mod_read_half(PHASE);
    if (phase != 8) return phase > 8;
    return (int16_t)psx_mod_read_half(LIFE_P1) <= 0 || (int16_t)psx_mod_read_half(LIFE_P2) <= 0
        || psx_mod_read_half(TIMER) == 0;
}
static int s_was_over;

/* A fight's start (its pack read): the ship anew (it goes on over the
 * rounds), no round decided yet. */
static void fight_start(void) {
    s_ship.started = 0;
    s_was_over = 0;
}

static void round_start(void) {
    objects_round_start();
    s_nmote = 0; s_dust_seen = 0;
    s_nfeather = 0; s_feather_seen = 0; s_npetal = 0; s_petal_seen = 0; s_npaper = 0; s_paper_seen = 0;
    s_nwisp = 0; s_wisp_seen = 0; s_wisp_t = 0;
    s_nweed = 0; s_weed_seen = 0; s_weed_t = 0;
    s_nffly = 0; s_ffly_seen = 0; s_ffly_t = 0;
    s_ndrop = 0; s_rain_seen = 0;
    diya_reset();
    for (int i = 0; i < XANIMS; i++) {          /* the windows dark again */
        XAnim *xa = &s_pack.xanim[i];
        if (!xa->frames || xa->mode != 1) continue;
        xa->count = xa->frame = 0;
        (void)psx_mod_gpu_high_vram_upload(xa->x, xa->y, xa->w, xa->h, xa->pix);
    }
}

/* As the PS1's stage routine starts, once a frame of the stage, not in the
 * pause (src/tekken2/tmst_tekken2_stages.c, while our pack is drawn): the
 * palettes' next frames, a new round. */
void tmst_tekken2_extras_tick(void) {
    s_axis_before = (int)psx_mod_read_word(AXIS);      /* the far background's turn (the engine's s_axis_before) */
    if (!s_pack.loaded || paused()) return;
    for (int i = 0; i < XANIMS; i++) {
        XAnim *xa = &s_pack.xanim[i];
        if (!xa->frames || ++xa->count < xa->period) continue;
        xa->count = 0;
        if (xa->mode == 1 && xa->frame + 1u >= xa->frames) continue;      /* all lit: held till the next round */
        xa->frame = (xa->frame + 1) % xa->frames;
        (void)psx_mod_gpu_high_vram_upload(xa->x, xa->y, xa->w, xa->h, xa->pix + (size_t)xa->frame * xa->w * xa->h);
    }
    if (s_pack.flit_on) {
        /* FLIT: the lightning's light on the floor, its palette lit while a
         * flash is (the old fork's anim_tick, e454a7e) */
        int lit = s_bolt_on && s_bolt >= 0;
        if (lit != s_pack.flit_was) {
            static const uint16_t black[16];
            (void)psx_mod_gpu_high_vram_upload(s_pack.flit_x, s_pack.flit_y, 16, 1, lit ? s_pack.flit_lit : black);
            s_pack.flit_was = lit;
        }
    }
    int over = round_over();
    if (s_was_over && !over) round_start();      /* the next round starts */
    s_was_over = over;
    objects_step();
}

/* Once the engine has drawn the stage (0x8006D014 returns): the particles. */
/* ==== T2GL: the Church's stained glass light on the floor (D17) ==== */
/* The old fork laid the windows' light on the floor in the tiles (a 64 x 64
 * stained picture of 15 colours, stretched over 2 x 4 m, and its shaft): the
 * tiles follow the scenery's point, so the light slid over the floor, and
 * its texels showed. The user (2026-11-05): "the window's light on the
 * floor must stay fixed, move and turn with the floor; it is a little
 * pixelled". The Church's scenery is fixed to the floor now (FIXW: src/
 * tekken2/tmst_tekken2_anchor.c, its anchor on the floor's grid, unturned),
 * so the patches are drawn in the scenery's frame, as the old fork's tiles
 * had them, and stand with the floor. (A first try kept them in the world's
 * frame on their own, repeated every period of the floor: the far ones
 * floated beyond the floor's end and over the pews, the user's screenshot,
 * the same day.) Gouraud quads added to what is under them, a quad a pane of
 * the lancet (its lead the gaps between, its edge fading), the shaft rising
 * out of it to the window in pieces, brightest at its foot. At main rank
 * 1011: over the floor (its ranks 1012..1021, t2_remake_stages.py), under
 * the scenery's tiles (1010: the pews and pillars in front of them) and the
 * fighters' shadows (1007). After ours an E1 of half and half blending (the
 * floor's), as the packets after them had it. */
enum { GLASS_MAX = 8, GLASS_PANES = 64, GLASS_RANK = 1011 };
typedef struct { int16_t cx, cz, ang, len, wid, up, out; uint8_t level, slevel, pieces, pad; } GlassPatch;
static struct {
    unsigned n, rows, cols;
    int32_t period;
    uint8_t rgb[GLASS_PANES][3];
    GlassPatch p[GLASS_MAX];
} s_glass;

static void glass_load(const uint8_t *b, long size) {
    uint32_t len = 0;
    const uint8_t *o = section(b, size, "T2GL", &len);
    memset(&s_glass, 0, sizeof s_glass);
    if (!o || len < 8) return;
    uint16_t h[4]; memcpy(h, o, 8);
    unsigned panes = (unsigned)h[1] * h[2];
    if (!h[0] || h[0] > GLASS_MAX || !panes || panes > GLASS_PANES || len < 8u + 3u * panes + 18u * h[0]) return;
    memcpy(s_glass.rgb, o + 8, 3u * panes);
    for (unsigned k = 0; k < h[0]; k++) memcpy(&s_glass.p[k], o + 8 + 3 * panes + 18 * k, 18);
    s_glass.n = h[0]; s_glass.rows = h[1]; s_glass.cols = h[2]; s_glass.period = 10 * (int32_t)h[3];
}

/* A point of a patch (cm, the scenery's frame) to the screen. */
static int glass_point(const Gte *g, double x, double y, double z, int *sx, int *sy, int *sz) {
    rtps(g, (int)lround(x), (int)lround(y), (int)lround(z), sx, sy, sz);
    return *sz >= 32;
}

static uint32_t glass_rgb(const uint8_t c[3], double k) {
    int r = (int)lround(c[0] * k), gg = (int)lround(c[1] * k), bl = (int)lround(c[2] * k);
    if (r > 255) r = 255; if (gg > 255) gg = 255; if (bl > 255) bl = 255;
    if (r < 0) r = 0; if (gg < 0) gg = 0; if (bl < 0) bl = 0;
    return (uint32_t)bl << 16 | (uint32_t)gg << 8 | (uint32_t)r;
}

/* A Gouraud quad added to what is under it (0x3A): corners 0 1 / 2 3. */
static void glass_quad(uint32_t ot, const int *x, const int *y, const uint32_t *c) {
    int x0 = x[0], x1 = x[0], y0 = y[0], y1 = y[0];
    for (int k = 1; k < 4; k++) {
        if (x[k] < x0) x0 = x[k]; if (x[k] > x1) x1 = x[k]; if (y[k] < y0) y0 = y[k]; if (y[k] > y1) y1 = y[k];
    }
    if (x1 - x0 >= 1024 || y1 - y0 >= 512 || x1 < -64 - s_margin || x0 > SCREEN_W + 64 + s_margin || y1 < 0 || y0 > 512)
        return;
    uint32_t q = alloc_words(9);
    if (!q) return;
    for (int k = 0; k < 4; k++) {
        psx_mod_write_word(q + 4 + 8 * k, k ? c[k] : (0x3Au << 24 | c[0]));
        psx_mod_write_word(q + 8 + 8 * k, xy(x[k], y[k]));
    }
    add_prim(ot, q, 8);
}

static void glass_patch(const Gte *g, uint32_t ot, const GlassPatch *p) {
    double a = p->ang * 6.283185307179586 / 4096.0;
    double rx = sin(a), rz = cos(a), tx = cos(a), tz = -sin(a);
    double len = p->len, wid = p->wid, k = 0.42 * p->level / 128.0;
    unsigned rows = s_glass.rows, cols = s_glass.cols;
    /* the lancet's half width at a row's edge (its pointed head toward the
     * middle, row 0), as the stained picture's */
    double hw[GLASS_PANES + 1];
    for (unsigned r = 0; r <= rows; r++) {
        double y = 64.0 * r / rows, f = y < 22 ? 1 - ((22 - y) / 22) * ((22 - y) / 22) : 1;
        hw[r] = f > 0 ? sqrt(f) : 0;
    }
    const double lead = 0.06;               /* of a pane: the lead between the panes */
    for (unsigned r = 0; r < rows; r++)
        for (unsigned c = 0; c < cols; c++) {
            int x[4], y[4], z[4], ok = 1;
            uint32_t col[4];
            for (int v = 0; v < 4; v++) {
                double fr = r + (v >> 1 ? 1 - lead : lead), fc = c + (v & 1 ? 1 - lead : lead);
                unsigned rr = v >> 1 ? r + 1 : r;
                double half = hw[rr] * wid / 2, s = -len / 2 + len * fr / rows;
                double t = -half + 2 * half * fc / cols;
                ok &= glass_point(g, p->cx + rx * s + tx * t, -1, p->cz + rz * s + tz * t, &x[v], &y[v], &z[v]);
                /* the light's edge soft: the lancet's outline dim */
                int edge = (c == 0 && !(v & 1)) || (c == cols - 1 && (v & 1)) || (r == 0 && !(v >> 1)) ||
                           (r == rows - 1 && (v >> 1));
                col[v] = glass_rgb(s_glass.rgb[r * cols + c], k * (edge ? 0.3 : 1.0));
            }
            if (ok) glass_quad(ot, x, y, col);
        }
    /* the shaft it comes down in: out and up toward the window, pale, its
     * middle brighter than its sides, fading out toward the top */
    if (!p->pieces || !p->slevel) return;
    unsigned n = p->pieces;
    double ks = 0.55 * p->slevel / 128.0;
    static const uint8_t pale[3] = { 150, 160, 190 };
    for (unsigned i = 0; i < n; i++)
        for (int side = -1; side <= 1; side += 2) {
            int x[4], y[4], z[4], ok = 1;
            uint32_t col[4];
            for (int v = 0; v < 4; v++) {
                unsigned kk = v >> 1 ? i : i + 1;          /* corners 0 1 the upper ring, 2 3 the lower */
                double f = (double)kk / n, w = (0.45 + 0.2 * f) * wid;
                double t = v & 1 ? 0 : side * w, s = p->out * f;
                double yy = -2 - (p->up - 2) * f;
                ok &= glass_point(g, p->cx + rx * s + tx * t, yy, p->cz + rz * s + tz * t, &x[v], &y[v], &z[v]);
                double along = pow(1 - f, 1.3), across = v & 1 ? 1.0 : 0.15;
                col[v] = glass_rgb(pale, ks * along * across);
            }
            if (ok) glass_quad(ot, x, y, col);
        }
}

static void glass_draw(const Gte *g) {
    if (!s_glass.n) return;
    uint32_t ot = main_ot(GLASS_RANK);
    uint32_t e = alloc_words(2);                /* drawn last: half and half blending back (the floor's) */
    if (!e) return;
    psx_mod_write_word(e + 4, 0xE1000200u);
    add_prim(ot, e, 1);
    for (unsigned k = 0; k < s_glass.n; k++) glass_patch(g, ot, &s_glass.p[k]);
    uint32_t b = alloc_words(2);                /* drawn first: added (abr 1) */
    if (!b) return;
    psx_mod_write_word(b + 4, 0xE1000220u);
    add_prim(ot, b, 1);
}

static void glass_clear(void) { memset(&s_glass, 0, sizeof s_glass); }
static int glass_on(void) { return s_glass.n > 0; }

/* ==== T2TL: a stage fixed to the floor, its tiles drawn here ==== */
/* The Church (FIXW): the engine gets no tiles (tools/t2_remake_stages.py
 * own_tiles). Its tile drawing (tiles(): each tile's vertices through the
 * stage's GTE, each primitive facing the camera into its tile's slot of two
 * private OTs of 16, A at main rank 1010, B at 1021) keeps the tiles of a
 * cone from the camera and drops every tile as near to the camera as its
 * grid's centre: on a stage that follows the fighters that is what lies
 * between them and the camera; fixed, the scenery near the fighters went as
 * they came to it ("they may go up to the objects, only these must not
 * vanish", the user, 2026-11-05). Here every tile, every primitive facing
 * the camera, on the screen, none of its vertices behind the camera. The
 * scenery's frame is the anchor's (src/tekken2/tmst_tekken2_anchor.c: the
 * floor's grid point, unturned), as for the objects. */
typedef struct { uint8_t slot, layer_b, skipped, pad; uint16_t nv, np; } OwnTileHead;
static struct {
    uint8_t *data;
    unsigned n;
    const uint8_t *tile[256];
} s_own;

static void own_tiles_clear(void) { free(s_own.data); memset(&s_own, 0, sizeof s_own); }
static int own_tiles_on(void) { return s_own.n > 0; }

static void own_tiles_load(const uint8_t *b, long size) {
    uint32_t len = 0;
    const uint8_t *o = section(b, size, "T2TL", &len);
    own_tiles_clear();
    if (!o || len < 8 || !(s_own.data = malloc(len))) return;
    memcpy(s_own.data, o, len);
    uint32_t n; memcpy(&n, s_own.data + 4, 4);
    if (n > 256) { own_tiles_clear(); return; }
    uint32_t at = 8;
    for (uint32_t i = 0; i < n; i++) {
        OwnTileHead h;
        if (at + 8 > len) { own_tiles_clear(); return; }
        memcpy(&h, s_own.data + at, 8);
        uint32_t next = at + 8 + 8u * h.nv + (uint32_t)sizeof(Prim) * h.np;
        if (next > len || h.nv > 0x100) { own_tiles_clear(); return; }
        s_own.tile[i] = s_own.data + at;
        at = next;
    }
    s_own.n = n;
    tmst_log("tekken2: remake extras: the stage's %u tiles drawn by the add-on (fixed to the floor)", n);
}

static void own_tiles_draw(const Gte *g) {
    static int sx[256], sy[256], sz[256];
    static int32_t px[256], py[256];
    if (!s_own.n) return;
    uint32_t ot[2], ot_shadow[2][16];
    for (int k = 0; k < 2; k++) {               /* ClearOTagR: each entry leads to the one before */
        if (!(ot[k] = alloc_words(16))) return;
        for (unsigned i = 0; i < 16; i++)
            ot_shadow[k][i] = i ? (ot[k] + 4 * (i - 1)) & 0xFFFFFFu : 0xFFFFFFu;
    }
    for (unsigned t = 0; t < s_own.n; t++) {
        OwnTileHead h;
        memcpy(&h, s_own.tile[t], 8);
        if (!h.np || h.skipped || !h.nv) continue;
        const uint8_t *vp = s_own.tile[t] + 8, *pp = vp + 8u * h.nv;
        for (unsigned i = 0; i < h.nv; i++) {
            Vert v; memcpy(&v, vp + 8 * i, sizeof v);
            rtps_p(g, v.x, v.y, v.z, &sx[i], &sy[i], &sz[i], &px[i], &py[i]);
        }
        uint32_t *slot = &ot_shadow[h.layer_b ? 1 : 0][h.slot & 15];
        for (unsigned k = 0; k < h.np; k++) {
            Prim pr; memcpy(&pr, pp + sizeof(Prim) * k, sizeof pr);
            if (pr.kind > 6) continue;
            unsigned nvp = PRIM_VERTS[pr.kind], a = pr.idx[0], b = pr.idx[1], c = pr.idx[pr.cull & 7], bad = 0;
            for (unsigned q = 0; q < nvp; q++) bad |= pr.idx[q] >= h.nv;
            if (bad || c >= h.nv) continue;
            int cross = (sx[b] - sx[a]) * (sy[c] - sy[b]) - (sy[b] - sy[a]) * (sx[c] - sx[b]);
            if (cross <= 0) continue;
            int behind = 0, x0 = 4096, x1 = -4096, y0 = 4096, y1 = -4096;
            for (unsigned q = 0; q < nvp; q++) {
                unsigned i = pr.idx[q];
                if (sz[i] < 16) behind = 1;
                if (sx[i] < x0) x0 = sx[i]; if (sx[i] > x1) x1 = sx[i];
                if (sy[i] < y0) y0 = sy[i]; if (sy[i] > y1) y1 = sy[i];
            }
            if (behind || x1 < -32 - s_margin || x0 > SCREEN_W + 32 + s_margin || y1 < -24 || y0 > 512) continue;
            emit_prim(slot, &pr, sx, sy, px, py, sz);
        }
    }
    static const unsigned rank[2] = { 1010, 1021 };
    for (int k = 0; k < 2; k++) {               /* entry 0 to the main rank's, the main rank to entry 15 */
        uint32_t r = main_ot(rank[k]);
        uint32_t re = psx_mod_read_word(r);
        ot_shadow[k][0] = (ot_shadow[k][0] & 0xFF000000u) | (re & 0xFFFFFFu);
        for (unsigned i = 0; i < 16; i++) psx_mod_write_word(ot[k] + 4u * i, ot_shadow[k][i]);
        psx_mod_write_word(r, (re & 0xFF000000u) | ((ot[k] + 4u * 15) & 0xFFFFFFu));
    }
}

void tmst_tekken2_extras_draw(const CPUState *cpu) {
    if (!s_pack.loaded) return;
    if (!s_memory) {
        /* the packets read the upper VRAM half (texpage bit 11, CLUT bit 15)
         * only from memory the GPU is told of */
        uint32_t m = psx_mod_alloc_gpu_dma_memory(2 * BUFFER_BYTES, 16);
        if (!m) return;
        if (!psx_mod_gpu_high_vram_source(m, 2 * BUFFER_BYTES))
            tmst_log("tekken2: remake extras: no upper VRAM source for their packets");
        s_memory = m;
    }
    tables();
    Gte g;
    g.ofx = (int32_t)cpu->gte_ctrl[24]; g.ofy = (int32_t)cpu->gte_ctrl[25]; g.h = cpu->gte_ctrl[26] & 0xFFFF;
    tile_matrix(&g);
    unsigned b = psx_mod_read_word(OT_BUFFER) & 1u;
    s_cursor = s_memory + BUFFER_BYTES * b;
    s_end = s_cursor + BUFFER_BYTES;
    s_margin = tmst_compat_widescreen_margin();
    if (s_margin < 0) s_margin = 0;
    s_precise = psx_mod_precision_wanted();
    uint64_t t0 = t2_now_ns();
    own_tiles_draw(&g);
    uint64_t t1 = t2_now_ns();
    glass_draw(&g);
    uint64_t t2 = t2_now_ns();
    objects(&g);
    uint64_t t3 = t2_now_ns();
    dust(&g);
    feathers(&g);
    fireflies(&g);
    diyas(&g);
    rain(&g);
    lightning(cpu);
    uint64_t t4 = t2_now_ns();
    g_t2_own_tiles_ns += (t1 - t0);
    g_t2_objects_ns += (t3 - t2);
    g_t2_fx_ns += (t2 - t1) + (t4 - t3);
    g_t2_extras_draw_ns += (t4 - t0);
    /* the buffer's use, said as its peak grows (a full buffer drops what comes after) */
    static uint32_t peak;
    uint32_t used = s_cursor - (s_memory + BUFFER_BYTES * b);
    if (used > peak + 8192u || (used >= BUFFER_BYTES - 64u && peak < BUFFER_BYTES - 64u)) {
        peak = used;
        tmst_log("tekken2: remake extras: %u of %u bytes of packets", (unsigned)used, (unsigned)BUFFER_BYTES);
    }
}
