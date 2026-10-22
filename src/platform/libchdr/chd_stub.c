#include "chd.h"

chd_error chd_open(const char* filename, int mode, chd_file* parent, chd_file** out) {
    (void)filename; (void)mode; (void)parent;
    if (out) *out = 0;
    return CHDERR_UNSUPPORTED_VERSION;
}

void chd_close(chd_file* file) {
    (void)file;
}

chd_error chd_read(chd_file* file, uint32_t hunknum, void* buffer) {
    (void)file; (void)hunknum; (void)buffer;
    return CHDERR_UNSUPPORTED_VERSION;
}

const chd_header* chd_get_header(chd_file* file) {
    (void)file;
    return 0;
}

chd_error chd_get_metadata(chd_file* file, uint32_t searchtag, uint32_t searchindex,
                           void* output, uint32_t output_len, uint32_t* result_len,
                           uint32_t* result_tag, uint8_t* result_flags) {
    (void)file; (void)searchtag; (void)searchindex;
    (void)output; (void)output_len; (void)result_len;
    (void)result_tag; (void)result_flags;
    return CHDERR_UNSUPPORTED_VERSION;
}
