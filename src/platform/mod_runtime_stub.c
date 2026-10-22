#include <stdint.h>

void mod_runtime_on_vblank(void) {}
void mod_runtime_on_dispatch(uint32_t target) { (void)target; }
void mod_runtime_patch_disc_sector(uint32_t lba, int raw_sector, uint8_t* buffer, int size) {
    (void)lba; (void)raw_sector; (void)buffer; (void)size;
}
