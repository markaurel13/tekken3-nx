#include "psx_fiber.h"

psx_fiber_t psx_fiber_convert_thread(void) {
    return (psx_fiber_t)1;
}

psx_fiber_t psx_fiber_current(void) {
    return (psx_fiber_t)1;
}

psx_fiber_t psx_fiber_create(size_t stack_size, psx_fiber_entry entry, void* arg) {
    (void)stack_size; (void)entry; (void)arg;
    return (psx_fiber_t)0;
}

void psx_fiber_switch(psx_fiber_t target) {
    (void)target;
}

void psx_fiber_destroy(psx_fiber_t fiber) {
    (void)fiber;
}
