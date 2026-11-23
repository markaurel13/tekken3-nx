/* The Jukebox's ADPCM packs: a .bgm that keeps the game's own PS-ADPCM
 * instead of its decoded frames (tools/adpcm_pack.py writes them), about a
 * quarter of the size, decoded here as it plays.
 *
 * File: 'T3ADP001', u32 rate (44 100), u32 frames, u32 loop frame (where the
 * piece starts again at its end; 0 the beginning), u32 gain (per 65 536,
 * applied to every decoded sample), u32 0, u32 0; then the blocks, 16 bytes
 * of the left channel and 16 of the right, each PS-ADPCM (shift and filter
 * byte, flags byte, 28 4-bit samples): 28 frames per 32 bytes. */
#ifndef TMST_ADPCM_H
#define TMST_ADPCM_H

#include <stdint.h>
#include <stdio.h>

#define TMST_ADPCM_MAGIC "T3ADP001"
enum { TMST_ADPCM_HEAD = 32, TMST_ADPCM_BLOCK = 32, TMST_ADPCM_FRAMES = 28 };

typedef struct {
    FILE *f;
    uint32_t frames, loop, gain;
    uint32_t pos;                     /* the frame of the next block */
    int32_t h[2][2];                  /* each channel's last two samples */
    int32_t loop_h[2][2]; int loop_saved;
} TmstAdpcm;

/* A pack whose 32-byte header is head (f just after it); 0 when it is not one. */
int tmst_adpcm_open(TmstAdpcm *a, FILE *f, const uint8_t head[TMST_ADPCM_HEAD]);
/* The next frames into out (stereo s16, room for max >= 28 frames): their
 * count, from out[*first] on (after the end, the loop's block again from
 * its frame); 0 on a read error. */
unsigned tmst_adpcm_fill(TmstAdpcm *a, int16_t *out, unsigned max, unsigned *first);

/* XA packs: a .bgm of a PlayStation disc's XA audio as it is (tools/
 * adpcm_pack.py's write_xa; Tekken 2's music), stereo at 37 800 Hz, brought
 * to 44 100 Hz here as it plays.
 *
 * File: 'T3XA0001', u32 rate (37 800), u32 frames (at 37 800 Hz, 112 a
 * group), u32 0 (it loops whole), u32 gain (per 65 536, after the
 * resampling), u32 0, u32 0; then the sound groups, 128 bytes each (16
 * header bytes, units' filter and shift at 4..11; 8 units of 28 4-bit
 * samples, even units left, odd right).
 *
 * The resampling: output frame i from the source frames round i * 6 / 7, a
 * Hann-windowed sinc of 16 taps a side (frames outside the piece are 0), the
 * one tools/tekken2_music.py's resample() is; floor(frames * 7 / 6) frames,
 * then again from the beginning. */
#define TMST_XA_MAGIC "T3XA0001"
enum { TMST_XA_GROUP = 128, TMST_XA_GROUP_FRAMES = 112, TMST_XA_TAPS = 16, TMST_XA_RING = 512 };

typedef struct {
    FILE *f;
    uint32_t frames, gain, out_frames, out_pos;
    uint32_t decoded;                 /* the source frames decoded so far */
    int32_t h[2][2];
    int16_t ring[TMST_XA_RING * 2];   /* the last decoded source frames */
} TmstXaPack;

int tmst_xa_open(TmstXaPack *x, FILE *f, const uint8_t head[TMST_ADPCM_HEAD]);
/* The next max frames (44 100 Hz) into out: their count, 0 on a read error. */
unsigned tmst_xa_fill(TmstXaPack *x, int16_t *out, unsigned max);

#endif
