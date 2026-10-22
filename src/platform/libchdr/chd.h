#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CHD_OPEN_READ 1
#define CHDERR_NONE 0
#define CHDERR_UNSUPPORTED_VERSION 1
#define CHDERR_FILE_NOT_FOUND 2
#define CHDERR_METADATA_NOT_FOUND 3

#define CDROM_TRACK_METADATA2_TAG 0x43445432
#define CDROM_TRACK_METADATA_TAG  0x43445452

typedef int chd_error;
typedef struct chd_file chd_file;

typedef struct chd_header {
    uint32_t hunkbytes;
    uint32_t unitbytes;
    uint64_t logicalbytes;
} chd_header;

chd_error chd_open(const char* filename, int mode, chd_file* parent, chd_file** out);
void chd_close(chd_file* file);
chd_error chd_read(chd_file* file, uint32_t hunknum, void* buffer);
const chd_header* chd_get_header(chd_file* file);
chd_error chd_get_metadata(chd_file* file, uint32_t searchtag, uint32_t searchindex,
                           void* output, uint32_t output_len, uint32_t* result_len,
                           uint32_t* result_tag, uint8_t* result_flags);

#ifdef __cplusplus
}
#endif
