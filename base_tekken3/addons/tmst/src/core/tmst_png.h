/* TMST core: a small PNG writer and reader (the runtime's png_write.h only stores, and
 * is private to it).
 *
 * 8-bit RGB, one IDAT: each row filtered (None, Sub, Up or Paeth, whichever
 * leaves the smallest sum of magnitudes), the whole compressed in one
 * fixed-Huffman deflate block (RFC 1951 3.2.6) with LZ77 matches found by a
 * 3-byte hash chain over a 32 KB window. No libraries (tests/test_png.py
 * builds it alone and checks the files with Python's zlib). */
#ifndef TMST_PNG_H
#define TMST_PNG_H
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* `argb` (0xAARRGGBB, alpha ignored), w x h, rows `pitch` pixels apart, as a
 * PNG file at `path`. Returns 1, or 0 (no memory, the file not written). */
int tmst_png_write(const char *path, const uint32_t *argb, int w, int h, int pitch);
/* The same into memory: *out (malloc'd, the caller frees), its size. */
int tmst_png_encode(const uint32_t *argb, int w, int h, int pitch, uint8_t **out, size_t *size);

/* And a reader: 8-bit grey, grey + alpha, RGB, RGBA or palette (tRNS too),
 * not interlaced, into *argb (malloc'd 0xAARRGGBB, the caller frees), w x h.
 * Returns 1, or 0 (no file, another format, no memory). */
int tmst_png_decode(const uint8_t *data, size_t size, uint32_t **argb, int *w, int *h);
int tmst_png_read(const char *path, uint32_t **argb, int *w, int *h);

#ifdef __cplusplus
}
#endif
#endif
