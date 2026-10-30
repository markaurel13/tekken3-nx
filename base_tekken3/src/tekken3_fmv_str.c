/* Auto-generated FMV STR stream decoder for Tekken 3 */
#include "tekken3_fmv_overlay.h"
#include <string.h>

extern void psx_check_interrupts_dispatch_entry(CPUState* cpu, uint32_t resume_pc);
extern uint8_t *memory_get_ram_ptr(void);

static const uint8_t s_fmv_str_rom[10240] = {
    0xB4u, 0xFBu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x80u, 0x40u, 0x00u, 0x21u, 0x18u, 0x00u, 0x02u,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x52u, 0x24u, 0x21u, 0x28u, 0x40u, 0x02u, 0x04u, 0x00u, 0x62u, 0x8Eu,
    0x00u, 0x00u, 0x64u, 0x8Eu, 0x21u, 0x80u, 0x02u, 0x02u, 0xC2u, 0x42u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x64u, 0x00u,
    0x21u, 0x20u, 0x00u, 0x02u, 0xC2u, 0x42u, 0x02u, 0x0Cu, 0x18u, 0x00u, 0x45u, 0x26u, 0x0Au, 0x00u, 0x22u, 0x96u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x42u, 0x30u, 0x40u, 0x00u, 0x42u, 0xA6u, 0x0Au, 0x00u, 0x22u, 0x96u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x12u, 0x02u, 0x00u, 0x0Fu, 0x00u, 0x42u, 0x30u, 0x42u, 0x00u, 0x42u, 0xA6u,
    0x0Au, 0x00u, 0x22u, 0x96u, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x42u, 0x30u, 0x44u, 0x00u, 0x42u, 0xA6u,
    0x0Au, 0x00u, 0x22u, 0x96u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x42u, 0x30u, 0x4Cu, 0x00u, 0x42u, 0xA6u,
    0x0Cu, 0x00u, 0x22u, 0x8Eu, 0x00u, 0x00u, 0x00u, 0x00u, 0x58u, 0x00u, 0x42u, 0xAEu, 0x08u, 0x00u, 0x24u, 0x92u,
    0x50u, 0xADu, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x5Eu, 0xAEu, 0x01u, 0x0Cu, 0xFFu, 0xFFu, 0x04u, 0x24u,
    0x5Eu, 0xAEu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x40u, 0x00u, 0x7Fu, 0x00u, 0x04u, 0x24u, 0x26u, 0xAFu, 0x01u, 0x0Cu,
    0xFFu, 0xFFu, 0x05u, 0x24u, 0x91u, 0xAEu, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x42u, 0x00u, 0x43u, 0x86u,
    0x03u, 0x00u, 0x02u, 0x24u, 0x03u, 0x00u, 0x62u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u, 0x8Cu, 0xF7u, 0x03u, 0x08u,
    0x54u, 0x00u, 0x40u, 0xA6u, 0x54u, 0x00u, 0x42u, 0xA6u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x44u, 0x24u,
    0x44u, 0x00u, 0x83u, 0x84u, 0x03u, 0x00u, 0x02u, 0x24u, 0x04u, 0x00u, 0x62u, 0x14u, 0x01u, 0x00u, 0x02u, 0x24u,
    0x44u, 0x00u, 0x80u, 0xA4u, 0x96u, 0xF7u, 0x03u, 0x08u, 0x46u, 0x00u, 0x82u, 0xA4u, 0x46u, 0x00u, 0x80u, 0xA4u,
    0x11u, 0x80u, 0x06u, 0x3Cu, 0x06u, 0x00u, 0x22u, 0x86u, 0x58u, 0x44u, 0xC5u, 0x24u, 0x04u, 0x00u, 0xA2u, 0xACu,
    0x00u, 0x00u, 0x62u, 0x8Eu, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0xA2u, 0xACu, 0x02u, 0x00u, 0x22u, 0x86u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x42u, 0x28u, 0x02u, 0x00u, 0x40u, 0x10u, 0x40u, 0x01u, 0x03u, 0x24u,
    0x00u, 0x01u, 0x03u, 0x24u, 0xF0u, 0x00u, 0x02u, 0x24u, 0x21u, 0x20u, 0x00u, 0x00u, 0x08u, 0x00u, 0xA3u, 0xA4u,
    0x0Au, 0x00u, 0xA2u, 0xA4u, 0x02u, 0x00u, 0x22u, 0x96u, 0x01u, 0x00u, 0x13u, 0x24u, 0x0Cu, 0x00u, 0xA2u, 0xA4u,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x04u, 0x00u, 0x23u, 0x96u, 0xF8u, 0x43u, 0x52u, 0x24u, 0x58u, 0x44u, 0xC0u, 0xACu,
    0x3Au, 0x00u, 0x53u, 0xA6u, 0x3Cu, 0x00u, 0x40u, 0xA6u, 0x53u, 0xADu, 0x01u, 0x0Cu, 0x0Eu, 0x00u, 0xA3u, 0xA4u,
    0x4Au, 0xABu, 0x01u, 0x0Cu, 0x05u, 0x00u, 0x04u, 0x24u, 0x42u, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x06u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0xEAu, 0xFDu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x42u, 0x00u, 0x44u, 0x86u, 0x0Cu, 0xFEu, 0x03u, 0x0Cu, 0xFFu, 0xFFu, 0x84u, 0x24u, 0x16u, 0x80u, 0x04u, 0x3Cu,
    0xB8u, 0x4Cu, 0x84u, 0x24u, 0x18u, 0x80u, 0x02u, 0x3Cu, 0x1Cu, 0x15u, 0x04u, 0x0Cu, 0xB8u, 0x4Cu, 0x40u, 0xACu,
    0x21u, 0x20u, 0x00u, 0x00u, 0xC3u, 0xF0u, 0x01u, 0x0Cu, 0x3Eu, 0x00u, 0x40u, 0xA6u, 0x42u, 0x00u, 0x42u, 0x86u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x40u, 0x10u, 0x18u, 0x00u, 0xB0u, 0x27u, 0x3Au, 0x00u, 0x44u, 0x86u,
    0x38u, 0xFEu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x00u, 0xB0u, 0x27u, 0x21u, 0x20u, 0x00u, 0x02u,
    0x21u, 0x28u, 0x00u, 0x00u, 0x03u, 0x00u, 0x02u, 0x24u, 0xF0u, 0x00u, 0x11u, 0x24u, 0x21u, 0x30u, 0xA0u, 0x00u,
    0x80u, 0x01u, 0x07u, 0x24u, 0x50u, 0x00u, 0x53u, 0xA6u, 0x52u, 0x00u, 0x42u, 0xA6u, 0x43u, 0xEEu, 0x01u, 0x0Cu,
    0x10u, 0x00u, 0xB1u, 0xAFu, 0x09u, 0x80u, 0x03u, 0x3Cu, 0xF0u, 0x7Eu, 0x63u, 0x24u, 0x10u, 0x00u, 0x13u, 0xA2u,
    0x34u, 0x00u, 0x62u, 0x90u, 0x21u, 0x20u, 0x00u, 0x00u, 0x00u, 0x16u, 0x02u, 0x00u, 0x03u, 0x16u, 0x02u, 0x00u,
    0x08u, 0x00u, 0x02u, 0xA6u, 0x35u, 0x00u, 0x62u, 0x90u, 0x02u, 0x01u, 0x03u, 0x24u, 0x0Cu, 0x00u, 0x03u, 0xA6u,
    0x0Eu, 0x00u, 0x11u, 0xA6u, 0x00u, 0x16u, 0x02u, 0x00u, 0x03u, 0x16u, 0x02u, 0x00u, 0x70u, 0xA7u, 0x00u, 0x0Cu,
    0x0Au, 0x00u, 0x02u, 0xA6u, 0x6Au, 0x16u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xF5u, 0xF2u, 0x01u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x02u, 0x05u, 0x00u, 0x04u, 0x24u, 0x4Eu, 0x00u, 0x40u, 0xA6u, 0x4Au, 0xABu, 0x01u, 0x0Cu,
    0x38u, 0x00u, 0x40u, 0xA6u, 0x21u, 0x10u, 0x00u, 0x00u, 0x4Au, 0x00u, 0x53u, 0xA6u, 0x40u, 0x00u, 0xBFu, 0x8Fu,
    0x3Cu, 0x00u, 0xB3u, 0x8Fu, 0x38u, 0x00u, 0xB2u, 0x8Fu, 0x34u, 0x00u, 0xB1u, 0x8Fu, 0x30u, 0x00u, 0xB0u, 0x8Fu,
    0x48u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xB8u, 0xFFu, 0xBDu, 0x27u,
    0x44u, 0x00u, 0xBFu, 0xAFu, 0x40u, 0x00u, 0xB4u, 0xAFu, 0x3Cu, 0x00u, 0xB3u, 0xAFu, 0x38u, 0x00u, 0xB2u, 0xAFu,
    0x34u, 0x00u, 0xB1u, 0xAFu, 0xBFu, 0xADu, 0x01u, 0x0Cu, 0x30u, 0x00u, 0xB0u, 0xAFu, 0x56u, 0xB1u, 0x01u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x40u, 0x14u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xE2u, 0xF9u, 0x03u, 0x08u,
    0xFFu, 0xFFu, 0x02u, 0x24u, 0xF8u, 0x43u, 0x50u, 0x24u, 0x4Au, 0x00u, 0x02u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x0Au, 0x00u, 0x40u, 0x18u, 0x10u, 0x80u, 0x04u, 0x3Cu, 0x80u, 0x5Fu, 0x84u, 0x24u, 0x21u, 0x28u, 0x00u, 0x00u,
    0x21u, 0x30u, 0xA0u, 0x00u, 0x6Eu, 0xF1u, 0x01u, 0x0Cu, 0x21u, 0x38u, 0xA0u, 0x00u, 0x4Au, 0x00u, 0x02u, 0x96u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x42u, 0x24u, 0x4Au, 0x00u, 0x02u, 0xA6u, 0x50u, 0x00u, 0x02u, 0x96u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x42u, 0x24u, 0x00u, 0x14u, 0x02u, 0x00u, 0x03u, 0x1Cu, 0x02u, 0x00u,
    0x05u, 0x00u, 0x62u, 0x2Cu, 0xC3u, 0x01u, 0x40u, 0x10u, 0x0Cu, 0x80u, 0x02u, 0x3Cu, 0xC8u, 0x95u, 0x42u, 0x24u,
    0x80u, 0x18u, 0x03u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u, 0x00u, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x08u, 0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x51u, 0x24u,
    0x52u, 0x00u, 0x22u, 0x96u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x42u, 0x24u, 0x52u, 0x00u, 0x22u, 0xA6u,
    0x00u, 0x14u, 0x02u, 0x00u, 0x12u, 0x00u, 0x40u, 0x14u, 0x11u, 0x80u, 0x10u, 0x3Cu, 0x78u, 0x44u, 0x10u, 0x26u,
    0x21u, 0x20u, 0x00u, 0x02u, 0x11u, 0x80u, 0x05u, 0x3Cu, 0xEBu, 0xF9u, 0x03u, 0x0Cu, 0x58u, 0x44u, 0xA5u, 0x24u,
    0x21u, 0x20u, 0x20u, 0x02u, 0x10u, 0x80u, 0x05u, 0x3Cu, 0x61u, 0xFAu, 0x03u, 0x0Cu, 0x08u, 0xEAu, 0xA5u, 0x24u,
    0x70u, 0xA7u, 0x00u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0x6Au, 0x16u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0xFDu, 0xFAu, 0x03u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u, 0x02u, 0x00u, 0x02u, 0x24u, 0x50u, 0x00u, 0x22u, 0xA6u,
    0x70u, 0xA7u, 0x00u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0x6Au, 0x16u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0xE2u, 0xF9u, 0x03u, 0x08u, 0x21u, 0x10u, 0x00u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0x78u, 0x44u, 0x51u, 0x24u,
    0x10u, 0x00u, 0x22u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x51u, 0x00u,
    0x00u, 0x00u, 0x44u, 0x8Cu, 0xB1u, 0x15u, 0x04u, 0x0Cu, 0x03u, 0x00u, 0x05u, 0x24u, 0x3Cu, 0x00u, 0x25u, 0x86u,
    0x3Eu, 0x00u, 0x22u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x00u, 0xA2u, 0x00u, 0x12u, 0x00u, 0x22u, 0x86u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x51u, 0x00u, 0x08u, 0x00u, 0x44u, 0x8Cu,
    0x12u, 0x28u, 0x00u, 0x00u, 0xC2u, 0x17u, 0x05u, 0x00u, 0x21u, 0x28u, 0xA2u, 0x00u, 0xD0u, 0x15u, 0x04u, 0x0Cu,
    0x43u, 0x28u, 0x05u, 0x00u, 0xFDu, 0xFAu, 0x03u, 0x0Cu, 0x21u, 0x20u, 0x20u, 0x02u, 0x20u, 0x00u, 0x40u, 0x10u,
    0x21u, 0x20u, 0x20u, 0x02u, 0x58u, 0x15u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xFFu, 0x15u, 0x04u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x00u, 0xE6u, 0x0Au, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x20u, 0x00u, 0x00u,
    0x26u, 0xAFu, 0x01u, 0x0Cu, 0xFFu, 0xFFu, 0x05u, 0x24u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x50u, 0x24u,
    0x3Cu, 0x00u, 0x02u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x40u, 0x14u, 0x21u, 0x20u, 0x20u, 0x02u,
    0x98u, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x09u, 0x00u, 0x04u, 0x24u, 0x21u, 0x28u, 0x00u, 0x00u,
    0x62u, 0x43u, 0x02u, 0x0Cu, 0x21u, 0x30u, 0xA0u, 0x00u, 0x21u, 0x20u, 0x20u, 0x02u, 0x8Bu, 0xF6u, 0x03u, 0x0Cu,
    0x21u, 0x28u, 0x00u, 0x00u, 0x05u, 0x00u, 0x02u, 0x24u, 0x50u, 0x00u, 0x02u, 0xA6u, 0xFFu, 0xFFu, 0x02u, 0x24u,
    0x46u, 0x00u, 0x00u, 0xA6u, 0x44u, 0x00u, 0x00u, 0xA6u, 0xE1u, 0xF9u, 0x03u, 0x08u, 0x4Eu, 0x00u, 0x02u, 0xA6u,
    0x80u, 0xFBu, 0x03u, 0x0Cu, 0x21u, 0x28u, 0x00u, 0x00u, 0x0Bu, 0x80u, 0x13u, 0x3Cu, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0xF8u, 0x43u, 0x52u, 0x24u, 0x42u, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x40u, 0x10u,
    0x68u, 0xDDu, 0x74u, 0x26u, 0x3Au, 0x00u, 0x44u, 0x86u, 0x38u, 0xFEu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x18u, 0x00u, 0xB0u, 0x27u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0x14u, 0x00u, 0x22u, 0x86u, 0x58u, 0x44u, 0x63u, 0x24u,
    0x01u, 0x00u, 0x42u, 0x2Cu, 0x38u, 0x00u, 0x42u, 0xA6u, 0xC0u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x51u, 0x00u,
    0x18u, 0x00u, 0x45u, 0x84u, 0x1Au, 0x00u, 0x46u, 0x84u, 0x08u, 0x00u, 0x67u, 0x84u, 0x0Au, 0x00u, 0x62u, 0x84u,
    0x21u, 0x20u, 0x00u, 0x02u, 0x43u, 0xEEu, 0x01u, 0x0Cu, 0x10u, 0x00u, 0xA2u, 0xAFu, 0x01u, 0x00u, 0x02u, 0x24u,
    0x09u, 0x80u, 0x03u, 0x3Cu, 0xF0u, 0x7Eu, 0x63u, 0x24u, 0x11u, 0x00u, 0x02u, 0xA2u, 0x10u, 0x00u, 0x02u, 0xA2u,
    0x34u, 0x00u, 0x62u, 0x90u, 0x21u, 0x20u, 0x00u, 0x00u, 0x00u, 0x16u, 0x02u, 0x00u, 0x03u, 0x16u, 0x02u, 0x00u,
    0x08u, 0x00u, 0x02u, 0xA6u, 0x35u, 0x00u, 0x63u, 0x90u, 0x00u, 0x01u, 0x02u, 0x24u, 0x0Cu, 0x00u, 0x02u, 0xA6u,
    0xF0u, 0x00u, 0x02u, 0x24u, 0x0Eu, 0x00u, 0x02u, 0xA6u, 0x00u, 0x1Eu, 0x03u, 0x00u, 0x03u, 0x1Eu, 0x03u, 0x00u,
    0x70u, 0xA7u, 0x00u, 0x0Cu, 0x0Au, 0x00u, 0x03u, 0xA6u, 0x6Au, 0x16u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0xF5u, 0xF2u, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u, 0xC3u, 0xF0u, 0x01u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u,
    0x3Eu, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x40u, 0x10u, 0x03u, 0x00u, 0x02u, 0x24u,
    0xE1u, 0xF9u, 0x03u, 0x08u, 0x50u, 0x00u, 0x42u, 0xA6u, 0x68u, 0xDDu, 0x62u, 0x96u, 0x02u, 0x00u, 0x83u, 0x96u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x25u, 0x10u, 0x43u, 0x00u, 0x00u, 0x08u, 0x42u, 0x30u, 0x24u, 0x01u, 0x40u, 0x10u,
    0x21u, 0x10u, 0x00u, 0x00u, 0x58u, 0x15u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xFFu, 0x15u, 0x04u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x00u, 0xE6u, 0x0Au, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x20u, 0x00u, 0x00u,
    0x26u, 0xAFu, 0x01u, 0x0Cu, 0xFFu, 0xFFu, 0x05u, 0x24u, 0x3Cu, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x08u, 0x00u, 0x40u, 0x14u, 0x05u, 0x00u, 0x02u, 0x24u, 0x98u, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x09u, 0x00u, 0x04u, 0x24u, 0x21u, 0x28u, 0x00u, 0x00u, 0x62u, 0x43u, 0x02u, 0x0Cu, 0x21u, 0x30u, 0xA0u, 0x00u,
    0x05u, 0x00u, 0x02u, 0x24u, 0x50u, 0x00u, 0x42u, 0xA6u, 0xFEu, 0xFFu, 0x02u, 0x24u, 0x46u, 0x00u, 0x40u, 0xA6u,
    0x44u, 0x00u, 0x40u, 0xA6u, 0xE1u, 0xF9u, 0x03u, 0x08u, 0x4Eu, 0x00u, 0x42u, 0xA6u, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x78u, 0x44u, 0x52u, 0x24u, 0x10u, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x52u, 0x00u, 0x00u, 0x00u, 0x44u, 0x8Cu, 0xB1u, 0x15u, 0x04u, 0x0Cu, 0x03u, 0x00u, 0x05u, 0x24u,
    0x3Cu, 0x00u, 0x45u, 0x86u, 0x3Eu, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x00u, 0xA2u, 0x00u,
    0x12u, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x52u, 0x00u,
    0x08u, 0x00u, 0x44u, 0x8Cu, 0x12u, 0x28u, 0x00u, 0x00u, 0xC2u, 0x17u, 0x05u, 0x00u, 0x21u, 0x28u, 0xA2u, 0x00u,
    0xD0u, 0x15u, 0x04u, 0x0Cu, 0x43u, 0x28u, 0x05u, 0x00u, 0x21u, 0x20u, 0x40u, 0x02u, 0x10u, 0x00u, 0x42u, 0x86u,
    0x21u, 0x28u, 0x00u, 0x00u, 0x01u, 0x00u, 0x42u, 0x2Cu, 0x80u, 0xFBu, 0x03u, 0x0Cu, 0x10u, 0x00u, 0x42u, 0xA6u,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x51u, 0x24u, 0x42u, 0x00u, 0x22u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x04u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x3Au, 0x00u, 0x24u, 0x86u, 0x38u, 0xFEu, 0x03u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x58u, 0x15u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xFFu, 0x15u, 0x04u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x00u, 0xE6u, 0x0Au, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x44u, 0x00u, 0x22u, 0x96u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x42u, 0x24u, 0x02u, 0x00u, 0x42u, 0x2Cu, 0x2Cu, 0x00u, 0x40u, 0x10u,
    0x21u, 0x20u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x05u, 0x24u, 0x04u, 0x00u, 0x02u, 0x24u, 0x26u, 0xAFu, 0x01u, 0x0Cu,
    0x50u, 0x00u, 0x22u, 0xA6u, 0x3Cu, 0x00u, 0x22u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x40u, 0x14u,
    0x21u, 0x20u, 0x00u, 0x00u, 0x98u, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x09u, 0x00u, 0x04u, 0x24u,
    0x21u, 0x28u, 0x00u, 0x00u, 0x62u, 0x43u, 0x02u, 0x0Cu, 0x21u, 0x30u, 0xA0u, 0x00u, 0x21u, 0x20u, 0x00u, 0x00u,
    0xB4u, 0x00u, 0x05u, 0x24u, 0x38u, 0x00u, 0x30u, 0x86u, 0x18u, 0x00u, 0x42u, 0x26u, 0xC0u, 0x80u, 0x10u, 0x00u,
    0x26u, 0xAFu, 0x01u, 0x0Cu, 0x21u, 0x80u, 0x02u, 0x02u, 0x10u, 0x80u, 0x03u, 0x3Cu, 0x00u, 0x00u, 0x02u, 0x96u,
    0x88u, 0x5Fu, 0x64u, 0x24u, 0x88u, 0x5Fu, 0x62u, 0xA4u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xB8u, 0x44u, 0x52u, 0x24u,
    0x02u, 0x00u, 0x02u, 0x96u, 0x21u, 0x28u, 0x40u, 0x02u, 0xACu, 0xF1u, 0x01u, 0x0Cu, 0x02u, 0x00u, 0x82u, 0xA4u,
    0x44u, 0x00u, 0x23u, 0x86u, 0x02u, 0x00u, 0x02u, 0x24u, 0x04u, 0x00u, 0x62u, 0x14u, 0x7Cu, 0x00u, 0x02u, 0x24u,
    0x11u, 0x15u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x40u, 0x02u, 0x7Cu, 0x00u, 0x02u, 0x24u, 0x48u, 0x00u, 0x22u, 0xA6u,
    0x4Au, 0xABu, 0x01u, 0x0Cu, 0x06u, 0x00u, 0x04u, 0x24u, 0xE2u, 0xF9u, 0x03u, 0x08u, 0x21u, 0x10u, 0x00u, 0x00u,
    0x46u, 0x00u, 0x22u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x87u, 0x00u, 0x40u, 0x14u, 0x05u, 0x00u, 0x02u, 0x24u,
    0xB3u, 0xF9u, 0x03u, 0x08u, 0x21u, 0x20u, 0x00u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x43u, 0x24u,
    0x48u, 0x00u, 0x64u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0x00u, 0x82u, 0x28u, 0x04u, 0x00u, 0x40u, 0x10u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x44u, 0x00u, 0x65u, 0x84u, 0x48u, 0xF9u, 0x03u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x44u, 0x00u, 0x65u, 0x84u, 0x40u, 0x00u, 0x04u, 0x24u, 0xF0u, 0x14u, 0x04u, 0x0Cu, 0xFFu, 0xFFu, 0xA5u, 0x24u,
    0x21u, 0x80u, 0x00u, 0x00u, 0x15u, 0x80u, 0x02u, 0x3Cu, 0xB8u, 0xC8u, 0x53u, 0x24u, 0x10u, 0x80u, 0x02u, 0x3Cu,
    0x90u, 0x5Fu, 0x52u, 0x24u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x54u, 0x24u, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0xB8u, 0x44u, 0x51u, 0x24u, 0x21u, 0x20u, 0x20u, 0x02u, 0xD0u, 0x14u, 0x04u, 0x0Cu, 0x21u, 0x28u, 0x60u, 0x02u,
    0x21u, 0x20u, 0x40u, 0x02u, 0x21u, 0x28u, 0x60u, 0x02u, 0x00u, 0x3Cu, 0x31u, 0x26u, 0x00u, 0x31u, 0x10u, 0x00u,
    0x10u, 0x80u, 0x03u, 0x3Cu, 0x38u, 0x00u, 0x82u, 0x86u, 0x98u, 0x5Fu, 0x63u, 0x24u, 0x80u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0x00u, 0x00u, 0x42u, 0x94u, 0x01u, 0x00u, 0x10u, 0x26u, 0x21u, 0x10u, 0x46u, 0x00u,
    0x94u, 0xF1u, 0x01u, 0x0Cu, 0x02u, 0x00u, 0x42u, 0xA6u, 0x0Fu, 0x00u, 0x02u, 0x2Au, 0xEDu, 0xFFu, 0x40u, 0x14u,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x51u, 0x24u, 0x18u, 0x00u, 0xB0u, 0x27u, 0x11u, 0x80u, 0x03u, 0x3Cu,
    0x38u, 0x00u, 0x22u, 0x86u, 0x78u, 0x44u, 0x63u, 0x24u, 0x01u, 0x00u, 0x42u, 0x2Cu, 0x38u, 0x00u, 0x22u, 0xA6u,
    0xC0u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0x58u, 0x44u, 0x63u, 0x24u,
    0x18u, 0x00u, 0x45u, 0x84u, 0x1Au, 0x00u, 0x46u, 0x84u, 0x08u, 0x00u, 0x67u, 0x84u, 0x0Au, 0x00u, 0x62u, 0x84u,
    0x21u, 0x20u, 0x00u, 0x02u, 0x43u, 0xEEu, 0x01u, 0x0Cu, 0x10u, 0x00u, 0xA2u, 0xAFu, 0x01u, 0x00u, 0x02u, 0x24u,
    0x09u, 0x80u, 0x03u, 0x3Cu, 0xF0u, 0x7Eu, 0x63u, 0x24u, 0x11u, 0x00u, 0x02u, 0xA2u, 0x10u, 0x00u, 0x02u, 0xA2u,
    0x34u, 0x00u, 0x62u, 0x90u, 0x21u, 0x20u, 0x00u, 0x00u, 0x00u, 0x16u, 0x02u, 0x00u, 0x03u, 0x16u, 0x02u, 0x00u,
    0x08u, 0x00u, 0x02u, 0xA6u, 0x35u, 0x00u, 0x63u, 0x90u, 0x00u, 0x01u, 0x02u, 0x24u, 0x0Cu, 0x00u, 0x02u, 0xA6u,
    0xF0u, 0x00u, 0x02u, 0x24u, 0x0Eu, 0x00u, 0x02u, 0xA6u, 0x00u, 0x1Eu, 0x03u, 0x00u, 0x03u, 0x1Eu, 0x03u, 0x00u,
    0x70u, 0xA7u, 0x00u, 0x0Cu, 0x0Au, 0x00u, 0x03u, 0xA6u, 0x6Au, 0x16u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0xF5u, 0xF2u, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x68u, 0xDDu, 0x43u, 0x24u,
    0x68u, 0xDDu, 0x42u, 0x94u, 0x02u, 0x00u, 0x63u, 0x94u, 0x00u, 0x00u, 0x00u, 0x00u, 0x25u, 0x10u, 0x43u, 0x00u,
    0x00u, 0x08u, 0x42u, 0x30u, 0x13u, 0x00u, 0x40u, 0x10u, 0x21u, 0x20u, 0x00u, 0x00u, 0x26u, 0xAFu, 0x01u, 0x0Cu,
    0xFFu, 0xFFu, 0x05u, 0x24u, 0x3Cu, 0x00u, 0x22u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x40u, 0x14u,
    0x05u, 0x00u, 0x02u, 0x24u, 0x98u, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x09u, 0x00u, 0x04u, 0x24u,
    0x21u, 0x28u, 0x00u, 0x00u, 0x62u, 0x43u, 0x02u, 0x0Cu, 0x21u, 0x30u, 0xA0u, 0x00u, 0x05u, 0x00u, 0x02u, 0x24u,
    0x50u, 0x00u, 0x22u, 0xA6u, 0xFEu, 0xFFu, 0x02u, 0x24u, 0x44u, 0x00u, 0x20u, 0xA6u, 0xE1u, 0xF9u, 0x03u, 0x08u,
    0x4Eu, 0x00u, 0x22u, 0xA6u, 0x48u, 0x00u, 0x22u, 0x96u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFEu, 0xFFu, 0x42u, 0x24u,
    0x48u, 0x00u, 0x22u, 0xA6u, 0x00u, 0x14u, 0x02u, 0x00u, 0x31u, 0x00u, 0x41u, 0x04u, 0x21u, 0x10u, 0x00u, 0x00u,
    0x21u, 0x20u, 0x00u, 0x00u, 0x26u, 0xAFu, 0x01u, 0x0Cu, 0xFFu, 0xFFu, 0x05u, 0x24u, 0x3Cu, 0x00u, 0x22u, 0x86u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x40u, 0x14u, 0x05u, 0x00u, 0x02u, 0x24u, 0x98u, 0x3Du, 0x02u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x09u, 0x00u, 0x04u, 0x24u, 0x21u, 0x28u, 0x00u, 0x00u, 0x62u, 0x43u, 0x02u, 0x0Cu,
    0x21u, 0x30u, 0xA0u, 0x00u, 0x05u, 0x00u, 0x02u, 0x24u, 0x50u, 0x00u, 0x22u, 0xA6u, 0x01u, 0x00u, 0x02u, 0x24u,
    0xE1u, 0xF9u, 0x03u, 0x08u, 0x4Eu, 0x00u, 0x22u, 0xA6u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x50u, 0x24u,
    0x11u, 0x80u, 0x04u, 0x3Cu, 0x4Cu, 0x00u, 0x05u, 0x96u, 0x8Bu, 0xF6u, 0x03u, 0x0Cu, 0x78u, 0x44u, 0x84u, 0x24u,
    0x46u, 0x00u, 0x02u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Bu, 0x00u, 0x40u, 0x10u, 0x50u, 0x00u, 0x00u, 0xA6u,
    0x03u, 0x43u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u, 0x18u, 0x00u, 0x04u, 0x26u, 0x03u, 0x43u, 0x02u, 0x0Cu,
    0x21u, 0x80u, 0x40u, 0x00u, 0x21u, 0x20u, 0x00u, 0x02u, 0x49u, 0xADu, 0x01u, 0x0Cu, 0x21u, 0x28u, 0x40u, 0x00u,
    0xD9u, 0xF9u, 0x03u, 0x08u, 0x07u, 0x00u, 0x04u, 0x24u, 0x01u, 0x00u, 0x04u, 0x24u, 0x4Au, 0xABu, 0x01u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0x46u, 0x44u, 0x42u, 0x84u, 0xE2u, 0xF9u, 0x03u, 0x08u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x96u, 0xEBu, 0x01u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0x21u, 0x10u, 0x00u, 0x00u,
    0x44u, 0x00u, 0xBFu, 0x8Fu, 0x40u, 0x00u, 0xB4u, 0x8Fu, 0x3Cu, 0x00u, 0xB3u, 0x8Fu, 0x38u, 0x00u, 0xB2u, 0x8Fu,
    0x34u, 0x00u, 0xB1u, 0x8Fu, 0x30u, 0x00u, 0xB0u, 0x8Fu, 0x48u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x13u, 0x80u, 0x03u, 0x3Cu, 0xB8u, 0xBCu, 0x63u, 0x24u, 0x15u, 0x80u, 0x02u, 0x3Cu,
    0xB8u, 0x3Cu, 0x42u, 0x24u, 0x04u, 0x00u, 0x82u, 0xACu, 0x00u, 0x88u, 0x62u, 0x24u, 0x00u, 0x00u, 0x83u, 0xACu,
    0x00u, 0xC4u, 0x63u, 0x24u, 0x10u, 0x00u, 0x80u, 0xA4u, 0x08u, 0x00u, 0x82u, 0xACu, 0x0Cu, 0x00u, 0x83u, 0xACu,
    0x12u, 0x00u, 0x80u, 0xA4u, 0x14u, 0x00u, 0x80u, 0xA4u, 0x16u, 0x00u, 0x80u, 0xA4u, 0x18u, 0x00u, 0x80u, 0xA4u,
    0x1Au, 0x00u, 0x80u, 0xA4u, 0x08u, 0x00u, 0xA3u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0x10u, 0x03u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u,
    0x1Cu, 0x00u, 0x82u, 0xA4u, 0x0Au, 0x00u, 0xA3u, 0x94u, 0xF0u, 0x00u, 0x02u, 0x24u, 0x20u, 0x00u, 0x80u, 0xA4u,
    0x22u, 0x00u, 0x82u, 0xA4u, 0x1Eu, 0x00u, 0x83u, 0xA4u, 0x08u, 0x00u, 0xA3u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x40u, 0x10u, 0x03u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x43u, 0x10u, 0x02u, 0x00u, 0x24u, 0x00u, 0x82u, 0xA4u, 0x0Au, 0x00u, 0xA2u, 0x94u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x26u, 0x00u, 0x82u, 0xA4u, 0x08u, 0x00u, 0xA3u, 0x84u, 0x0Cu, 0x00u, 0xA2u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x23u, 0x18u, 0x62u, 0x00u, 0xC2u, 0x17u, 0x03u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u, 0x43u, 0x18u, 0x03u, 0x00u,
    0x40u, 0x10u, 0x03u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x43u, 0x10u, 0x02u, 0x00u, 0x28u, 0x00u, 0x82u, 0xA4u, 0x0Au, 0x00u, 0xA2u, 0x84u, 0x0Eu, 0x00u, 0xA3u, 0x84u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x23u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x43u, 0x10u, 0x02u, 0x00u, 0x2Au, 0x00u, 0x82u, 0xA4u, 0x0Cu, 0x00u, 0xA3u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x40u, 0x10u, 0x03u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x28u, 0x00u, 0x83u, 0x94u, 0x43u, 0x10u, 0x02u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u, 0x2Cu, 0x00u, 0x83u, 0xA4u,
    0x08u, 0x00u, 0xA3u, 0x84u, 0x0Cu, 0x00u, 0xA2u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u, 0x23u, 0x18u, 0x62u, 0x00u,
    0xC2u, 0x17u, 0x03u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u, 0x43u, 0x18u, 0x03u, 0x00u, 0x40u, 0x10u, 0x03u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u,
    0x30u, 0x00u, 0x82u, 0xA4u, 0x0Au, 0x00u, 0xA2u, 0x84u, 0x0Eu, 0x00u, 0xA3u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x23u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u,
    0xF0u, 0x00u, 0x42u, 0x24u, 0x32u, 0x00u, 0x82u, 0xA4u, 0x0Cu, 0x00u, 0xA3u, 0x84u, 0x18u, 0x00u, 0x02u, 0x24u,
    0x3Cu, 0x00u, 0x82u, 0xA4u, 0x40u, 0x10u, 0x03u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0xC2u, 0x1Fu, 0x02u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0x30u, 0x00u, 0x83u, 0x94u, 0x43u, 0x10u, 0x02u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u,
    0x14u, 0x00u, 0x82u, 0x84u, 0x34u, 0x00u, 0x83u, 0xA4u, 0x0Eu, 0x00u, 0xA3u, 0x94u, 0xC0u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x82u, 0x00u, 0x3Eu, 0x00u, 0x83u, 0xA4u, 0x28u, 0x00u, 0x43u, 0x94u, 0x14u, 0x00u, 0x82u, 0x84u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xC0u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x82u, 0x00u, 0x38u, 0x00u, 0x83u, 0xA4u,
    0x2Au, 0x00u, 0x42u, 0x94u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x3Au, 0x00u, 0x82u, 0xA4u, 0xD8u, 0xFFu, 0xBDu, 0x27u,
    0x1Cu, 0x00u, 0xB1u, 0xAFu, 0x21u, 0x88u, 0x80u, 0x00u, 0x18u, 0x00u, 0xB0u, 0xAFu, 0x21u, 0x80u, 0xA0u, 0x00u,
    0x20u, 0x00u, 0xBFu, 0xAFu, 0x58u, 0x15u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xFFu, 0x15u, 0x04u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x02u, 0x11u, 0x80u, 0x04u, 0x3Cu, 0xB8u, 0x44u, 0x84u, 0x24u, 0xC2u, 0x0Au, 0x02u, 0x0Cu,
    0x20u, 0x00u, 0x05u, 0x24u, 0x01u, 0x00u, 0x04u, 0x24u, 0x21u, 0x28u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x06u, 0x24u,
    0x21u, 0x38u, 0xA0u, 0x00u, 0xDAu, 0x12u, 0x02u, 0x0Cu, 0x10u, 0x00u, 0xA0u, 0xAFu, 0xCEu, 0x0Au, 0x02u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x20u, 0x20u, 0x02u, 0x16u, 0x45u, 0x02u, 0x0Cu, 0xC0u, 0x01u, 0x05u, 0x24u,
    0xFDu, 0xFFu, 0x40u, 0x10u, 0x21u, 0x20u, 0x20u, 0x02u, 0x20u, 0x00u, 0xBFu, 0x8Fu, 0x1Cu, 0x00u, 0xB1u, 0x8Fu,
    0x18u, 0x00u, 0xB0u, 0x8Fu, 0x28u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xE0u, 0xFFu, 0xBDu, 0x27u, 0x18u, 0x00u, 0xB0u, 0xAFu, 0x0Bu, 0x80u, 0x10u, 0x3Cu, 0x20u, 0x92u, 0x02u, 0x8Eu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x40u, 0x10u, 0x1Cu, 0x00u, 0xBFu, 0xAFu, 0x72u, 0x13u, 0x02u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x20u, 0x92u, 0x00u, 0xAEu, 0x11u, 0x80u, 0x02u, 0x3Cu, 0x78u, 0x44u, 0x46u, 0x24u,
    0x3Bu, 0x00u, 0xC2u, 0x88u, 0x38u, 0x00u, 0xC2u, 0x98u, 0x3Fu, 0x00u, 0xC3u, 0x88u, 0x3Cu, 0x00u, 0xC3u, 0x98u,
    0x13u, 0x00u, 0xA2u, 0xABu, 0x10u, 0x00u, 0xA2u, 0xBBu, 0x17u, 0x00u, 0xA3u, 0xABu, 0x14u, 0x00u, 0xA3u, 0xBBu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x4Cu, 0x44u, 0x42u, 0x84u, 0x12u, 0x00u, 0xD0u, 0x84u, 0x20u, 0x00u, 0x40u, 0x10u,
    0x21u, 0x28u, 0x00u, 0x00u, 0x14u, 0x00u, 0xA4u, 0x87u, 0x16u, 0x00u, 0xA2u, 0x87u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x18u, 0x00u, 0x82u, 0x00u, 0x80u, 0x10u, 0x10u, 0x00u, 0x21u, 0x10u, 0x46u, 0x00u, 0x00u, 0x21u, 0x04u, 0x00u,
    0x08u, 0x00u, 0x42u, 0x8Cu, 0x12u, 0x40u, 0x00u, 0x00u, 0x42u, 0x18u, 0x08u, 0x00u, 0x80u, 0x18u, 0x03u, 0x00u,
    0x08u, 0x00u, 0x80u, 0x18u, 0x21u, 0x18u, 0x43u, 0x00u, 0x00u, 0x00u, 0x60u, 0xACu, 0x14u, 0x00u, 0xA2u, 0x87u,
    0x01u, 0x00u, 0xA5u, 0x24u, 0x00u, 0x11u, 0x02u, 0x00u, 0x2Au, 0x10u, 0xA2u, 0x00u, 0xFAu, 0xFFu, 0x40u, 0x14u,
    0x04u, 0x00u, 0x63u, 0x24u, 0x16u, 0x00u, 0xA2u, 0x97u, 0x00u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x42u, 0x24u,
    0x16u, 0x00u, 0xA2u, 0xA7u, 0x00u, 0x14u, 0x02u, 0x00u, 0x03u, 0x14u, 0x02u, 0x00u, 0xF0u, 0x00u, 0x42u, 0x28u,
    0x04u, 0x00u, 0x40u, 0x14u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF0u, 0x00u, 0x02u, 0x24u, 0x16u, 0x00u, 0xA2u, 0xA7u,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x3Au, 0x44u, 0x42u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x40u, 0x10u,
    0x11u, 0x80u, 0x03u, 0x3Cu, 0x78u, 0x44u, 0x63u, 0x24u, 0x80u, 0x10u, 0x10u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x38u, 0x00u, 0x64u, 0x84u, 0x08u, 0x00u, 0x45u, 0x8Cu, 0x09u, 0xFFu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x78u, 0x44u, 0x44u, 0x24u, 0x12u, 0x00u, 0x82u, 0x84u, 0x3Cu, 0x00u, 0x85u, 0x94u,
    0x14u, 0x00u, 0x86u, 0x84u, 0x01u, 0x00u, 0x47u, 0x2Cu, 0xC0u, 0x18u, 0x06u, 0x00u, 0x38u, 0x00u, 0x82u, 0x94u,
    0x21u, 0x18u, 0x64u, 0x00u, 0x12u, 0x00u, 0x87u, 0xA4u, 0x21u, 0x10u, 0x45u, 0x00u, 0x38u, 0x00u, 0x82u, 0xA4u,
    0x00u, 0x14u, 0x02u, 0x00u, 0x2Cu, 0x00u, 0x63u, 0x84u, 0x03u, 0x14u, 0x02u, 0x00u, 0x2Au, 0x10u, 0x43u, 0x00u,
    0x0Eu, 0x00u, 0x40u, 0x10u, 0x00u, 0x2Cu, 0x05u, 0x00u, 0x3Eu, 0x00u, 0x82u, 0x84u, 0x03u, 0x2Cu, 0x05u, 0x00u,
    0x18u, 0x00u, 0xA2u, 0x00u, 0x80u, 0x10u, 0x07u, 0x00u, 0x21u, 0x10u, 0x44u, 0x00u, 0x08u, 0x00u, 0x44u, 0x8Cu,
    0x12u, 0x28u, 0x00u, 0x00u, 0xC2u, 0x17u, 0x05u, 0x00u, 0x21u, 0x28u, 0xA2u, 0x00u, 0xD0u, 0x15u, 0x04u, 0x0Cu,
    0x43u, 0x28u, 0x05u, 0x00u, 0xF2u, 0xFAu, 0x03u, 0x08u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0x01u, 0x00u, 0x02u, 0x24u,
    0x16u, 0x00u, 0x82u, 0xA4u, 0x01u, 0x00u, 0xC2u, 0x2Cu, 0x14u, 0x00u, 0x82u, 0xA4u, 0xC0u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x44u, 0x00u, 0x28u, 0x00u, 0x43u, 0x94u, 0x00u, 0x00u, 0x00u, 0x00u, 0x38u, 0x00u, 0x83u, 0xA4u,
    0x2Au, 0x00u, 0x42u, 0x94u, 0x00u, 0x00u, 0x00u, 0x00u, 0x3Au, 0x00u, 0x82u, 0xA4u, 0x11u, 0x80u, 0x03u, 0x3Cu,
    0x78u, 0x44u, 0x63u, 0x24u, 0x80u, 0x10u, 0x10u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x08u, 0x00u, 0x45u, 0x8Cu,
    0x94u, 0xF1u, 0x01u, 0x0Cu, 0x10u, 0x00u, 0xA4u, 0x27u, 0x1Cu, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xB0u, 0x8Fu,
    0x20u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xD8u, 0xFFu, 0xBDu, 0x27u,
    0x14u, 0x00u, 0xB1u, 0xAFu, 0x21u, 0x88u, 0x80u, 0x00u, 0xFFu, 0xFFu, 0x04u, 0x24u, 0x20u, 0x00u, 0xBFu, 0xAFu,
    0x1Cu, 0x00u, 0xB3u, 0xAFu, 0x18u, 0x00u, 0xB2u, 0xAFu, 0x6Au, 0x16u, 0x02u, 0x0Cu, 0x10u, 0x00u, 0xB0u, 0xAFu,
    0x21u, 0x90u, 0x40u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x53u, 0x24u, 0x2Eu, 0xFBu, 0x03u, 0x0Cu,
    0x21u, 0x20u, 0x20u, 0x02u, 0x21u, 0x80u, 0x40u, 0x00u, 0x0Du, 0x00u, 0x00u, 0x16u, 0x21u, 0x20u, 0x00u, 0x02u,
    0x6Au, 0x16u, 0x02u, 0x0Cu, 0xFFu, 0xFFu, 0x04u, 0x24u, 0x23u, 0x10u, 0x52u, 0x00u, 0x58u, 0x02u, 0x42u, 0x28u,
    0x13u, 0x00u, 0x40u, 0x10u, 0xFFu, 0xFFu, 0x02u, 0x24u, 0x3Cu, 0x00u, 0x62u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xF2u, 0xFFu, 0x40u, 0x10u, 0xFFu, 0xFFu, 0x02u, 0x24u, 0x26u, 0xFBu, 0x03u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x10u, 0x00u, 0x22u, 0x86u, 0x16u, 0x80u, 0x06u, 0x3Cu, 0x01u, 0x00u, 0x42u, 0x2Cu, 0x10u, 0x00u, 0x22u, 0xA6u,
    0x80u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x51u, 0x00u, 0x00u, 0x00u, 0x45u, 0x8Cu, 0x08u, 0x17u, 0x04u, 0x0Cu,
    0xB8u, 0x4Cu, 0xC6u, 0x24u, 0xFEu, 0x12u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u, 0x21u, 0x10u, 0x00u, 0x00u,
    0x20u, 0x00u, 0xBFu, 0x8Fu, 0x1Cu, 0x00u, 0xB3u, 0x8Fu, 0x18u, 0x00u, 0xB2u, 0x8Fu, 0x14u, 0x00u, 0xB1u, 0x8Fu,
    0x10u, 0x00u, 0xB0u, 0x8Fu, 0x28u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xD8u, 0xFFu, 0xBDu, 0x27u, 0xFFu, 0xFFu, 0x04u, 0x24u, 0x20u, 0x00u, 0xBFu, 0xAFu, 0x1Cu, 0x00u, 0xB1u, 0xAFu,
    0x6Au, 0x16u, 0x02u, 0x0Cu, 0x18u, 0x00u, 0xB0u, 0xAFu, 0x21u, 0x88u, 0x40u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0xF8u, 0x43u, 0x50u, 0x24u, 0x10u, 0x00u, 0xA4u, 0x27u, 0x3Au, 0x13u, 0x02u, 0x0Cu, 0x14u, 0x00u, 0xA5u, 0x27u,
    0x13u, 0x00u, 0x40u, 0x10u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0xD3u, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x10u, 0x00u, 0x42u, 0x30u, 0x02u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u, 0x3Cu, 0x00u, 0x02u, 0xA6u,
    0x6Au, 0x16u, 0x02u, 0x0Cu, 0xFFu, 0xFFu, 0x04u, 0x24u, 0x23u, 0x10u, 0x51u, 0x00u, 0x58u, 0x02u, 0x42u, 0x28u,
    0x33u, 0x00u, 0x40u, 0x10u, 0x21u, 0x10u, 0x00u, 0x00u, 0x3Cu, 0x00u, 0x02u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xEDu, 0xFFu, 0x40u, 0x10u, 0x10u, 0x00u, 0xA4u, 0x27u, 0x7Au, 0xFBu, 0x03u, 0x08u, 0x21u, 0x10u, 0x00u, 0x00u,
    0x14u, 0x00u, 0xA2u, 0x8Fu, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x50u, 0x8Cu, 0x5Cu, 0x44u, 0x62u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x2Bu, 0x10u, 0x02u, 0x02u, 0x06u, 0x00u, 0x40u, 0x10u, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x32u, 0x44u, 0x42u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2Au, 0x10u, 0x02u, 0x02u, 0x12u, 0x00u, 0x40u, 0x10u,
    0x11u, 0x80u, 0x03u, 0x3Cu, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x44u, 0x24u, 0x40u, 0x00u, 0x82u, 0x84u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u, 0xCEu, 0x0Au, 0x02u, 0x0Cu,
    0x21u, 0x88u, 0x80u, 0x00u, 0x21u, 0x20u, 0x20u, 0x02u, 0x16u, 0x45u, 0x02u, 0x0Cu, 0xC0u, 0x01u, 0x05u, 0x24u,
    0xFDu, 0xFFu, 0x40u, 0x10u, 0x21u, 0x20u, 0x20u, 0x02u, 0x6Cu, 0xFBu, 0x03u, 0x08u, 0x11u, 0x80u, 0x03u, 0x3Cu,
    0x3Eu, 0x00u, 0x82u, 0xA4u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0x18u, 0x80u, 0x05u, 0x3Cu, 0x10u, 0x00u, 0xA2u, 0x8Fu,
    0x14u, 0x00u, 0xA6u, 0x8Fu, 0xB8u, 0x4Cu, 0xA7u, 0x24u, 0x32u, 0x44u, 0x70u, 0xA4u, 0xB8u, 0x4Cu, 0xA3u, 0x8Cu,
    0x08u, 0x00u, 0xC4u, 0x8Cu, 0x01u, 0x00u, 0x63u, 0x24u, 0x26u, 0x18u, 0x64u, 0x00u, 0x2Bu, 0x18u, 0x03u, 0x00u,
    0x04u, 0x00u, 0xE3u, 0xACu, 0x08u, 0x00u, 0xC3u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0xB8u, 0x4Cu, 0xA3u, 0xACu,
    0x20u, 0x00u, 0xBFu, 0x8Fu, 0x1Cu, 0x00u, 0xB1u, 0x8Fu, 0x18u, 0x00u, 0xB0u, 0x8Fu, 0x28u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xB0u, 0xAFu,
    0x21u, 0x80u, 0x80u, 0x00u, 0xFFu, 0xFFu, 0x04u, 0x24u, 0x1Cu, 0x00u, 0xBFu, 0xAFu, 0x18u, 0x00u, 0xB2u, 0xAFu,
    0x6Au, 0x16u, 0x02u, 0x0Cu, 0x14u, 0x00u, 0xB1u, 0xAFu, 0x16u, 0x00u, 0x03u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x21u, 0x00u, 0x60u, 0x14u, 0x21u, 0x88u, 0x40u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x43u, 0x52u, 0x24u,
    0x6Au, 0x16u, 0x02u, 0x0Cu, 0xFFu, 0xFFu, 0x04u, 0x24u, 0x23u, 0x10u, 0x51u, 0x00u, 0x58u, 0x02u, 0x42u, 0x28u,
    0x05u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x3Cu, 0x00u, 0x42u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x11u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x02u, 0x86u, 0x01u, 0x00u, 0x03u, 0x24u,
    0x16u, 0x00u, 0x03u, 0xA6u, 0x01u, 0x00u, 0x42u, 0x2Cu, 0x14u, 0x00u, 0x02u, 0xA6u, 0xC0u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x02u, 0x02u, 0x28u, 0x00u, 0x43u, 0x94u, 0x14u, 0x00u, 0x02u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xC0u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x02u, 0x02u, 0x38u, 0x00u, 0x03u, 0xA6u, 0x2Au, 0x00u, 0x42u, 0x94u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x3Au, 0x00u, 0x02u, 0xA6u, 0x16u, 0x00u, 0x02u, 0x86u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xE3u, 0xFFu, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0xA6u, 0x1Cu, 0x00u, 0xBFu, 0x8Fu,
    0x18u, 0x00u, 0xB2u, 0x8Fu, 0x14u, 0x00u, 0xB1u, 0x8Fu, 0x10u, 0x00u, 0xB0u, 0x8Fu, 0x20u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x90u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xA5u, 0x27u,
    0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x30u, 0x96u, 0x46u, 0x24u, 0x0Cu, 0x80u, 0x03u, 0x3Cu, 0x30u, 0x96u, 0x42u, 0x90u,
    0x38u, 0x96u, 0x63u, 0x24u, 0x6Cu, 0x00u, 0xBFu, 0xAFu, 0x08u, 0x00u, 0x40u, 0x10u, 0x68u, 0x00u, 0xB0u, 0xAFu,
    0x00u, 0x00u, 0xC2u, 0x90u, 0x01u, 0x00u, 0xC6u, 0x24u, 0x00u, 0x00u, 0xA2u, 0xA0u, 0x00u, 0x00u, 0xC2u, 0x90u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xFAu, 0xFFu, 0x40u, 0x14u, 0x01u, 0x00u, 0xA5u, 0x24u, 0x00u, 0x00u, 0x82u, 0x90u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x84u, 0x24u,
    0x00u, 0x00u, 0xA2u, 0xA0u, 0x00u, 0x00u, 0x82u, 0x90u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFBu, 0xFFu, 0x40u, 0x14u,
    0x01u, 0x00u, 0xA5u, 0x24u, 0x00u, 0x00u, 0x62u, 0x90u, 0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x40u, 0x10u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x63u, 0x24u, 0x00u, 0x00u, 0xA2u, 0xA0u, 0x00u, 0x00u, 0x62u, 0x90u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xFBu, 0xFFu, 0x40u, 0x14u, 0x01u, 0x00u, 0xA5u, 0x24u, 0x00u, 0x00u, 0xA0u, 0xA0u,
    0x50u, 0x00u, 0xB0u, 0x27u, 0x21u, 0x20u, 0x00u, 0x02u, 0x56u, 0x45u, 0x02u, 0x0Cu, 0x10u, 0x00u, 0xA5u, 0x27u,
    0x03u, 0x43u, 0x02u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u, 0x6Cu, 0x00u, 0xBFu, 0x8Fu, 0x68u, 0x00u, 0xB0u, 0x8Fu,
    0x70u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u,
    0x10u, 0x00u, 0xBFu, 0xAFu, 0x1Eu, 0xF7u, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0xBFu, 0x8Fu,
    0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u,
    0x10u, 0x00u, 0xBFu, 0xAFu, 0xF9u, 0xF7u, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0xBFu, 0x8Fu,
    0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Bu, 0x80u, 0x02u, 0x3Cu,
    0x10u, 0xDFu, 0x43u, 0x94u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x90u, 0xDCu, 0x40u, 0xA0u, 0x03u, 0x00u, 0x62u, 0x28u,
    0x10u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Eu, 0x00u, 0x60u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x40u, 0x10u, 0x03u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x00u, 0x11u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0xC0u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x80u, 0x10u, 0x02u, 0x00u, 0x23u, 0x10u, 0x43u, 0x00u,
    0x80u, 0x10u, 0x02u, 0x00u, 0x0Au, 0x80u, 0x03u, 0x3Cu, 0x9Cu, 0x79u, 0x63u, 0x24u, 0x10u, 0xFCu, 0x03u, 0x08u,
    0x21u, 0x18u, 0x43u, 0x00u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x01u, 0x00u, 0x02u, 0x24u, 0x90u, 0xDCu, 0x62u, 0xA0u,
    0x2Fu, 0xFCu, 0x03u, 0x08u, 0x21u, 0x10u, 0x00u, 0x00u, 0x14u, 0x00u, 0x62u, 0x84u, 0x14u, 0x00u, 0x63u, 0x94u,
    0x03u, 0x00u, 0x41u, 0x04u, 0x83u, 0x28u, 0x02u, 0x00u, 0x03u, 0x00u, 0x42u, 0x24u, 0x83u, 0x28u, 0x02u, 0x00u,
    0x15u, 0x00u, 0xA2u, 0x2Cu, 0x03u, 0x00u, 0x40u, 0x14u, 0x03u, 0x00u, 0x64u, 0x30u, 0x21u, 0x20u, 0x00u, 0x00u,
    0x21u, 0x28u, 0x80u, 0x00u, 0x0Cu, 0x80u, 0x03u, 0x3Cu, 0xDCu, 0x95u, 0x63u, 0x24u, 0x80u, 0x10u, 0x05u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0x21u, 0x10u, 0x44u, 0x00u, 0x00u, 0x00u, 0x44u, 0x90u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x02u, 0x00u, 0x81u, 0x04u, 0x10u, 0x00u, 0x02u, 0x24u, 0x21u, 0x20u, 0x00u, 0x00u, 0x09u, 0x00u, 0xA2u, 0x14u,
    0x21u, 0x10u, 0x80u, 0x00u, 0x09u, 0x80u, 0x02u, 0x3Cu, 0xFCu, 0x7Eu, 0x42u, 0x8Cu, 0x01u, 0x00u, 0x03u, 0x3Cu,
    0x24u, 0x10u, 0x43u, 0x00u, 0x03u, 0x00u, 0x40u, 0x10u, 0x21u, 0x10u, 0x80u, 0x00u, 0x18u, 0x00u, 0x04u, 0x24u,
    0x21u, 0x10u, 0x80u, 0x00u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xB8u, 0xFFu, 0xBDu, 0x27u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0xFCu, 0xDEu, 0x43u, 0x8Cu, 0x0Bu, 0x80u, 0x04u, 0x3Cu, 0x40u, 0x00u, 0xB2u, 0xAFu,
    0x64u, 0xDFu, 0x92u, 0x8Cu, 0x44u, 0x00u, 0xBFu, 0xAFu, 0x3Cu, 0x00u, 0xB1u, 0xAFu, 0x38u, 0x00u, 0xB0u, 0xAFu,
    0x80u, 0x10u, 0x03u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x00u, 0x13u, 0x02u, 0x00u, 0x18u, 0x80u, 0x03u, 0x3Cu,
    0xE0u, 0x4Du, 0x63u, 0x24u, 0x21u, 0x10u, 0x43u, 0x00u, 0x64u, 0xDFu, 0x82u, 0xACu, 0x0Bu, 0x80u, 0x02u, 0x3Cu,
    0x08u, 0xDFu, 0x43u, 0x24u, 0x0Bu, 0x80u, 0x04u, 0x3Cu, 0x08u, 0xDFu, 0x45u, 0x94u, 0x02u, 0x00u, 0x62u, 0x94u,
    0x24u, 0xE2u, 0x83u, 0x84u, 0x25u, 0x88u, 0xA2u, 0x00u, 0x0Bu, 0x00u, 0x62u, 0x2Cu, 0x95u, 0x01u, 0x40u, 0x10u,
    0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x90u, 0x96u, 0x42u, 0x24u, 0x80u, 0x18u, 0x03u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u,
    0x00u, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x21u, 0x56u, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xA2u, 0x2Du, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0x0Bu, 0x80u, 0x11u, 0x3Cu, 0x90u, 0xDCu, 0x30u, 0x26u, 0x03u, 0x00u, 0x02u, 0x24u, 0xD6u, 0x03u, 0x04u, 0x0Cu,
    0x01u, 0x00u, 0x02u, 0xA2u, 0x21u, 0x20u, 0x00u, 0x00u, 0x57u, 0x08u, 0x04u, 0x0Cu, 0x0Cu, 0x00u, 0x02u, 0xA6u,
    0xF5u, 0xFBu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x90u, 0xDCu, 0x23u, 0x92u, 0x04u, 0x00u, 0x02u, 0xA2u,
    0x37u, 0x00u, 0x60u, 0x10u, 0x06u, 0x00u, 0x02u, 0xA2u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x01u, 0x00u, 0x02u, 0x24u,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0x10u, 0x80u, 0x04u, 0x3Cu, 0xA0u, 0x5Fu, 0x84u, 0x24u, 0x11u, 0x80u, 0x05u, 0x3Cu,
    0xDBu, 0x43u, 0xA5u, 0x24u, 0xFCu, 0xFFu, 0x02u, 0x24u, 0x90u, 0x32u, 0x01u, 0x0Cu, 0x24u, 0x28u, 0xA2u, 0x00u,
    0xB9u, 0x0Au, 0x04u, 0x0Cu, 0x04u, 0x00u, 0x04u, 0x24u, 0xB9u, 0x0Au, 0x04u, 0x0Cu, 0x05u, 0x00u, 0x04u, 0x24u,
    0xB9u, 0x0Au, 0x04u, 0x0Cu, 0x06u, 0x00u, 0x04u, 0x24u, 0xB9u, 0x0Au, 0x04u, 0x0Cu, 0x07u, 0x00u, 0x04u, 0x24u,
    0xE2u, 0xFDu, 0x03u, 0x08u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0xFAu, 0x07u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0xD3u, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x42u, 0x30u, 0x05u, 0x00u, 0x40u, 0x10u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x53u, 0xADu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0x24u, 0xFDu, 0x03u, 0x08u,
    0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x5Fu, 0x08u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x18u, 0x40u, 0x00u,
    0x06u, 0x00u, 0x62u, 0x2Cu, 0x27u, 0x00u, 0x40u, 0x10u, 0x0Cu, 0x80u, 0x02u, 0x3Cu, 0xC0u, 0x96u, 0x42u, 0x24u,
    0x80u, 0x18u, 0x03u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u, 0x00u, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x08u, 0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x0Au, 0x00u, 0x02u, 0x24u,
    0xE1u, 0xFDu, 0x03u, 0x08u, 0x24u, 0xE2u, 0x62u, 0xA4u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x90u, 0xDCu, 0x44u, 0x24u,
    0x90u, 0xDCu, 0x43u, 0x90u, 0x01u, 0x00u, 0x02u, 0x24u, 0x05u, 0x00u, 0x62u, 0x14u, 0xFEu, 0xFFu, 0x62u, 0x24u,
    0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x02u, 0x00u, 0x02u, 0x24u, 0xE1u, 0xFDu, 0x03u, 0x08u, 0x24u, 0xE2u, 0x62u, 0xA4u,
    0x02u, 0x00u, 0x42u, 0x2Cu, 0x42u, 0x01u, 0x40u, 0x10u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x05u, 0x00u, 0x84u, 0x90u,
    0x65u, 0xACu, 0x01u, 0x0Cu, 0x21u, 0x28u, 0x00u, 0x00u, 0xE2u, 0xFDu, 0x03u, 0x08u, 0x0Bu, 0x80u, 0x02u, 0x3Cu,
    0x53u, 0xADu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xE2u, 0xFDu, 0x03u, 0x08u, 0x0Bu, 0x80u, 0x02u, 0x3Cu,
    0x3Cu, 0x00u, 0x04u, 0x24u, 0xB2u, 0xADu, 0x01u, 0x0Cu, 0x21u, 0x28u, 0x00u, 0x00u, 0xE2u, 0xFDu, 0x03u, 0x08u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x96u, 0xEBu, 0x01u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0x0Bu, 0x80u, 0x02u, 0x3Cu,
    0x94u, 0xDCu, 0x44u, 0x90u, 0x1Eu, 0xF7u, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x56u, 0xB1u, 0x01u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x40u, 0x10u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0xF9u, 0xF7u, 0x03u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0xB5u, 0xFCu, 0x03u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x90u, 0xDCu, 0x42u, 0x90u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x40u, 0x10u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x01u, 0x00u, 0x02u, 0x24u,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0xB9u, 0x0Au, 0x04u, 0x0Cu, 0x06u, 0x00u, 0x04u, 0x24u, 0xE2u, 0xFDu, 0x03u, 0x08u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0xBBu, 0x0Bu, 0x04u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0xA5u, 0xD3u, 0x01u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x04u, 0x24u, 0x85u, 0xABu, 0x01u, 0x0Cu, 0x01u, 0x00u, 0x05u, 0x24u,
    0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x03u, 0x00u, 0x02u, 0x24u, 0xE1u, 0xFDu, 0x03u, 0x08u, 0x24u, 0xE2u, 0x62u, 0xA4u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0xFCu, 0xDEu, 0x50u, 0x8Cu, 0x11u, 0x80u, 0x02u, 0x3Cu, 0xD8u, 0x43u, 0x42u, 0x24u,
    0x00u, 0x81u, 0x10u, 0x00u, 0x21u, 0x80u, 0x02u, 0x02u, 0x24u, 0xEFu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x21u, 0x28u, 0x00u, 0x02u, 0x06u, 0x00u, 0x00u, 0xA2u, 0x05u, 0x00u, 0x00u, 0xA2u,
    0x04u, 0x00u, 0x00u, 0xA2u, 0x18u, 0x92u, 0x44u, 0x8Cu, 0x70u, 0x01u, 0x02u, 0x24u, 0x0Cu, 0x00u, 0xA2u, 0xA4u,
    0xE0u, 0x01u, 0x02u, 0x24u, 0x08u, 0x00u, 0xA0u, 0xA4u, 0x0Au, 0x00u, 0xA0u, 0xA4u, 0x9Du, 0xEEu, 0x01u, 0x0Cu,
    0x0Eu, 0x00u, 0xA2u, 0xA4u, 0xBBu, 0x0Bu, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0xF8u, 0x00u, 0x40u, 0x10u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x64u, 0x40u, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0xE2u, 0xFDu, 0x03u, 0x08u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x53u, 0xADu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0x08u, 0x00u, 0x04u, 0x24u,
    0x3Eu, 0x43u, 0x02u, 0x0Cu, 0x21u, 0x28u, 0x00u, 0x00u, 0x57u, 0x08u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x05u, 0x00u, 0x02u, 0x24u, 0x24u, 0xE2u, 0x62u, 0xA4u, 0xF0u, 0x08u, 0x22u, 0x32u,
    0x03u, 0x00u, 0x40u, 0x10u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x06u, 0x00u, 0x02u, 0x24u, 0x24u, 0xE2u, 0x62u, 0xA4u,
    0xFAu, 0x07u, 0x04u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0xD3u, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x22u, 0xFDu, 0x03u, 0x08u, 0x10u, 0x00u, 0x42u, 0x30u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x9Cu, 0xDCu, 0x42u, 0x84u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x40u, 0x10u, 0x04u, 0x00u, 0x04u, 0x24u, 0x0Cu, 0xFDu, 0x03u, 0x08u,
    0x02u, 0x00u, 0x04u, 0x24u, 0x04u, 0x00u, 0x04u, 0x24u, 0xFAu, 0x07u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0xBCu, 0x3Du, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x03u, 0x24u, 0xD0u, 0x00u, 0x43u, 0x14u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x08u, 0x00u, 0x02u, 0x24u, 0xE1u, 0xFDu, 0x03u, 0x08u,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0x36u, 0x49u, 0x02u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x18u, 0x40u, 0x00u,
    0x04u, 0x00u, 0x02u, 0x24u, 0x05u, 0x00u, 0x62u, 0x14u, 0xFFu, 0xFFu, 0x62u, 0x24u, 0x0Bu, 0x80u, 0x03u, 0x3Cu,
    0x09u, 0x00u, 0x02u, 0x24u, 0xE1u, 0xFDu, 0x03u, 0x08u, 0x24u, 0xE2u, 0x62u, 0xA4u, 0x02u, 0x00u, 0x42u, 0x2Cu,
    0xBEu, 0x00u, 0x40u, 0x10u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x06u, 0x00u, 0x02u, 0x24u, 0xE1u, 0xFDu, 0x03u, 0x08u,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0xFAu, 0x07u, 0x04u, 0x0Cu, 0x03u, 0x00u, 0x04u, 0x24u, 0x20u, 0x00u, 0xA4u, 0x27u,
    0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu, 0x40u, 0x96u, 0xA5u, 0x24u, 0x0Cu, 0x00u, 0x40u, 0x10u,
    0x01u, 0x00u, 0x11u, 0x24u, 0x2Cu, 0x00u, 0xA2u, 0x8Fu, 0x30u, 0x00u, 0xA3u, 0x8Fu, 0x34u, 0x00u, 0xA4u, 0x8Fu,
    0x10u, 0x00u, 0xA2u, 0xAFu, 0x14u, 0x00u, 0xA3u, 0xAFu, 0x18u, 0x00u, 0xA4u, 0xAFu, 0x20u, 0x00u, 0xA5u, 0x8Fu,
    0x24u, 0x00u, 0xA6u, 0x8Fu, 0x28u, 0x00u, 0xA7u, 0x8Fu, 0x4Fu, 0xFDu, 0x03u, 0x08u, 0x21u, 0x20u, 0x00u, 0x00u,
    0x20u, 0x00u, 0xA4u, 0x27u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu, 0x50u, 0x96u, 0xA5u, 0x24u,
    0x16u, 0x00u, 0x40u, 0x10u, 0x20u, 0x00u, 0xA4u, 0x27u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu,
    0x60u, 0x96u, 0xA5u, 0x24u, 0x11u, 0x00u, 0x40u, 0x10u, 0x02u, 0x00u, 0x11u, 0x24u, 0x2Cu, 0x00u, 0xA2u, 0x8Fu,
    0x30u, 0x00u, 0xA3u, 0x8Fu, 0x34u, 0x00u, 0xA4u, 0x8Fu, 0x10u, 0x00u, 0xA2u, 0xAFu, 0x14u, 0x00u, 0xA3u, 0xAFu,
    0x18u, 0x00u, 0xA4u, 0xAFu, 0x01u, 0x00u, 0x04u, 0x24u, 0x20u, 0x00u, 0xA5u, 0x8Fu, 0x24u, 0x00u, 0xA6u, 0x8Fu,
    0x28u, 0x00u, 0xA7u, 0x8Fu, 0x6Cu, 0xABu, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x37u, 0xABu, 0x01u, 0x0Cu,
    0x21u, 0x20u, 0x20u, 0x02u, 0x5Eu, 0xFDu, 0x03u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0xA4u, 0x27u,
    0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu, 0x78u, 0x96u, 0xA5u, 0x24u, 0x04u, 0x00u, 0x40u, 0x10u,
    0xFFu, 0xFFu, 0x11u, 0x24u, 0x37u, 0xABu, 0x01u, 0x0Cu, 0x03u, 0x00u, 0x04u, 0x24u, 0x03u, 0x00u, 0x11u, 0x24u,
    0x57u, 0x08u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x90u, 0xDCu, 0x43u, 0x24u,
    0x0Cu, 0x00u, 0x62u, 0x84u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Eu, 0x00u, 0x40u, 0x10u, 0xFFu, 0xFFu, 0x22u, 0x26u,
    0x03u, 0x00u, 0x42u, 0x2Cu, 0x17u, 0x00u, 0x40u, 0x10u, 0x40u, 0x80u, 0x11u, 0x00u, 0xFEu, 0xFFu, 0x04u, 0x26u,
    0x01u, 0x00u, 0x71u, 0xA0u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x01u, 0x00u, 0x02u, 0x24u, 0xB9u, 0x0Au, 0x04u, 0x0Cu,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0xB9u, 0x0Au, 0x04u, 0x0Cu, 0xFFu, 0xFFu, 0x04u, 0x26u, 0xE2u, 0xFDu, 0x03u, 0x08u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x03u, 0x00u, 0x02u, 0x24u, 0x0Au, 0x00u, 0x22u, 0x16u, 0x04u, 0x00u, 0x04u, 0x24u,
    0x01u, 0x00u, 0x71u, 0xA0u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x01u, 0x00u, 0x02u, 0x24u, 0xB9u, 0x0Au, 0x04u, 0x0Cu,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0xB9u, 0x0Au, 0x04u, 0x0Cu, 0x05u, 0x00u, 0x04u, 0x24u, 0xE2u, 0xFDu, 0x03u, 0x08u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x04u, 0x00u, 0x02u, 0x24u, 0xE1u, 0xFDu, 0x03u, 0x08u,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0xFCu, 0xDEu, 0x50u, 0x8Cu, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0xD8u, 0x43u, 0x42u, 0x24u, 0x00u, 0x81u, 0x10u, 0x00u, 0x21u, 0x80u, 0x02u, 0x02u, 0x24u, 0xEFu, 0x01u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x02u, 0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x21u, 0x28u, 0x00u, 0x02u, 0x06u, 0x00u, 0x00u, 0xA2u,
    0x05u, 0x00u, 0x00u, 0xA2u, 0x04u, 0x00u, 0x00u, 0xA2u, 0x18u, 0x92u, 0x44u, 0x8Cu, 0x70u, 0x01u, 0x02u, 0x24u,
    0x0Cu, 0x00u, 0xA2u, 0xA4u, 0xE0u, 0x01u, 0x02u, 0x24u, 0x08u, 0x00u, 0xA0u, 0xA4u, 0x0Au, 0x00u, 0xA0u, 0xA4u,
    0x9Du, 0xEEu, 0x01u, 0x0Cu, 0x0Eu, 0x00u, 0xA2u, 0xA4u, 0x53u, 0xADu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0x20u, 0x00u, 0xA4u, 0x27u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu, 0x40u, 0x96u, 0xA5u, 0x24u,
    0x10u, 0x00u, 0x40u, 0x10u, 0x20u, 0x00u, 0xA4u, 0x27u, 0x2Cu, 0x00u, 0xA2u, 0x8Fu, 0x30u, 0x00u, 0xA3u, 0x8Fu,
    0x34u, 0x00u, 0xA4u, 0x8Fu, 0x10u, 0x00u, 0xA2u, 0xAFu, 0x14u, 0x00u, 0xA3u, 0xAFu, 0x18u, 0x00u, 0xA4u, 0xAFu,
    0x20u, 0x00u, 0xA5u, 0x8Fu, 0x24u, 0x00u, 0xA6u, 0x8Fu, 0x28u, 0x00u, 0xA7u, 0x8Fu, 0x6Cu, 0xABu, 0x01u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x00u, 0x37u, 0xABu, 0x01u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0xD0u, 0xFDu, 0x03u, 0x08u,
    0x01u, 0x00u, 0x03u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu, 0x50u, 0x96u, 0xA5u, 0x24u,
    0x15u, 0x00u, 0x40u, 0x10u, 0x20u, 0x00u, 0xA4u, 0x27u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu,
    0x60u, 0x96u, 0xA5u, 0x24u, 0x10u, 0x00u, 0x40u, 0x10u, 0x20u, 0x00u, 0xA4u, 0x27u, 0x2Cu, 0x00u, 0xA2u, 0x8Fu,
    0x30u, 0x00u, 0xA3u, 0x8Fu, 0x34u, 0x00u, 0xA4u, 0x8Fu, 0x10u, 0x00u, 0xA2u, 0xAFu, 0x14u, 0x00u, 0xA3u, 0xAFu,
    0x18u, 0x00u, 0xA4u, 0xAFu, 0x20u, 0x00u, 0xA5u, 0x8Fu, 0x24u, 0x00u, 0xA6u, 0x8Fu, 0x28u, 0x00u, 0xA7u, 0x8Fu,
    0x6Cu, 0xABu, 0x01u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0x37u, 0xABu, 0x01u, 0x0Cu, 0x02u, 0x00u, 0x04u, 0x24u,
    0xD0u, 0xFDu, 0x03u, 0x08u, 0x02u, 0x00u, 0x03u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x56u, 0x45u, 0x02u, 0x0Cu,
    0x78u, 0x96u, 0xA5u, 0x24u, 0x04u, 0x00u, 0x40u, 0x10u, 0xFFu, 0xFFu, 0x03u, 0x24u, 0x37u, 0xABu, 0x01u, 0x0Cu,
    0x03u, 0x00u, 0x04u, 0x24u, 0x03u, 0x00u, 0x03u, 0x24u, 0x03u, 0x00u, 0x02u, 0x24u, 0x05u, 0x00u, 0x62u, 0x14u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x3Eu, 0x3Eu, 0x01u, 0x0Cu, 0x04u, 0x00u, 0x04u, 0x24u, 0xE2u, 0xFDu, 0x03u, 0x08u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0xFAu, 0x07u, 0x04u, 0x0Cu, 0x05u, 0x00u, 0x04u, 0x24u, 0xF0u, 0x08u, 0x22u, 0x32u,
    0x06u, 0x00u, 0x40u, 0x10u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x07u, 0x00u, 0x02u, 0x24u, 0xE1u, 0xFDu, 0x03u, 0x08u,
    0x24u, 0xE2u, 0x62u, 0xA4u, 0x96u, 0xEBu, 0x01u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0x0Bu, 0x80u, 0x02u, 0x3Cu,
    0x64u, 0xDFu, 0x52u, 0xACu, 0x44u, 0x00u, 0xBFu, 0x8Fu, 0x40u, 0x00u, 0xB2u, 0x8Fu, 0x3Cu, 0x00u, 0xB1u, 0x8Fu,
    0x38u, 0x00u, 0xB0u, 0x8Fu, 0x48u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x18u, 0x80u, 0x02u, 0x3Cu, 0xFFu, 0xFFu, 0x03u, 0x24u, 0xC0u, 0x4Cu, 0x43u, 0xA4u, 0xC0u, 0x4Cu, 0x42u, 0x24u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x02u, 0x00u, 0x43u, 0xA4u, 0x18u, 0x80u, 0x02u, 0x3Cu, 0xC0u, 0x4Cu, 0x44u, 0xA4u,
    0x17u, 0x00u, 0x80u, 0x04u, 0xC0u, 0x4Cu, 0x4Au, 0x24u, 0x0Cu, 0x80u, 0x03u, 0x3Cu, 0xD8u, 0x96u, 0x63u, 0x24u,
    0x80u, 0x10u, 0x04u, 0x00u, 0x21u, 0x10u, 0x44u, 0x00u, 0x80u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x00u, 0x00u, 0x43u, 0x8Cu, 0x04u, 0x00u, 0x44u, 0x94u, 0x06u, 0x00u, 0x45u, 0x94u, 0x08u, 0x00u, 0x46u, 0x94u,
    0x0Au, 0x00u, 0x47u, 0x94u, 0x0Cu, 0x00u, 0x48u, 0x94u, 0x0Eu, 0x00u, 0x49u, 0x94u, 0x10u, 0x00u, 0x42u, 0x94u,
    0x04u, 0x00u, 0x43u, 0xADu, 0x08u, 0x00u, 0x44u, 0xA5u, 0x0Au, 0x00u, 0x45u, 0xA5u, 0x0Cu, 0x00u, 0x46u, 0xA5u,
    0x0Eu, 0x00u, 0x47u, 0xA5u, 0x10u, 0x00u, 0x48u, 0xA5u, 0x12u, 0x00u, 0x49u, 0xA5u, 0x14u, 0x00u, 0x42u, 0xA5u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x14u, 0x00u, 0xBFu, 0xAFu,
    0x07u, 0x00u, 0x81u, 0x04u, 0x10u, 0x00u, 0xB0u, 0xAFu, 0x18u, 0x80u, 0x04u, 0x3Cu, 0xC0u, 0x4Cu, 0x83u, 0x24u,
    0xFFu, 0xFFu, 0x02u, 0x24u, 0x02u, 0x00u, 0x62u, 0xA4u, 0x33u, 0xFEu, 0x03u, 0x08u, 0xC0u, 0x4Cu, 0x82u, 0xA4u,
    0x21u, 0x28u, 0x00u, 0x00u, 0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x8Cu, 0x97u, 0x43u, 0x24u, 0x00u, 0x00u, 0x62u, 0x94u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x40u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x84u, 0x24u,
    0x06u, 0x00u, 0x80u, 0x04u, 0x18u, 0x80u, 0x10u, 0x3Cu, 0x01u, 0x00u, 0xA5u, 0x24u, 0x1Eu, 0x00u, 0xA2u, 0x2Cu,
    0xF6u, 0xFFu, 0x40u, 0x14u, 0x04u, 0x00u, 0x63u, 0x24u, 0x18u, 0x80u, 0x10u, 0x3Cu, 0xC0u, 0x4Cu, 0x02u, 0x26u,
    0x02u, 0x00u, 0x45u, 0xA4u, 0x09u, 0x00u, 0x80u, 0x04u, 0xC0u, 0x4Cu, 0x05u, 0xA6u, 0x0Cu, 0x80u, 0x03u, 0x3Cu,
    0x8Cu, 0x97u, 0x63u, 0x24u, 0x00u, 0x14u, 0x05u, 0x00u, 0x83u, 0x13u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x02u, 0x00u, 0x44u, 0x84u, 0xF0u, 0xFDu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x02u, 0x24u,
    0xC0u, 0x4Cu, 0x02u, 0xA6u, 0x14u, 0x00u, 0xBFu, 0x8Fu, 0x10u, 0x00u, 0xB0u, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x80u, 0x02u, 0x3Cu, 0xC2u, 0x4Cu, 0x45u, 0x84u,
    0xE8u, 0xFFu, 0xBDu, 0x27u, 0x19u, 0x00u, 0x80u, 0x18u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0x17u, 0x00u, 0xA0u, 0x04u,
    0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x8Cu, 0x97u, 0x42u, 0x24u, 0x80u, 0x18u, 0x05u, 0x00u, 0x21u, 0x18u, 0x62u, 0x00u,
    0x00u, 0x00u, 0x62u, 0x94u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2Au, 0x10u, 0x44u, 0x00u, 0x07u, 0x00u, 0x40u, 0x10u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x63u, 0x24u, 0x00u, 0x00u, 0x62u, 0x94u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x2Au, 0x10u, 0x44u, 0x00u, 0xFBu, 0xFFu, 0x40u, 0x14u, 0x01u, 0x00u, 0xA5u, 0x24u, 0x0Cu, 0x80u, 0x03u, 0x3Cu,
    0x8Cu, 0x97u, 0x63u, 0x24u, 0xFFu, 0xFFu, 0xA2u, 0x24u, 0x80u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x02u, 0x00u, 0x44u, 0x84u, 0xF0u, 0xFDu, 0x03u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0xBFu, 0x8Fu,
    0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x24u,
    0x0Fu, 0x00u, 0xE2u, 0x10u, 0x03u, 0x00u, 0xE2u, 0x28u, 0x05u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x08u, 0x00u, 0xE0u, 0x10u, 0x21u, 0x70u, 0x00u, 0x00u, 0x70u, 0xFEu, 0x03u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x03u, 0x00u, 0x02u, 0x24u, 0x09u, 0x00u, 0xE2u, 0x10u, 0x21u, 0x70u, 0x00u, 0x00u, 0x70u, 0xFEu, 0x03u, 0x08u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x6Fu, 0xFEu, 0x03u, 0x08u, 0x04u, 0x98u, 0x4Bu, 0x24u,
    0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x6Fu, 0xFEu, 0x03u, 0x08u, 0x84u, 0x98u, 0x4Bu, 0x24u, 0x0Cu, 0x80u, 0x02u, 0x3Cu,
    0x04u, 0x99u, 0x4Bu, 0x24u, 0x21u, 0x70u, 0x00u, 0x00u, 0x96u, 0x00u, 0xC0u, 0x18u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x21u, 0x68u, 0x00u, 0x00u, 0x0Bu, 0x00u, 0xA7u, 0x24u, 0x00u, 0x00u, 0x83u, 0x94u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xFFu, 0xFFu, 0x63u, 0x30u, 0x42u, 0x12u, 0x03u, 0x00u, 0x78u, 0x00u, 0x4Cu, 0x30u, 0x42u, 0x11u, 0x03u, 0x00u,
    0x78u, 0x00u, 0x4Au, 0x30u, 0x42u, 0x10u, 0x03u, 0x00u, 0x78u, 0x00u, 0x49u, 0x30u, 0xC0u, 0x18u, 0x03u, 0x00u,
    0x78u, 0x00u, 0x62u, 0x30u, 0x1Eu, 0x00u, 0x40u, 0x10u, 0x02u, 0x00u, 0x84u, 0x24u, 0x21u, 0x40u, 0x4Bu, 0x00u,
    0x03u, 0x00u, 0x02u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x40u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x02u, 0x95u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xA2u, 0xA4u, 0x02u, 0x00u, 0x02u, 0x91u,
    0x9Eu, 0xFEu, 0x03u, 0x08u, 0xF7u, 0xFFu, 0xE2u, 0xA0u, 0x00u, 0x00u, 0xA2u, 0x90u, 0x00u, 0x00u, 0x03u, 0x91u,
    0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u, 0x00u, 0x00u, 0xA2u, 0xA0u,
    0xF6u, 0xFFu, 0xE2u, 0x90u, 0x01u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x43u, 0x10u, 0x02u, 0x00u, 0xF6u, 0xFFu, 0xE2u, 0xA0u, 0xF7u, 0xFFu, 0xE2u, 0x90u, 0x02u, 0x00u, 0x03u, 0x91u,
    0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u, 0xF7u, 0xFFu, 0xE2u, 0xA0u,
    0x20u, 0x00u, 0x20u, 0x11u, 0x21u, 0x40u, 0x2Bu, 0x01u, 0x03u, 0x00u, 0x02u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x0Au, 0x00u, 0x40u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xF8u, 0xFFu, 0xE2u, 0xA0u, 0x01u, 0x00u, 0x02u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u, 0xF9u, 0xFFu, 0xE2u, 0xA0u,
    0x02u, 0x00u, 0x02u, 0x91u, 0xBFu, 0xFEu, 0x03u, 0x08u, 0xFAu, 0xFFu, 0xE2u, 0xA0u, 0xF8u, 0xFFu, 0xE2u, 0x90u,
    0x00u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u,
    0xF8u, 0xFFu, 0xE2u, 0xA0u, 0xF9u, 0xFFu, 0xE2u, 0x90u, 0x01u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u, 0xF9u, 0xFFu, 0xE2u, 0xA0u, 0xFAu, 0xFFu, 0xE2u, 0x90u,
    0x02u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u,
    0xFAu, 0xFFu, 0xE2u, 0xA0u, 0x1Du, 0x00u, 0x40u, 0x11u, 0x21u, 0x40u, 0x4Bu, 0x01u, 0x03u, 0x00u, 0x02u, 0x91u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x40u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x95u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xFBu, 0xFFu, 0xE2u, 0xA4u, 0x02u, 0x00u, 0x02u, 0x91u, 0xDDu, 0xFEu, 0x03u, 0x08u,
    0xFDu, 0xFFu, 0xE2u, 0xA0u, 0xFBu, 0xFFu, 0xE2u, 0x90u, 0x00u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u, 0xFBu, 0xFFu, 0xE2u, 0xA0u, 0xFCu, 0xFFu, 0xE2u, 0x90u,
    0x01u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u,
    0xFCu, 0xFFu, 0xE2u, 0xA0u, 0xFDu, 0xFFu, 0xE2u, 0x90u, 0x02u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u, 0xFDu, 0xFFu, 0xE2u, 0xA0u, 0x20u, 0x00u, 0x80u, 0x11u,
    0x21u, 0x40u, 0x8Bu, 0x01u, 0x03u, 0x00u, 0x02u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x40u, 0x14u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFEu, 0xFFu, 0xE2u, 0xA0u,
    0x01u, 0x00u, 0x02u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0xE2u, 0xA0u, 0x02u, 0x00u, 0x02u, 0x91u,
    0xFEu, 0xFEu, 0x03u, 0x08u, 0x00u, 0x00u, 0xE2u, 0xA0u, 0xFEu, 0xFFu, 0xE2u, 0x90u, 0x00u, 0x00u, 0x03u, 0x91u,
    0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u, 0xFEu, 0xFFu, 0xE2u, 0xA0u,
    0xFFu, 0xFFu, 0xE2u, 0x90u, 0x01u, 0x00u, 0x03u, 0x91u, 0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u,
    0x43u, 0x10u, 0x02u, 0x00u, 0xFFu, 0xFFu, 0xE2u, 0xA0u, 0x00u, 0x00u, 0xE2u, 0x90u, 0x02u, 0x00u, 0x03u, 0x91u,
    0x42u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x43u, 0x00u, 0x43u, 0x10u, 0x02u, 0x00u, 0x00u, 0x00u, 0xE2u, 0xA0u,
    0x0Cu, 0x00u, 0xE7u, 0x24u, 0x04u, 0x00u, 0xADu, 0x25u, 0x10u, 0x00u, 0xA2u, 0x29u, 0x72u, 0xFFu, 0x40u, 0x14u,
    0x0Cu, 0x00u, 0xA5u, 0x24u, 0x01u, 0x00u, 0xCEu, 0x25u, 0x2Au, 0x10u, 0xC6u, 0x01u, 0x6Cu, 0xFFu, 0x40u, 0x14u,
    0x78u, 0x00u, 0x84u, 0x24u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u,
    0x21u, 0x40u, 0xA0u, 0x00u, 0x18u, 0x80u, 0x02u, 0x3Cu, 0xC0u, 0x4Cu, 0x43u, 0x84u, 0xC0u, 0x4Cu, 0x47u, 0x24u,
    0x24u, 0x00u, 0x60u, 0x04u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0x55u, 0x55u, 0x03u, 0x3Cu, 0x56u, 0x55u, 0x63u, 0x34u,
    0x40u, 0x10u, 0x04u, 0x00u, 0x18u, 0x00u, 0x43u, 0x00u, 0xC3u, 0x17u, 0x02u, 0x00u, 0x10u, 0x48u, 0x00u, 0x00u,
    0x23u, 0x20u, 0x22u, 0x01u, 0x08u, 0x00u, 0xE2u, 0x84u, 0x0Cu, 0x00u, 0xE3u, 0x84u, 0x23u, 0x20u, 0x82u, 0x00u,
    0x2Au, 0x10u, 0x83u, 0x00u, 0x17u, 0x00u, 0x40u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Eu, 0x00u, 0xE2u, 0x84u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x10u, 0x62u, 0x00u, 0x2Au, 0x10u, 0x82u, 0x00u, 0x11u, 0x00u, 0x40u, 0x10u,
    0x11u, 0x80u, 0x03u, 0x3Cu, 0x82u, 0x20u, 0x04u, 0x00u, 0x40u, 0x20u, 0x04u, 0x00u, 0x04u, 0x00u, 0xE2u, 0x8Cu,
    0xE0u, 0xE1u, 0x63u, 0x8Cu, 0x10u, 0x00u, 0xE6u, 0x84u, 0x42u, 0x10u, 0x02u, 0x00u, 0x40u, 0x10u, 0x02u, 0x00u,
    0x21u, 0x18u, 0x62u, 0x00u, 0x21u, 0x20u, 0x64u, 0x00u, 0x0Au, 0x00u, 0xE2u, 0x84u, 0x14u, 0x00u, 0xE7u, 0x84u,
    0x40u, 0x28u, 0x02u, 0x00u, 0x21u, 0x28u, 0xA2u, 0x00u, 0x00u, 0x29u, 0x05u, 0x00u, 0x59u, 0xFEu, 0x03u, 0x0Cu,
    0x21u, 0x28u, 0x05u, 0x01u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xC8u, 0xFFu, 0xBDu, 0x27u, 0x1Cu, 0x00u, 0xB1u, 0xAFu, 0x21u, 0x88u, 0x80u, 0x00u,
    0x30u, 0x00u, 0xB6u, 0xAFu, 0x14u, 0x00u, 0x16u, 0x3Cu, 0x0Cu, 0x00u, 0xD6u, 0x36u, 0x00u, 0x65u, 0x06u, 0x3Cu,
    0x2Cu, 0x00u, 0xB5u, 0xAFu, 0x0Bu, 0x80u, 0x15u, 0x3Cu, 0x28u, 0x00u, 0xB4u, 0xAFu, 0x0Bu, 0x80u, 0x14u, 0x3Cu,
    0xFFu, 0xFFu, 0x27u, 0x32u, 0x24u, 0x00u, 0xB3u, 0xAFu, 0x00u, 0x9Cu, 0x05u, 0x00u, 0x64u, 0xDFu, 0xA3u, 0x8Eu,
    0x25u, 0x38u, 0xF3u, 0x00u, 0x20u, 0x00u, 0xB2u, 0xAFu, 0x0Cu, 0x80u, 0x12u, 0x3Cu, 0x18u, 0x00u, 0xB0u, 0xAFu,
    0x18u, 0x92u, 0x90u, 0x8Eu, 0x00u, 0xB7u, 0x42u, 0x8Eu, 0x00u, 0xB7u, 0x52u, 0x26u, 0x34u, 0x00u, 0xBFu, 0xAFu,
    0x10u, 0x00u, 0xB6u, 0xAFu, 0x1Cu, 0x00u, 0x10u, 0x26u, 0x21u, 0x20u, 0x00u, 0x02u, 0x21u, 0x28u, 0x60u, 0x00u,
    0x6Fu, 0x36u, 0x01u, 0x0Cu, 0x14u, 0x00u, 0xA2u, 0xAFu, 0x21u, 0x20u, 0x00u, 0x02u, 0x21u, 0x28u, 0x40u, 0x00u,
    0xAAu, 0x36u, 0x01u, 0x0Cu, 0x1Fu, 0x00u, 0x06u, 0x24u, 0x21u, 0x18u, 0x40u, 0x00u, 0x21u, 0x28u, 0x60u, 0x00u,
    0x00u, 0x65u, 0x06u, 0x3Cu, 0x12u, 0x00u, 0x27u, 0x26u, 0x18u, 0x92u, 0x90u, 0x8Eu, 0xFFu, 0xFFu, 0xE7u, 0x30u,
    0x10u, 0x00u, 0xB6u, 0xAFu, 0x04u, 0x00u, 0x42u, 0x8Eu, 0x25u, 0x38u, 0xF3u, 0x00u, 0x64u, 0xDFu, 0xA5u, 0xAEu,
    0x1Cu, 0x00u, 0x10u, 0x26u, 0x21u, 0x20u, 0x00u, 0x02u, 0x6Fu, 0x36u, 0x01u, 0x0Cu, 0x14u, 0x00u, 0xA2u, 0xAFu,
    0x21u, 0x20u, 0x00u, 0x02u, 0x21u, 0x28u, 0x40u, 0x00u, 0xAAu, 0x36u, 0x01u, 0x0Cu, 0x1Fu, 0x00u, 0x06u, 0x24u,
    0x21u, 0x28u, 0x40u, 0x00u, 0x00u, 0x65u, 0x06u, 0x3Cu, 0x24u, 0x00u, 0x27u, 0x26u, 0x18u, 0x92u, 0x90u, 0x8Eu,
    0xFFu, 0xFFu, 0xE7u, 0x30u, 0x10u, 0x00u, 0xB6u, 0xAFu, 0x08u, 0x00u, 0x42u, 0x8Eu, 0x25u, 0x38u, 0xF3u, 0x00u,
    0x64u, 0xDFu, 0xA5u, 0xAEu, 0x1Cu, 0x00u, 0x10u, 0x26u, 0x21u, 0x20u, 0x00u, 0x02u, 0x6Fu, 0x36u, 0x01u, 0x0Cu,
    0x14u, 0x00u, 0xA2u, 0xAFu, 0x21u, 0x20u, 0x00u, 0x02u, 0x21u, 0x28u, 0x40u, 0x00u, 0xAAu, 0x36u, 0x01u, 0x0Cu,
    0x1Fu, 0x00u, 0x06u, 0x24u, 0x21u, 0x28u, 0x40u, 0x00u, 0x00u, 0x65u, 0x06u, 0x3Cu, 0x50u, 0x00u, 0x31u, 0x26u,
    0x18u, 0x92u, 0x90u, 0x8Eu, 0xFFu, 0xFFu, 0x31u, 0x32u, 0x10u, 0x00u, 0xB6u, 0xAFu, 0x0Cu, 0x00u, 0x42u, 0x8Eu,
    0x25u, 0x38u, 0x33u, 0x02u, 0x64u, 0xDFu, 0xA5u, 0xAEu, 0x1Cu, 0x00u, 0x10u, 0x26u, 0x21u, 0x20u, 0x00u, 0x02u,
    0x6Fu, 0x36u, 0x01u, 0x0Cu, 0x14u, 0x00u, 0xA2u, 0xAFu, 0x21u, 0x20u, 0x00u, 0x02u, 0x21u, 0x28u, 0x40u, 0x00u,
    0xAAu, 0x36u, 0x01u, 0x0Cu, 0x1Fu, 0x00u, 0x06u, 0x24u, 0x64u, 0xDFu, 0xA2u, 0xAEu, 0x34u, 0x00u, 0xBFu, 0x8Fu,
    0x30u, 0x00u, 0xB6u, 0x8Fu, 0x2Cu, 0x00u, 0xB5u, 0x8Fu, 0x28u, 0x00u, 0xB4u, 0x8Fu, 0x24u, 0x00u, 0xB3u, 0x8Fu,
    0x20u, 0x00u, 0xB2u, 0x8Fu, 0x1Cu, 0x00u, 0xB1u, 0x8Fu, 0x18u, 0x00u, 0xB0u, 0x8Fu, 0x38u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0xC0u, 0x10u, 0x05u, 0x00u,
    0x23u, 0x10u, 0x45u, 0x00u, 0x80u, 0x10u, 0x02u, 0x00u, 0x21u, 0x10u, 0x44u, 0x00u, 0x10u, 0x00u, 0xBFu, 0xAFu,
    0x10u, 0x00u, 0x45u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xA3u, 0x90u, 0x30u, 0x00u, 0x02u, 0x24u,
    0x25u, 0x00u, 0x62u, 0x14u, 0x21u, 0x20u, 0x00u, 0x00u, 0x09u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x7Eu, 0x42u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x43u, 0x30u, 0x2Bu, 0x20u, 0x03u, 0x00u, 0x08u, 0x00u, 0x03u, 0x3Cu,
    0x24u, 0x10u, 0x43u, 0x00u, 0x02u, 0x00u, 0x40u, 0x10u, 0x02u, 0x00u, 0x02u, 0x24u, 0x02u, 0x00u, 0x84u, 0x34u,
    0x10u, 0x00u, 0x82u, 0x10u, 0x03u, 0x00u, 0x82u, 0x28u, 0x05u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u,
    0x08u, 0x00u, 0x82u, 0x10u, 0x18u, 0x80u, 0x04u, 0x3Cu, 0x3Bu, 0x00u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x03u, 0x00u, 0x02u, 0x24u, 0x0Cu, 0x00u, 0x82u, 0x10u, 0x18u, 0x80u, 0x04u, 0x3Cu, 0x3Bu, 0x00u, 0x04u, 0x08u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u,
    0x40u, 0xABu, 0xA5u, 0x24u, 0x18u, 0x80u, 0x04u, 0x3Cu, 0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu,
    0x39u, 0x00u, 0x04u, 0x08u, 0xE4u, 0xB0u, 0xA5u, 0x24u, 0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu,
    0x39u, 0x00u, 0x04u, 0x08u, 0x10u, 0xB7u, 0xA5u, 0x24u, 0x31u, 0x00u, 0x02u, 0x24u, 0x29u, 0x00u, 0x62u, 0x14u,
    0x32u, 0x00u, 0x02u, 0x24u, 0x09u, 0x80u, 0x02u, 0x3Cu, 0xF0u, 0x7Eu, 0x43u, 0x24u, 0x08u, 0x00u, 0x62u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x42u, 0x30u, 0x02u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x01u, 0x00u, 0x04u, 0x24u, 0x0Cu, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x42u, 0x30u,
    0x02u, 0x00u, 0x40u, 0x10u, 0x02u, 0x00u, 0x02u, 0x24u, 0x02u, 0x00u, 0x84u, 0x34u, 0x10u, 0x00u, 0x82u, 0x10u,
    0x03u, 0x00u, 0x82u, 0x28u, 0x05u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u, 0x08u, 0x00u, 0x82u, 0x10u,
    0x18u, 0x80u, 0x04u, 0x3Cu, 0x3Bu, 0x00u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x02u, 0x24u,
    0x0Cu, 0x00u, 0x82u, 0x10u, 0x18u, 0x80u, 0x04u, 0x3Cu, 0x3Bu, 0x00u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u, 0xBCu, 0xAAu, 0xA5u, 0x24u,
    0x18u, 0x80u, 0x04u, 0x3Cu, 0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u,
    0x28u, 0xB7u, 0xA5u, 0x24u, 0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u,
    0x30u, 0xB7u, 0xA5u, 0x24u, 0x1Du, 0x00u, 0x62u, 0x14u, 0x33u, 0x00u, 0x02u, 0x24u, 0x09u, 0x80u, 0x02u, 0x3Cu,
    0xF0u, 0x7Eu, 0x45u, 0x24u, 0x08u, 0x00u, 0xA2u, 0x8Cu, 0x01u, 0x00u, 0x03u, 0x3Cu, 0x24u, 0x10u, 0x43u, 0x00u,
    0x02u, 0x00u, 0x40u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x04u, 0x24u, 0x0Cu, 0x00u, 0xA2u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x10u, 0x43u, 0x00u, 0x02u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u,
    0x02u, 0x00u, 0x84u, 0x34u, 0x05u, 0x00u, 0x82u, 0x14u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x18u, 0x80u, 0x04u, 0x3Cu,
    0x08u, 0x4Du, 0x84u, 0x24u, 0x39u, 0x00u, 0x04u, 0x08u, 0x3Cu, 0xB7u, 0xA5u, 0x24u, 0x31u, 0x00u, 0x80u, 0x18u,
    0x04u, 0x00u, 0x82u, 0x28u, 0x2Fu, 0x00u, 0x40u, 0x10u, 0x18u, 0x80u, 0x04u, 0x3Cu, 0x08u, 0x4Du, 0x84u, 0x24u,
    0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u, 0xF8u, 0xB0u, 0xA5u, 0x24u, 0x25u, 0x00u, 0x62u, 0x14u,
    0x09u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x7Eu, 0x43u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0x62u, 0x30u,
    0x02u, 0x00u, 0x40u, 0x10u, 0x10u, 0x00u, 0x02u, 0x3Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0x24u, 0x10u, 0x62u, 0x00u,
    0x02u, 0x00u, 0x40u, 0x10u, 0x02u, 0x00u, 0x02u, 0x24u, 0x02u, 0x00u, 0x84u, 0x34u, 0x10u, 0x00u, 0x82u, 0x10u,
    0x03u, 0x00u, 0x82u, 0x28u, 0x05u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u, 0x08u, 0x00u, 0x82u, 0x10u,
    0x18u, 0x80u, 0x04u, 0x3Cu, 0x3Bu, 0x00u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x02u, 0x24u,
    0x0Cu, 0x00u, 0x82u, 0x10u, 0x18u, 0x80u, 0x04u, 0x3Cu, 0x3Bu, 0x00u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u, 0x04u, 0xB1u, 0xA5u, 0x24u,
    0x18u, 0x80u, 0x04u, 0x3Cu, 0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u,
    0xD8u, 0xB0u, 0xA5u, 0x24u, 0x08u, 0x4Du, 0x84u, 0x24u, 0x0Cu, 0x80u, 0x05u, 0x3Cu, 0x39u, 0x00u, 0x04u, 0x08u,
    0x4Cu, 0xB7u, 0xA5u, 0x24u, 0x18u, 0x80u, 0x04u, 0x3Cu, 0x08u, 0x4Du, 0x84u, 0x24u, 0x02u, 0x33u, 0x01u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xC8u, 0xFFu, 0xBDu, 0x27u, 0x28u, 0x00u, 0xB6u, 0xAFu, 0x21u, 0xB0u, 0x80u, 0x00u,
    0x2Cu, 0x00u, 0xB7u, 0xAFu, 0x21u, 0xB8u, 0xA0u, 0x00u, 0x30u, 0x00u, 0xBEu, 0xAFu, 0x0Bu, 0x80u, 0x1Eu, 0x3Cu,
    0x40u, 0x18u, 0x06u, 0x00u, 0x21u, 0x18u, 0x66u, 0x00u, 0x80u, 0x18u, 0x03u, 0x00u, 0x0Cu, 0x80u, 0x02u, 0x3Cu,
    0x84u, 0x99u, 0x42u, 0x24u, 0x24u, 0x00u, 0xB5u, 0xAFu, 0x64u, 0xDFu, 0xD5u, 0x8Fu, 0x21u, 0x18u, 0x62u, 0x00u,
    0x34u, 0x00u, 0xBFu, 0xAFu, 0x20u, 0x00u, 0xB4u, 0xAFu, 0x1Cu, 0x00u, 0xB3u, 0xAFu, 0x18u, 0x00u, 0xB2u, 0xAFu,
    0x14u, 0x00u, 0xB1u, 0xAFu, 0x2Cu, 0x00u, 0xC0u, 0x04u, 0x10u, 0x00u, 0xB0u, 0xAFu, 0x21u, 0x20u, 0xA0u, 0x02u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x00u, 0x00u, 0x70u, 0x94u, 0x18u, 0x92u, 0x54u, 0x8Cu, 0x02u, 0x00u, 0x72u, 0x84u,
    0x04u, 0x00u, 0x71u, 0x84u, 0x06u, 0x00u, 0x73u, 0x84u, 0x00u, 0x84u, 0x10u, 0x00u, 0x10u, 0xEFu, 0x01u, 0x0Cu,
    0x1Cu, 0x00u, 0x94u, 0x26u, 0x7Fu, 0x00u, 0x22u, 0x32u, 0x40u, 0x10u, 0x02u, 0x00u, 0x0Cu, 0x00u, 0xA2u, 0xA2u,
    0x30u, 0x00u, 0x02u, 0x24u, 0x10u, 0x00u, 0xA2u, 0xA6u, 0x40u, 0x00u, 0x02u, 0x24u, 0x80u, 0x91u, 0x12u, 0x00u,
    0x03u, 0x85u, 0x10u, 0x00u, 0x3Fu, 0x00u, 0x10u, 0x32u, 0x25u, 0x90u, 0x50u, 0x02u, 0x12u, 0x00u, 0xA2u, 0xA6u,
    0x80u, 0x00u, 0x02u, 0x24u, 0x21u, 0x28u, 0xA0u, 0x02u, 0x08u, 0x00u, 0xB6u, 0xA6u, 0x0Au, 0x00u, 0xB7u, 0xA6u,
    0x0Du, 0x00u, 0xB3u, 0xA2u, 0x0Eu, 0x00u, 0xB2u, 0xA6u, 0x06u, 0x00u, 0xA2u, 0xA2u, 0x05u, 0x00u, 0xA2u, 0xA2u,
    0x04u, 0x00u, 0xA2u, 0xA2u, 0x14u, 0x00u, 0xB5u, 0x26u, 0x9Du, 0xEEu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x80u, 0x02u,
    0x21u, 0x20u, 0x80u, 0x02u, 0x21u, 0x28u, 0xA0u, 0x02u, 0x00u, 0x01u, 0x73u, 0x32u, 0x03u, 0x99u, 0x13u, 0x00u,
    0x80u, 0x03u, 0x31u, 0x32u, 0x83u, 0x89u, 0x11u, 0x00u, 0x80u, 0x00u, 0x31u, 0x36u, 0xAAu, 0x36u, 0x01u, 0x0Cu,
    0x25u, 0x30u, 0x71u, 0x02u, 0x64u, 0xDFu, 0xC2u, 0xAFu, 0x34u, 0x00u, 0xBFu, 0x8Fu, 0x30u, 0x00u, 0xBEu, 0x8Fu,
    0x2Cu, 0x00u, 0xB7u, 0x8Fu, 0x28u, 0x00u, 0xB6u, 0x8Fu, 0x24u, 0x00u, 0xB5u, 0x8Fu, 0x20u, 0x00u, 0xB4u, 0x8Fu,
    0x1Cu, 0x00u, 0xB3u, 0x8Fu, 0x18u, 0x00u, 0xB2u, 0x8Fu, 0x14u, 0x00u, 0xB1u, 0x8Fu, 0x10u, 0x00u, 0xB0u, 0x8Fu,
    0x38u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xC8u, 0xFFu, 0xBDu, 0x27u,
    0x28u, 0x00u, 0xB6u, 0xAFu, 0x21u, 0xB0u, 0x80u, 0x00u, 0x2Cu, 0x00u, 0xB7u, 0xAFu, 0x21u, 0xB8u, 0xA0u, 0x00u,
    0x30u, 0x00u, 0xBEu, 0xAFu, 0x0Bu, 0x80u, 0x1Eu, 0x3Cu, 0x10u, 0x00u, 0xB0u, 0xAFu, 0x64u, 0xDFu, 0xD0u, 0x8Fu,
    0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x14u, 0x00u, 0xB1u, 0xAFu, 0x60u, 0x9Cu, 0x51u, 0x94u, 0x0Bu, 0x80u, 0x05u, 0x3Cu,
    0x24u, 0x00u, 0xB5u, 0xAFu, 0x18u, 0x92u, 0xB5u, 0x8Cu, 0x60u, 0x9Cu, 0x43u, 0x24u, 0x34u, 0x00u, 0xBFu, 0xAFu,
    0x20u, 0x00u, 0xB4u, 0xAFu, 0x1Cu, 0x00u, 0xB3u, 0xAFu, 0x18u, 0x00u, 0xB2u, 0xAFu, 0x02u, 0x00u, 0x72u, 0x84u,
    0x04u, 0x00u, 0x74u, 0x84u, 0x06u, 0x00u, 0x73u, 0x84u, 0x21u, 0x20u, 0x00u, 0x02u, 0x00u, 0x8Cu, 0x11u, 0x00u,
    0x10u, 0xEFu, 0x01u, 0x0Cu, 0x1Cu, 0x00u, 0xB5u, 0x26u, 0x60u, 0x00u, 0x02u, 0x24u, 0x0Cu, 0x00u, 0x02u, 0xA2u,
    0x30u, 0x00u, 0x02u, 0x24u, 0x10u, 0x00u, 0x02u, 0xA6u, 0x40u, 0x00u, 0x02u, 0x24u, 0x80u, 0x91u, 0x12u, 0x00u,
    0x03u, 0x8Du, 0x11u, 0x00u, 0x3Fu, 0x00u, 0x31u, 0x32u, 0x25u, 0x90u, 0x51u, 0x02u, 0x12u, 0x00u, 0x02u, 0xA6u,
    0x80u, 0x00u, 0x02u, 0x24u, 0x21u, 0x28u, 0x00u, 0x02u, 0x08u, 0x00u, 0x16u, 0xA6u, 0x0Au, 0x00u, 0x17u, 0xA6u,
    0x0Du, 0x00u, 0x13u, 0xA2u, 0x0Eu, 0x00u, 0x12u, 0xA6u, 0x06u, 0x00u, 0x02u, 0xA2u, 0x05u, 0x00u, 0x02u, 0xA2u,
    0x04u, 0x00u, 0x02u, 0xA2u, 0x14u, 0x00u, 0x10u, 0x26u, 0x9Du, 0xEEu, 0x01u, 0x0Cu, 0x21u, 0x20u, 0xA0u, 0x02u,
    0x21u, 0x20u, 0xA0u, 0x02u, 0x21u, 0x28u, 0x00u, 0x02u, 0x00u, 0x01u, 0x73u, 0x32u, 0x03u, 0x99u, 0x13u, 0x00u,
    0xC0u, 0x03u, 0x94u, 0x32u, 0x83u, 0xA1u, 0x14u, 0x00u, 0xAAu, 0x36u, 0x01u, 0x0Cu, 0x25u, 0x30u, 0x74u, 0x02u,
    0x64u, 0xDFu, 0xC2u, 0xAFu, 0x34u, 0x00u, 0xBFu, 0x8Fu, 0x30u, 0x00u, 0xBEu, 0x8Fu, 0x2Cu, 0x00u, 0xB7u, 0x8Fu,
    0x28u, 0x00u, 0xB6u, 0x8Fu, 0x24u, 0x00u, 0xB5u, 0x8Fu, 0x20u, 0x00u, 0xB4u, 0x8Fu, 0x1Cu, 0x00u, 0xB3u, 0x8Fu,
    0x18u, 0x00u, 0xB2u, 0x8Fu, 0x14u, 0x00u, 0xB1u, 0x8Fu, 0x10u, 0x00u, 0xB0u, 0x8Fu, 0x38u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xC8u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xB0u, 0xAFu,
    0x21u, 0x80u, 0x80u, 0x00u, 0x14u, 0x00u, 0xB1u, 0xAFu, 0x21u, 0x88u, 0xA0u, 0x00u, 0x0Bu, 0x80u, 0x02u, 0x3Cu,
    0xC0u, 0x28u, 0x06u, 0x00u, 0x23u, 0x28u, 0xA6u, 0x00u, 0x40u, 0x28u, 0x05u, 0x00u, 0x18u, 0x00u, 0xB2u, 0xAFu,
    0x64u, 0xDFu, 0x52u, 0x8Cu, 0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x18u, 0x92u, 0x63u, 0x8Cu,
    0x6Cu, 0x9Cu, 0x42u, 0x24u, 0x1Cu, 0x00u, 0xB3u, 0xAFu, 0x21u, 0x98u, 0xA2u, 0x00u, 0x34u, 0x00u, 0xBFu, 0xAFu,
    0x30u, 0x00u, 0xBEu, 0xAFu, 0x2Cu, 0x00u, 0xB7u, 0xAFu, 0x28u, 0x00u, 0xB6u, 0xAFu, 0x24u, 0x00u, 0xB5u, 0xAFu,
    0x20u, 0x00u, 0xB4u, 0xAFu, 0x00u, 0x00u, 0x75u, 0x86u, 0x02u, 0x00u, 0x74u, 0x86u, 0x04u, 0x00u, 0x7Eu, 0x86u,
    0x06u, 0x00u, 0x77u, 0x86u, 0x21u, 0x20u, 0x40u, 0x02u, 0x10u, 0xEFu, 0x01u, 0x0Cu, 0x1Cu, 0x00u, 0x76u, 0x24u,
    0x08u, 0x00u, 0x50u, 0xA6u, 0x0Au, 0x00u, 0x51u, 0xA6u, 0x0Cu, 0x00u, 0x62u, 0x96u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x01u, 0x00u, 0x42u, 0x30u, 0x03u, 0x00u, 0x40u, 0x10u, 0x3Fu, 0x00u, 0xC3u, 0x33u, 0xFCu, 0x00u, 0x04u, 0x08u,
    0x40u, 0x10u, 0x03u, 0x00u, 0x80u, 0x10u, 0x03u, 0x00u, 0x21u, 0x20u, 0xC0u, 0x02u, 0x21u, 0x28u, 0x40u, 0x02u,
    0x0Cu, 0x00u, 0x42u, 0xA2u, 0x0Du, 0x00u, 0x57u, 0xA2u, 0x08u, 0x00u, 0x62u, 0x96u, 0x80u, 0x19u, 0x14u, 0x00u,
    0x10u, 0x00u, 0x42u, 0xA6u, 0x03u, 0x11u, 0x15u, 0x00u, 0x3Fu, 0x00u, 0x42u, 0x30u, 0x25u, 0x18u, 0x62u, 0x00u,
    0x0Au, 0x00u, 0x66u, 0x96u, 0x80u, 0x00u, 0x02u, 0x24u, 0x0Eu, 0x00u, 0x43u, 0xA6u, 0x06u, 0x00u, 0x42u, 0xA2u,
    0x05u, 0x00u, 0x42u, 0xA2u, 0x04u, 0x00u, 0x42u, 0xA2u, 0x12u, 0x00u, 0x46u, 0xA6u, 0x9Du, 0xEEu, 0x01u, 0x0Cu,
    0x14u, 0x00u, 0x52u, 0x26u, 0x21u, 0x20u, 0xC0u, 0x02u, 0x21u, 0x28u, 0x40u, 0x02u, 0x00u, 0x01u, 0xE2u, 0x32u,
    0x03u, 0x11u, 0x02u, 0x00u, 0x80u, 0x03u, 0xC6u, 0x33u, 0x0Cu, 0x00u, 0x63u, 0x96u, 0x83u, 0x31u, 0x06u, 0x00u,
    0x01u, 0x00u, 0x63u, 0x30u, 0xC0u, 0x19u, 0x03u, 0x00u, 0x25u, 0x18u, 0x62u, 0x00u, 0xAAu, 0x36u, 0x01u, 0x0Cu,
    0x25u, 0x30u, 0x66u, 0x00u, 0x0Bu, 0x80u, 0x03u, 0x3Cu, 0x64u, 0xDFu, 0x62u, 0xACu, 0x34u, 0x00u, 0xBFu, 0x8Fu,
    0x30u, 0x00u, 0xBEu, 0x8Fu, 0x2Cu, 0x00u, 0xB7u, 0x8Fu, 0x28u, 0x00u, 0xB6u, 0x8Fu, 0x24u, 0x00u, 0xB5u, 0x8Fu,
    0x20u, 0x00u, 0xB4u, 0x8Fu, 0x1Cu, 0x00u, 0xB3u, 0x8Fu, 0x18u, 0x00u, 0xB2u, 0x8Fu, 0x14u, 0x00u, 0xB1u, 0x8Fu,
    0x10u, 0x00u, 0xB0u, 0x8Fu, 0x38u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xE8u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0x08u, 0x00u, 0x85u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x3Du, 0x00u, 0xA0u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Cu, 0x00u, 0x83u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x09u, 0x00u, 0x60u, 0x14u, 0x02u, 0x00u, 0x02u, 0x24u, 0x09u, 0x80u, 0x02u, 0x3Cu, 0xF8u, 0x7Eu, 0x42u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x10u, 0x45u, 0x00u, 0x33u, 0x00u, 0x40u, 0x14u, 0xFFu, 0xFFu, 0x02u, 0x24u,
    0x71u, 0x01u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x19u, 0x00u, 0x62u, 0x10u, 0x03u, 0x00u, 0x62u, 0x2Cu,
    0x05u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x02u, 0x24u, 0x08u, 0x00u, 0x62u, 0x10u, 0x09u, 0x80u, 0x02u, 0x3Cu,
    0x6Fu, 0x01u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x02u, 0x24u, 0x21u, 0x00u, 0x62u, 0x10u,
    0x09u, 0x80u, 0x02u, 0x3Cu, 0x6Fu, 0x01u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0xF0u, 0x7Eu, 0x43u, 0x24u,
    0x0Cu, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x42u, 0x30u, 0x0Fu, 0x00u, 0x40u, 0x14u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x42u, 0x30u,
    0x19u, 0x00u, 0x40u, 0x14u, 0xFFu, 0xFFu, 0x02u, 0x24u, 0x71u, 0x01u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
};


/* Generated by PSXRecomp - PlayStation 1 Static Recompiler */

#include "psx_runtime.h"

#ifndef PSX_NO_DEBUG_TOOLS
extern void debug_server_log_call_entry(uint32_t func_addr);
extern void debug_server_cyc_observe(uint32_t block_leader_phys);
#endif
#ifdef PSX_COSIM
extern void cosim_block(uint32_t block_leader_phys);
extern void cosim_instr(uint32_t pc);
#endif
extern int  psx_datashard_enter(CPUState* cpu, uint32_t key);  /* data-shard replay/capture (data_shards.c) */
extern void psx_mod_function_entry(CPUState* cpu, uint32_t address);  /* trusted opt-in game-mod hook */
extern void psx_datashard_ret(CPUState* cpu);                  /* data-shard capture finalize */
extern int  psx_vsync_query_hle_enter(CPUState* cpu, uint32_t func, uint32_t counter_addr, uint32_t gpustat_ptr_addr, uint32_t timer1_ptr_addr, uint32_t timer1_cache_addr);  /* load_accel.c */
extern void psx_ws_sprite_tag(CPUState* cpu);  /* widescreen prim tag (gpu.c) */
extern void psx_ws_mmx6_bg_stage_init(void);    /* ws 2D stage reveal invalidation (gpu.c) */
extern int  psx_ws_x_margin(void);  /* widescreen cull-margin term (gpu.c) */
extern uint32_t psx_ws_shell_source_index(uint32_t index, uint32_t base_cols, uint32_t wide_cols); /* finite-shell descriptor map */
extern int32_t psx_ws_player_x_bound(int32_t vanilla);  /* typed gameplay X bound */
extern int  psx_ws_cull_sltiu(uint32_t sx, uint32_t imm);  /* ws auto screen-x cull (gpu.c) */
extern int  psx_ws_cull_slti(uint32_t sx, uint32_t imm);   /* ws cull signed right edge (gpu.c) */
extern int  psx_ws_cull_slti_lower(uint32_t sx, uint32_t imm); /* ws cull signed lower edge (gpu.c) */
extern int  psx_ws_cull_bltz(uint32_t v);                  /* ws cull signed left edge (gpu.c) */
extern int  psx_ws_background_cull_bltz(uint32_t v);       /* resident background left edge */
extern int  psx_ws_background_cull_margin(void);           /* background-only overdraw margin */
extern int  psx_ws_cull_vxrange(uint32_t x, uint32_t imm); /* ws masked-u16 X window */
extern int32_t psx_ws_depth_bound(int32_t imm);            /* ws aspect-scaled far bound */
extern int32_t psx_ws_plane_nx(int32_t nx);                /* ws side-plane normal-X scale (gpu.c) */
extern uint32_t psx_ws_xclip_bound(uint32_t vanilla);      /* ws per-prim X reject bound load (gpu.c) */
extern uint32_t psx_ws_cull_keep_result(uint32_t vanilla, uint32_t forced); /* ws guarded keep compare (gpu.c) */
extern uint32_t psx_ws_aspect_cone_result(uint32_t site, uint32_t vanilla, uint32_t object, int32_t x, int32_t z, int32_t y); /* aspect-aware participation cone */
extern uint32_t psx_ws_angle_widen(uint32_t vanilla); /* aspect-scaled 12-bit terrain-frustum half-angle */
extern uint32_t psx_ws_full_angle_widen(uint32_t vanilla); /* full aperture with live viewport width */
extern int  psx_ws_backdrop_x(int x);  /* widescreen backdrop screenX squash (gpu.c) */
extern void psx_ws_background_packet_tag(uint32_t addr); /* exact background packet provenance */
extern int  psx_ws_bg2d_cols(int base);                    /* ws 2D bg tile-loop widen: col count (gpu.c) */
extern int  psx_ws_bg2d_startcol(int col, unsigned mask);  /* ws 2D bg tile-loop widen: start tile col (gpu.c) */
extern int  psx_ws_bg2d_startx(int x);                     /* ws 2D bg tile-loop widen: start screen-x (gpu.c) */
extern int  psx_ws_bg2d_stream_left(int x);                /* ws 2D bg tile-ring streamer: left edge (gpu.c) */
extern int  psx_ws_bg2d_stream_right(int x);               /* ws 2D bg tile-ring streamer: right edge (gpu.c) */
extern int  psx_ws_bg2d_undercap(int counter, int native_cap); /* ws 2D BG packet cap (gpu.c) */
extern int  psx_ws_mmx6_bg_bufbase(int addr);   /* ws 2D bg packet-buffer relocation (gpu.c) */
extern int  psx_ws_mmx6_bg_undercap(int counter);/* ws 2D bg per-frame tile cap (gpu.c) */
extern int  psx_game_option_store(uint32_t addr, int val);  /* persisted OPTION restore-at-init (game_options.c) */
extern uint32_t psx_ws_backdrop_value(uint32_t orig, int is_end, int window_cols);  /* ws backdrop preload (gpu.c) */
extern void gte_ws_set_suppress(int on);  /* widescreen far-backdrop un-squash (gte.cpp) */
extern uint32_t g_debug_last_store_pc;  /* exact PC of the executing SW/SH/SB — wtrace/readtrace producer attribution (debug_server.c) */

/* --- Unaligned memory access helper implementations --- */

/* LWL/LWR/SWL/SWR implement little-endian R3000A unaligned word
 * semantics. Keep these switch tables in sync with psx_interpreter.c
 * and dirty_ram_interp.c.
 */

/* LWL: Load Word Left - merges high bytes from aligned word into rt.
 * addr is the effective (possibly unaligned) address.
 * rt_value is the current value of the destination register.
 */
static uint32_t psx_lwl(CPUState* cpu, uint32_t addr, uint32_t rt_value, uint32_t rt, uint32_t reg_mask) {
    uint32_t word = psx_cyc_load_word(cpu, addr & ~3u, rt, reg_mask);  /* full load interlock */
    switch (addr & 3u) {
        case 0: return (rt_value & 0x00FFFFFFu) | (word << 24);
        case 1: return (rt_value & 0x0000FFFFu) | (word << 16);
        case 2: return (rt_value & 0x000000FFu) | (word << 8);
        default: return word;
    }
}

/* LWR: Load Word Right - merges low bytes from aligned word into rt.
 * addr is the effective (possibly unaligned) address.
 * rt_value is the current value of the destination register.
 */
static uint32_t psx_lwr(CPUState* cpu, uint32_t addr, uint32_t rt_value, uint32_t rt, uint32_t reg_mask) {
    uint32_t word = psx_cyc_load_word(cpu, addr & ~3u, rt, reg_mask);  /* full load interlock */
    switch (addr & 3u) {
        case 0: return word;
        case 1: return (rt_value & 0xFF000000u) | (word >> 8);
        case 2: return (rt_value & 0xFFFF0000u) | (word >> 16);
        default: return (rt_value & 0xFFFFFF00u) | (word >> 24);
    }
}

/* SWL: Store Word Left - stores the high bytes of rt_value into memory.
 * addr is the effective (possibly unaligned) address.
 * rt_value is the value of the source register.
 */
static void psx_swl(CPUState* cpu, uint32_t addr, uint32_t rt_value) {
    psx_store_cycle_barrier();
    uint32_t aligned_addr = addr & ~3u;
    uint32_t word = cpu->read_word(aligned_addr);
    switch (addr & 3u) {
        case 0: word = (word & 0xFFFFFF00u) | (rt_value >> 24); break;
        case 1: word = (word & 0xFFFF0000u) | (rt_value >> 16); break;
        case 2: word = (word & 0xFF000000u) | (rt_value >> 8); break;
        default: word = rt_value; break;
    }
    cpu->write_word(aligned_addr, word);
}

/* SWR: Store Word Right - stores the low bytes of rt_value into memory.
 * addr is the effective (possibly unaligned) address.
 * rt_value is the value of the source register.
 */
static void psx_swr(CPUState* cpu, uint32_t addr, uint32_t rt_value) {
    psx_store_cycle_barrier();
    uint32_t aligned_addr = addr & ~3u;
    uint32_t word = cpu->read_word(aligned_addr);
    switch (addr & 3u) {
        case 0: word = rt_value; break;
        case 1: word = (word & 0x000000FFu) | (rt_value << 8); break;
        case 2: word = (word & 0x0000FFFFu) | (rt_value << 16); break;
        default: word = (word & 0x00FFFFFFu) | (rt_value << 24); break;
    }
    cpu->write_word(aligned_addr, word);
}

/* Forward declarations */
static void func_800FDFE4(CPUState* cpu);
static void func_800FE984(CPUState* cpu);
static void func_800FEA08(CPUState* cpu);
static void func_800FEBF4(CPUState* cpu);
static void func_800FECB8(CPUState* cpu);
static void func_800FEE00(CPUState* cpu);
static void func_800FEED0(CPUState* cpu);
static void func_800FEF94(CPUState* cpu);
static void func_800FEFB4(CPUState* cpu);
static void func_800FF0C4(CPUState* cpu);
static void func_800FF7C0(CPUState* cpu);
static void func_800FF830(CPUState* cpu);
static void func_800FF8E8(CPUState* cpu);
static void func_800FF964(CPUState* cpu);
static void func_800FFC24(CPUState* cpu);
static void func_800FFCDC(CPUState* cpu);
static void func_800FEBE0(CPUState* cpu);
void psx_alias_body_800FEA08(CPUState* cpu, uint32_t entry);

static void func_800FDFE4(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FE004u: goto block_800FE004;
            case 0x800FE00Cu: goto block_800FE00C;
            case 0x800FE044u: goto block_800FE044;
            case 0x800FE0C8u: goto block_800FE0C8;
            case 0x800FE0D8u: goto block_800FE0D8;
            case 0x800FE0E0u: goto block_800FE0E0;
            case 0x800FE0E8u: goto block_800FE0E8;
            case 0x800FE0F0u: goto block_800FE0F0;
            case 0x800FE100u: goto block_800FE100;
            case 0x800FE108u: goto block_800FE108;
            case 0x800FE134u: goto block_800FE134;
            case 0x800FE16Cu: goto block_800FE16C;
            case 0x800FE174u: goto block_800FE174;
            case 0x800FE184u: goto block_800FE184;
            case 0x800FE18Cu: goto block_800FE18C;
            case 0x800FE194u: goto block_800FE194;
            case 0x800FE1A0u: goto block_800FE1A0;
            case 0x800FE1C0u: goto block_800FE1C0;
            case 0x800FE1D0u: goto block_800FE1D0;
            case 0x800FE1DCu: goto block_800FE1DC;
            case 0x800FE200u: goto block_800FE200;
            case 0x800FE228u: goto block_800FE228;
            case 0x800FE264u: goto block_800FE264;
            case 0x800FE2B0u: goto block_800FE2B0;
            case 0x800FE2B8u: goto block_800FE2B8;
            case 0x800FE2C0u: goto block_800FE2C0;
            case 0x800FE2C8u: goto block_800FE2C8;
            case 0x800FE304u: goto block_800FE304;
            case 0x800FE30Cu: goto block_800FE30C;
            case 0x800FE314u: goto block_800FE314;
            case 0x800FE320u: goto block_800FE320;
            case 0x800FE338u: goto block_800FE338;
            case 0x800FE348u: goto block_800FE348;
            case 0x800FE388u: goto block_800FE388;
            case 0x800FE3C0u: goto block_800FE3C0;
            case 0x800FE3D8u: goto block_800FE3D8;
            case 0x800FE3FCu: goto block_800FE3FC;
            case 0x800FE404u: goto block_800FE404;
            case 0x800FE40Cu: goto block_800FE40C;
            case 0x800FE414u: goto block_800FE414;
            case 0x800FE43Cu: goto block_800FE43C;
            case 0x800FE454u: goto block_800FE454;
            case 0x800FE464u: goto block_800FE464;
            case 0x800FE480u: goto block_800FE480;
            case 0x800FE4A8u: goto block_800FE4A8;
            case 0x800FE4C0u: goto block_800FE4C0;
            case 0x800FE4D0u: goto block_800FE4D0;
            case 0x800FE528u: goto block_800FE528;
            case 0x800FE558u: goto block_800FE558;
            case 0x800FE590u: goto block_800FE590;
            case 0x800FE5E4u: goto block_800FE5E4;
            case 0x800FE630u: goto block_800FE630;
            case 0x800FE638u: goto block_800FE638;
            case 0x800FE640u: goto block_800FE640;
            case 0x800FE66Cu: goto block_800FE66C;
            case 0x800FE684u: goto block_800FE684;
            case 0x800FE694u: goto block_800FE694;
            case 0x800FE6D4u: goto block_800FE6D4;
            case 0x800FE6ECu: goto block_800FE6EC;
            case 0x800FE6FCu: goto block_800FE6FC;
            case 0x800FE728u: goto block_800FE728;
            case 0x800FE740u: goto block_800FE740;
            case 0x800FE74Cu: goto block_800FE74C;
            case 0x800FE758u: goto block_800FE758;
            case 0x800FE76Cu: goto block_800FE76C;
            case 0x800FE784u: goto block_800FE784;
            case 0x800FE014u: goto block_800FE014;
            case 0x800FE01Cu: goto block_800FE01C;
            case 0x800FE030u: goto block_800FE030;
            case 0x800FE054u: goto block_800FE054;
            case 0x800FE074u: goto block_800FE074;
            case 0x800FE090u: goto block_800FE090;
            case 0x800FE0B4u: goto block_800FE0B4;
            case 0x800FE0F8u: goto block_800FE0F8;
            case 0x800FE110u: goto block_800FE110;
            case 0x800FE17Cu: goto block_800FE17C;
            case 0x800FE1B8u: goto block_800FE1B8;
            case 0x800FE1D4u: goto block_800FE1D4;
            case 0x800FE1F8u: goto block_800FE1F8;
            case 0x800FE21Cu: goto block_800FE21C;
            case 0x800FE2D8u: goto block_800FE2D8;
            case 0x800FE2E0u: goto block_800FE2E0;
            case 0x800FE2FCu: goto block_800FE2FC;
            case 0x800FE330u: goto block_800FE330;
            case 0x800FE34Cu: goto block_800FE34C;
            case 0x800FE364u: goto block_800FE364;
            case 0x800FE3F0u: goto block_800FE3F0;
            case 0x800FE42Cu: goto block_800FE42C;
            case 0x800FE44Cu: goto block_800FE44C;
            case 0x800FE468u: goto block_800FE468;
            case 0x800FE4B8u: goto block_800FE4B8;
            case 0x800FE4C4u: goto block_800FE4C4;
            case 0x800FE4D8u: goto block_800FE4D8;
            case 0x800FE4E8u: goto block_800FE4E8;
            case 0x800FE4F0u: goto block_800FE4F0;
            case 0x800FE50Cu: goto block_800FE50C;
            case 0x800FE518u: goto block_800FE518;
            case 0x800FE520u: goto block_800FE520;
            case 0x800FE54Cu: goto block_800FE54C;
            case 0x800FE59Cu: goto block_800FE59C;
            case 0x800FE664u: goto block_800FE664;
            case 0x800FE67Cu: goto block_800FE67C;
            case 0x800FE698u: goto block_800FE698;
            case 0x800FE6ACu: goto block_800FE6AC;
            case 0x800FE6C8u: goto block_800FE6C8;
            case 0x800FE6CCu: goto block_800FE6CC;
            case 0x800FE6E4u: goto block_800FE6E4;
            case 0x800FE700u: goto block_800FE700;
            case 0x800FE710u: goto block_800FE710;
            case 0x800FE738u: goto block_800FE738;
            case 0x800FE760u: goto block_800FE760;
            case 0x800FE764u: goto block_800FE764;
            case 0x800FE77Cu: goto block_800FE77C;
            case 0x800FE788u: goto block_800FE788;
            case 0x800FDFE4u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FDFE4u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FDFE4u);
    /* Address: 0x800FDFE4, Size: 1992 bytes, Blocks: 115 */

block_800FDFE4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FDFE4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FDFE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FDFE4u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FDFE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -72;  /* 0x800FDFE4: 0x27BDFFB8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FDFE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FDFE8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 68, cpu->gpr[31]);  /* 0x800FDFE8: 0xAFBF0044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FDFE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20100000u);
#endif
    g_debug_last_store_pc = 0x800FDFECu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 64, cpu->gpr[20]);  /* 0x800FDFEC: 0xAFB40040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FDFECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FDFF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20080000u);
#endif
    g_debug_last_store_pc = 0x800FDFF0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 60, cpu->gpr[19]);  /* 0x800FDFF0: 0xAFB3003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FDFF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20040000u);
#endif
    g_debug_last_store_pc = 0x800FDFF4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[18]);  /* 0x800FDFF4: 0xAFB20038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FDFF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x800FDFF8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 52, cpu->gpr[17]);  /* 0x800FDFF8: 0xAFB10034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FDFF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE004u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE000u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FE000u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[16]);  /* 0x800FE000: 0xAFB00030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE000u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B6FCu);
    cpu->pc = 0x8006B6FCu; return;  /* CPS jal -> 0x8006B6FC */

block_800FE004:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE004u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE004u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE004u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE004u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE00Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE008: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE008u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006C558u);
    cpu->pc = 0x8006C558u; return;  /* CPS jal -> 0x8006C558 */

block_800FE00C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE00Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE00Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE00Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE00Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE00C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE010u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE010: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE010u);
#endif
    if (_bc_800FE00C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE01Cu);
        goto block_800FE01C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE014u);
        goto block_800FE014;  /* not taken */
    }

block_800FE014:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE014u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE014u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE014u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE014u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x800FE018: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE018u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE788u);
    goto block_800FE788;  /* j */

block_800FE01C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE01Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE01Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE01Cu, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE01Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[16] = cpu->gpr[2] + 17400;  /* 0x800FE01C: 0x245043F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE01Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE020u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 74, 2, 0x10000u);  /* 0x800FE020: 0x8602004A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE020u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE024: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE024u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    int _bc_800FE028 = ((int32_t)cpu->gpr[2] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8010 << 16;  /* 0x80100000 */  /* 0x800FE02C: 0x3C048010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE02Cu);
#endif
    if (_bc_800FE028) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE054u);
        goto block_800FE054;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE030u);
        goto block_800FE030;  /* not taken */
    }

block_800FE030:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE030u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE030u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE030u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE030u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 24448;  /* 0x800FE030: 0x24845F80 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE030u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE034: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE034u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x61u);
#endif
    cpu->gpr[6] = cpu->gpr[5];  /* move */  /* 0x800FE038: 0x00A03021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE038u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE044u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE040u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA1u);
#endif
    cpu->gpr[7] = cpu->gpr[5];  /* move */  /* 0x800FE040: 0x00A03821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE040u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C5B8u);
    cpu->pc = 0x8007C5B8u; return;  /* CPS jal -> 0x8007C5B8 */

block_800FE044:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE044u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE044u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE044u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE044u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[16] + 74, 2, 0x10000u);  /* 0x800FE044: 0x9602004A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE044u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE048: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE048u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -1;  /* 0x800FE04C: 0x2442FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE04Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE050u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE050u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 74, (uint16_t)cpu->gpr[2]);  /* 0x800FE050: 0xA602004A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE050u);
#endif
    /* fall through to block_800FE054 */

block_800FE054:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE054u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE054u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE054u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE054u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[16] + 80, 2, 0x10000u);  /* 0x800FE054: 0x96020050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE054u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE058: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE058u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -1;  /* 0x800FE05C: 0x2442FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE05Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE060u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 16;  /* 0x800FE060: 0x00021400 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE060u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = (int32_t)cpu->gpr[2] >> 16;  /* 0x800FE064: 0x00021C03 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE064u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)5) ? 1 : 0;  /* 0x800FE068: 0x2C620005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE068u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE06C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE070u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FE070: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE070u);
#endif
    if (_bc_800FE06C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE77Cu);
        goto block_800FE77C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE074u);
        goto block_800FE074;  /* not taken */
    }

block_800FE074:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE074u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE074u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE074u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE074u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -27192;  /* 0x800FE074: 0x244295C8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE074u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] << 2;  /* 0x800FE078: 0x00031880 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE078u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];  /* 0x800FE07C: 0x00621821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE07Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE080u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FE080: 0x8C620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE080u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE084: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE084u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    uint32_t _jt_800FE088 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE08C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE08Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FE088);
    cpu->pc = _jt_800FE088; return;  /* CPS: jr cpu->gpr[2] */

block_800FE090:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE090u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE090u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE090u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE090u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE090: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE090u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[17] = cpu->gpr[2] + 17400;  /* 0x800FE094: 0x245143F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE094u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[17] + 82, 2, 0x20000u);  /* 0x800FE098: 0x96220052 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE098u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE09C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE09Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -1;  /* 0x800FE0A0: 0x2442FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE0A4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 82, (uint16_t)cpu->gpr[2]);  /* 0x800FE0A4: 0xA6220052 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 16;  /* 0x800FE0A8: 0x00021400 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE0AC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE0B0: 0x3C108011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0B0u);
#endif
    if (_bc_800FE0AC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE0F8u);
        goto block_800FE0F8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE0B4u);
        goto block_800FE0B4;  /* not taken */
    }

block_800FE0B4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE0B4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE0B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE0B4u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + 17528;  /* 0x800FE0B4: 0x26104478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE0B8: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE0BC: 0x3C058011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE0C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 17496;  /* 0x800FE0C4: 0x24A54458 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE7ACu);
    cpu->pc = 0x800FE7ACu; return;  /* CPS jal -> 0x800FE7AC */

block_800FE0C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE0C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE0C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE0C8u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE0C8: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x8010 << 16;  /* 0x80100000 */  /* 0x800FE0CC: 0x3C058010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE0D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -5624;  /* 0x800FE0D4: 0x24A5EA08 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE984u);
    cpu->pc = 0x800FE984u; return;  /* CPS jal -> 0x800FE984 */

block_800FE0D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE0D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE0D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE0D8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE0E0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE0DC: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80029DC0u);
    cpu->pc = 0x80029DC0u; return;  /* CPS jal -> 0x80029DC0 */

block_800FE0E0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE0E0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE0E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE0E0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE0E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE0E4: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FE0E8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE0E8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE0E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE0E8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE0F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE0EC: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEBF4u);
    cpu->pc = 0x800FEBF4u; return;  /* CPS jal -> 0x800FEBF4 */

block_800FE0F0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE0F0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE0F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE0F0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 2;  /* 0x800FE0F0: 0x24020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE0F4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 80, (uint16_t)cpu->gpr[2]);  /* 0x800FE0F4: 0xA6220050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0F4u);
#endif
    /* fall through to block_800FE0F8 */

block_800FE0F8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE0F8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE0F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE0F8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE0F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE100u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE0FC: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE0FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80029DC0u);
    cpu->pc = 0x80029DC0u; return;  /* CPS jal -> 0x80029DC0 */

block_800FE100:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE100u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE100u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE100u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE100u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE108u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE104: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE104u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FE108:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE108u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE108u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE108u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE108u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FE10C: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE10Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE788u);
    goto block_800FE788;  /* j */

block_800FE110:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE110u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE110u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE110u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE110u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE110: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE110u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[17] = cpu->gpr[2] + 17528;  /* 0x800FE114: 0x24514478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE114u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 16, 2, 0x20000u);  /* 0x800FE118: 0x86220010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE118u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE11C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE11Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE120u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FE120: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE120u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];  /* 0x800FE124: 0x00511021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE124u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[2], 4, 0x4u);  /* 0x800FE128: 0x8C440000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE128u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE134u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE130u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 3;  /* 0x800FE130: 0x24050003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE130u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801056C4u);
    cpu->pc = 0x801056C4u; return;  /* CPS jal -> 0x801056C4 */

block_800FE134:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE134u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE134u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE134u, 14u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE134u);
#endif
    cpu->gpr[5] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 60, 5, 0x20000u);  /* 0x800FE134: 0x8625003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE134u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 62, 2, 0x20000u);  /* 0x800FE138: 0x8622003E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE138u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE13C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE13Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE140u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[5] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[5]));
#endif  /* 0x800FE140: 0x00A20018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE140u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 18, 2, 0x20000u);  /* 0x800FE144: 0x86220012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE144u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE148: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE148u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FE14C: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE14Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE150u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];  /* 0x800FE150: 0x00511021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE150u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[2] + 8, 4, 0x4u);  /* 0x800FE154: 0x8C440008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE154u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x800FE158: 0x00002812 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE158u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[5] >> 31;  /* 0x800FE15C: 0x000517C2 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE15Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE160u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];  /* 0x800FE160: 0x00A22821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE160u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE16Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = (int32_t)cpu->gpr[5] >> 1;  /* 0x800FE168: 0x00052843 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE168u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105740u);
    cpu->pc = 0x80105740u; return;  /* CPS jal -> 0x80105740 */

block_800FE16C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE16Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE16Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE16Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE16Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE174u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE170u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE170: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE170u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEBF4u);
    cpu->pc = 0x800FEBF4u; return;  /* CPS jal -> 0x800FEBF4 */

block_800FE174:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE174u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE174u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE174u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE174u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE174 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE178: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE178u);
#endif
    if (_bc_800FE174) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE1F8u);
        goto block_800FE1F8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE17Cu);
        goto block_800FE17C;  /* not taken */
    }

block_800FE17C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE17Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE17Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE17Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE17Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE184u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE180u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE180: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE180u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105560u);
    cpu->pc = 0x80105560u; return;  /* CPS jal -> 0x80105560 */

block_800FE184:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE184u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE184u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE184u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE184u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE18Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE188: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE188u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801057FCu);
    cpu->pc = 0x801057FCu; return;  /* CPS jal -> 0x801057FC */

block_800FE18C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE18Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE18Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE18Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE18Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE194u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE190u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE190: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE190u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80082B98u);
    cpu->pc = 0x80082B98u; return;  /* CPS jal -> 0x80082B98 */

block_800FE194:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE194u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE194u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE194u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE194u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE194: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE194u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE1A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = -1;  /* 0x800FE19C: 0x2405FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE19Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006BC98u);
    cpu->pc = 0x8006BC98u; return;  /* CPS jal -> 0x8006BC98 */

block_800FE1A0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE1A0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE1A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE1A0u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE1A0: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[16] = cpu->gpr[2] + 17400;  /* 0x800FE1A4: 0x245043F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1A4u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 60, 2, 0x10000u);  /* 0x800FE1A8: 0x8602003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE1AC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE1B0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE1B4: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1B4u);
#endif
    if (_bc_800FE1B0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE1D4u);
        goto block_800FE1D4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE1B8u);
        goto block_800FE1B8;  /* not taken */
    }

block_800FE1B8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE1B8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE1B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE1B8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE1C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE1BC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F660u);
    cpu->pc = 0x8008F660u; return;  /* CPS jal -> 0x8008F660 */

block_800FE1C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE1C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE1C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE1C0u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 9;  /* 0x800FE1C0: 0x24040009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE1C4: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE1D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x61u);
#endif
    cpu->gpr[6] = cpu->gpr[5];  /* move */  /* 0x800FE1CC: 0x00A03021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090D88u);
    cpu->pc = 0x80090D88u; return;  /* CPS jal -> 0x80090D88 */

block_800FE1D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE1D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE1D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE1D0u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE1D0: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1D0u);
#endif
    /* fall through to block_800FE1D4 */

block_800FE1D4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE1D4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE1D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE1D4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE1DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE1D8: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FDA2Cu);
    cpu->pc = 0x800FDA2Cu; return;  /* CPS jal -> 0x800FDA2C */

block_800FE1DC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE1DCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE1DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE1DCu, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE1DC: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE1E0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 80, (uint16_t)cpu->gpr[2]);  /* 0x800FE1E0: 0xA6020050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x800FE1E4: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FE1E8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 70, (uint16_t)cpu->gpr[0]);  /* 0x800FE1E8: 0xA6000046 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FE1ECu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 68, (uint16_t)cpu->gpr[0]);  /* 0x800FE1EC: 0xA6000044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE1F4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 78, (uint16_t)cpu->gpr[2]);  /* 0x800FE1F4: 0xA602004E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE784u);
    goto block_800FE784;  /* j */

block_800FE1F8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE1F8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE1F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE1F8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE1F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE200u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE1FC: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE1FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEE00u);
    cpu->pc = 0x800FEE00u; return;  /* CPS jal -> 0x800FEE00 */

block_800FE200:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE200u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE200u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE200u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE200u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000u);
#endif
    cpu->gpr[19] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FE200: 0x3C13800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FE200u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE204: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE204u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[18] = cpu->gpr[2] + 17400;  /* 0x800FE208: 0x245243F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE208u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 66, 2, 0x40000u);  /* 0x800FE20C: 0x86420042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE20Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE210u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE210: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE210u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE214 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x180000u);
#endif
    cpu->gpr[20] = cpu->gpr[19] + -8856;  /* 0x800FE218: 0x2674DD68 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE218u);
#endif
    if (_bc_800FE214) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE228u);
        goto block_800FE228;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE21Cu);
        goto block_800FE21C;  /* not taken */
    }

block_800FE21C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE21Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE21Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE21Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE21Cu);
#endif
    cpu->gpr[4] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 58, 4, 0x40000u);  /* 0x800FE21C: 0x8644003A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE21Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE220u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE228u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE224: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE224u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF8E0u);
    cpu->pc = 0x800FF8E0u; return;  /* CPS jal -> 0x800FF8E0 */

block_800FE228:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE228u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE228u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE228u, 15u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE228u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    cpu->gpr[16] = cpu->gpr[29] + 24;  /* 0x800FE228: 0x27B00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE228u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE22C: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE22Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE230u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 20, 2, 0x20000u);  /* 0x800FE230: 0x86220014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE230u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 17496;  /* 0x800FE234: 0x24634458 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE234u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)1) ? 1 : 0;  /* 0x800FE238: 0x2C420001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE238u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    g_debug_last_store_pc = 0x800FE23Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 56, (uint16_t)cpu->gpr[2]);  /* 0x800FE23C: 0xA6420038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE23Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE240u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 3;  /* 0x800FE240: 0x000210C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE240u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];  /* 0x800FE244: 0x00511021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE244u);
#endif
    cpu->gpr[5] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 24, 5, 0x4u);  /* 0x800FE248: 0x84450018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE248u);
#endif
    cpu->gpr[6] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 26, 6, 0x4u);  /* 0x800FE24C: 0x8446001A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE24Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE250u);
#endif
    cpu->gpr[7] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 8, 7, 0x8u);  /* 0x800FE250: 0x84670008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE250u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 10, 2, 0x8u);  /* 0x800FE254: 0x8462000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE254u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE258: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE258u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE264u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE260u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FE260u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x800FE260: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE260u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007B90Cu);
    cpu->pc = 0x8007B90Cu; return;  /* CPS jal -> 0x8007B90C */

block_800FE264:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE264u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE264u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE264u, 19u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE264u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FE264: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE264u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8009 << 16;  /* 0x80090000 */  /* 0x800FE268: 0x3C038009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE268u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 32496;  /* 0x800FE26C: 0x24637EF0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE26Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE270u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE270u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 17, (uint8_t)cpu->gpr[2]);  /* 0x800FE270: 0xA2020011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE270u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE274u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 16, (uint8_t)cpu->gpr[2]);  /* 0x800FE274: 0xA2020010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE274u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[3] + 52, 2, 0x8u);  /* 0x800FE278: 0x90620034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE278u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE27C: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE27Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE280u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 24;  /* 0x800FE280: 0x00021600 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE280u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;  /* 0x800FE284: 0x00021603 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE284u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE288u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 8, (uint16_t)cpu->gpr[2]);  /* 0x800FE288: 0xA6020008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE288u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[3] + 53, 3, 0x8u);  /* 0x800FE28C: 0x90630035 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE28Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE290u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 256;  /* 0x800FE290: 0x24020100 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE290u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE294u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 12, (uint16_t)cpu->gpr[2]);  /* 0x800FE294: 0xA602000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE294u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 240;  /* 0x800FE298: 0x240200F0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE298u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE29Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 14, (uint16_t)cpu->gpr[2]);  /* 0x800FE29C: 0xA602000E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE29Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] << 24;  /* 0x800FE2A0: 0x00031E00 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 24;  /* 0x800FE2A4: 0x00031E03 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE2B0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10008u);
#endif
    g_debug_last_store_pc = 0x800FE2ACu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 10, (uint16_t)cpu->gpr[3]);  /* 0x800FE2AC: 0xA603000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80029DC0u);
    cpu->pc = 0x80029DC0u; return;  /* CPS jal -> 0x80029DC0 */

block_800FE2B0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE2B0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE2B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE2B0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE2B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE2B4: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FE2B8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE2B8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE2B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE2B8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE2C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE2BC: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007CBD4u);
    cpu->pc = 0x8007CBD4u; return;  /* CPS jal -> 0x8007CBD4 */

block_800FE2C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE2C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE2C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE2C0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE2C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FE2C4: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C30Cu);
    cpu->pc = 0x8007C30Cu; return;  /* CPS jal -> 0x8007C30C */

block_800FE2C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE2C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE2C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE2C8u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2C8u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 62, 2, 0x40000u);  /* 0x800FE2C8: 0x8642003E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE2CC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE2D0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 3;  /* 0x800FE2D4: 0x24020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2D4u);
#endif
    if (_bc_800FE2D0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE2E0u);
        goto block_800FE2E0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE2D8u);
        goto block_800FE2D8;  /* not taken */
    }

block_800FE2D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE2D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE2D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE2D8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    g_debug_last_store_pc = 0x800FE2DCu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 80, (uint16_t)cpu->gpr[2]);  /* 0x800FE2DC: 0xA6420050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE784u);
    goto block_800FE784;  /* j */

block_800FE2E0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE2E0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE2E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE2E0u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2E0u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[19] + -8856, 2, 0x80000u);  /* 0x800FE2E0: 0x9662DD68 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2E0u);
#endif
    cpu->gpr[3] = psx_cyc_load_half(cpu, cpu->gpr[20] + 2, 3, 0x100000u);  /* 0x800FE2E4: 0x96830002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE2E8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];  /* 0x800FE2EC: 0x00431025 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x0800;  /* 0x800FE2F0: 0x30420800 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE2F4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FE2F8: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE2F8u);
#endif
    if (_bc_800FE2F4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE788u);
        goto block_800FE788;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE2FCu);
        goto block_800FE2FC;  /* not taken */
    }

block_800FE2FC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE2FCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE2FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE2FCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE2FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE304u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE300u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE300: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE300u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105560u);
    cpu->pc = 0x80105560u; return;  /* CPS jal -> 0x80105560 */

block_800FE304:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE304u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE304u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE304u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE304u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE30Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE308: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE308u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801057FCu);
    cpu->pc = 0x801057FCu; return;  /* CPS jal -> 0x801057FC */

block_800FE30C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE30Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE30Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE30Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE30Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE314u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE310u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE310: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE310u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80082B98u);
    cpu->pc = 0x80082B98u; return;  /* CPS jal -> 0x80082B98 */

block_800FE314:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE314u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE314u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE314u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE314u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE314: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE314u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE320u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = -1;  /* 0x800FE31C: 0x2405FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE31Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006BC98u);
    cpu->pc = 0x8006BC98u; return;  /* CPS jal -> 0x8006BC98 */

block_800FE320:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE320u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE320u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE320u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE320u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 60, 2, 0x40000u);  /* 0x800FE320: 0x8642003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE320u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE324: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE324u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE328 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE32C: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE32Cu);
#endif
    if (_bc_800FE328) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE34Cu);
        goto block_800FE34C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE330u);
        goto block_800FE330;  /* not taken */
    }

block_800FE330:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE330u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE330u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE330u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE330u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE338u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE334: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE334u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F660u);
    cpu->pc = 0x8008F660u; return;  /* CPS jal -> 0x8008F660 */

block_800FE338:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE338u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE338u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE338u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE338u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 9;  /* 0x800FE338: 0x24040009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE338u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE33C: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE33Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE340u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE348u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x61u);
#endif
    cpu->gpr[6] = cpu->gpr[5];  /* move */  /* 0x800FE344: 0x00A03021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE344u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090D88u);
    cpu->pc = 0x80090D88u; return;  /* CPS jal -> 0x80090D88 */

block_800FE348:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE348u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE348u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE348u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE348u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE348: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE348u);
#endif
    /* fall through to block_800FE34C */

block_800FE34C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE34Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE34Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE34Cu, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE34Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    g_debug_last_store_pc = 0x800FE34Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 80, (uint16_t)cpu->gpr[2]);  /* 0x800FE34C: 0xA6420050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE34Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE350u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -2;  /* 0x800FE350: 0x2402FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FE350u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40001u);
#endif
    g_debug_last_store_pc = 0x800FE354u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 70, (uint16_t)cpu->gpr[0]);  /* 0x800FE354: 0xA6400046 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE354u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40001u);
#endif
    g_debug_last_store_pc = 0x800FE358u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 68, (uint16_t)cpu->gpr[0]);  /* 0x800FE358: 0xA6400044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE358u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE360u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    g_debug_last_store_pc = 0x800FE360u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 78, (uint16_t)cpu->gpr[2]);  /* 0x800FE360: 0xA642004E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE360u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE784u);
    goto block_800FE784;  /* j */

block_800FE364:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE364u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE364u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE364u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE364u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE364: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE364u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[18] = cpu->gpr[2] + 17528;  /* 0x800FE368: 0x24524478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE368u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 16, 2, 0x40000u);  /* 0x800FE36C: 0x86420010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE36Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE370u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE370: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE370u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FE374: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE374u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[18];  /* 0x800FE378: 0x00521021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE378u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[2], 4, 0x4u);  /* 0x800FE37C: 0x8C440000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE37Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE380u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE388u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 3;  /* 0x800FE384: 0x24050003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE384u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801056C4u);
    cpu->pc = 0x801056C4u; return;  /* CPS jal -> 0x801056C4 */

block_800FE388:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE388u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE388u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE388u, 14u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE388u);
#endif
    cpu->gpr[5] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 60, 5, 0x40000u);  /* 0x800FE388: 0x8645003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE388u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 62, 2, 0x40000u);  /* 0x800FE38C: 0x8642003E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE38Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE390u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE390: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE390u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[5] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[5]));
#endif  /* 0x800FE394: 0x00A20018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE394u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 18, 2, 0x40000u);  /* 0x800FE398: 0x86420012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE398u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE39C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE39Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FE3A0: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[18];  /* 0x800FE3A4: 0x00521021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3A4u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[2] + 8, 4, 0x4u);  /* 0x800FE3A8: 0x8C440008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x800FE3AC: 0x00002812 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[5] >> 31;  /* 0x800FE3B0: 0x000517C2 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];  /* 0x800FE3B4: 0x00A22821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE3C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = (int32_t)cpu->gpr[5] >> 1;  /* 0x800FE3BC: 0x00052843 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105740u);
    cpu->pc = 0x80105740u; return;  /* CPS jal -> 0x80105740 */

block_800FE3C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE3C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE3C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE3C0u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40011u);
#endif
    cpu->gpr[4] = cpu->gpr[18];  /* move */  /* 0x800FE3C0: 0x02402021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3C0u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 16, 2, 0x40000u);  /* 0x800FE3C4: 0x86420010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE3C8: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)1) ? 1 : 0;  /* 0x800FE3CC: 0x2C420001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE3D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    g_debug_last_store_pc = 0x800FE3D4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 16, (uint16_t)cpu->gpr[2]);  /* 0x800FE3D4: 0xA6420010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEE00u);
    cpu->pc = 0x800FEE00u; return;  /* CPS jal -> 0x800FEE00 */

block_800FE3D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE3D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE3D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE3D8u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE3D8: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[17] = cpu->gpr[2] + 17400;  /* 0x800FE3DC: 0x245143F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3E0u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 66, 2, 0x20000u);  /* 0x800FE3E0: 0x86220042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE3E4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE3E8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE3EC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3ECu);
#endif
    if (_bc_800FE3E8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE3FCu);
        goto block_800FE3FC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE3F0u);
        goto block_800FE3F0;  /* not taken */
    }

block_800FE3F0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE3F0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE3F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE3F0u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3F0u);
#endif
    cpu->gpr[4] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 58, 4, 0x20000u);  /* 0x800FE3F0: 0x8624003A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE3FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE3F8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE3F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF8E0u);
    cpu->pc = 0x800FF8E0u; return;  /* CPS jal -> 0x800FF8E0 */

block_800FE3FC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE3FCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE3FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE3FCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE3FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE404u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE400u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE400: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE400u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105560u);
    cpu->pc = 0x80105560u; return;  /* CPS jal -> 0x80105560 */

block_800FE404:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE404u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE404u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE404u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE404u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE40Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE408: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE408u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801057FCu);
    cpu->pc = 0x801057FCu; return;  /* CPS jal -> 0x801057FC */

block_800FE40C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE40Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE40Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE40Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE40Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE414u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE410u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE410: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE410u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80082B98u);
    cpu->pc = 0x80082B98u; return;  /* CPS jal -> 0x80082B98 */

block_800FE414:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE414u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE414u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE414u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE414u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[17] + 68, 2, 0x20000u);  /* 0x800FE414: 0x96220044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE414u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE418: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE418u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -1;  /* 0x800FE41C: 0x2442FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE41Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE420u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)2) ? 1 : 0;  /* 0x800FE420: 0x2C420002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE420u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE424 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE428: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE428u);
#endif
    if (_bc_800FE424) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE4D8u);
        goto block_800FE4D8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE42Cu);
        goto block_800FE42C;  /* not taken */
    }

block_800FE42C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE42Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE42Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE42Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE42Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = -1;  /* 0x800FE42C: 0x2405FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE42Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE430u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 4;  /* 0x800FE430: 0x24020004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE430u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE43Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE438u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 80, (uint16_t)cpu->gpr[2]);  /* 0x800FE438: 0xA6220050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE438u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006BC98u);
    cpu->pc = 0x8006BC98u; return;  /* CPS jal -> 0x8006BC98 */

block_800FE43C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE43Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE43Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE43Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE43Cu);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 60, 2, 0x20000u);  /* 0x800FE43C: 0x8622003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE43Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE440u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE440: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE440u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE444 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE448: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE448u);
#endif
    if (_bc_800FE444) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE468u);
        goto block_800FE468;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE44Cu);
        goto block_800FE44C;  /* not taken */
    }

block_800FE44C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE44Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE44Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE44Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE44Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE454u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE450u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE450: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE450u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F660u);
    cpu->pc = 0x8008F660u; return;  /* CPS jal -> 0x8008F660 */

block_800FE454:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE454u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE454u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE454u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE454u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 9;  /* 0x800FE454: 0x24040009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE454u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE458: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE458u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE464u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE460u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x61u);
#endif
    cpu->gpr[6] = cpu->gpr[5];  /* move */  /* 0x800FE460: 0x00A03021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE460u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090D88u);
    cpu->pc = 0x80090D88u; return;  /* CPS jal -> 0x80090D88 */

block_800FE464:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE464u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE464u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE464u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE464u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE464: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE464u);
#endif
    /* fall through to block_800FE468 */

block_800FE468:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE468u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE468u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE468u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE468u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 180;  /* 0x800FE468: 0x240500B4 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE468u);
#endif
    cpu->gpr[16] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 56, 16, 0x20000u);  /* 0x800FE46C: 0x86300038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE46Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE470u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[2] = cpu->gpr[18] + 24;  /* 0x800FE470: 0x26420018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE470u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] << 3;  /* 0x800FE474: 0x001080C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE474u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE480u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[2];  /* 0x800FE47C: 0x02028021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE47Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006BC98u);
    cpu->pc = 0x8006BC98u; return;  /* CPS jal -> 0x8006BC98 */

block_800FE480:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE480u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE480u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE480u, 10u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE480u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8010 << 16;  /* 0x80100000 */  /* 0x800FE480: 0x3C038010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE480u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[16], 2, 0x10000u);  /* 0x800FE484: 0x96020000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE484u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    cpu->gpr[4] = cpu->gpr[3] + 24456;  /* 0x800FE488: 0x24645F88 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE488u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FE48Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + 24456, (uint16_t)cpu->gpr[2]);  /* 0x800FE48C: 0xA4625F88 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE48Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE490u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE490: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE490u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[18] = cpu->gpr[2] + 17592;  /* 0x800FE494: 0x245244B8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE494u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[16] + 2, 2, 0x10000u);  /* 0x800FE498: 0x96020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE498u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40021u);
#endif
    cpu->gpr[5] = cpu->gpr[18];  /* move */  /* 0x800FE49C: 0x02402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE49Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE4A8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FE4A4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 2, (uint16_t)cpu->gpr[2]);  /* 0x800FE4A4: 0xA4820002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C6B0u);
    cpu->pc = 0x8007C6B0u; return;  /* CPS jal -> 0x8007C6B0 */

block_800FE4A8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4A8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4A8u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4A8u);
#endif
    cpu->gpr[3] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 68, 3, 0x20000u);  /* 0x800FE4A8: 0x86230044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 2;  /* 0x800FE4AC: 0x24020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    int _bc_800FE4B0 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 124;  /* 0x800FE4B4: 0x2402007C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4B4u);
#endif
    if (_bc_800FE4B0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE4C4u);
        goto block_800FE4C4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE4B8u);
        goto block_800FE4B8;  /* not taken */
    }

block_800FE4B8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4B8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4B8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE4C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40011u);
#endif
    cpu->gpr[4] = cpu->gpr[18];  /* move */  /* 0x800FE4BC: 0x02402021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105444u);
    cpu->pc = 0x80105444u; return;  /* CPS jal -> 0x80105444 */

block_800FE4C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4C0u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 124;  /* 0x800FE4C0: 0x2402007C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4C0u);
#endif
    /* fall through to block_800FE4C4 */

block_800FE4C4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4C4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4C4u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE4C4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 72, (uint16_t)cpu->gpr[2]);  /* 0x800FE4C4: 0xA6220048 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE4D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 6;  /* 0x800FE4CC: 0x24040006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006AD28u);
    cpu->pc = 0x8006AD28u; return;  /* CPS jal -> 0x8006AD28 */

block_800FE4D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4D0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FE4D4: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE788u);
    goto block_800FE788;  /* j */

block_800FE4D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4D8u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4D8u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 70, 2, 0x20000u);  /* 0x800FE4D8: 0x86220046 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE4DC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE4E0 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE4E4: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4E4u);
#endif
    if (_bc_800FE4E0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE700u);
        goto block_800FE700;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE4E8u);
        goto block_800FE4E8;  /* not taken */
    }

block_800FE4E8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4E8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4E8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE4EC: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE6CCu);
    goto block_800FE6CC;  /* j */

block_800FE4F0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE4F0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE4F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE4F0u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE4F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE4F0: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[2] + 17400;  /* 0x800FE4F4: 0x244343F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4F4u);
#endif
    cpu->gpr[4] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 72, 4, 0x8u);  /* 0x800FE4F8: 0x84640048 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE4FC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE4FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE500u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < 64) ? 1 : 0;  /* 0x800FE500: 0x28820040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE500u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE504 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE508: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE508u);
#endif
    if (_bc_800FE504) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE518u);
        goto block_800FE518;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE50Cu);
        goto block_800FE50C;  /* not taken */
    }

block_800FE50C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE50Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE50Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE50Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE50Cu);
#endif
    cpu->gpr[5] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 68, 5, 0x8u);  /* 0x800FE50C: 0x84650044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE50Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE510u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE514: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE514u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE520u);
    goto block_800FE520;  /* j */

block_800FE518:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE518u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE518u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE518u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE518u);
#endif
    cpu->gpr[5] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 68, 5, 0x8u);  /* 0x800FE518: 0x84650044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE518u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 64;  /* 0x800FE51C: 0x24040040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE51Cu);
#endif
    /* fall through to block_800FE520 */

block_800FE520:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE520u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE520u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE520u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE520u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE528u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -1;  /* 0x800FE524: 0x24A5FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE524u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801053C0u);
    cpu->pc = 0x801053C0u; return;  /* CPS jal -> 0x801053C0 */

block_800FE528:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE528u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE528u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE528u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE528u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    cpu->gpr[16] = cpu->gpr[0];  /* move */  /* 0x800FE528: 0x00008021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE528u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8015 << 16;  /* 0x80150000 */  /* 0x800FE52C: 0x3C028015 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE52Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE530u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80004u);
#endif
    cpu->gpr[19] = cpu->gpr[2] + -14152;  /* 0x800FE530: 0x2453C8B8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE530u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8010 << 16;  /* 0x80100000 */  /* 0x800FE534: 0x3C028010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE534u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[18] = cpu->gpr[2] + 24464;  /* 0x800FE538: 0x24525F90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE538u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE53C: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE53Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE540u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100004u);
#endif
    cpu->gpr[20] = cpu->gpr[2] + 17400;  /* 0x800FE540: 0x245443F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE540u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE544: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE544u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[17] = cpu->gpr[2] + 17592;  /* 0x800FE548: 0x245144B8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE548u);
#endif
    /* fall through to block_800FE54C */

block_800FE54C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE54Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE54Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE54Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE54Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE54C: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE54Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE550u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE558u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80021u);
#endif
    cpu->gpr[5] = cpu->gpr[19];  /* move */  /* 0x800FE554: 0x02602821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE554u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105340u);
    cpu->pc = 0x80105340u; return;  /* CPS jal -> 0x80105340 */

block_800FE558:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE558u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE558u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE558u, 14u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE558u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40011u);
#endif
    cpu->gpr[4] = cpu->gpr[18];  /* move */  /* 0x800FE558: 0x02402021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE558u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80021u);
#endif
    cpu->gpr[5] = cpu->gpr[19];  /* move */  /* 0x800FE55C: 0x02602821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE55Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE560u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000u);
#endif
    cpu->gpr[17] = cpu->gpr[17] + 15360;  /* 0x800FE560: 0x26313C00 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE560u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10040u);
#endif
    cpu->gpr[6] = cpu->gpr[16] << 4;  /* 0x800FE564: 0x00103100 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE564u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8010 << 16;  /* 0x80100000 */  /* 0x800FE568: 0x3C038010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE568u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[20] + 56, 2, 0x100000u);  /* 0x800FE56C: 0x86820038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE56Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE570u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 24472;  /* 0x800FE570: 0x24635F98 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE570u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FE574: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE574u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FE578: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE578u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[2], 2, 0x4u);  /* 0x800FE57C: 0x94420000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE57Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE580u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + 1;  /* 0x800FE580: 0x26100001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE580u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x44u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[6];  /* 0x800FE584: 0x00461021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE584u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE590u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    g_debug_last_store_pc = 0x800FE58Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[18] + 2, (uint16_t)cpu->gpr[2]);  /* 0x800FE58C: 0xA6420002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE58Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C650u);
    cpu->pc = 0x8007C650u; return;  /* CPS jal -> 0x8007C650 */

block_800FE590:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE590u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE590u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE590u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE590u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[16] < 15) ? 1 : 0;  /* 0x800FE590: 0x2A02000F */
#ifdef PSX_COSIM
    cosim_instr(0x800FE590u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE594 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE598: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE598u);
#endif
    if (_bc_800FE594) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE54Cu);
        goto block_800FE54C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE59Cu);
        goto block_800FE59C;  /* not taken */
    }

block_800FE59C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE59Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE59Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE59Cu, 18u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE59Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[17] = cpu->gpr[2] + 17400;  /* 0x800FE59C: 0x245143F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE59Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE5A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    cpu->gpr[16] = cpu->gpr[29] + 24;  /* 0x800FE5A0: 0x27B00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE5A4: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5A4u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 56, 2, 0x20000u);  /* 0x800FE5A8: 0x86220038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 17528;  /* 0x800FE5AC: 0x24634478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE5B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)1) ? 1 : 0;  /* 0x800FE5B0: 0x2C420001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE5B4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 56, (uint16_t)cpu->gpr[2]);  /* 0x800FE5B4: 0xA6220038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 3;  /* 0x800FE5B8: 0x000210C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FE5BC: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE5C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE5C0: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 17496;  /* 0x800FE5C4: 0x24634458 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5C4u);
#endif
    cpu->gpr[5] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 24, 5, 0x4u);  /* 0x800FE5C8: 0x84450018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5C8u);
#endif
    cpu->gpr[6] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 26, 6, 0x4u);  /* 0x800FE5CC: 0x8446001A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE5D0u);
#endif
    cpu->gpr[7] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 8, 7, 0x8u);  /* 0x800FE5D0: 0x84670008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5D0u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 10, 2, 0x8u);  /* 0x800FE5D4: 0x8462000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE5D8: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE5E4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE5E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FE5E0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x800FE5E0: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007B90Cu);
    cpu->pc = 0x8007B90Cu; return;  /* CPS jal -> 0x8007B90C */

block_800FE5E4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE5E4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE5E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE5E4u, 19u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE5E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FE5E4: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8009 << 16;  /* 0x80090000 */  /* 0x800FE5E8: 0x3C038009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 32496;  /* 0x800FE5EC: 0x24637EF0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE5F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE5F0u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 17, (uint8_t)cpu->gpr[2]);  /* 0x800FE5F0: 0xA2020011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE5F4u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 16, (uint8_t)cpu->gpr[2]);  /* 0x800FE5F4: 0xA2020010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5F4u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[3] + 52, 2, 0x8u);  /* 0x800FE5F8: 0x90620034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE5FC: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE5FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE600u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 24;  /* 0x800FE600: 0x00021600 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE600u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 24;  /* 0x800FE604: 0x00021603 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE604u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE608u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 8, (uint16_t)cpu->gpr[2]);  /* 0x800FE608: 0xA6020008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE608u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[3] + 53, 3, 0x8u);  /* 0x800FE60C: 0x90630035 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE60Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE610u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 256;  /* 0x800FE610: 0x24020100 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE610u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE614u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 12, (uint16_t)cpu->gpr[2]);  /* 0x800FE614: 0xA602000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE614u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 240;  /* 0x800FE618: 0x240200F0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE618u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FE61Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 14, (uint16_t)cpu->gpr[2]);  /* 0x800FE61C: 0xA602000E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE61Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE620u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] << 24;  /* 0x800FE620: 0x00031E00 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE620u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = (int32_t)cpu->gpr[3] >> 24;  /* 0x800FE624: 0x00031E03 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE624u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE630u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10008u);
#endif
    g_debug_last_store_pc = 0x800FE62Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 10, (uint16_t)cpu->gpr[3]);  /* 0x800FE62C: 0xA603000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FE62Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80029DC0u);
    cpu->pc = 0x80029DC0u; return;  /* CPS jal -> 0x80029DC0 */

block_800FE630:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE630u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE630u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE630u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE630u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE638u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE634: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE634u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FE638:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE638u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE638u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE638u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE638u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE640u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE63C: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE63Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007CBD4u);
    cpu->pc = 0x8007CBD4u; return;  /* CPS jal -> 0x8007CBD4 */

block_800FE640:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE640u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE640u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE640u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE640u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FE640: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FE640u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[2] + -8856;  /* 0x800FE644: 0x2443DD68 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE644u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[2] + -8856, 2, 0x4u);  /* 0x800FE648: 0x9442DD68 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE648u);
#endif
    cpu->gpr[3] = psx_cyc_load_half(cpu, cpu->gpr[3] + 2, 3, 0x8u);  /* 0x800FE64C: 0x94630002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE64Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE650u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE650: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE650u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];  /* 0x800FE654: 0x00431025 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE654u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x0800;  /* 0x800FE658: 0x30420800 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE658u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE65C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE660u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE660: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE660u);
#endif
    if (_bc_800FE65C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE6ACu);
        goto block_800FE6AC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE664u);
        goto block_800FE664;  /* not taken */
    }

block_800FE664:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE664u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE664u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE664u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE664u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE66Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = -1;  /* 0x800FE668: 0x2405FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE668u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006BC98u);
    cpu->pc = 0x8006BC98u; return;  /* CPS jal -> 0x8006BC98 */

block_800FE66C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE66Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE66Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE66Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE66Cu);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 60, 2, 0x20000u);  /* 0x800FE66C: 0x8622003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE66Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE670u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE670: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE670u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE674 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE678: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE678u);
#endif
    if (_bc_800FE674) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE698u);
        goto block_800FE698;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE67Cu);
        goto block_800FE67C;  /* not taken */
    }

block_800FE67C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE67Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE67Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE67Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE67Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE684u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE680u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE680: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE680u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F660u);
    cpu->pc = 0x8008F660u; return;  /* CPS jal -> 0x8008F660 */

block_800FE684:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE684u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE684u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE684u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE684u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 9;  /* 0x800FE684: 0x24040009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE684u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE688: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE688u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE694u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE690u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x61u);
#endif
    cpu->gpr[6] = cpu->gpr[5];  /* move */  /* 0x800FE690: 0x00A03021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE690u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090D88u);
    cpu->pc = 0x80090D88u; return;  /* CPS jal -> 0x80090D88 */

block_800FE694:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE694u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE694u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE694u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE694u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE694: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE694u);
#endif
    /* fall through to block_800FE698 */

block_800FE698:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE698u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE698u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE698u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE698u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE698u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 80, (uint16_t)cpu->gpr[2]);  /* 0x800FE698: 0xA6220050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE698u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -2;  /* 0x800FE69C: 0x2402FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FE69Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20001u);
#endif
    g_debug_last_store_pc = 0x800FE6A0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 68, (uint16_t)cpu->gpr[0]);  /* 0x800FE6A0: 0xA6200044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE6A8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 78, (uint16_t)cpu->gpr[2]);  /* 0x800FE6A8: 0xA622004E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE784u);
    goto block_800FE784;  /* j */

block_800FE6AC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE6ACu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE6ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE6ACu, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6ACu);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[17] + 72, 2, 0x20000u);  /* 0x800FE6AC: 0x96220048 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE6B0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -2;  /* 0x800FE6B4: 0x2442FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE6B8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 72, (uint16_t)cpu->gpr[2]);  /* 0x800FE6B8: 0xA6220048 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 16;  /* 0x800FE6BC: 0x00021400 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    int _bc_800FE6C0 = ((int32_t)cpu->gpr[2] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FE6C4: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6C4u);
#endif
    if (_bc_800FE6C0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE788u);
        goto block_800FE788;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE6C8u);
        goto block_800FE6C8;  /* not taken */
    }

block_800FE6C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE6C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE6C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE6C8u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE6C8: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6C8u);
#endif
    /* fall through to block_800FE6CC */

block_800FE6CC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE6CCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE6CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE6CCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE6D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = -1;  /* 0x800FE6D0: 0x2405FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006BC98u);
    cpu->pc = 0x8006BC98u; return;  /* CPS jal -> 0x8006BC98 */

block_800FE6D4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE6D4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE6D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE6D4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6D4u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 60, 2, 0x20000u);  /* 0x800FE6D4: 0x8622003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE6D8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE6DC = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE6E0: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6E0u);
#endif
    if (_bc_800FE6DC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE700u);
        goto block_800FE700;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE6E4u);
        goto block_800FE6E4;  /* not taken */
    }

block_800FE6E4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE6E4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE6E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE6E4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE6ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE6E8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F660u);
    cpu->pc = 0x8008F660u; return;  /* CPS jal -> 0x8008F660 */

block_800FE6EC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE6ECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE6ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE6ECu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 9;  /* 0x800FE6EC: 0x24040009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE6F0: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE6FCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x61u);
#endif
    cpu->gpr[6] = cpu->gpr[5];  /* move */  /* 0x800FE6F8: 0x00A03021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090D88u);
    cpu->pc = 0x80090D88u; return;  /* CPS jal -> 0x80090D88 */

block_800FE6FC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE6FCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE6FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE6FCu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE6FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FE6FC: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE6FCu);
#endif
    /* fall through to block_800FE700 */

block_800FE700:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE700u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE700u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE700u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE700u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE700u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 80, (uint16_t)cpu->gpr[2]);  /* 0x800FE700: 0xA6220050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE700u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FE704: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE704u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FE70Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 78, (uint16_t)cpu->gpr[2]);  /* 0x800FE70C: 0xA622004E */
#ifdef PSX_COSIM
    cosim_instr(0x800FE70Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE784u);
    goto block_800FE784;  /* j */

block_800FE710:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE710u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE710u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE710u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE710u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE710: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE710u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[16] = cpu->gpr[2] + 17400;  /* 0x800FE714: 0x245043F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE714u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE718: 0x3C048011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE718u);
#endif
    cpu->gpr[5] = psx_cyc_load_half(cpu, cpu->gpr[16] + 76, 5, 0x10000u);  /* 0x800FE71C: 0x9605004C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE71Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE720u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE728u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 17528;  /* 0x800FE724: 0x24844478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE724u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FDA2Cu);
    cpu->pc = 0x800FDA2Cu; return;  /* CPS jal -> 0x800FDA2C */

block_800FE728:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE728u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE728u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE728u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE728u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 70, 2, 0x10000u);  /* 0x800FE728: 0x86020046 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE728u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE72C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE72Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE730u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE730 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FE734u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 80, (uint16_t)cpu->gpr[0]);  /* 0x800FE734: 0xA6000050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE734u);
#endif
    if (_bc_800FE730) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE760u);
        goto block_800FE760;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE738u);
        goto block_800FE738;  /* not taken */
    }

block_800FE738:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE738u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE738u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE738u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE738u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE740u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE73C: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE73Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090C0Cu);
    cpu->pc = 0x80090C0Cu; return;  /* CPS jal -> 0x80090C0C */

block_800FE740:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE740u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE740u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10010u);
#endif
    cpu->gpr[4] = cpu->gpr[16] + 24;  /* 0x800FE740: 0x26040018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE74Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10005u);
#endif
    cpu->gpr[16] = cpu->gpr[2];  /* move */  /* 0x800FE748: 0x00408021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE748u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090C0Cu);
    cpu->pc = 0x80090C0Cu; return;  /* CPS jal -> 0x80090C0C */

block_800FE74C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE74Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE74Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE74Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE74Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE74C: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE74Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE750u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE758u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x25u);
#endif
    cpu->gpr[5] = cpu->gpr[2];  /* move */  /* 0x800FE754: 0x00402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE754u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B524u);
    cpu->pc = 0x8006B524u; return;  /* CPS jal -> 0x8006B524 */

block_800FE758:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE758u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE758u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE758u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE758u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 7;  /* 0x800FE75C: 0x24040007 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE75Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE764u);
    goto block_800FE764;  /* j */

block_800FE760:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE760u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE760u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE760u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE760u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FE760: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE760u);
#endif
    /* fall through to block_800FE764 */

block_800FE764:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE764u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE764u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE764u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE764u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE76Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE768: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE768u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006AD28u);
    cpu->pc = 0x8006AD28u; return;  /* CPS jal -> 0x8006AD28 */

block_800FE76C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE76Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE76Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE76Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE76Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE76C: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE76Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE770u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 17478, 2, 0x4u);  /* 0x800FE770: 0x84424446 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE770u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE778: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE778u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FE788u);
    goto block_800FE788;  /* j */

block_800FE77C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE77Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE77Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE77Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE77Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE784u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE780u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FE780: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE780u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007AE58u);
    cpu->pc = 0x8007AE58u; return;  /* CPS jal -> 0x8007AE58 */

block_800FE784:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE784u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE784u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE784u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE784u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FE784: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE784u);
#endif
    /* fall through to block_800FE788 */

block_800FE788:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE788u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE788u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE788u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE788u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 68, 31, 0x20000000u);  /* 0x800FE788: 0x8FBF0044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE788u);
#endif
    cpu->gpr[20] = psx_cyc_load_word(cpu, cpu->gpr[29] + 64, 20, 0x20000000u);  /* 0x800FE78C: 0x8FB40040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE78Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE790u);
#endif
    cpu->gpr[19] = psx_cyc_load_word(cpu, cpu->gpr[29] + 60, 19, 0x20000000u);  /* 0x800FE790: 0x8FB3003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE790u);
#endif
    cpu->gpr[18] = psx_cyc_load_word(cpu, cpu->gpr[29] + 56, 18, 0x20000000u);  /* 0x800FE794: 0x8FB20038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE794u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 52, 17, 0x20000000u);  /* 0x800FE798: 0x8FB10034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE798u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 48, 16, 0x20000000u);  /* 0x800FE79C: 0x8FB00030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE79Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE7A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 72;  /* 0x800FE7A0: 0x27BD0048 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE7A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FE7A4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE7A8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE7A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FE7A4);
    cpu->pc = _jt_800FE7A4; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FE984(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FE9A4u: goto block_800FE9A4;
            case 0x800FE9ACu: goto block_800FE9AC;
            case 0x800FE9BCu: goto block_800FE9BC;
            case 0x800FE9D4u: goto block_800FE9D4;
            case 0x800FE9DCu: goto block_800FE9DC;
            case 0x800FE9E8u: goto block_800FE9E8;
            case 0x800FE9E0u: goto block_800FE9E0;
            case 0x800FE9F0u: goto block_800FE9F0;
            case 0x800FE984u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FE984u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FE984u);
    /* Address: 0x800FE984, Size: 132 bytes, Blocks: 9 */

block_800FE984:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE984u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE984u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE984u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE984u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -40;  /* 0x800FE984: 0x27BDFFD8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE984u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x800FE988u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[17]);  /* 0x800FE988: 0xAFB1001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE988u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[17] = cpu->gpr[4];  /* move */  /* 0x800FE98C: 0x00808821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE98Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE990u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FE990u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[16]);  /* 0x800FE990: 0xAFB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE990u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10021u);
#endif
    cpu->gpr[16] = cpu->gpr[5];  /* move */  /* 0x800FE994: 0x00A08021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE994u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FE998u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[31]);  /* 0x800FE998: 0xAFBF0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE998u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE9A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FE9A0: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105560u);
    cpu->pc = 0x80105560u; return;  /* CPS jal -> 0x80105560 */

block_800FE9A4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9A4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9A4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE9ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FE9A8: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801057FCu);
    cpu->pc = 0x801057FCu; return;  /* CPS jal -> 0x801057FC */

block_800FE9AC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9ACu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9ACu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FE9AC: 0x3C048011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 17592;  /* 0x800FE9B0: 0x248444B8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE9BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 32;  /* 0x800FE9B8: 0x24050020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80082B08u);
    cpu->pc = 0x80082B08u; return;  /* CPS jal -> 0x80082B08 */

block_800FE9BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9BCu, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FE9BC: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FE9C0: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x41u);
#endif
    cpu->gpr[6] = -1;  /* 0x800FE9C4: 0x2406FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA1u);
#endif
    cpu->gpr[7] = cpu->gpr[5];  /* move */  /* 0x800FE9C8: 0x00A03821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE9D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000001u);
#endif
    g_debug_last_store_pc = 0x800FE9D0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[0]);  /* 0x800FE9D0: 0xAFA00010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80084B68u);
    cpu->pc = 0x80084B68u; return;  /* CPS jal -> 0x80084B68 */

block_800FE9D4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9D4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9D4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE9DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FE9D8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80082B38u);
    cpu->pc = 0x80082B38u; return;  /* CPS jal -> 0x80082B38 */

block_800FE9DC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9DCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9DCu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE9DC: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9DCu);
#endif
    /* fall through to block_800FE9E0 */

block_800FE9E0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9E0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9E0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FE9E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 448;  /* 0x800FE9E4: 0x240501C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091458u);
    cpu->pc = 0x80091458u; return;  /* CPS jal -> 0x80091458 */

block_800FE9E8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9E8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9E8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FE9E8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FE9EC: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9ECu);
#endif
    if (_bc_800FE9E8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE9E0u);
        goto block_800FE9E0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FE9F0u);
        goto block_800FE9F0;  /* not taken */
    }

block_800FE9F0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FE9F0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FE9F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FE9F0u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FE9F0u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 31, 0x20000000u);  /* 0x800FE9F0: 0x8FBF0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9F0u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 17, 0x20000000u);  /* 0x800FE9F4: 0x8FB1001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9F4u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 16, 0x20000000u);  /* 0x800FE9F8: 0x8FB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 40;  /* 0x800FE9FC: 0x27BD0028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FE9FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEA00 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEA04: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEA00);
    cpu->pc = _jt_800FEA00; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FEA08(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FEA2Cu: goto block_800FEA2C;
            case 0x800FEB18u: goto block_800FEB18;
            case 0x800FEB8Cu: goto block_800FEB8C;
            case 0x800FEBE0u: goto block_800FEBE0;
            case 0x800FEA24u: goto block_800FEA24;
            case 0x800FEA30u: goto block_800FEA30;
            case 0x800FEA6Cu: goto block_800FEA6C;
            case 0x800FEAA0u: goto block_800FEAA0;
            case 0x800FEABCu: goto block_800FEABC;
            case 0x800FEAE0u: goto block_800FEAE0;
            case 0x800FEAE8u: goto block_800FEAE8;
            case 0x800FEAECu: goto block_800FEAEC;
            case 0x800FEAFCu: goto block_800FEAFC;
            case 0x800FEB60u: goto block_800FEB60;
            case 0x800FEB94u: goto block_800FEB94;
            case 0x800FEBC8u: goto block_800FEBC8;
            case 0x800FEA08u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FEA08u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FEA08u);
    /* Address: 0x800FEA08, Size: 492 bytes, Blocks: 17 */

block_800FEA08:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEA08u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEA08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEA08u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -32;  /* 0x800FEA08: 0x27BDFFE0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FEA0Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[16]);  /* 0x800FEA0C: 0xAFB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FEA10: 0x3C10800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA10u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[16] + -28128, 2, 0x10000u);  /* 0x800FEA14: 0x8E029220 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEA18: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEA1C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FEA20u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[31]);  /* 0x800FEA20: 0xAFBF001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA20u);
#endif
    if (_bc_800FEA1C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEA30u);
        goto block_800FEA30;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEA24u);
        goto block_800FEA24;  /* not taken */
    }

block_800FEA24:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEA24u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEA24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEA24u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEA2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEA28: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80084DC8u);
    cpu->pc = 0x80084DC8u; return;  /* CPS jal -> 0x80084DC8 */

block_800FEA2C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEA2Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEA2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEA2Cu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FEA2Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[16] + -28128, cpu->gpr[0]);  /* 0x800FEA2C: 0xAE009220 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA2Cu);
#endif
    /* fall through to block_800FEA30 */

block_800FEA30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEA30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEA30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEA30u, 15u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEA30: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x44u);
#endif
    cpu->gpr[6] = cpu->gpr[2] + 17528;  /* 0x800FEA34: 0x24464478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA34u);
#endif
    cpu->gpr[2] = psx_lwl(cpu, cpu->gpr[6] + 59, cpu->gpr[2], 2, 0x40u);  /* 0x800FEA38: 0x88C2003B */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA38u);
#endif
    cpu->gpr[2] = psx_lwr(cpu, cpu->gpr[6] + 56, cpu->gpr[2], 2, 0x40u);  /* 0x800FEA3C: 0x98C20038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA40u);
#endif
    cpu->gpr[3] = psx_lwl(cpu, cpu->gpr[6] + 63, cpu->gpr[3], 3, 0x40u);  /* 0x800FEA40: 0x88C3003F */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA40u);
#endif
    cpu->gpr[3] = psx_lwr(cpu, cpu->gpr[6] + 60, cpu->gpr[3], 3, 0x40u);  /* 0x800FEA44: 0x98C3003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FEA48u; psx_swl(cpu, cpu->gpr[29] + 19, cpu->gpr[2]);  /* 0x800FEA48: 0xABA20013 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FEA4Cu; psx_swr(cpu, cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x800FEA4C: 0xBBA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000008u);
#endif
    g_debug_last_store_pc = 0x800FEA50u; psx_swl(cpu, cpu->gpr[29] + 23, cpu->gpr[3]);  /* 0x800FEA50: 0xABA30017 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000008u);
#endif
    g_debug_last_store_pc = 0x800FEA54u; psx_swr(cpu, cpu->gpr[29] + 20, cpu->gpr[3]);  /* 0x800FEA54: 0xBBA30014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEA58: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA58u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 17484, 2, 0x4u);  /* 0x800FEA5C: 0x8442444C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA60u);
#endif
    cpu->gpr[16] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[6] + 18, 16, 0x40u);  /* 0x800FEA60: 0x84D00012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEA64 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FEA68: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA68u);
#endif
    if (_bc_800FEA64) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEAE8u);
        goto block_800FEAE8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEA6Cu);
        goto block_800FEA6C;  /* not taken */
    }

block_800FEA6C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEA6Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEA6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEA6Cu, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA6Cu);
#endif
    cpu->gpr[4] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[29] + 20, 4, 0x20000000u);  /* 0x800FEA6C: 0x87A40014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA70u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[29] + 22, 2, 0x20000000u);  /* 0x800FEA70: 0x87A20016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEA74: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[4] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[4]));
#endif  /* 0x800FEA78: 0x00820018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = cpu->gpr[16] << 2;  /* 0x800FEA7C: 0x00101080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x44u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[6];  /* 0x800FEA80: 0x00461021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] << 4;  /* 0x800FEA84: 0x00042100 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA84u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 8, 2, 0x4u);  /* 0x800FEA88: 0x8C420008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x800FEA8C: 0x00004012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEA90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x108u);
#endif
    cpu->gpr[3] = (uint32_t)cpu->gpr[8] >> 1;  /* 0x800FEA90: 0x00081842 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] << 2;  /* 0x800FEA94: 0x00031880 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    int _bc_800FEA98 = ((int32_t)cpu->gpr[4] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FEA9C: 0x00431821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEA9Cu);
#endif
    if (_bc_800FEA98) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEABCu);
        goto block_800FEABC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEAA0u);
        goto block_800FEAA0;  /* not taken */
    }

block_800FEAA0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEAA0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEAA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEAA0u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    g_debug_last_store_pc = 0x800FEAA0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[0]);  /* 0x800FEAA0: 0xAC600000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAA0u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[29] + 20, 2, 0x20000000u);  /* 0x800FEAA4: 0x87A20014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 1;  /* 0x800FEAA8: 0x24A50001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 4;  /* 0x800FEAAC: 0x00021100 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[5] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x800FEAB0: 0x00A2102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEAB4 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 4;  /* 0x800FEAB8: 0x24630004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAB8u);
#endif
    if (_bc_800FEAB4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEAA0u);
        goto block_800FEAA0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEABCu);
        goto block_800FEABC;  /* not taken */
    }

block_800FEABC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEABCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEABCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEABCu, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEABCu);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[29] + 22, 2, 0x20000000u);  /* 0x800FEABC: 0x97A20016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEABCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEAC0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + 32;  /* 0x800FEAC4: 0x24420020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FEAC8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[29] + 22, (uint16_t)cpu->gpr[2]);  /* 0x800FEAC8: 0xA7A20016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 16;  /* 0x800FEACC: 0x00021400 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEACCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;  /* 0x800FEAD0: 0x00021403 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 240) ? 1 : 0;  /* 0x800FEAD4: 0x284200F0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEAD8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEADC: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEADCu);
#endif
    if (_bc_800FEAD8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEAECu);
        goto block_800FEAEC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEAE0u);
        goto block_800FEAE0;  /* not taken */
    }

block_800FEAE0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEAE0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEAE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEAE0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 240;  /* 0x800FEAE0: 0x240200F0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FEAE4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[29] + 22, (uint16_t)cpu->gpr[2]);  /* 0x800FEAE4: 0xA7A20016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAE4u);
#endif
    /* fall through to block_800FEAE8 */

block_800FEAE8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEAE8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEAE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEAE8u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEAE8: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAE8u);
#endif
    /* fall through to block_800FEAEC */

block_800FEAEC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEAECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEAECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEAECu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAECu);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 17466, 2, 0x4u);  /* 0x800FEAEC: 0x8442443A */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEAF0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEAF4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEAF8: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAF8u);
#endif
    if (_bc_800FEAF4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEB18u);
        goto block_800FEB18;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEAFCu);
        goto block_800FEAFC;  /* not taken */
    }

block_800FEAFC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEAFCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEAFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEAFCu, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEAFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 17528;  /* 0x800FEAFC: 0x24634478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEAFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = cpu->gpr[16] << 2;  /* 0x800FEB00: 0x00101080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FEB04: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB04u);
#endif
    cpu->gpr[4] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 56, 4, 0x8u);  /* 0x800FEB08: 0x84640038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB08u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[2] + 8, 5, 0x4u);  /* 0x800FEB0C: 0x8C450008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEB18u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEB14: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FFC24u);
    cpu->pc = 0x800FFC24u; return;  /* CPS jal -> 0x800FFC24 */

block_800FEB18:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEB18u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEB18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEB18u, 18u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEB18: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[4] = cpu->gpr[2] + 17528;  /* 0x800FEB1C: 0x24444478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB20u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[4] + 18, 2, 0x10u);  /* 0x800FEB20: 0x84820012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB20u);
#endif
    cpu->gpr[5] = psx_cyc_load_half(cpu, cpu->gpr[4] + 60, 5, 0x10u);  /* 0x800FEB24: 0x9485003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB24u);
#endif
    cpu->gpr[6] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[4] + 20, 6, 0x10u);  /* 0x800FEB28: 0x84860014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    cpu->gpr[7] = (cpu->gpr[2] < (uint32_t)1) ? 1 : 0;  /* 0x800FEB2C: 0x2C470001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x48u);
#endif
    cpu->gpr[3] = cpu->gpr[6] << 3;  /* 0x800FEB30: 0x000618C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB30u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[4] + 56, 2, 0x10u);  /* 0x800FEB34: 0x94820038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[4];  /* 0x800FEB38: 0x00641821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x90u);
#endif
    g_debug_last_store_pc = 0x800FEB3Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 18, (uint16_t)cpu->gpr[7]);  /* 0x800FEB3C: 0xA4870012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[5];  /* 0x800FEB40: 0x00451021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FEB44u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 56, (uint16_t)cpu->gpr[2]);  /* 0x800FEB44: 0xA4820038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 16;  /* 0x800FEB48: 0x00021400 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB48u);
#endif
    cpu->gpr[3] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 44, 3, 0x8u);  /* 0x800FEB4C: 0x8463002C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 16;  /* 0x800FEB50: 0x00021403 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x800FEB54: 0x0043102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEB58 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] << 16;  /* 0x800FEB5C: 0x00052C00 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB5Cu);
#endif
    if (_bc_800FEB58) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEB94u);
        goto block_800FEB94;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEB60u);
        goto block_800FEB60;  /* not taken */
    }

block_800FEB60:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEB60u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEB60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEB60u, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB60u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[4] + 62, 2, 0x10u);  /* 0x800FEB60: 0x8482003E */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = (int32_t)cpu->gpr[5] >> 16;  /* 0x800FEB64: 0x00052C03 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[5] * (int64_t)(int32_t)cpu->gpr[2]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[5]));
#endif  /* 0x800FEB68: 0x00A20018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    cpu->gpr[2] = cpu->gpr[7] << 2;  /* 0x800FEB6C: 0x00071080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];  /* 0x800FEB70: 0x00441021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB70u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[2] + 8, 4, 0x4u);  /* 0x800FEB74: 0x8C440008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x800FEB78: 0x00002812 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[5] >> 31;  /* 0x800FEB7C: 0x000517C2 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];  /* 0x800FEB80: 0x00A22821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEB8Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = (int32_t)cpu->gpr[5] >> 1;  /* 0x800FEB88: 0x00052843 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105740u);
    cpu->pc = 0x80105740u; return;  /* CPS jal -> 0x80105740 */

block_800FEB8C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEB8Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEB8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEB8Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEB90: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEBC8u);
    goto block_800FEBC8;  /* j */

block_800FEB94:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEB94u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEB94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEB94u, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEB94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FEB94: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FEB98u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 22, (uint16_t)cpu->gpr[2]);  /* 0x800FEB98: 0xA4820016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x44u);
#endif
    cpu->gpr[2] = (cpu->gpr[6] < (uint32_t)1) ? 1 : 0;  /* 0x800FEB9C: 0x2CC20001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEB9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FEBA0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 20, (uint16_t)cpu->gpr[2]);  /* 0x800FEBA0: 0xA4820014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 3;  /* 0x800FEBA4: 0x000210C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];  /* 0x800FEBA8: 0x00441021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBA8u);
#endif
    cpu->gpr[3] = psx_cyc_load_half(cpu, cpu->gpr[2] + 40, 3, 0x4u);  /* 0x800FEBAC: 0x94430028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEBB0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    g_debug_last_store_pc = 0x800FEBB4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 56, (uint16_t)cpu->gpr[3]);  /* 0x800FEBB4: 0xA4830038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBB4u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[2] + 42, 2, 0x4u);  /* 0x800FEBB8: 0x9442002A */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEBBC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FEBC0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 58, (uint16_t)cpu->gpr[2]);  /* 0x800FEBC0: 0xA482003A */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEBC4: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBC4u);
#endif
    /* fall through to block_800FEBC8 */

block_800FEBC8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEBC8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEBC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEBC8u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 17528;  /* 0x800FEBC8: 0x24634478 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = cpu->gpr[16] << 2;  /* 0x800FEBCC: 0x00101080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FEBD0: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBD0u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[2] + 8, 5, 0x4u);  /* 0x800FEBD4: 0x8C450008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEBE0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 16;  /* 0x800FEBDC: 0x27A40010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C650u);
    cpu->pc = 0x8007C650u; return;  /* CPS jal -> 0x8007C650 */

block_800FEBE0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEBE0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEBE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEBE0u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBE0u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 31, 0x20000000u);  /* 0x800FEBE0: 0x8FBF001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBE0u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 16, 0x20000000u);  /* 0x800FEBE4: 0x8FB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x800FEBE8: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEBEC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEBF0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEBEC);
    cpu->pc = _jt_800FEBEC; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FEBF4(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FEC18u: goto block_800FEC18;
            case 0x800FEC2Cu: goto block_800FEC2C;
            case 0x800FEC40u: goto block_800FEC40;
            case 0x800FEC8Cu: goto block_800FEC8C;
            case 0x800FEC94u: goto block_800FEC94;
            case 0x800FEC24u: goto block_800FEC24;
            case 0x800FEC38u: goto block_800FEC38;
            case 0x800FEC50u: goto block_800FEC50;
            case 0x800FEC60u: goto block_800FEC60;
            case 0x800FEC68u: goto block_800FEC68;
            case 0x800FEC98u: goto block_800FEC98;
            case 0x800FEBF4u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FEBF4u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FEBF4u);
    /* Address: 0x800FEBF4, Size: 196 bytes, Blocks: 12 */

block_800FEBF4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEBF4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEBF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEBF4u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -40;  /* 0x800FEBF4: 0x27BDFFD8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x800FEBF8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);  /* 0x800FEBF8: 0xAFB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[17] = cpu->gpr[4];  /* move */  /* 0x800FEBFC: 0x00808821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x800FEC00: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FEC04u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[31]);  /* 0x800FEC04: 0xAFBF0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20080000u);
#endif
    g_debug_last_store_pc = 0x800FEC08u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[19]);  /* 0x800FEC08: 0xAFB3001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20040000u);
#endif
    g_debug_last_store_pc = 0x800FEC0Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);  /* 0x800FEC0C: 0xAFB20018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEC18u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FEC14u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);  /* 0x800FEC14: 0xAFB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FEC18:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC18u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC18u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40005u);
#endif
    cpu->gpr[18] = cpu->gpr[2];  /* move */  /* 0x800FEC18: 0x00409021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEC1C: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80004u);
#endif
    cpu->gpr[19] = cpu->gpr[2] + 17400;  /* 0x800FEC20: 0x245343F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC20u);
#endif
    /* fall through to block_800FEC24 */

block_800FEC24:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC24u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC24u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEC2Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FEC28: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FECB8u);
    cpu->pc = 0x800FECB8u; return;  /* CPS jal -> 0x800FECB8 */

block_800FEC2C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC2Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC2Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10005u);
#endif
    cpu->gpr[16] = cpu->gpr[2];  /* move */  /* 0x800FEC2C: 0x00408021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    int _bc_800FEC30 = (cpu->gpr[16] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FEC34: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC34u);
#endif
    if (_bc_800FEC30) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEC68u);
        goto block_800FEC68;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEC38u);
        goto block_800FEC38;  /* not taken */
    }

block_800FEC38:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC38u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC38u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEC40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x800FEC3C: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FEC40:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC40u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC40u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[18];  /* 0x800FEC40: 0x00521023 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 600) ? 1 : 0;  /* 0x800FEC44: 0x28420258 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEC48 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x800FEC4C: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC4Cu);
#endif
    if (_bc_800FEC48) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEC98u);
        goto block_800FEC98;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEC50u);
        goto block_800FEC50;  /* not taken */
    }

block_800FEC50:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC50u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC50u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC50u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[19] + 60, 2, 0x80000u);  /* 0x800FEC50: 0x8662003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEC54: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEC58 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x800FEC5C: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC5Cu);
#endif
    if (_bc_800FEC58) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEC24u);
        goto block_800FEC24;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEC60u);
        goto block_800FEC60;  /* not taken */
    }

block_800FEC60:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC60u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC60u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEC64: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEC98u);
    goto block_800FEC98;  /* j */

block_800FEC68:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC68u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC68u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC68u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[17] + 16, 2, 0x20000u);  /* 0x800FEC68: 0x86220010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = 0x8016 << 16;  /* 0x80160000 */  /* 0x800FEC6C: 0x3C068016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)1) ? 1 : 0;  /* 0x800FEC70: 0x2C420001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x800FEC74u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[17] + 16, (uint16_t)cpu->gpr[2]);  /* 0x800FEC74: 0xA6220010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FEC78: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[17];  /* 0x800FEC7C: 0x00511021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC80u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[2], 5, 0x4u);  /* 0x800FEC80: 0x8C450000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEC8Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = cpu->gpr[6] + 19640;  /* 0x800FEC88: 0x24C64CB8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105C20u);
    cpu->pc = 0x80105C20u; return;  /* CPS jal -> 0x80105C20 */

block_800FEC8C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC8Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC8Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEC94u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FEC90: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80084BF8u);
    cpu->pc = 0x80084BF8u; return;  /* CPS jal -> 0x80084BF8 */

block_800FEC94:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC94u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC94u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FEC94: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC94u);
#endif
    /* fall through to block_800FEC98 */

block_800FEC98:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEC98u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEC98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEC98u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEC98u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 31, 0x20000000u);  /* 0x800FEC98: 0x8FBF0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC98u);
#endif
    cpu->gpr[19] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 19, 0x20000000u);  /* 0x800FEC9C: 0x8FB3001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEC9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECA0u);
#endif
    cpu->gpr[18] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 18, 0x20000000u);  /* 0x800FECA0: 0x8FB20018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECA0u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 17, 0x20000000u);  /* 0x800FECA4: 0x8FB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECA4u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 16, 0x20000000u);  /* 0x800FECA8: 0x8FB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 40;  /* 0x800FECAC: 0x27BD0028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FECB0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FECB4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FECB0);
    cpu->pc = _jt_800FECB0; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FECB8(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FECD0u: goto block_800FECD0;
            case 0x800FECE8u: goto block_800FECE8;
            case 0x800FECF8u: goto block_800FECF8;
            case 0x800FED10u: goto block_800FED10;
            case 0x800FED8Cu: goto block_800FED8C;
            case 0x800FED98u: goto block_800FED98;
            case 0x800FECE0u: goto block_800FECE0;
            case 0x800FECF0u: goto block_800FECF0;
            case 0x800FED04u: goto block_800FED04;
            case 0x800FED08u: goto block_800FED08;
            case 0x800FED20u: goto block_800FED20;
            case 0x800FED30u: goto block_800FED30;
            case 0x800FED38u: goto block_800FED38;
            case 0x800FED58u: goto block_800FED58;
            case 0x800FED6Cu: goto block_800FED6C;
            case 0x800FED84u: goto block_800FED84;
            case 0x800FED90u: goto block_800FED90;
            case 0x800FEDA0u: goto block_800FEDA0;
            case 0x800FEDA8u: goto block_800FEDA8;
            case 0x800FEDB0u: goto block_800FEDB0;
            case 0x800FEDE8u: goto block_800FEDE8;
            case 0x800FECB8u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FECB8u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FECB8u);
    /* Address: 0x800FECB8, Size: 328 bytes, Blocks: 22 */

block_800FECB8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FECB8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FECB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FECB8u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -40;  /* 0x800FECB8: 0x27BDFFD8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x800FECBC: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FECBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FECC0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[31]);  /* 0x800FECC0: 0xAFBF0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x800FECC4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[17]);  /* 0x800FECC4: 0xAFB1001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FECC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FECD0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FECCCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[16]);  /* 0x800FECCC: 0xAFB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FECD0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FECD0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FECD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FECD0u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20005u);
#endif
    cpu->gpr[17] = cpu->gpr[2];  /* move */  /* 0x800FECD0: 0x00408821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FECD4: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[16] = cpu->gpr[2] + 17400;  /* 0x800FECD8: 0x245043F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 16;  /* 0x800FECDC: 0x27A40010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECDCu);
#endif
    /* fall through to block_800FECE0 */

block_800FECE0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FECE0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FECE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FECE0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FECE8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000020u);
#endif
    cpu->gpr[5] = cpu->gpr[29] + 20;  /* 0x800FECE4: 0x27A50014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80084CE8u);
    cpu->pc = 0x80084CE8u; return;  /* CPS jal -> 0x80084CE8 */

block_800FECE8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FECE8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FECE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FECE8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FECE8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FECEC: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECECu);
#endif
    if (_bc_800FECE8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED38u);
        goto block_800FED38;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FECF0u);
        goto block_800FECF0;  /* not taken */
    }

block_800FECF0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FECF0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FECF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FECF0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FECF8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FECF4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F74Cu);
    cpu->pc = 0x8008F74Cu; return;  /* CPS jal -> 0x8008F74C */

block_800FECF8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FECF8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FECF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FECF8u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FECF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x800FECF8: 0x30420010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FECF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FECFC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FED00: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED00u);
#endif
    if (_bc_800FECFC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED08u);
        goto block_800FED08;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED04u);
        goto block_800FED04;  /* not taken */
    }

block_800FED04:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED04u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED04u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FED04u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 60, (uint16_t)cpu->gpr[2]);  /* 0x800FED04: 0xA602003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FED04u);
#endif
    /* fall through to block_800FED08 */

block_800FED08:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED08u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED08u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FED10u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x800FED0C: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FED0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FED10:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED10u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED10u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[17];  /* 0x800FED10: 0x00511023 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 600) ? 1 : 0;  /* 0x800FED14: 0x28420258 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FED18 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FED1C: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED1Cu);
#endif
    if (_bc_800FED18) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEDE8u);
        goto block_800FEDE8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED20u);
        goto block_800FED20;  /* not taken */
    }

block_800FED20:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED20u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED20u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED20u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 60, 2, 0x10000u);  /* 0x800FED20: 0x8602003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FED20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FED24: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FED28 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 16;  /* 0x800FED2C: 0x27A40010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED2Cu);
#endif
    if (_bc_800FED28) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FECE0u);
        goto block_800FECE0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED30u);
        goto block_800FED30;  /* not taken */
    }

block_800FED30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED30u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x800FED34: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEDE8u);
    goto block_800FEDE8;  /* j */

block_800FED38:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED38u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED38u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED38u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 2, 0x20000000u);  /* 0x800FED38: 0x8FA20014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FED3C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED40u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[2] + 8, 16, 0x4u);  /* 0x800FED40: 0x8C500008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED40u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17500, 2, 0x8u);  /* 0x800FED44: 0x8C62445C */
#ifdef PSX_COSIM
    cosim_instr(0x800FED44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FED48: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = (cpu->gpr[16] < cpu->gpr[2]) ? 1 : 0;  /* 0x800FED4C: 0x0202102B */
#ifdef PSX_COSIM
    cosim_instr(0x800FED4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FED50 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FED54: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED54u);
#endif
    if (_bc_800FED50) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED6Cu);
        goto block_800FED6C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED58u);
        goto block_800FED58;  /* not taken */
    }

block_800FED58:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED58u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED58u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED58u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 17458, 2, 0x4u);  /* 0x800FED58: 0x84424432 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FED5C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[16] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x800FED60: 0x0202102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FED60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FED64 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FED68: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED68u);
#endif
    if (_bc_800FED64) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEDB0u);
        goto block_800FEDB0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED6Cu);
        goto block_800FED6C;  /* not taken */
    }

block_800FED6C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED6Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED6Cu, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FED6C: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[4] = cpu->gpr[2] + 17400;  /* 0x800FED70: 0x244443F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED70u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[4] + 64, 2, 0x10u);  /* 0x800FED74: 0x84820040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FED78: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FED7C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FED80: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED80u);
#endif
    if (_bc_800FED7C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEDA8u);
        goto block_800FEDA8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED84u);
        goto block_800FED84;  /* not taken */
    }

block_800FED84:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED84u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED84u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FED8Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[17] = cpu->gpr[4];  /* move */  /* 0x800FED88: 0x00808821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80082B38u);
    cpu->pc = 0x80082B38u; return;  /* CPS jal -> 0x80082B38 */

block_800FED8C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED8Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED8Cu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FED8C: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED8Cu);
#endif
    /* fall through to block_800FED90 */

block_800FED90:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED90u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED90u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FED98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 448;  /* 0x800FED94: 0x240501C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091458u);
    cpu->pc = 0x80091458u; return;  /* CPS jal -> 0x80091458 */

block_800FED98:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FED98u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FED98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FED98u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FED98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FED98 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FED9C: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FED9Cu);
#endif
    if (_bc_800FED98) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FED90u);
        goto block_800FED90;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEDA0u);
        goto block_800FEDA0;  /* not taken */
    }

block_800FEDA0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEDA0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEDA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEDA0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEDA4: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEDB0u);
    goto block_800FEDB0;  /* j */

block_800FEDA8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEDA8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEDA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEDA8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FEDA8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 62, (uint16_t)cpu->gpr[2]);  /* 0x800FEDA8: 0xA482003E */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEDAC: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDACu);
#endif
    /* fall through to block_800FEDB0 */

block_800FEDB0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEDB0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEDB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEDB0u, 14u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x8018 << 16;  /* 0x80180000 */  /* 0x800FEDB0: 0x3C058018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDB0u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 2, 0x20000000u);  /* 0x800FEDB4: 0x8FA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDB4u);
#endif
    cpu->gpr[6] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 6, 0x20000000u);  /* 0x800FEDB8: 0x8FA60014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0u);
#endif
    cpu->gpr[7] = cpu->gpr[5] + 19640;  /* 0x800FEDBC: 0x24A74CB8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10008u);
#endif
    g_debug_last_store_pc = 0x800FEDC0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + 17458, (uint16_t)cpu->gpr[16]);  /* 0x800FEDC0: 0xA4704432 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDC0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[5] + 19640, 3, 0x20u);  /* 0x800FEDC4: 0x8CA34CB8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDC4u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[6] + 8, 4, 0x40u);  /* 0x800FEDC8: 0x8CC40008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 1;  /* 0x800FEDCC: 0x24630001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    cpu->gpr[3] = cpu->gpr[3] ^ cpu->gpr[4];  /* 0x800FEDD0: 0x00641826 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = (cpu->gpr[0] < cpu->gpr[3]) ? 1 : 0;  /* 0x800FEDD4: 0x0003182B */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x88u);
#endif
    g_debug_last_store_pc = 0x800FEDD8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[7] + 4, cpu->gpr[3]);  /* 0x800FEDD8: 0xACE30004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDD8u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[6] + 8, 3, 0x40u);  /* 0x800FEDDC: 0x8CC30008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEDE0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x28u);
#endif
    g_debug_last_store_pc = 0x800FEDE4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[5] + 19640, cpu->gpr[3]);  /* 0x800FEDE4: 0xACA34CB8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDE4u);
#endif
    /* fall through to block_800FEDE8 */

block_800FEDE8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEDE8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEDE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEDE8u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDE8u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 31, 0x20000000u);  /* 0x800FEDE8: 0x8FBF0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDE8u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 17, 0x20000000u);  /* 0x800FEDEC: 0x8FB1001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEDF0u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 16, 0x20000000u);  /* 0x800FEDF0: 0x8FB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 40;  /* 0x800FEDF4: 0x27BD0028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEDF8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEDFC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEDFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEDF8);
    cpu->pc = _jt_800FEDF8; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FEE00(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FEE20u: goto block_800FEE20;
            case 0x800FEE40u: goto block_800FEE40;
            case 0x800FEE30u: goto block_800FEE30;
            case 0x800FEE38u: goto block_800FEE38;
            case 0x800FEE50u: goto block_800FEE50;
            case 0x800FEE60u: goto block_800FEE60;
            case 0x800FEEA0u: goto block_800FEEA0;
            case 0x800FEEB0u: goto block_800FEEB0;
            case 0x800FEE00u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FEE00u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FEE00u);
    /* Address: 0x800FEE00, Size: 208 bytes, Blocks: 9 */

block_800FEE00:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEE00u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEE00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEE00u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -32;  /* 0x800FEE00: 0x27BDFFE0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FEE04u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);  /* 0x800FEE04: 0xAFB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[16] = cpu->gpr[4];  /* move */  /* 0x800FEE08: 0x00808021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x800FEE0C: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FEE10u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[31]);  /* 0x800FEE10: 0xAFBF001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20040000u);
#endif
    g_debug_last_store_pc = 0x800FEE14u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[18]);  /* 0x800FEE14: 0xAFB20018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEE20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x800FEE1Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);  /* 0x800FEE1C: 0xAFB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FEE20:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEE20u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEE20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEE20u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE20u);
#endif
    cpu->gpr[3] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 22, 3, 0x10000u);  /* 0x800FEE20: 0x86030016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEE24: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    int _bc_800FEE28 = (cpu->gpr[3] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20005u);
#endif
    cpu->gpr[17] = cpu->gpr[2];  /* move */  /* 0x800FEE2C: 0x00408821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE2Cu);
#endif
    if (_bc_800FEE28) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEEB0u);
        goto block_800FEEB0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEE30u);
        goto block_800FEE30;  /* not taken */
    }

block_800FEE30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEE30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEE30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEE30u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FEE30: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    cpu->gpr[18] = cpu->gpr[2] + 17400;  /* 0x800FEE34: 0x245243F8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE34u);
#endif
    /* fall through to block_800FEE38 */

block_800FEE38:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEE38u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEE38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEE38u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEE40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x800FEE3C: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800859A8u);
    cpu->pc = 0x800859A8u; return;  /* CPS jal -> 0x800859A8 */

block_800FEE40:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEE40u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEE40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEE40u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] - cpu->gpr[17];  /* 0x800FEE40: 0x00511023 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < 600) ? 1 : 0;  /* 0x800FEE44: 0x28420258 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEE48 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEE4C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE4Cu);
#endif
    if (_bc_800FEE48) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEE60u);
        goto block_800FEE60;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEE50u);
        goto block_800FEE50;  /* not taken */
    }

block_800FEE50:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEE50u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEE50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEE50u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE50u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[18] + 60, 2, 0x40000u);  /* 0x800FEE50: 0x8642003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEE54: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEE58 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEE5C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE5Cu);
#endif
    if (_bc_800FEE58) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEEA0u);
        goto block_800FEEA0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEE60u);
        goto block_800FEE60;  /* not taken */
    }

block_800FEE60:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEE60u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEE60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEE60u, 16u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE60u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 20, 2, 0x10000u);  /* 0x800FEE60: 0x86020014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = 1;  /* 0x800FEE64: 0x24030001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10008u);
#endif
    g_debug_last_store_pc = 0x800FEE68u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 22, (uint16_t)cpu->gpr[3]);  /* 0x800FEE68: 0xA6030016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)1) ? 1 : 0;  /* 0x800FEE6C: 0x2C420001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FEE70u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 20, (uint16_t)cpu->gpr[2]);  /* 0x800FEE70: 0xA6020014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 3;  /* 0x800FEE74: 0x000210C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];  /* 0x800FEE78: 0x02021021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE78u);
#endif
    cpu->gpr[3] = psx_cyc_load_half(cpu, cpu->gpr[2] + 40, 3, 0x4u);  /* 0x800FEE7C: 0x94430028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE80u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 20, 2, 0x10000u);  /* 0x800FEE80: 0x86020014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEE84: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 3;  /* 0x800FEE88: 0x000210C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = cpu->gpr[16] + cpu->gpr[2];  /* 0x800FEE8C: 0x02021021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEE90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10008u);
#endif
    g_debug_last_store_pc = 0x800FEE90u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 56, (uint16_t)cpu->gpr[3]);  /* 0x800FEE90: 0xA6030038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE90u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[2] + 42, 2, 0x4u);  /* 0x800FEE94: 0x9442002A */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEE98: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FEE9Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 58, (uint16_t)cpu->gpr[2]);  /* 0x800FEE9C: 0xA602003A */
#ifdef PSX_COSIM
    cosim_instr(0x800FEE9Cu);
#endif
    /* fall through to block_800FEEA0 */

block_800FEEA0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEEA0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEEA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEEA0u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEEA0u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[16] + 22, 2, 0x10000u);  /* 0x800FEEA0: 0x86020016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEEA4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEEA8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEEAC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEACu);
#endif
    if (_bc_800FEEA8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEE38u);
        goto block_800FEE38;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEEB0u);
        goto block_800FEEB0;  /* not taken */
    }

block_800FEEB0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEEB0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEEB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEEB0u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEEB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FEEB0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 22, (uint16_t)cpu->gpr[0]);  /* 0x800FEEB0: 0xA6000016 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEB0u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 31, 0x20000000u);  /* 0x800FEEB4: 0x8FBF001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEB4u);
#endif
    cpu->gpr[18] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 18, 0x20000000u);  /* 0x800FEEB8: 0x8FB20018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEB8u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 17, 0x20000000u);  /* 0x800FEEBC: 0x8FB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEEC0u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 16, 0x20000000u);  /* 0x800FEEC0: 0x8FB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x800FEEC4: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEEC8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEECC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEECCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEEC8);
    cpu->pc = _jt_800FEEC8; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FEED0(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FEF78u: goto block_800FEF78;
            case 0x800FEF80u: goto block_800FEF80;
            case 0x800FEEF8u: goto block_800FEEF8;
            case 0x800FEF14u: goto block_800FEF14;
            case 0x800FEF24u: goto block_800FEF24;
            case 0x800FEF3Cu: goto block_800FEF3C;
            case 0x800FEF4Cu: goto block_800FEF4C;
            case 0x800FEF64u: goto block_800FEF64;
            case 0x800FEED0u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FEED0u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FEED0u);
    /* Address: 0x800FEED0, Size: 196 bytes, Blocks: 9 */

block_800FEED0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEED0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEED0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEED0u, 10u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEED0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -112;  /* 0x800FEED0: 0x27BDFF90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEED0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000020u);
#endif
    cpu->gpr[5] = cpu->gpr[29] + 16;  /* 0x800FEED4: 0x27A50010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEED4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FEED8: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEED8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x44u);
#endif
    cpu->gpr[6] = cpu->gpr[2] + -27088;  /* 0x800FEEDC: 0x24469630 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEEE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FEEE0: 0x3C03800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEE0u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[2] + -27088, 2, 0x4u);  /* 0x800FEEE4: 0x90429630 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + -27080;  /* 0x800FEEE8: 0x24639638 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FEEECu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 108, cpu->gpr[31]);  /* 0x800FEEEC: 0xAFBF006C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEEF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEEF0 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FEEF4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 104, cpu->gpr[16]);  /* 0x800FEEF4: 0xAFB00068 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEF4u);
#endif
    if (_bc_800FEEF0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF14u);
        goto block_800FEF14;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEEF8u);
        goto block_800FEEF8;  /* not taken */
    }

block_800FEEF8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEEF8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEEF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEEF8u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEEF8u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[6], 2, 0x40u);  /* 0x800FEEF8: 0x90C20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = cpu->gpr[6] + 1;  /* 0x800FEEFC: 0x24C60001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEEFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FEF00u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[5], (uint8_t)cpu->gpr[2]);  /* 0x800FEF00: 0xA0A20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF00u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[6], 2, 0x40u);  /* 0x800FEF04: 0x90C20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF08: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEF0C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 1;  /* 0x800FEF10: 0x24A50001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF10u);
#endif
    if (_bc_800FEF0C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEEF8u);
        goto block_800FEEF8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF14u);
        goto block_800FEF14;  /* not taken */
    }

block_800FEF14:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF14u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF14u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF14u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x800FEF14: 0x90820000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF18: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEF1C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF20: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF20u);
#endif
    if (_bc_800FEF1C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF3Cu);
        goto block_800FEF3C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF24u);
        goto block_800FEF24;  /* not taken */
    }

block_800FEF24:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF24u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF24u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 1;  /* 0x800FEF24: 0x24840001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FEF28u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[5], (uint8_t)cpu->gpr[2]);  /* 0x800FEF28: 0xA0A20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF28u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x800FEF2C: 0x90820000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF30: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEF34 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 1;  /* 0x800FEF38: 0x24A50001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF38u);
#endif
    if (_bc_800FEF34) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF24u);
        goto block_800FEF24;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF3Cu);
        goto block_800FEF3C;  /* not taken */
    }

block_800FEF3C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF3Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF3Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF3Cu);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FEF3C: 0x90620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF40: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEF44 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF48: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF48u);
#endif
    if (_bc_800FEF44) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF64u);
        goto block_800FEF64;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF4Cu);
        goto block_800FEF4C;  /* not taken */
    }

block_800FEF4C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF4Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF4Cu, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 1;  /* 0x800FEF4C: 0x24630001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FEF50u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[5], (uint8_t)cpu->gpr[2]);  /* 0x800FEF50: 0xA0A20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF50u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FEF54: 0x90620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF58: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FEF5C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 1;  /* 0x800FEF60: 0x24A50001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF60u);
#endif
    if (_bc_800FEF5C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF4Cu);
        goto block_800FEF4C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FEF64u);
        goto block_800FEF64;  /* not taken */
    }

block_800FEF64:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF64u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF64u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    g_debug_last_store_pc = 0x800FEF64u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[5], (uint8_t)cpu->gpr[0]);  /* 0x800FEF64: 0xA0A00000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    cpu->gpr[16] = cpu->gpr[29] + 80;  /* 0x800FEF68: 0x27B00050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FEF6C: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEF78u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000020u);
#endif
    cpu->gpr[5] = cpu->gpr[29] + 16;  /* 0x800FEF74: 0x27A50010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FEF78:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF78u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF78u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEF80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FEF7C: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090C0Cu);
    cpu->pc = 0x80090C0Cu; return;  /* CPS jal -> 0x80090C0C */

block_800FEF80:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF80u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF80u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF80u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 108, 31, 0x20000000u);  /* 0x800FEF80: 0x8FBF006C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF80u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 104, 16, 0x20000000u);  /* 0x800FEF84: 0x8FB00068 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 112;  /* 0x800FEF88: 0x27BD0070 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEF8C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEF90: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEF8C);
    cpu->pc = _jt_800FEF8C; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FEF94(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FEFA4u: goto block_800FEFA4;
            case 0x800FEF94u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FEF94u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FEF94u);
    /* Address: 0x800FEF94, Size: 32 bytes, Blocks: 2 */

block_800FEF94:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEF94u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEF94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEF94u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEF94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x800FEF94: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FEF98u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x800FEF98: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEF98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEFA4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEFA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEFA0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FDC78u);
    cpu->pc = 0x800FDC78u; return;  /* CPS jal -> 0x800FDC78 */

block_800FEFA4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEFA4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEFA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEFA4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEFA4u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x800FEFA4: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x800FEFA8: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEFAC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEFB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEFB0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEFAC);
    cpu->pc = _jt_800FEFAC; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FEFB4(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FEFC4u: goto block_800FEFC4;
            case 0x800FEFB4u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FEFB4u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FEFB4u);
    /* Address: 0x800FEFB4, Size: 32 bytes, Blocks: 2 */

block_800FEFB4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEFB4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEFB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEFB4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEFB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x800FEFB4: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FEFB8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x800FEFB8: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FEFC4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEFC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEFC0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FDFE4u);
    cpu->pc = 0x800FDFE4u; return;  /* CPS jal -> 0x800FDFE4 */

block_800FEFC4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEFC4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEFC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEFC4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEFC4u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x800FEFC4: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x800FEFC8: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEFCC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEFD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEFD0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEFD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEFCC);
    cpu->pc = _jt_800FEFCC; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FF0C4(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FF150u: goto block_800FF150;
            case 0x800FF158u: goto block_800FF158;
            case 0x800FF16Cu: goto block_800FF16C;
            case 0x800FF178u: goto block_800FF178;
            case 0x800FF180u: goto block_800FF180;
            case 0x800FF1B8u: goto block_800FF1B8;
            case 0x800FF1C0u: goto block_800FF1C0;
            case 0x800FF1C8u: goto block_800FF1C8;
            case 0x800FF1D0u: goto block_800FF1D0;
            case 0x800FF1D8u: goto block_800FF1D8;
            case 0x800FF1E8u: goto block_800FF1E8;
            case 0x800FF1F0u: goto block_800FF1F0;
            case 0x800FF204u: goto block_800FF204;
            case 0x800FF214u: goto block_800FF214;
            case 0x800FF290u: goto block_800FF290;
            case 0x800FF2A0u: goto block_800FF2A0;
            case 0x800FF2B4u: goto block_800FF2B4;
            case 0x800FF2C4u: goto block_800FF2C4;
            case 0x800FF2D4u: goto block_800FF2D4;
            case 0x800FF2DCu: goto block_800FF2DC;
            case 0x800FF2ECu: goto block_800FF2EC;
            case 0x800FF314u: goto block_800FF314;
            case 0x800FF324u: goto block_800FF324;
            case 0x800FF32Cu: goto block_800FF32C;
            case 0x800FF338u: goto block_800FF338;
            case 0x800FF368u: goto block_800FF368;
            case 0x800FF39Cu: goto block_800FF39C;
            case 0x800FF3A4u: goto block_800FF3A4;
            case 0x800FF3B4u: goto block_800FF3B4;
            case 0x800FF3C4u: goto block_800FF3C4;
            case 0x800FF3D0u: goto block_800FF3D0;
            case 0x800FF3D8u: goto block_800FF3D8;
            case 0x800FF400u: goto block_800FF400;
            case 0x800FF408u: goto block_800FF408;
            case 0x800FF438u: goto block_800FF438;
            case 0x800FF440u: goto block_800FF440;
            case 0x800FF464u: goto block_800FF464;
            case 0x800FF4A4u: goto block_800FF4A4;
            case 0x800FF4B4u: goto block_800FF4B4;
            case 0x800FF4F8u: goto block_800FF4F8;
            case 0x800FF50Cu: goto block_800FF50C;
            case 0x800FF544u: goto block_800FF544;
            case 0x800FF54Cu: goto block_800FF54C;
            case 0x800FF564u: goto block_800FF564;
            case 0x800FF574u: goto block_800FF574;
            case 0x800FF580u: goto block_800FF580;
            case 0x800FF5BCu: goto block_800FF5BC;
            case 0x800FF5C4u: goto block_800FF5C4;
            case 0x800FF5ECu: goto block_800FF5EC;
            case 0x800FF5F4u: goto block_800FF5F4;
            case 0x800FF62Cu: goto block_800FF62C;
            case 0x800FF660u: goto block_800FF660;
            case 0x800FF668u: goto block_800FF668;
            case 0x800FF678u: goto block_800FF678;
            case 0x800FF6ACu: goto block_800FF6AC;
            case 0x800FF6B4u: goto block_800FF6B4;
            case 0x800FF6C8u: goto block_800FF6C8;
            case 0x800FF6DCu: goto block_800FF6DC;
            case 0x800FF710u: goto block_800FF710;
            case 0x800FF718u: goto block_800FF718;
            case 0x800FF72Cu: goto block_800FF72C;
            case 0x800FF73Cu: goto block_800FF73C;
            case 0x800FF754u: goto block_800FF754;
            case 0x800FF764u: goto block_800FF764;
            case 0x800FF784u: goto block_800FF784;
            case 0x800FF12Cu: goto block_800FF12C;
            case 0x800FF148u: goto block_800FF148;
            case 0x800FF190u: goto block_800FF190;
            case 0x800FF1E0u: goto block_800FF1E0;
            case 0x800FF1FCu: goto block_800FF1FC;
            case 0x800FF20Cu: goto block_800FF20C;
            case 0x800FF224u: goto block_800FF224;
            case 0x800FF240u: goto block_800FF240;
            case 0x800FF250u: goto block_800FF250;
            case 0x800FF268u: goto block_800FF268;
            case 0x800FF278u: goto block_800FF278;
            case 0x800FF284u: goto block_800FF284;
            case 0x800FF298u: goto block_800FF298;
            case 0x800FF2A8u: goto block_800FF2A8;
            case 0x800FF2BCu: goto block_800FF2BC;
            case 0x800FF2E4u: goto block_800FF2E4;
            case 0x800FF2F4u: goto block_800FF2F4;
            case 0x800FF304u: goto block_800FF304;
            case 0x800FF31Cu: goto block_800FF31C;
            case 0x800FF348u: goto block_800FF348;
            case 0x800FF3ACu: goto block_800FF3AC;
            case 0x800FF3BCu: goto block_800FF3BC;
            case 0x800FF3F0u: goto block_800FF3F0;
            case 0x800FF3F8u: goto block_800FF3F8;
            case 0x800FF410u: goto block_800FF410;
            case 0x800FF424u: goto block_800FF424;
            case 0x800FF42Cu: goto block_800FF42C;
            case 0x800FF430u: goto block_800FF430;
            case 0x800FF44Cu: goto block_800FF44C;
            case 0x800FF45Cu: goto block_800FF45C;
            case 0x800FF474u: goto block_800FF474;
            case 0x800FF484u: goto block_800FF484;
            case 0x800FF488u: goto block_800FF488;
            case 0x800FF490u: goto block_800FF490;
            case 0x800FF49Cu: goto block_800FF49C;
            case 0x800FF4BCu: goto block_800FF4BC;
            case 0x800FF4E8u: goto block_800FF4E8;
            case 0x800FF500u: goto block_800FF500;
            case 0x800FF514u: goto block_800FF514;
            case 0x800FF53Cu: goto block_800FF53C;
            case 0x800FF554u: goto block_800FF554;
            case 0x800FF56Cu: goto block_800FF56C;
            case 0x800FF578u: goto block_800FF578;
            case 0x800FF598u: goto block_800FF598;
            case 0x800FF5A4u: goto block_800FF5A4;
            case 0x800FF5CCu: goto block_800FF5CC;
            case 0x800FF5D8u: goto block_800FF5D8;
            case 0x800FF5FCu: goto block_800FF5FC;
            case 0x800FF60Cu: goto block_800FF60C;
            case 0x800FF680u: goto block_800FF680;
            case 0x800FF6BCu: goto block_800FF6BC;
            case 0x800FF6D0u: goto block_800FF6D0;
            case 0x800FF6E4u: goto block_800FF6E4;
            case 0x800FF720u: goto block_800FF720;
            case 0x800FF734u: goto block_800FF734;
            case 0x800FF740u: goto block_800FF740;
            case 0x800FF74Cu: goto block_800FF74C;
            case 0x800FF75Cu: goto block_800FF75C;
            case 0x800FF770u: goto block_800FF770;
            case 0x800FF77Cu: goto block_800FF77C;
            case 0x800FF788u: goto block_800FF788;
            case 0x800FF0C4u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FF0C4u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FF0C4u);
    /* Address: 0x800FF0C4, Size: 1764 bytes, Blocks: 127 */

block_800FF0C4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF0C4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF0C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF0C4u, 26u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF0C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -72;  /* 0x800FF0C4: 0x27BDFFB8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF0C8: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0C8u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[2] + -8452, 3, 0x4u);  /* 0x800FF0CC: 0x8C43DEFC */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF0D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF0D0: 0x3C04800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20040000u);
#endif
    g_debug_last_store_pc = 0x800FF0D4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 64, cpu->gpr[18]);  /* 0x800FF0D4: 0xAFB20040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0D4u);
#endif
    cpu->gpr[18] = psx_cyc_load_word(cpu, cpu->gpr[4] + -8348, 18, 0x10u);  /* 0x800FF0D8: 0x8C92DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FF0DCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 68, cpu->gpr[31]);  /* 0x800FF0DC: 0xAFBF0044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF0E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x800FF0E0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 60, cpu->gpr[17]);  /* 0x800FF0E0: 0xAFB1003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FF0E4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 56, cpu->gpr[16]);  /* 0x800FF0E4: 0xAFB00038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[3] << 2;  /* 0x800FF0E8: 0x00031080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FF0EC: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF0F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 12;  /* 0x800FF0F0: 0x00021300 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8018 << 16;  /* 0x80180000 */  /* 0x800FF0F4: 0x3C038018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 19936;  /* 0x800FF0F8: 0x24634DE0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FF0FC: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF0FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF100u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FF100u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[4] + -8348, cpu->gpr[2]);  /* 0x800FF100: 0xAC82DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF100u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF104: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF104u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[2] + -8440;  /* 0x800FF108: 0x2443DF08 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF108u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF10C: 0x3C04800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF10Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF110u);
#endif
    cpu->gpr[5] = psx_cyc_load_half(cpu, cpu->gpr[2] + -8440, 5, 0x4u);  /* 0x800FF110: 0x9445DF08 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF110u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[3] + 2, 2, 0x8u);  /* 0x800FF114: 0x94620002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF114u);
#endif
    cpu->gpr[3] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[4] + -7644, 3, 0x10u);  /* 0x800FF118: 0x8483E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF118u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20024u);
#endif
    cpu->gpr[17] = cpu->gpr[5] | cpu->gpr[2];  /* 0x800FF11C: 0x00A28825 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF11Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF120u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)11) ? 1 : 0;  /* 0x800FF120: 0x2C62000B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF120u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF124 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF128: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF128u);
#endif
    if (_bc_800FF124) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF77Cu);
        goto block_800FF77C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF12Cu);
        goto block_800FF12C;  /* not taken */
    }

block_800FF12C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF12Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF12Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF12Cu, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF12Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -26992;  /* 0x800FF12C: 0x24429690 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF12Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF130u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] << 2;  /* 0x800FF130: 0x00031880 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF130u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];  /* 0x800FF134: 0x00621821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF134u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FF138: 0x8C620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF138u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF13C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF13Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF140u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    uint32_t _jt_800FF140 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF144: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF144u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FF140);
    cpu->pc = _jt_800FF140; return;  /* CPS: jr cpu->gpr[2] */

block_800FF148:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF148u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF148u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF148u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF148u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF150u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF14C: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF14Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80055884u);
    cpu->pc = 0x80055884u; return;  /* CPS jal -> 0x80055884 */

block_800FF150:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF150u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF150u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF150u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF150u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF158u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF154: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF154u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004B688u);
    cpu->pc = 0x8004B688u; return;  /* CPS jal -> 0x8004B688 */

block_800FF158:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF158u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF158u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF158u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF158u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000u);
#endif
    cpu->gpr[17] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF158: 0x3C11800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF158u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x30000u);
#endif
    cpu->gpr[16] = cpu->gpr[17] + -9072;  /* 0x800FF15C: 0x2630DC90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF15Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF160u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 3;  /* 0x800FF160: 0x24020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF160u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF16Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FF168u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 1, (uint8_t)cpu->gpr[2]);  /* 0x800FF168: 0xA2020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF168u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80100F58u);
    cpu->pc = 0x80100F58u; return;  /* CPS jal -> 0x80100F58 */

block_800FF16C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF16Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF16Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF16Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF16Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF16C: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF16Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF170u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF178u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FF174u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 12, (uint16_t)cpu->gpr[2]);  /* 0x800FF174: 0xA602000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF174u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8010215Cu);
    cpu->pc = 0x8010215Cu; return;  /* CPS jal -> 0x8010215C */

block_800FF178:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF178u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF178u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF178u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF178u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF180u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF17C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF17Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FEFD4u);
    cpu->pc = 0x800FEFD4u; return;  /* CPS jal -> 0x800FEFD4 */

block_800FF180:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF180u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF180u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF180u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF180u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[17] + -9072, 3, 0x20000u);  /* 0x800FF180: 0x9223DC90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF180u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FF184u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 4, (uint8_t)cpu->gpr[2]);  /* 0x800FF184: 0xA2020004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF184u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    int _bc_800FF188 = (cpu->gpr[3] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FF18Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 6, (uint8_t)cpu->gpr[2]);  /* 0x800FF18C: 0xA2020006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF18Cu);
#endif
    if (_bc_800FF188) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF268u);
        goto block_800FF268;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF190u);
        goto block_800FF190;  /* not taken */
    }

block_800FF190:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF190u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF190u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF190u, 10u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF190u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF190: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF190u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FF194: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF194u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF198u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF198: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF198u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8010 << 16;  /* 0x80100000 */  /* 0x800FF19C: 0x3C048010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF19Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 24480;  /* 0x800FF1A0: 0x24845FA0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FF1A4: 0x3C058011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 17371;  /* 0x800FF1A8: 0x24A543DB */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -4;  /* 0x800FF1AC: 0x2402FFFC */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF1B8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[5] = cpu->gpr[5] & cpu->gpr[2];  /* 0x800FF1B4: 0x00A22824 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004CA40u);
    cpu->pc = 0x8004CA40u; return;  /* CPS jal -> 0x8004CA40 */

block_800FF1B8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1B8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1B8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF1C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 4;  /* 0x800FF1BC: 0x24040004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF1C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1C0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF1C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 5;  /* 0x800FF1C4: 0x24040005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF1C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1C8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF1D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 6;  /* 0x800FF1CC: 0x24040006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF1D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1D0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF1D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 7;  /* 0x800FF1D4: 0x24040007 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF1D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1D8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF1DC: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF1E0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1E0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1E0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF1E8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF1E4: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80101FE8u);
    cpu->pc = 0x80101FE8u; return;  /* CPS jal -> 0x80101FE8 */

block_800FF1E8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1E8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1E8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF1F0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF1EC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F74Cu);
    cpu->pc = 0x8008F74Cu; return;  /* CPS jal -> 0x8008F74C */

block_800FF1F0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1F0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1F0u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x800FF1F0: 0x30420010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF1F4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF1F8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF1F8u);
#endif
    if (_bc_800FF1F4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF20Cu);
        goto block_800FF20C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF1FCu);
        goto block_800FF1FC;  /* not taken */
    }

block_800FF1FC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF1FCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF1FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF1FCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF1FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF204u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF200u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF200: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF200u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B54Cu);
    cpu->pc = 0x8006B54Cu; return;  /* CPS jal -> 0x8006B54C */

block_800FF204:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF204u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF204u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF204u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF204u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF208: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF208u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF490u);
    goto block_800FF490;  /* j */

block_800FF20C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF20Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF20Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF20Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF20Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF214u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF210u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF210: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF210u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8010217Cu);
    cpu->pc = 0x8010217Cu; return;  /* CPS jal -> 0x8010217C */

block_800FF214:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF214u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF214u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF214u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF214u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xDu);
#endif
    cpu->gpr[3] = cpu->gpr[2];  /* move */  /* 0x800FF214: 0x00401821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF214u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = (cpu->gpr[3] < (uint32_t)6) ? 1 : 0;  /* 0x800FF218: 0x2C620006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF218u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF21C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF220u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF220: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF220u);
#endif
    if (_bc_800FF21C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF2BCu);
        goto block_800FF2BC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF224u);
        goto block_800FF224;  /* not taken */
    }

block_800FF224:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF224u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF224u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF224u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF224u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -26944;  /* 0x800FF224: 0x244296C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF224u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] << 2;  /* 0x800FF228: 0x00031880 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF228u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];  /* 0x800FF22C: 0x00621821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF22Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF230u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FF230: 0x8C620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF230u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF234: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF234u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    uint32_t _jt_800FF238 = cpu->gpr[2];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF23C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF23Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FF238);
    cpu->pc = _jt_800FF238; return;  /* CPS: jr cpu->gpr[2] */

block_800FF240:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF240u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF240u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF240u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF240u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF240: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF240u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 10;  /* 0x800FF244: 0x2402000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FF244u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF24Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF24C: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF24Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF250:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF250u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF250u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF250u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF250u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF250: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF250u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[4] = cpu->gpr[2] + -9072;  /* 0x800FF254: 0x2444DC90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF254u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[2] + -9072, 3, 0x4u);  /* 0x800FF258: 0x9043DC90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF258u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FF25C: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF25Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF260u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    int _bc_800FF260 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[3] + -2;  /* 0x800FF264: 0x2462FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FF264u);
#endif
    if (_bc_800FF260) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF278u);
        goto block_800FF278;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF268u);
        goto block_800FF268;  /* not taken */
    }

block_800FF268:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF268u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF268u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF268u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF268u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF268: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF268u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 2;  /* 0x800FF26C: 0x24020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF26Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF270u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF274u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF274: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF274u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF278:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF278u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF278u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF278u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF278u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)2) ? 1 : 0;  /* 0x800FF278: 0x2C420002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF278u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF27C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF280u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF280: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF280u);
#endif
    if (_bc_800FF27C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF788u);
        goto block_800FF788;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF284u);
        goto block_800FF284;  /* not taken */
    }

block_800FF284:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF284u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF284u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF284u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF284u);
#endif
    cpu->gpr[4] = psx_cyc_load_byte(cpu, cpu->gpr[4] + 5, 4, 0x10u);  /* 0x800FF284: 0x90840005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF284u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF290u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FF28C: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF28Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B194u);
    cpu->pc = 0x8006B194u; return;  /* CPS jal -> 0x8006B194 */

block_800FF290:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF290u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF290u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF290u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF290u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF294: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF294u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF298:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF298u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF298u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF298u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF298u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF2A0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF29C: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF29Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B54Cu);
    cpu->pc = 0x8006B54Cu; return;  /* CPS jal -> 0x8006B54C */

block_800FF2A0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2A0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2A0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF2A4: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF2A8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2A8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2A8u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 60;  /* 0x800FF2A8: 0x2404003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF2B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FF2B0: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B6C8u);
    cpu->pc = 0x8006B6C8u; return;  /* CPS jal -> 0x8006B6C8 */

block_800FF2B4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2B4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2B4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF2B8: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF2BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2BCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF2C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FF2C0: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007AE58u);
    cpu->pc = 0x8007AE58u; return;  /* CPS jal -> 0x8007AE58 */

block_800FF2C4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2C4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2C4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF2C4: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2C4u);
#endif
    cpu->gpr[4] = psx_cyc_load_byte(cpu, cpu->gpr[2] + -9068, 4, 0x4u);  /* 0x800FF2C8: 0x9044DC94 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF2D4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF2D0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FDC78u);
    cpu->pc = 0x800FDC78u; return;  /* CPS jal -> 0x800FDC78 */

block_800FF2D4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2D4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2D4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF2DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF2D8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006C558u);
    cpu->pc = 0x8006C558u; return;  /* CPS jal -> 0x8006C558 */

block_800FF2DC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2DCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2DCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF2DC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF2E0: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2E0u);
#endif
    if (_bc_800FF2DC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF2F4u);
        goto block_800FF2F4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF2E4u);
        goto block_800FF2E4;  /* not taken */
    }

block_800FF2E4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2E4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2E4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF2ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF2E8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FDFE4u);
    cpu->pc = 0x800FDFE4u; return;  /* CPS jal -> 0x800FDFE4 */

block_800FF2EC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2ECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2ECu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF2F0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF2D4u);
    goto block_800FF2D4;  /* j */

block_800FF2F4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF2F4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF2F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF2F4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF2F4u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[2] + -9072, 2, 0x4u);  /* 0x800FF2F4: 0x9042DC90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF2F8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF2F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF2FC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF300u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF300: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF300u);
#endif
    if (_bc_800FF2FC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF31Cu);
        goto block_800FF31C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF304u);
        goto block_800FF304;  /* not taken */
    }

block_800FF304:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF304u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF304u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF304u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF304u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FF304: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF304u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF308u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF308: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF308u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF314u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF310u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 6;  /* 0x800FF310: 0x24040006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF310u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF314:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF314u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF314u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF314u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF314u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF318: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF318u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF31C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF31Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF31Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF31Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF31Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF324u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF320u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FF320: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF320u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102EECu);
    cpu->pc = 0x80102EECu; return;  /* CPS jal -> 0x80102EEC */

block_800FF324:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF324u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF324u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF324u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF324u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF32Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF328: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF328u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80074E94u);
    cpu->pc = 0x80074E94u; return;  /* CPS jal -> 0x80074E94 */

block_800FF32C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF32Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF32Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF32Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF32Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 4;  /* 0x800FF32C: 0x24040004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF32Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF330u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF338u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 1;  /* 0x800FF334: 0x24050001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF334u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006AE14u);
    cpu->pc = 0x8006AE14u; return;  /* CPS jal -> 0x8006AE14 */

block_800FF338:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF338u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF338u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF338u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF338u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF338: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF338u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 3;  /* 0x800FF33C: 0x24020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF33Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF340u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF344u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF344: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF344u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF348:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF348u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF348u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF348u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF348u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF348: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF348u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[2] + -8452, 16, 0x4u);  /* 0x800FF34C: 0x8C50DEFC */
#ifdef PSX_COSIM
    cosim_instr(0x800FF34Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF350u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FF350: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF350u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + 17368;  /* 0x800FF354: 0x244243D8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF354u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] << 4;  /* 0x800FF358: 0x00108100 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF358u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[2];  /* 0x800FF35C: 0x02028021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF35Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF360u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF368u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FF364: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF364u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007BC90u);
    cpu->pc = 0x8007BC90u; return;  /* CPS jal -> 0x8007BC90 */

block_800FF368:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF368u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF368u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF368u, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF368u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF368: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF368u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10021u);
#endif
    cpu->gpr[5] = cpu->gpr[16];  /* move */  /* 0x800FF36C: 0x02002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF36Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF370u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FF370u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 6, (uint8_t)cpu->gpr[0]);  /* 0x800FF370: 0xA2000006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF370u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FF374u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 5, (uint8_t)cpu->gpr[0]);  /* 0x800FF374: 0xA2000005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF374u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FF378u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 4, (uint8_t)cpu->gpr[0]);  /* 0x800FF378: 0xA2000004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF378u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[2] + -28136, 4, 0x4u);  /* 0x800FF37C: 0x8C449218 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF37Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF380u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 368;  /* 0x800FF380: 0x24020170 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF380u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FF384u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 12, (uint16_t)cpu->gpr[2]);  /* 0x800FF384: 0xA4A2000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF384u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 480;  /* 0x800FF388: 0x240201E0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF388u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    g_debug_last_store_pc = 0x800FF38Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 8, (uint16_t)cpu->gpr[0]);  /* 0x800FF38C: 0xA4A00008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF38Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF390u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    g_debug_last_store_pc = 0x800FF390u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 10, (uint16_t)cpu->gpr[0]);  /* 0x800FF390: 0xA4A0000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FF390u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF39Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FF398u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 14, (uint16_t)cpu->gpr[2]);  /* 0x800FF398: 0xA4A2000E */
#ifdef PSX_COSIM
    cosim_instr(0x800FF398u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007BA74u);
    cpu->pc = 0x8007BA74u; return;  /* CPS jal -> 0x8007BA74 */

block_800FF39C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF39Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF39Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF39Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF39Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF3A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF3A0: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102EECu);
    cpu->pc = 0x80102EECu; return;  /* CPS jal -> 0x80102EEC */

block_800FF3A4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3A4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3A4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF3A4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF3A8: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3A8u);
#endif
    if (_bc_800FF3A4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF788u);
        goto block_800FF788;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF3ACu);
        goto block_800FF3AC;  /* not taken */
    }

block_800FF3AC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3ACu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3ACu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF3B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF3B0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80050190u);
    cpu->pc = 0x80050190u; return;  /* CPS jal -> 0x80050190 */

block_800FF3B4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3B4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3B4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF3B8: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF3BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3BCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF3C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF3C0: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B54Cu);
    cpu->pc = 0x8006B54Cu; return;  /* CPS jal -> 0x8006B54C */

block_800FF3C4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3C4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3C4u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 8;  /* 0x800FF3C4: 0x24040008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF3D0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FF3CC: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80090CF8u);
    cpu->pc = 0x80090CF8u; return;  /* CPS jal -> 0x80090CF8 */

block_800FF3D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3D0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF3D8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF3D4: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8010215Cu);
    cpu->pc = 0x8010215Cu; return;  /* CPS jal -> 0x8010215C */

block_800FF3D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3D8u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF3D8: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 5;  /* 0x800FF3DC: 0x24020005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF3E0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF3E0: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[17] & 0x08F0;  /* 0x800FF3E4: 0x322208F0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF3E8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF3EC: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3ECu);
#endif
    if (_bc_800FF3E8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF3F8u);
        goto block_800FF3F8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF3F0u);
        goto block_800FF3F0;  /* not taken */
    }

block_800FF3F0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3F0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3F0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 6;  /* 0x800FF3F0: 0x24020006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF3F4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF3F4: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3F4u);
#endif
    /* fall through to block_800FF3F8 */

block_800FF3F8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF3F8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF3F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF3F8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF3F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF400u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FF3FC: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF3FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80101FE8u);
    cpu->pc = 0x80101FE8u; return;  /* CPS jal -> 0x80101FE8 */

block_800FF400:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF400u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF400u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF400u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF400u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF408u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF404: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF404u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F74Cu);
    cpu->pc = 0x8008F74Cu; return;  /* CPS jal -> 0x8008F74C */

block_800FF408:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF408u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF408u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF408u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF408u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x0010;  /* 0x800FF40C: 0x30420010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF40Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF488u);
    goto block_800FF488;  /* j */

block_800FF410:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF410u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF410u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF410u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF410u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF410: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF410u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + -9060, 2, 0x4u);  /* 0x800FF414: 0x8442DC9C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF414u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF418: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF418u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF41C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF420u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 4;  /* 0x800FF420: 0x24040004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF420u);
#endif
    if (_bc_800FF41C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF430u);
        goto block_800FF430;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF424u);
        goto block_800FF424;  /* not taken */
    }

block_800FF424:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF424u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF424u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF424u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF424u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 2;  /* 0x800FF428: 0x24040002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF428u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF430u);
    goto block_800FF430;  /* j */

block_800FF42C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF42Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF42Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF42Cu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF42Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 4;  /* 0x800FF42C: 0x24040004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF42Cu);
#endif
    /* fall through to block_800FF430 */

block_800FF430:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF430u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF430u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF430u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF430u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF438u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF434: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF434u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80101FE8u);
    cpu->pc = 0x80101FE8u; return;  /* CPS jal -> 0x80101FE8 */

block_800FF438:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF438u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF438u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF438u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF438u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF440u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF43C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF43Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8008F6F0u);
    cpu->pc = 0x8008F6F0u; return;  /* CPS jal -> 0x8008F6F0 */

block_800FF440:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF440u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF440u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF440u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF440u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = 1;  /* 0x800FF440: 0x24030001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF440u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    int _bc_800FF444 = (cpu->gpr[2] != cpu->gpr[3]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF448: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF448u);
#endif
    if (_bc_800FF444) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF788u);
        goto block_800FF788;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF44Cu);
        goto block_800FF44C;  /* not taken */
    }

block_800FF44C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF44Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF44Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF44Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF44Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF44C: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF44Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF450u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 8;  /* 0x800FF450: 0x24020008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF450u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF458u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF458: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF458u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF45C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF45Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF45Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF45Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF45Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF464u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF460u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF460: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF460u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800924D8u);
    cpu->pc = 0x800924D8u; return;  /* CPS jal -> 0x800924D8 */

block_800FF464:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF464u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF464u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF464u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF464u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xDu);
#endif
    cpu->gpr[3] = cpu->gpr[2];  /* move */  /* 0x800FF464: 0x00401821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF464u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 4;  /* 0x800FF468: 0x24020004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF468u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    int _bc_800FF46C = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF470u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[3] + -1;  /* 0x800FF470: 0x2462FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF470u);
#endif
    if (_bc_800FF46C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF484u);
        goto block_800FF484;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF474u);
        goto block_800FF474;  /* not taken */
    }

block_800FF474:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF474u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF474u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF474u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF474u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF474: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF474u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 9;  /* 0x800FF478: 0x24020009 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF478u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF480u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF480u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF480: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF480u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF484:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF484u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF484u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF484u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF484u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)2) ? 1 : 0;  /* 0x800FF484: 0x2C420002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF484u);
#endif
    /* fall through to block_800FF488 */

block_800FF488:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF488u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF488u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF488u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF488u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF488 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF48C: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF48Cu);
#endif
    if (_bc_800FF488) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF784u);
        goto block_800FF784;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF490u);
        goto block_800FF490;  /* not taken */
    }

block_800FF490:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF490u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF490u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF490u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF490u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 6;  /* 0x800FF490: 0x24020006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF490u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF498u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF498: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF498u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF49C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF49Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF49Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF49Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF49Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF4A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 3;  /* 0x800FF4A0: 0x24040003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80101FE8u);
    cpu->pc = 0x80101FE8u; return;  /* CPS jal -> 0x80101FE8 */

block_800FF4A4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF4A4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF4A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF4A4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF4A4: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF4A8: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF4B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27072;  /* 0x800FF4B0: 0x24A59640 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF4B4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF4B4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF4B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF4B4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF4B4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20001u);
#endif
    cpu->gpr[17] = 1;  /* 0x800FF4B8: 0x24110001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4B8u);
#endif
    if (_bc_800FF4B4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF4E8u);
        goto block_800FF4E8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF4BCu);
        goto block_800FF4BC;  /* not taken */
    }

block_800FF4BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF4BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF4BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF4BCu, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4BCu);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 44, 2, 0x20000000u);  /* 0x800FF4BC: 0x8FA2002C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4C0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[29] + 48, 3, 0x20000000u);  /* 0x800FF4C0: 0x8FA30030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4C0u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[29] + 52, 4, 0x20000000u);  /* 0x800FF4C4: 0x8FA40034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FF4C8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x800FF4C8: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000008u);
#endif
    g_debug_last_store_pc = 0x800FF4CCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[3]);  /* 0x800FF4CC: 0xAFA30014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    g_debug_last_store_pc = 0x800FF4D0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[4]);  /* 0x800FF4D0: 0xAFA40018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4D0u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 5, 0x20000000u);  /* 0x800FF4D4: 0x8FA50020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4D4u);
#endif
    cpu->gpr[6] = psx_cyc_load_word(cpu, cpu->gpr[29] + 36, 6, 0x20000000u);  /* 0x800FF4D8: 0x8FA60024 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4D8u);
#endif
    cpu->gpr[7] = psx_cyc_load_word(cpu, cpu->gpr[29] + 40, 7, 0x20000000u);  /* 0x800FF4DC: 0x8FA70028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF4E4: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF53Cu);
    goto block_800FF53C;  /* j */

block_800FF4E8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF4E8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF4E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF4E8u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF4E8: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF4EC: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF4F8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27056;  /* 0x800FF4F4: 0x24A59650 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF4F8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF4F8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF4F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF4F8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF4F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF4F8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF4FC: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF4FCu);
#endif
    if (_bc_800FF4F8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF554u);
        goto block_800FF554;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF500u);
        goto block_800FF500;  /* not taken */
    }

block_800FF500:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF500u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF500u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF500u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF500u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF500: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF500u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF50Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27040;  /* 0x800FF508: 0x24A59660 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF508u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF50C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF50Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF50Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF50Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF50Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF50C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF510u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20001u);
#endif
    cpu->gpr[17] = 2;  /* 0x800FF510: 0x24110002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF510u);
#endif
    if (_bc_800FF50C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF554u);
        goto block_800FF554;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF514u);
        goto block_800FF514;  /* not taken */
    }

block_800FF514:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF514u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF514u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF514u, 10u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF514u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 44, 2, 0x20000000u);  /* 0x800FF514: 0x8FA2002C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF514u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[29] + 48, 3, 0x20000000u);  /* 0x800FF518: 0x8FA30030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF518u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[29] + 52, 4, 0x20000000u);  /* 0x800FF51C: 0x8FA40034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF51Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF520u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FF520u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x800FF520: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF520u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000008u);
#endif
    g_debug_last_store_pc = 0x800FF524u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[3]);  /* 0x800FF524: 0xAFA30014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF524u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    g_debug_last_store_pc = 0x800FF528u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[4]);  /* 0x800FF528: 0xAFA40018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF528u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FF52C: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF52Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF530u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 5, 0x20000000u);  /* 0x800FF530: 0x8FA50020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF530u);
#endif
    cpu->gpr[6] = psx_cyc_load_word(cpu, cpu->gpr[29] + 36, 6, 0x20000000u);  /* 0x800FF534: 0x8FA60024 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF534u);
#endif
    cpu->gpr[7] = psx_cyc_load_word(cpu, cpu->gpr[29] + 40, 7, 0x20000000u);  /* 0x800FF538: 0x8FA70028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF538u);
#endif
    /* fall through to block_800FF53C */

block_800FF53C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF53Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF53Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF53Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF53Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF544u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF540u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF540: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF540u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ADB0u);
    cpu->pc = 0x8006ADB0u; return;  /* CPS jal -> 0x8006ADB0 */

block_800FF544:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF544u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF544u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF544u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF544u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF54Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[4] = cpu->gpr[17];  /* move */  /* 0x800FF548: 0x02202021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF548u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ACDCu);
    cpu->pc = 0x8006ACDCu; return;  /* CPS jal -> 0x8006ACDC */

block_800FF54C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF54Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF54Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF54Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF54Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF550u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF550: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF550u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF578u);
    goto block_800FF578;  /* j */

block_800FF554:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF554u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF554u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF554u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF554u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF554: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF554u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF558: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF558u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF564u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF560u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27016;  /* 0x800FF560: 0x24A59678 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF560u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF564:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF564u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF564u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF564u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF564u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF564 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20001u);
#endif
    cpu->gpr[17] = -1;  /* 0x800FF568: 0x2411FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF568u);
#endif
    if (_bc_800FF564) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF578u);
        goto block_800FF578;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF56Cu);
        goto block_800FF56C;  /* not taken */
    }

block_800FF56C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF56Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF56Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF56Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF56Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF574u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF570u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 3;  /* 0x800FF570: 0x24040003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF570u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ACDCu);
    cpu->pc = 0x8006ACDCu; return;  /* CPS jal -> 0x8006ACDC */

block_800FF574:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF574u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF574u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF574u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF574u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20001u);
#endif
    cpu->gpr[17] = 3;  /* 0x800FF574: 0x24110003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF574u);
#endif
    /* fall through to block_800FF578 */

block_800FF578:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF578u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF578u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF578u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF578u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF580u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF57C: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF57Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8010215Cu);
    cpu->pc = 0x8010215Cu; return;  /* CPS jal -> 0x8010215C */

block_800FF580:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF580u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF580u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF580u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF580u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF580: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF580u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[2] + -9072;  /* 0x800FF584: 0x2443DC90 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF584u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[3] + 12, 2, 0x8u);  /* 0x800FF588: 0x8462000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF588u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF58C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF58Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF590u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF590 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[17] + -1;  /* 0x800FF594: 0x2622FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF594u);
#endif
    if (_bc_800FF590) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF5CCu);
        goto block_800FF5CC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF598u);
        goto block_800FF598;  /* not taken */
    }

block_800FF598:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF598u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF598u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF598u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF598u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (cpu->gpr[2] < (uint32_t)3) ? 1 : 0;  /* 0x800FF598: 0x2C420003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF598u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF59C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x30000u);
#endif
    cpu->gpr[16] = cpu->gpr[17] << 1;  /* 0x800FF5A0: 0x00118040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5A0u);
#endif
    if (_bc_800FF59C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF5FCu);
        goto block_800FF5FC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF5A4u);
        goto block_800FF5A4;  /* not taken */
    }

block_800FF5A4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5A4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5A4u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10010u);
#endif
    cpu->gpr[4] = cpu->gpr[16] + -2;  /* 0x800FF5A4: 0x2604FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20008u);
#endif
    g_debug_last_store_pc = 0x800FF5A8u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[3] + 1, (uint8_t)cpu->gpr[17]);  /* 0x800FF5A8: 0xA0710001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF5AC: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FF5B0: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF5BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF5B8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF5B8: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF5BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5BCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF5C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10010u);
#endif
    cpu->gpr[4] = cpu->gpr[16] + -1;  /* 0x800FF5C0: 0x2604FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF5C4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5C4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5C4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF5C8: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF5CC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5CCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5CCu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 3;  /* 0x800FF5CC: 0x24020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    int _bc_800FF5D0 = (cpu->gpr[17] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 4;  /* 0x800FF5D4: 0x24040004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5D4u);
#endif
    if (_bc_800FF5D0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF5FCu);
        goto block_800FF5FC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF5D8u);
        goto block_800FF5D8;  /* not taken */
    }

block_800FF5D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5D8u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20008u);
#endif
    g_debug_last_store_pc = 0x800FF5D8u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[3] + 1, (uint8_t)cpu->gpr[17]);  /* 0x800FF5D8: 0xA0710001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF5DC: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x800FF5E0: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF5ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF5E8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF5E8: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF5EC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5ECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5ECu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF5F4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 5;  /* 0x800FF5F0: 0x24040005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80102AE4u);
    cpu->pc = 0x80102AE4u; return;  /* CPS jal -> 0x80102AE4 */

block_800FF5F4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5F4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5F4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF5F8: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF5FC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF5FCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF5FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF5FCu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF5FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF5FC: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF5FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF600u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 4;  /* 0x800FF600: 0x24020004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF600u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF608u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF608: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF608u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF60C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF60Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF60Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF60Cu, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF60Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF60C: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF60Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF610u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[2] + -8452, 16, 0x4u);  /* 0x800FF610: 0x8C50DEFC */
#ifdef PSX_COSIM
    cosim_instr(0x800FF610u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FF614: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF614u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + 17368;  /* 0x800FF618: 0x244243D8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF618u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] << 4;  /* 0x800FF61C: 0x00108100 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF61Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF620u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + cpu->gpr[2];  /* 0x800FF620: 0x02028021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF620u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF62Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FF628: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF628u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007BC90u);
    cpu->pc = 0x8007BC90u; return;  /* CPS jal -> 0x8007BC90 */

block_800FF62C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF62Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF62Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF62Cu, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF62Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF62C: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF62Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF630u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10021u);
#endif
    cpu->gpr[5] = cpu->gpr[16];  /* move */  /* 0x800FF630: 0x02002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF630u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FF634u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 6, (uint8_t)cpu->gpr[0]);  /* 0x800FF634: 0xA2000006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF634u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FF638u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 5, (uint8_t)cpu->gpr[0]);  /* 0x800FF638: 0xA2000005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF638u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    g_debug_last_store_pc = 0x800FF63Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[16] + 4, (uint8_t)cpu->gpr[0]);  /* 0x800FF63C: 0xA2000004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF63Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF640u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[2] + -28136, 4, 0x4u);  /* 0x800FF640: 0x8C449218 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF640u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 368;  /* 0x800FF644: 0x24020170 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF644u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FF648u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 12, (uint16_t)cpu->gpr[2]);  /* 0x800FF648: 0xA4A2000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF648u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 480;  /* 0x800FF64C: 0x240201E0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF64Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF650u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    g_debug_last_store_pc = 0x800FF650u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 8, (uint16_t)cpu->gpr[0]);  /* 0x800FF650: 0xA4A00008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF650u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    g_debug_last_store_pc = 0x800FF654u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 10, (uint16_t)cpu->gpr[0]);  /* 0x800FF654: 0xA4A0000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FF654u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF660u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FF65Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5] + 14, (uint16_t)cpu->gpr[2]);  /* 0x800FF65C: 0xA4A2000E */
#ifdef PSX_COSIM
    cosim_instr(0x800FF65Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007BA74u);
    cpu->pc = 0x8007BA74u; return;  /* CPS jal -> 0x8007BA74 */

block_800FF660:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF660u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF660u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF660u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF660u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF668u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF664: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF664u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006B54Cu);
    cpu->pc = 0x8006B54Cu; return;  /* CPS jal -> 0x8006B54C */

block_800FF668:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF668u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF668u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF668u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF668u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF668: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF668u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF66C: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF66Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF670u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF678u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27072;  /* 0x800FF674: 0x24A59640 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF674u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF678:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF678u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF678u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF678u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF678u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF678 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF67C: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF67Cu);
#endif
    if (_bc_800FF678) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF6BCu);
        goto block_800FF6BC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF680u);
        goto block_800FF680;  /* not taken */
    }

block_800FF680:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF680u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF680u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF680u, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF680u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 44, 2, 0x20000000u);  /* 0x800FF680: 0x8FA2002C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF680u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[29] + 48, 3, 0x20000000u);  /* 0x800FF684: 0x8FA30030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF684u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[29] + 52, 4, 0x20000000u);  /* 0x800FF688: 0x8FA40034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF688u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FF68Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x800FF68C: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF68Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF690u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000008u);
#endif
    g_debug_last_store_pc = 0x800FF690u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[3]);  /* 0x800FF690: 0xAFA30014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF690u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    g_debug_last_store_pc = 0x800FF694u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[4]);  /* 0x800FF694: 0xAFA40018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF694u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 5, 0x20000000u);  /* 0x800FF698: 0x8FA50020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF698u);
#endif
    cpu->gpr[6] = psx_cyc_load_word(cpu, cpu->gpr[29] + 36, 6, 0x20000000u);  /* 0x800FF69C: 0x8FA60024 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF69Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6A0u);
#endif
    cpu->gpr[7] = psx_cyc_load_word(cpu, cpu->gpr[29] + 40, 7, 0x20000000u);  /* 0x800FF6A0: 0x8FA70028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF6ACu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x800FF6A8: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ADB0u);
    cpu->pc = 0x8006ADB0u; return;  /* CPS jal -> 0x8006ADB0 */

block_800FF6AC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF6ACu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF6ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF6ACu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF6B4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FF6B0: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ACDCu);
    cpu->pc = 0x8006ACDCu; return;  /* CPS jal -> 0x8006ACDC */

block_800FF6B4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF6B4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF6B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF6B4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = 1;  /* 0x800FF6B8: 0x24030001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF740u);
    goto block_800FF740;  /* j */

block_800FF6BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF6BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF6BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF6BCu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF6BC: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF6C8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27056;  /* 0x800FF6C4: 0x24A59650 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF6C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF6C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF6C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF6C8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF6C8 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF6CC: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6CCu);
#endif
    if (_bc_800FF6C8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF720u);
        goto block_800FF720;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF6D0u);
        goto block_800FF6D0;  /* not taken */
    }

block_800FF6D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF6D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF6D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF6D0u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF6D0: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF6DCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27040;  /* 0x800FF6D8: 0x24A59660 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF6DC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF6DCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF6DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF6DCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF6DC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 32;  /* 0x800FF6E0: 0x27A40020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6E0u);
#endif
    if (_bc_800FF6DC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF720u);
        goto block_800FF720;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF6E4u);
        goto block_800FF6E4;  /* not taken */
    }

block_800FF6E4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF6E4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF6E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF6E4u, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6E4u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 44, 2, 0x20000000u);  /* 0x800FF6E4: 0x8FA2002C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6E4u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[29] + 48, 3, 0x20000000u);  /* 0x800FF6E8: 0x8FA30030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6E8u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[29] + 52, 4, 0x20000000u);  /* 0x800FF6EC: 0x8FA40034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF6F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FF6F0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x800FF6F0: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000008u);
#endif
    g_debug_last_store_pc = 0x800FF6F4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[3]);  /* 0x800FF6F4: 0xAFA30014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    g_debug_last_store_pc = 0x800FF6F8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[4]);  /* 0x800FF6F8: 0xAFA40018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6F8u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 5, 0x20000000u);  /* 0x800FF6FC: 0x8FA50020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF6FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF700u);
#endif
    cpu->gpr[6] = psx_cyc_load_word(cpu, cpu->gpr[29] + 36, 6, 0x20000000u);  /* 0x800FF700: 0x8FA60024 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF700u);
#endif
    cpu->gpr[7] = psx_cyc_load_word(cpu, cpu->gpr[29] + 40, 7, 0x20000000u);  /* 0x800FF704: 0x8FA70028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF704u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF710u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FF70C: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF70Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ADB0u);
    cpu->pc = 0x8006ADB0u; return;  /* CPS jal -> 0x8006ADB0 */

block_800FF710:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF710u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF710u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF710u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF710u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF718u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 2;  /* 0x800FF714: 0x24040002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF714u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ACDCu);
    cpu->pc = 0x8006ACDCu; return;  /* CPS jal -> 0x8006ACDC */

block_800FF718:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF718u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF718u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF718u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF718u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = 2;  /* 0x800FF71C: 0x24030002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF71Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF740u);
    goto block_800FF740;  /* j */

block_800FF720:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF720u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF720u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF720u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF720u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF720: 0x3C05800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF720u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF72Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + -27016;  /* 0x800FF728: 0x24A59678 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF728u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80091558u);
    cpu->pc = 0x80091558u; return;  /* CPS jal -> 0x80091558 */

block_800FF72C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF72Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF72Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF72Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF72Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF72C = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF730u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = -1;  /* 0x800FF730: 0x2403FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF730u);
#endif
    if (_bc_800FF72C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF740u);
        goto block_800FF740;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF734u);
        goto block_800FF734;  /* not taken */
    }

block_800FF734:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF734u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF734u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF734u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF734u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF73Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 3;  /* 0x800FF738: 0x24040003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF738u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8006ACDCu);
    cpu->pc = 0x8006ACDCu; return;  /* CPS jal -> 0x8006ACDC */

block_800FF73C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF73Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF73Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF73Cu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF73Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = 3;  /* 0x800FF73C: 0x24030003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF73Cu);
#endif
    /* fall through to block_800FF740 */

block_800FF740:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF740u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF740u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 3;  /* 0x800FF740: 0x24020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    int _bc_800FF744 = (cpu->gpr[3] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF748: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF748u);
#endif
    if (_bc_800FF744) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF75Cu);
        goto block_800FF75C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF74Cu);
        goto block_800FF74C;  /* not taken */
    }

block_800FF74C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF74Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF74Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF74Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF74Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF754u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF750u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 4;  /* 0x800FF750: 0x24040004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF750u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004F8F8u);
    cpu->pc = 0x8004F8F8u; return;  /* CPS jal -> 0x8004F8F8 */

block_800FF754:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF754u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF754u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF754u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF754u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF758: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF758u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF788u);
    goto block_800FF788;  /* j */

block_800FF75C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF75Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF75Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF75Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF75Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF764u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF760u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 5;  /* 0x800FF760: 0x24040005 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF760u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80101FE8u);
    cpu->pc = 0x80101FE8u; return;  /* CPS jal -> 0x80101FE8 */

block_800FF764:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF764u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF764u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF764u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF764u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[17] & 0x08F0;  /* 0x800FF764: 0x322208F0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF764u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF768 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF76C: 0x3C03800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF76Cu);
#endif
    if (_bc_800FF768) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF784u);
        goto block_800FF784;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF770u);
        goto block_800FF770;  /* not taken */
    }

block_800FF770:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF770u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF770u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF770u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF770u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 7;  /* 0x800FF770: 0x24020007 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF770u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF778u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + -7644, (uint16_t)cpu->gpr[2]);  /* 0x800FF778: 0xA462E224 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF778u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF784u);
    goto block_800FF784;  /* j */

block_800FF77C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF77Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF77Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF77Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF77Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF784u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF780u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x800FF780: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF780u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007AE58u);
    cpu->pc = 0x8007AE58u; return;  /* CPS jal -> 0x8007AE58 */

block_800FF784:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF784u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF784u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF784u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF784u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FF784: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF784u);
#endif
    /* fall through to block_800FF788 */

block_800FF788:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF788u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF788u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF788u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF788u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40004u);
#endif
    g_debug_last_store_pc = 0x800FF788u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2] + -8348, cpu->gpr[18]);  /* 0x800FF788: 0xAC52DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF788u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 68, 31, 0x20000000u);  /* 0x800FF78C: 0x8FBF0044 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF78Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF790u);
#endif
    cpu->gpr[18] = psx_cyc_load_word(cpu, cpu->gpr[29] + 64, 18, 0x20000000u);  /* 0x800FF790: 0x8FB20040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF790u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 60, 17, 0x20000000u);  /* 0x800FF794: 0x8FB1003C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF794u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 56, 16, 0x20000000u);  /* 0x800FF798: 0x8FB00038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF798u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 72;  /* 0x800FF79C: 0x27BD0048 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF79Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF7A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FF7A0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF7A4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FF7A0);
    cpu->pc = _jt_800FF7A0; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FF7C0(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FF7D0u: goto block_800FF7D0;
            case 0x800FF828u: goto block_800FF828;
            case 0x800FF7C0u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FF7C0u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FF7C0u);
    /* Address: 0x800FF7C0, Size: 112 bytes, Blocks: 3 */

block_800FF7C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF7C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF7C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF7C0u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF7C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8018 << 16;  /* 0x80180000 */  /* 0x800FF7C0: 0x3C028018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FF7C4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[2] + 19648, (uint16_t)cpu->gpr[4]);  /* 0x800FF7C4: 0xA4444CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    int _bc_800FF7C8 = ((int32_t)cpu->gpr[4] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x404u);
#endif
    cpu->gpr[10] = cpu->gpr[2] + 19648;  /* 0x800FF7CC: 0x244A4CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7CCu);
#endif
    if (_bc_800FF7C8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF828u);
        goto block_800FF828;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF7D0u);
        goto block_800FF7D0;  /* not taken */
    }

block_800FF7D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF7D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF7D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF7D0u, 22u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF7D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF7D0: 0x3C03800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + -26920;  /* 0x800FF7D4: 0x246396D8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[4] << 2;  /* 0x800FF7D8: 0x00041080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[4];  /* 0x800FF7DC: 0x00441021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF7E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FF7E0: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FF7E4: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7E4u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[2], 3, 0x4u);  /* 0x800FF7E8: 0x8C430000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7E8u);
#endif
    cpu->gpr[4] = psx_cyc_load_half(cpu, cpu->gpr[2] + 4, 4, 0x4u);  /* 0x800FF7EC: 0x94440004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF7F0u);
#endif
    cpu->gpr[5] = psx_cyc_load_half(cpu, cpu->gpr[2] + 6, 5, 0x4u);  /* 0x800FF7F0: 0x94450006 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7F0u);
#endif
    cpu->gpr[6] = psx_cyc_load_half(cpu, cpu->gpr[2] + 8, 6, 0x4u);  /* 0x800FF7F4: 0x94460008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7F4u);
#endif
    cpu->gpr[7] = psx_cyc_load_half(cpu, cpu->gpr[2] + 10, 7, 0x4u);  /* 0x800FF7F8: 0x9447000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7F8u);
#endif
    cpu->gpr[8] = psx_cyc_load_half(cpu, cpu->gpr[2] + 12, 8, 0x4u);  /* 0x800FF7FC: 0x9448000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF7FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF800u);
#endif
    cpu->gpr[9] = psx_cyc_load_half(cpu, cpu->gpr[2] + 14, 9, 0x4u);  /* 0x800FF800: 0x9449000E */
#ifdef PSX_COSIM
    cosim_instr(0x800FF800u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[2] + 16, 2, 0x4u);  /* 0x800FF804: 0x94420010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF804u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x408u);
#endif
    g_debug_last_store_pc = 0x800FF808u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[10] + 4, cpu->gpr[3]);  /* 0x800FF808: 0xAD430004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF808u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x410u);
#endif
    g_debug_last_store_pc = 0x800FF80Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[10] + 8, (uint16_t)cpu->gpr[4]);  /* 0x800FF80C: 0xA5440008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF80Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF810u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x420u);
#endif
    g_debug_last_store_pc = 0x800FF810u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[10] + 10, (uint16_t)cpu->gpr[5]);  /* 0x800FF810: 0xA545000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FF810u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x440u);
#endif
    g_debug_last_store_pc = 0x800FF814u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[10] + 12, (uint16_t)cpu->gpr[6]);  /* 0x800FF814: 0xA546000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF814u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x480u);
#endif
    g_debug_last_store_pc = 0x800FF818u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[10] + 14, (uint16_t)cpu->gpr[7]);  /* 0x800FF818: 0xA547000E */
#ifdef PSX_COSIM
    cosim_instr(0x800FF818u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x500u);
#endif
    g_debug_last_store_pc = 0x800FF81Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[10] + 16, (uint16_t)cpu->gpr[8]);  /* 0x800FF81C: 0xA5480010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF81Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF820u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x600u);
#endif
    g_debug_last_store_pc = 0x800FF820u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[10] + 18, (uint16_t)cpu->gpr[9]);  /* 0x800FF820: 0xA5490012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF820u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x404u);
#endif
    g_debug_last_store_pc = 0x800FF824u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[10] + 20, (uint16_t)cpu->gpr[2]);  /* 0x800FF824: 0xA5420014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF824u);
#endif
    /* fall through to block_800FF828 */

block_800FF828:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF828u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF828u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF828u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF828u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FF828 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF82C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF82Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FF828);
    cpu->pc = _jt_800FF828; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FF830(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FF8C4u: goto block_800FF8C4;
            case 0x800FF840u: goto block_800FF840;
            case 0x800FF858u: goto block_800FF858;
            case 0x800FF864u: goto block_800FF864;
            case 0x800FF874u: goto block_800FF874;
            case 0x800FF880u: goto block_800FF880;
            case 0x800FF890u: goto block_800FF890;
            case 0x800FF894u: goto block_800FF894;
            case 0x800FF8A4u: goto block_800FF8A4;
            case 0x800FF8CCu: goto block_800FF8CC;
            case 0x800FF830u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FF830u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FF830u);
    /* Address: 0x800FF830, Size: 176 bytes, Blocks: 11 */

block_800FF830:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF830u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF830u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF830u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF830u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x800FF830: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF830u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FF834u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);  /* 0x800FF834: 0xAFBF0014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF834u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    int _bc_800FF838 = ((int32_t)cpu->gpr[4] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FF83Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);  /* 0x800FF83C: 0xAFB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF83Cu);
#endif
    if (_bc_800FF838) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF858u);
        goto block_800FF858;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF840u);
        goto block_800FF840;  /* not taken */
    }

block_800FF840:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF840u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF840u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF840u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF840u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8018 << 16;  /* 0x80180000 */  /* 0x800FF840: 0x3C048018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF840u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    cpu->gpr[3] = cpu->gpr[4] + 19648;  /* 0x800FF844: 0x24834CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF844u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x800FF848: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF848u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x800FF84Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[3] + 2, (uint16_t)cpu->gpr[2]);  /* 0x800FF84C: 0xA4620002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF84Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF850u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x800FF854u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 19648, (uint16_t)cpu->gpr[2]);  /* 0x800FF854: 0xA4824CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF854u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF8CCu);
    goto block_800FF8CC;  /* j */

block_800FF858:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF858u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF858u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF858u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF858u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x800FF858: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF858u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF85C: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF85Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF860u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[2] + -26740;  /* 0x800FF860: 0x2443978C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF860u);
#endif
    /* fall through to block_800FF864 */

block_800FF864:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF864u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF864u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF864u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF864u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FF864: 0x94620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF864u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF868: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF868u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF86C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF870u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF870: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF870u);
#endif
    if (_bc_800FF86C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF880u);
        goto block_800FF880;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF874u);
        goto block_800FF874;  /* not taken */
    }

block_800FF874:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF874u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF874u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF874u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF874u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + -1;  /* 0x800FF874: 0x2484FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF874u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    int _bc_800FF878 = ((int32_t)cpu->gpr[4] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = 0x8018 << 16;  /* 0x80180000 */  /* 0x800FF87C: 0x3C108018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF87Cu);
#endif
    if (_bc_800FF878) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF894u);
        goto block_800FF894;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF880u);
        goto block_800FF880;  /* not taken */
    }

block_800FF880:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF880u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF880u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF880u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF880u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 1;  /* 0x800FF880: 0x24A50001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF880u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = (cpu->gpr[5] < (uint32_t)30) ? 1 : 0;  /* 0x800FF884: 0x2CA2001E */
#ifdef PSX_COSIM
    cosim_instr(0x800FF884u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF888 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 4;  /* 0x800FF88C: 0x24630004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF88Cu);
#endif
    if (_bc_800FF888) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF864u);
        goto block_800FF864;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF890u);
        goto block_800FF890;  /* not taken */
    }

block_800FF890:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF890u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF890u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF890u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF890u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = 0x8018 << 16;  /* 0x80180000 */  /* 0x800FF890: 0x3C108018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF890u);
#endif
    /* fall through to block_800FF894 */

block_800FF894:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF894u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF894u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF894u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF894u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    cpu->gpr[2] = cpu->gpr[16] + 19648;  /* 0x800FF894: 0x26024CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF894u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FF898u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[2] + 2, (uint16_t)cpu->gpr[5]);  /* 0x800FF898: 0xA4450002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF898u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    int _bc_800FF89C = ((int32_t)cpu->gpr[4] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10020u);
#endif
    g_debug_last_store_pc = 0x800FF8A0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 19648, (uint16_t)cpu->gpr[5]);  /* 0x800FF8A0: 0xA6054CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8A0u);
#endif
    if (_bc_800FF89C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF8C4u);
        goto block_800FF8C4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF8A4u);
        goto block_800FF8A4;  /* not taken */
    }

block_800FF8A4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF8A4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF8A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF8A4u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF8A4: 0x3C03800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + -26740;  /* 0x800FF8A8: 0x2463978C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = cpu->gpr[5] << 16;  /* 0x800FF8AC: 0x00051400 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 14;  /* 0x800FF8B0: 0x00021383 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FF8B4: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8B4u);
#endif
    cpu->gpr[4] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 2, 4, 0x4u);  /* 0x800FF8B8: 0x84440002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF8C4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF8C0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF7C0u);
    cpu->pc = 0x800FF7C0u; return;  /* CPS jal -> 0x800FF7C0 */

block_800FF8C4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF8C4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF8C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF8C4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x800FF8C4: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x800FF8C8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[16] + 19648, (uint16_t)cpu->gpr[2]);  /* 0x800FF8C8: 0xA6024CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8C8u);
#endif
    /* fall through to block_800FF8CC */

block_800FF8CC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF8CCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF8CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF8CCu, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8CCu);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 31, 0x20000000u);  /* 0x800FF8CC: 0x8FBF0014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8D0u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 16, 0x20000000u);  /* 0x800FF8D0: 0x8FB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x800FF8D4: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FF8D8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF8DC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FF8D8);
    cpu->pc = _jt_800FF8D8; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FF8E8(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FF954u: goto block_800FF954;
            case 0x800FF8F4u: goto block_800FF8F4;
            case 0x800FF8FCu: goto block_800FF8FC;
            case 0x800FF91Cu: goto block_800FF91C;
            case 0x800FF934u: goto block_800FF934;
            case 0x800FF8E8u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FF8E8u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FF8E8u);
    /* Address: 0x800FF8E8, Size: 124 bytes, Blocks: 6 */

block_800FF8E8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF8E8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF8E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF8E8u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x800FF8E8: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    int _bc_800FF8EC = ((int32_t)cpu->gpr[4] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FF8F0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x800FF8F0: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8F0u);
#endif
    if (_bc_800FF8EC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF954u);
        goto block_800FF954;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF8F4u);
        goto block_800FF8F4;  /* not taken */
    }

block_800FF8F4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF8F4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF8F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF8F4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    int _bc_800FF8F4 = ((int32_t)cpu->gpr[5] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF8F8: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8F8u);
#endif
    if (_bc_800FF8F4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF954u);
        goto block_800FF954;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF8FCu);
        goto block_800FF8FC;  /* not taken */
    }

block_800FF8FC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF8FCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF8FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF8FCu, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF8FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -26740;  /* 0x800FF8FC: 0x2442978C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF8FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF900u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x28u);
#endif
    cpu->gpr[3] = cpu->gpr[5] << 2;  /* 0x800FF900: 0x00051880 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF900u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];  /* 0x800FF904: 0x00621821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF904u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FF908: 0x94620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF908u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF90C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF90Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF910u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[4]) ? 1 : 0;  /* 0x800FF910: 0x0044102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FF910u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF914 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF918: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF918u);
#endif
    if (_bc_800FF914) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF934u);
        goto block_800FF934;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF91Cu);
        goto block_800FF91C;  /* not taken */
    }

block_800FF91C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF91Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF91Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF91Cu, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF91Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 4;  /* 0x800FF91C: 0x24630004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF91Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF920u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x800FF920: 0x94620000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF920u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF924: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF924u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[2] < (int32_t)cpu->gpr[4]) ? 1 : 0;  /* 0x800FF928: 0x0044102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FF928u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF92C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF930u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 1;  /* 0x800FF930: 0x24A50001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF930u);
#endif
    if (_bc_800FF92C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF91Cu);
        goto block_800FF91C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF934u);
        goto block_800FF934;  /* not taken */
    }

block_800FF934:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF934u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF934u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF934u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF934u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF934: 0x3C03800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF934u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + -26740;  /* 0x800FF938: 0x2463978C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF938u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = cpu->gpr[5] + -1;  /* 0x800FF93C: 0x24A2FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF93Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF940u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 2;  /* 0x800FF940: 0x00021080 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF940u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FF944: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF944u);
#endif
    cpu->gpr[4] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 2, 4, 0x4u);  /* 0x800FF948: 0x84440002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF948u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FF954u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF950u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF950: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF950u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF7C0u);
    cpu->pc = 0x800FF7C0u; return;  /* CPS jal -> 0x800FF7C0 */

block_800FF954:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF954u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF954u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF954u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF954u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x800FF954: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF954u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x800FF958: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF958u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FF95C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF960u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF960: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF960u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FF95C);
    cpu->pc = _jt_800FF95C; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FF964(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FF970u: goto block_800FF970;
            case 0x800FF978u: goto block_800FF978;
            case 0x800FF980u: goto block_800FF980;
            case 0x800FF988u: goto block_800FF988;
            case 0x800FF994u: goto block_800FF994;
            case 0x800FF99Cu: goto block_800FF99C;
            case 0x800FF9A8u: goto block_800FF9A8;
            case 0x800FF9B4u: goto block_800FF9B4;
            case 0x800FF9BCu: goto block_800FF9BC;
            case 0x800FF9C0u: goto block_800FF9C0;
            case 0x800FF9C8u: goto block_800FF9C8;
            case 0x800FF9D0u: goto block_800FF9D0;
            case 0x800FFA04u: goto block_800FFA04;
            case 0x800FFA18u: goto block_800FFA18;
            case 0x800FFA30u: goto block_800FFA30;
            case 0x800FFA78u: goto block_800FFA78;
            case 0x800FFA80u: goto block_800FFA80;
            case 0x800FFA90u: goto block_800FFA90;
            case 0x800FFAB4u: goto block_800FFAB4;
            case 0x800FFAFCu: goto block_800FFAFC;
            case 0x800FFB04u: goto block_800FFB04;
            case 0x800FFB14u: goto block_800FFB14;
            case 0x800FFB2Cu: goto block_800FFB2C;
            case 0x800FFB74u: goto block_800FFB74;
            case 0x800FFB7Cu: goto block_800FFB7C;
            case 0x800FFB8Cu: goto block_800FFB8C;
            case 0x800FFBB0u: goto block_800FFBB0;
            case 0x800FFBF8u: goto block_800FFBF8;
            case 0x800FFC0Cu: goto block_800FFC0C;
            case 0x800FFC1Cu: goto block_800FFC1C;
            case 0x800FF964u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FF964u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FF964u);
    /* Address: 0x800FF964, Size: 704 bytes, Blocks: 31 */

block_800FF964:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF964u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF964u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF964u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF964u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 2;  /* 0x800FF964: 0x24020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF964u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    int _bc_800FF968 = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[7] < 3) ? 1 : 0;  /* 0x800FF96C: 0x28E20003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF96Cu);
#endif
    if (_bc_800FF968) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF9A8u);
        goto block_800FF9A8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF970u);
        goto block_800FF970;  /* not taken */
    }

block_800FF970:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF970u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF970u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF970u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF970u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF970 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF974: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF974u);
#endif
    if (_bc_800FF970) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF988u);
        goto block_800FF988;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF978u);
        goto block_800FF978;  /* not taken */
    }

block_800FF978:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF978u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF978u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF978u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF978u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x81u);
#endif
    int _bc_800FF978 = (cpu->gpr[7] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4001u);
#endif
    cpu->gpr[14] = cpu->gpr[0];  /* move */  /* 0x800FF97C: 0x00007021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF97Cu);
#endif
    if (_bc_800FF978) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF99Cu);
        goto block_800FF99C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF980u);
        goto block_800FF980;  /* not taken */
    }

block_800FF980:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF980u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF980u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF980u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF980u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF984: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF984u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF9C0u);
    goto block_800FF9C0;  /* j */

block_800FF988:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF988u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF988u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF988u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF988u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 3;  /* 0x800FF988: 0x24020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF988u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    int _bc_800FF98C = (cpu->gpr[7] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF990u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4001u);
#endif
    cpu->gpr[14] = cpu->gpr[0];  /* move */  /* 0x800FF990: 0x00007021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF990u);
#endif
    if (_bc_800FF98C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF9B4u);
        goto block_800FF9B4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF994u);
        goto block_800FF994;  /* not taken */
    }

block_800FF994:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF994u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF994u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF994u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF994u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF998: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF998u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF9C0u);
    goto block_800FF9C0;  /* j */

block_800FF99C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF99Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF99Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF99Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF99Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF99C: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF99Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x804u);
#endif
    cpu->gpr[11] = cpu->gpr[2] + -26620;  /* 0x800FF9A4: 0x244B9804 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF9BCu);
    goto block_800FF9BC;  /* j */

block_800FF9A8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF9A8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF9A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF9A8u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF9A8: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x804u);
#endif
    cpu->gpr[11] = cpu->gpr[2] + -26492;  /* 0x800FF9B0: 0x244B9884 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF9BCu);
    goto block_800FF9BC;  /* j */

block_800FF9B4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF9B4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF9B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF9B4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FF9B4: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x804u);
#endif
    cpu->gpr[11] = cpu->gpr[2] + -26364;  /* 0x800FF9B8: 0x244B9904 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9B8u);
#endif
    /* fall through to block_800FF9BC */

block_800FF9BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF9BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF9BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF9BCu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4001u);
#endif
    cpu->gpr[14] = cpu->gpr[0];  /* move */  /* 0x800FF9BC: 0x00007021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9BCu);
#endif
    /* fall through to block_800FF9C0 */

block_800FF9C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF9C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF9C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF9C0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    int _bc_800FF9C0 = ((int32_t)cpu->gpr[6] <= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF9C4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9C4u);
#endif
    if (_bc_800FF9C0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFC1Cu);
        goto block_800FFC1C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF9C8u);
        goto block_800FF9C8;  /* not taken */
    }

block_800FF9C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF9C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF9C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF9C8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2001u);
#endif
    cpu->gpr[13] = cpu->gpr[0];  /* move */  /* 0x800FF9C8: 0x00006821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0u);
#endif
    cpu->gpr[7] = cpu->gpr[5] + 11;  /* 0x800FF9CC: 0x24A7000B */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9CCu);
#endif
    /* fall through to block_800FF9D0 */

block_800FF9D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FF9D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FF9D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FF9D0u, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9D0u);
#endif
    cpu->gpr[3] = psx_cyc_load_half(cpu, cpu->gpr[4], 3, 0x10u);  /* 0x800FF9D0: 0x94830000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FF9D4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] & 0xFFFF;  /* 0x800FF9D8: 0x3063FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[3] >> 9;  /* 0x800FF9DC: 0x00031242 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1004u);
#endif
    cpu->gpr[12] = cpu->gpr[2] & 0x0078;  /* 0x800FF9E0: 0x304C0078 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[3] >> 5;  /* 0x800FF9E4: 0x00031142 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x404u);
#endif
    cpu->gpr[10] = cpu->gpr[2] & 0x0078;  /* 0x800FF9E8: 0x304A0078 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[3] >> 1;  /* 0x800FF9EC: 0x00031042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FF9F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x204u);
#endif
    cpu->gpr[9] = cpu->gpr[2] & 0x0078;  /* 0x800FF9F0: 0x30490078 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] << 3;  /* 0x800FF9F4: 0x000318C0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[3] & 0x0078;  /* 0x800FF9F8: 0x30620078 */
#ifdef PSX_COSIM
    cosim_instr(0x800FF9F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FF9FC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 2;  /* 0x800FFA00: 0x24840002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA00u);
#endif
    if (_bc_800FF9FC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFA78u);
        goto block_800FFA78;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFA04u);
        goto block_800FFA04;  /* not taken */
    }

block_800FFA04:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFA04u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFA04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFA04u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x904u);
#endif
    cpu->gpr[8] = cpu->gpr[2] + cpu->gpr[11];  /* 0x800FFA04: 0x004B4021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA04u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 3, 2, 0x100u);  /* 0x800FFA08: 0x91020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFA0C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFA10 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFA14: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA14u);
#endif
    if (_bc_800FFA10) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFA30u);
        goto block_800FFA30;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFA18u);
        goto block_800FFA18;  /* not taken */
    }

block_800FFA18:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFA18u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFA18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFA18u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA18u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[8], 2, 0x100u);  /* 0x800FFA18: 0x95020000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFA1C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FFA20u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[2]);  /* 0x800FFA20: 0xA4A20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA20u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 2, 0x100u);  /* 0x800FFA24: 0x91020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFA2Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -9, (uint8_t)cpu->gpr[2]);  /* 0x800FFA2C: 0xA0E2FFF7 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FFA78u);
    goto block_800FFA78;  /* j */

block_800FFA30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFA30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFA30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFA30u, 18u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA30u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[5], 2, 0x20u);  /* 0x800FFA30: 0x90A20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA30u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8], 3, 0x100u);  /* 0x800FFA34: 0x91030000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFA38: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFA3C: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFA40: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x800FFA44u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[5], (uint8_t)cpu->gpr[2]);  /* 0x800FFA44: 0xA0A20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA44u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -10, 2, 0x80u);  /* 0x800FFA48: 0x90E2FFF6 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA48u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 1, 3, 0x100u);  /* 0x800FFA4C: 0x91030001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFA50: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFA54: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFA58: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFA5Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -10, (uint8_t)cpu->gpr[2]);  /* 0x800FFA5C: 0xA0E2FFF6 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA60u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -9, 2, 0x80u);  /* 0x800FFA60: 0x90E2FFF7 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA60u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 3, 0x100u);  /* 0x800FFA64: 0x91030002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFA68: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFA6C: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFA70: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFA74u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -9, (uint8_t)cpu->gpr[2]);  /* 0x800FFA74: 0xA0E2FFF7 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA74u);
#endif
    /* fall through to block_800FFA78 */

block_800FFA78:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFA78u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFA78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFA78u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x201u);
#endif
    int _bc_800FFA78 = (cpu->gpr[9] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xB00u);
#endif
    cpu->gpr[8] = cpu->gpr[9] + cpu->gpr[11];  /* 0x800FFA7C: 0x012B4021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA7Cu);
#endif
    if (_bc_800FFA78) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFAFCu);
        goto block_800FFAFC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFA80u);
        goto block_800FFA80;  /* not taken */
    }

block_800FFA80:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFA80u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFA80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFA80u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA80u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 3, 2, 0x100u);  /* 0x800FFA80: 0x91020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFA84: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFA88 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFA8C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA8Cu);
#endif
    if (_bc_800FFA88) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFAB4u);
        goto block_800FFAB4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFA90u);
        goto block_800FFA90;  /* not taken */
    }

block_800FFA90:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFA90u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFA90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFA90u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFA90u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8], 2, 0x100u);  /* 0x800FFA90: 0x91020000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFA94: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFA98u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -8, (uint8_t)cpu->gpr[2]);  /* 0x800FFA98: 0xA0E2FFF8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA98u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 1, 2, 0x100u);  /* 0x800FFA9C: 0x91020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFA9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFAA0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFAA4u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -7, (uint8_t)cpu->gpr[2]);  /* 0x800FFAA4: 0xA0E2FFF9 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAA4u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 2, 0x100u);  /* 0x800FFAA8: 0x91020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFAB0u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -6, (uint8_t)cpu->gpr[2]);  /* 0x800FFAB0: 0xA0E2FFFA */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FFAFCu);
    goto block_800FFAFC;  /* j */

block_800FFAB4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFAB4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFAB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFAB4u, 18u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAB4u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -8, 2, 0x80u);  /* 0x800FFAB4: 0x90E2FFF8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAB4u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8], 3, 0x100u);  /* 0x800FFAB8: 0x91030000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFABC: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFABCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFAC0: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFAC4: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFAC8u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -8, (uint8_t)cpu->gpr[2]);  /* 0x800FFAC8: 0xA0E2FFF8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAC8u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -7, 2, 0x80u);  /* 0x800FFACC: 0x90E2FFF9 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFACCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAD0u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 1, 3, 0x100u);  /* 0x800FFAD0: 0x91030001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFAD4: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFAD8: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFADC: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFADCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFAE0u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -7, (uint8_t)cpu->gpr[2]);  /* 0x800FFAE0: 0xA0E2FFF9 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAE0u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -6, 2, 0x80u);  /* 0x800FFAE4: 0x90E2FFFA */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAE4u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 3, 0x100u);  /* 0x800FFAE8: 0x91030002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFAEC: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFAF0: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFAF4: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFAF8u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -6, (uint8_t)cpu->gpr[2]);  /* 0x800FFAF8: 0xA0E2FFFA */
#ifdef PSX_COSIM
    cosim_instr(0x800FFAF8u);
#endif
    /* fall through to block_800FFAFC */

block_800FFAFC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFAFCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFAFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFAFCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFAFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x401u);
#endif
    int _bc_800FFAFC = (cpu->gpr[10] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xD00u);
#endif
    cpu->gpr[8] = cpu->gpr[10] + cpu->gpr[11];  /* 0x800FFB00: 0x014B4021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB00u);
#endif
    if (_bc_800FFAFC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFB74u);
        goto block_800FFB74;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFB04u);
        goto block_800FFB04;  /* not taken */
    }

block_800FFB04:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFB04u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFB04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFB04u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB04u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 3, 2, 0x100u);  /* 0x800FFB04: 0x91020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFB08: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFB0C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFB10: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB10u);
#endif
    if (_bc_800FFB0C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFB2Cu);
        goto block_800FFB2C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFB14u);
        goto block_800FFB14;  /* not taken */
    }

block_800FFB14:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFB14u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFB14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFB14u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB14u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[8], 2, 0x100u);  /* 0x800FFB14: 0x95020000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFB18: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFB1Cu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[7] + -5, (uint16_t)cpu->gpr[2]);  /* 0x800FFB1C: 0xA4E2FFFB */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB20u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 2, 0x100u);  /* 0x800FFB20: 0x91020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFB28u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -3, (uint8_t)cpu->gpr[2]);  /* 0x800FFB28: 0xA0E2FFFD */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FFB74u);
    goto block_800FFB74;  /* j */

block_800FFB2C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFB2Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFB2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFB2Cu, 18u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB2Cu);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -5, 2, 0x80u);  /* 0x800FFB2C: 0x90E2FFFB */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB30u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8], 3, 0x100u);  /* 0x800FFB30: 0x91030000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFB34: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFB38: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFB3C: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFB40u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -5, (uint8_t)cpu->gpr[2]);  /* 0x800FFB40: 0xA0E2FFFB */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB40u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -4, 2, 0x80u);  /* 0x800FFB44: 0x90E2FFFC */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB44u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 1, 3, 0x100u);  /* 0x800FFB48: 0x91030001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFB4C: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFB50: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFB54: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFB58u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -4, (uint8_t)cpu->gpr[2]);  /* 0x800FFB58: 0xA0E2FFFC */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB58u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -3, 2, 0x80u);  /* 0x800FFB5C: 0x90E2FFFD */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB60u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 3, 0x100u);  /* 0x800FFB60: 0x91030002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFB64: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFB68: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFB6C: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFB70u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -3, (uint8_t)cpu->gpr[2]);  /* 0x800FFB70: 0xA0E2FFFD */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB70u);
#endif
    /* fall through to block_800FFB74 */

block_800FFB74:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFB74u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFB74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFB74u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1001u);
#endif
    int _bc_800FFB74 = (cpu->gpr[12] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1900u);
#endif
    cpu->gpr[8] = cpu->gpr[12] + cpu->gpr[11];  /* 0x800FFB78: 0x018B4021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB78u);
#endif
    if (_bc_800FFB74) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFBF8u);
        goto block_800FFBF8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFB7Cu);
        goto block_800FFB7C;  /* not taken */
    }

block_800FFB7C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFB7Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFB7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFB7Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB7Cu);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 3, 2, 0x100u);  /* 0x800FFB7C: 0x91020003 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFB80: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFB84 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFB88: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB88u);
#endif
    if (_bc_800FFB84) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFBB0u);
        goto block_800FFBB0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFB8Cu);
        goto block_800FFB8C;  /* not taken */
    }

block_800FFB8C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFB8Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFB8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFB8Cu, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB8Cu);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8], 2, 0x100u);  /* 0x800FFB8C: 0x91020000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFB90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFB90: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFB94u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -2, (uint8_t)cpu->gpr[2]);  /* 0x800FFB94: 0xA0E2FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB94u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 1, 2, 0x100u);  /* 0x800FFB98: 0x91020001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFB9C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFB9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFBA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFBA0u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -1, (uint8_t)cpu->gpr[2]);  /* 0x800FFBA0: 0xA0E2FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBA0u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 2, 0x100u);  /* 0x800FFBA4: 0x91020002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFBACu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7], (uint8_t)cpu->gpr[2]);  /* 0x800FFBAC: 0xA0E20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FFBF8u);
    goto block_800FFBF8;  /* j */

block_800FFBB0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFBB0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFBB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFBB0u, 18u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFBB0u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -2, 2, 0x80u);  /* 0x800FFBB0: 0x90E2FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBB0u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8], 3, 0x100u);  /* 0x800FFBB4: 0x91030000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFBB8: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFBBC: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFBC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFBC0: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFBC4u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -2, (uint8_t)cpu->gpr[2]);  /* 0x800FFBC4: 0xA0E2FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBC4u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7] + -1, 2, 0x80u);  /* 0x800FFBC8: 0x90E2FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBC8u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 1, 3, 0x100u);  /* 0x800FFBCC: 0x91030001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFBD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFBD0: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFBD4: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFBD8: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFBDCu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7] + -1, (uint8_t)cpu->gpr[2]);  /* 0x800FFBDC: 0xA0E2FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFBE0u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[7], 2, 0x80u);  /* 0x800FFBE0: 0x90E20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBE0u);
#endif
    cpu->gpr[3] = psx_cyc_load_byte(cpu, cpu->gpr[8] + 2, 3, 0x100u);  /* 0x800FFBE4: 0x91030002 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFBE8: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] + cpu->gpr[3];  /* 0x800FFBEC: 0x00431021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFBF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 1;  /* 0x800FFBF0: 0x00021043 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    g_debug_last_store_pc = 0x800FFBF4u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[7], (uint8_t)cpu->gpr[2]);  /* 0x800FFBF4: 0xA0E20000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBF4u);
#endif
    /* fall through to block_800FFBF8 */

block_800FFBF8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFBF8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFBF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFBF8u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFBF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80u);
#endif
    cpu->gpr[7] = cpu->gpr[7] + 12;  /* 0x800FFBF8: 0x24E7000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000u);
#endif
    cpu->gpr[13] = cpu->gpr[13] + 4;  /* 0x800FFBFC: 0x25AD0004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFBFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2004u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[13] < 16) ? 1 : 0;  /* 0x800FFC00: 0x29A20010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFC04 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 12;  /* 0x800FFC08: 0x24A5000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC08u);
#endif
    if (_bc_800FFC04) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF9D0u);
        goto block_800FF9D0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFC0Cu);
        goto block_800FFC0C;  /* not taken */
    }

block_800FFC0C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFC0Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFC0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFC0Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4000u);
#endif
    cpu->gpr[14] = cpu->gpr[14] + 1;  /* 0x800FFC0C: 0x25CE0001 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4044u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[14] < (int32_t)cpu->gpr[6]) ? 1 : 0;  /* 0x800FFC10: 0x01C6102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFC14 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 120;  /* 0x800FFC18: 0x24840078 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC18u);
#endif
    if (_bc_800FFC14) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FF9C8u);
        goto block_800FF9C8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFC1Cu);
        goto block_800FFC1C;  /* not taken */
    }

block_800FFC1C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFC1Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFC1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFC1Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FFC1C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFC20: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FFC1C);
    cpu->pc = _jt_800FFC1C; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FFC24(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FFCCCu: goto block_800FFCCC;
            case 0x800FFC40u: goto block_800FFC40;
            case 0x800FFC74u: goto block_800FFC74;
            case 0x800FFC8Cu: goto block_800FFC8C;
            case 0x800FFC24u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FFC24u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FFC24u);
    /* Address: 0x800FFC24, Size: 184 bytes, Blocks: 5 */

block_800FFC24:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFC24u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFC24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFC24u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x800FFC24: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x121u);
#endif
    cpu->gpr[8] = cpu->gpr[5];  /* move */  /* 0x800FFC28: 0x00A04021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8018 << 16;  /* 0x80180000 */  /* 0x800FFC2C: 0x3C028018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC30u);
#endif
    cpu->gpr[3] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[2] + 19648, 3, 0x4u);  /* 0x800FFC30: 0x84434CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x84u);
#endif
    cpu->gpr[7] = cpu->gpr[2] + 19648;  /* 0x800FFC34: 0x24474CC0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    int _bc_800FFC38 = ((int32_t)cpu->gpr[3] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FFC3Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x800FFC3C: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC3Cu);
#endif
    if (_bc_800FFC38) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFCCCu);
        goto block_800FFCCC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFC40u);
        goto block_800FFC40;  /* not taken */
    }

block_800FFC40:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFC40u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFC40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFC40u, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x5555 << 16;  /* 0x55550000 */  /* 0x800FFC40: 0x3C035555 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] | 0x5556;  /* 0x800FFC44: 0x34635556 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[4] << 1;  /* 0x800FFC48: 0x00041040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[3]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
#endif  /* 0x800FFC4C: 0x00430018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (int32_t)cpu->gpr[2] >> 31;  /* 0x800FFC50: 0x000217C3 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->hi;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x800FFC54: 0x00004810 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x214u);
#endif
    cpu->gpr[4] = cpu->gpr[9] - cpu->gpr[2];  /* 0x800FFC58: 0x01222023 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC58u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[7] + 8, 2, 0x80u);  /* 0x800FFC5C: 0x84E20008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC60u);
#endif
    cpu->gpr[3] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[7] + 12, 3, 0x80u);  /* 0x800FFC60: 0x84E3000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[4] = cpu->gpr[4] - cpu->gpr[2];  /* 0x800FFC64: 0x00822023 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1Cu);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < (int32_t)cpu->gpr[3]) ? 1 : 0;  /* 0x800FFC68: 0x0083102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFC6C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFC70: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC70u);
#endif
    if (_bc_800FFC6C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFCCCu);
        goto block_800FFCCC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFC74u);
        goto block_800FFC74;  /* not taken */
    }

block_800FFC74:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFC74u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFC74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFC74u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC74u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[7] + 14, 2, 0x80u);  /* 0x800FFC74: 0x84E2000E */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFC78: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[3] + cpu->gpr[2];  /* 0x800FFC7C: 0x00621021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = ((int32_t)cpu->gpr[4] < (int32_t)cpu->gpr[2]) ? 1 : 0;  /* 0x800FFC80: 0x0082102A */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_800FFC84 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x800FFC88: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC88u);
#endif
    if (_bc_800FFC84) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFCCCu);
        goto block_800FFCCC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x800FFC8Cu);
        goto block_800FFC8C;  /* not taken */
    }

block_800FFC8C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFC8Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFC8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFC8Cu, 16u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = (uint32_t)cpu->gpr[4] >> 2;  /* 0x800FFC8C: 0x00042082 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFC90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] << 1;  /* 0x800FFC90: 0x00042040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC90u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[7] + 4, 2, 0x80u);  /* 0x800FFC94: 0x8CE20004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC94u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + -7712, 3, 0x8u);  /* 0x800FFC98: 0x8C63E1E0 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC98u);
#endif
    cpu->gpr[6] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[7] + 16, 6, 0x80u);  /* 0x800FFC9C: 0x84E60010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFC9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 1;  /* 0x800FFCA0: 0x00021042 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 1;  /* 0x800FFCA4: 0x00021040 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[2];  /* 0x800FFCA8: 0x00621821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    cpu->gpr[4] = cpu->gpr[3] + cpu->gpr[4];  /* 0x800FFCAC: 0x00642021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCB0u);
#endif
    cpu->gpr[2] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[7] + 10, 2, 0x80u);  /* 0x800FFCB0: 0x84E2000A */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCB0u);
#endif
    cpu->gpr[7] = (int32_t)(int16_t)psx_cyc_load_half(cpu, cpu->gpr[7] + 20, 7, 0x80u);  /* 0x800FFCB4: 0x84E70014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[5] = cpu->gpr[2] << 1;  /* 0x800FFCB8: 0x00022840 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + cpu->gpr[2];  /* 0x800FFCBC: 0x00A22821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] << 4;  /* 0x800FFCC0: 0x00052900 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFCCCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x120u);
#endif
    cpu->gpr[5] = cpu->gpr[8] + cpu->gpr[5];  /* 0x800FFCC8: 0x01052821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x800FF964u);
    cpu->pc = 0x800FF964u; return;  /* CPS jal -> 0x800FF964 */

block_800FFCCC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFCCCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFCCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFCCCu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCCCu);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x800FFCCC: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x800FFCD0: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FFCD4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFCD8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FFCD4);
    cpu->pc = _jt_800FFCD4; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_800FFCDC(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FFD50u: goto block_800FFD50;
            case 0x800FFD60u: goto block_800FFD60;
            case 0x800FFD98u: goto block_800FFD98;
            case 0x800FFDA8u: goto block_800FFDA8;
            case 0x800FFDDCu: goto block_800FFDDC;
            case 0x800FFDECu: goto block_800FFDEC;
            case 0x800FFE20u: goto block_800FFE20;
            case 0x800FFE30u: goto block_800FFE30;
            case 0x800FFCDCu: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FFCDCu, _cont); return;
        }
    }
    debug_server_log_call_entry(0x800FFCDCu);
    /* Address: 0x800FFCDC, Size: 388 bytes, Blocks: 9 */

block_800FFCDC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFCDCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFCDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFCDCu, 29u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -56;  /* 0x800FFCDC: 0x27BDFFC8 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x800FFCE0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[17]);  /* 0x800FFCE0: 0xAFB1001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[17] = cpu->gpr[4];  /* move */  /* 0x800FFCE4: 0x00808821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20400000u);
#endif
    g_debug_last_store_pc = 0x800FFCE8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 48, cpu->gpr[22]);  /* 0x800FFCE8: 0xAFB60030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x400000u);
#endif
    cpu->gpr[22] = 0x0014 << 16;  /* 0x00140000 */  /* 0x800FFCEC: 0x3C160014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFCF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x400000u);
#endif
    cpu->gpr[22] = cpu->gpr[22] | 0x000C;  /* 0x800FFCF0: 0x36D6000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = 0x6500 << 16;  /* 0x65000000 */  /* 0x800FFCF4: 0x3C066500 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20200000u);
#endif
    g_debug_last_store_pc = 0x800FFCF8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 44, cpu->gpr[21]);  /* 0x800FFCF8: 0xAFB5002C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200000u);
#endif
    cpu->gpr[21] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FFCFC: 0x3C15800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FFCFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20100000u);
#endif
    g_debug_last_store_pc = 0x800FFD00u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[20]);  /* 0x800FFD00: 0xAFB40028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100000u);
#endif
    cpu->gpr[20] = 0x800B << 16;  /* 0x800B0000 */  /* 0x800FFD04: 0x3C14800B */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20080u);
#endif
    cpu->gpr[7] = cpu->gpr[17] & 0xFFFF;  /* 0x800FFD08: 0x3227FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20080000u);
#endif
    g_debug_last_store_pc = 0x800FFD0Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 36, cpu->gpr[19]);  /* 0x800FFD0C: 0xAFB30024 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80020u);
#endif
    cpu->gpr[19] = cpu->gpr[5] << 16;  /* 0x800FFD10: 0x00059C00 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD10u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[21] + -8348, 3, 0x200000u);  /* 0x800FFD14: 0x8EA3DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80080u);
#endif
    cpu->gpr[7] = cpu->gpr[7] | cpu->gpr[19];  /* 0x800FFD18: 0x00F33825 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20040000u);
#endif
    g_debug_last_store_pc = 0x800FFD1Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 32, cpu->gpr[18]);  /* 0x800FFD1C: 0xAFB20020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40000u);
#endif
    cpu->gpr[18] = 0x800C << 16;  /* 0x800C0000 */  /* 0x800FFD20: 0x3C12800C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x800FFD24u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[16]);  /* 0x800FFD24: 0xAFB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD24u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[20] + -28136, 16, 0x100000u);  /* 0x800FFD28: 0x8E909218 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD28u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[18] + -18688, 2, 0x40000u);  /* 0x800FFD2C: 0x8E42B700 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40000u);
#endif
    cpu->gpr[18] = cpu->gpr[18] + -18688;  /* 0x800FFD30: 0x2652B700 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x800FFD34u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 52, cpu->gpr[31]);  /* 0x800FFD34: 0xAFBF0034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20400000u);
#endif
    g_debug_last_store_pc = 0x800FFD38u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[22]);  /* 0x800FFD38: 0xAFB60010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + 28;  /* 0x800FFD3C: 0x2610001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFD40: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x29u);
#endif
    cpu->gpr[5] = cpu->gpr[3];  /* move */  /* 0x800FFD44: 0x00602821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFD50u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FFD4Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);  /* 0x800FFD4C: 0xAFA20014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004D9BCu);
    cpu->pc = 0x8004D9BCu; return;  /* CPS jal -> 0x8004D9BC */

block_800FFD50:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFD50u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFD50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFD50u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFD50: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x25u);
#endif
    cpu->gpr[5] = cpu->gpr[2];  /* move */  /* 0x800FFD54: 0x00402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFD60u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x41u);
#endif
    cpu->gpr[6] = 31;  /* 0x800FFD5C: 0x2406001F */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004DAA8u);
    cpu->pc = 0x8004DAA8u; return;  /* CPS jal -> 0x8004DAA8 */

block_800FFD60:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFD60u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFD60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFD60u, 14u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xDu);
#endif
    cpu->gpr[3] = cpu->gpr[2];  /* move */  /* 0x800FFD60: 0x00401821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x29u);
#endif
    cpu->gpr[5] = cpu->gpr[3];  /* move */  /* 0x800FFD64: 0x00602821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = 0x6500 << 16;  /* 0x65000000 */  /* 0x800FFD68: 0x3C066500 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20080u);
#endif
    cpu->gpr[7] = cpu->gpr[17] + 18;  /* 0x800FFD6C: 0x26270012 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD70u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[20] + -28136, 16, 0x100000u);  /* 0x800FFD70: 0x8E909218 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80u);
#endif
    cpu->gpr[7] = cpu->gpr[7] & 0xFFFF;  /* 0x800FFD74: 0x30E7FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20400000u);
#endif
    g_debug_last_store_pc = 0x800FFD78u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[22]);  /* 0x800FFD78: 0xAFB60010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD78u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[18] + 4, 2, 0x40000u);  /* 0x800FFD7C: 0x8E420004 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80080u);
#endif
    cpu->gpr[7] = cpu->gpr[7] | cpu->gpr[19];  /* 0x800FFD80: 0x00F33825 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200020u);
#endif
    g_debug_last_store_pc = 0x800FFD84u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[21] + -8348, cpu->gpr[5]);  /* 0x800FFD84: 0xAEA5DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + 28;  /* 0x800FFD88: 0x2610001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFD8C: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFD98u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FFD94u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);  /* 0x800FFD94: 0xAFA20014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004D9BCu);
    cpu->pc = 0x8004D9BCu; return;  /* CPS jal -> 0x8004D9BC */

block_800FFD98:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFD98u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFD98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFD98u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFD98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFD98: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x25u);
#endif
    cpu->gpr[5] = cpu->gpr[2];  /* move */  /* 0x800FFD9C: 0x00402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFD9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFDA8u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x41u);
#endif
    cpu->gpr[6] = 31;  /* 0x800FFDA4: 0x2406001F */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004DAA8u);
    cpu->pc = 0x8004DAA8u; return;  /* CPS jal -> 0x8004DAA8 */

block_800FFDA8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFDA8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFDA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFDA8u, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x25u);
#endif
    cpu->gpr[5] = cpu->gpr[2];  /* move */  /* 0x800FFDA8: 0x00402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = 0x6500 << 16;  /* 0x65000000 */  /* 0x800FFDAC: 0x3C066500 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20080u);
#endif
    cpu->gpr[7] = cpu->gpr[17] + 36;  /* 0x800FFDB0: 0x26270024 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDB0u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[20] + -28136, 16, 0x100000u);  /* 0x800FFDB4: 0x8E909218 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80u);
#endif
    cpu->gpr[7] = cpu->gpr[7] & 0xFFFF;  /* 0x800FFDB8: 0x30E7FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20400000u);
#endif
    g_debug_last_store_pc = 0x800FFDBCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[22]);  /* 0x800FFDBC: 0xAFB60010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDC0u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[18] + 8, 2, 0x40000u);  /* 0x800FFDC0: 0x8E420008 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80080u);
#endif
    cpu->gpr[7] = cpu->gpr[7] | cpu->gpr[19];  /* 0x800FFDC4: 0x00F33825 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200020u);
#endif
    g_debug_last_store_pc = 0x800FFDC8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[21] + -8348, cpu->gpr[5]);  /* 0x800FFDC8: 0xAEA5DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + 28;  /* 0x800FFDCC: 0x2610001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFDD0: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFDDCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FFDD8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);  /* 0x800FFDD8: 0xAFA20014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004D9BCu);
    cpu->pc = 0x8004D9BCu; return;  /* CPS jal -> 0x8004D9BC */

block_800FFDDC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFDDCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFDDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFDDCu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFDDC: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x25u);
#endif
    cpu->gpr[5] = cpu->gpr[2];  /* move */  /* 0x800FFDE0: 0x00402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFDECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x41u);
#endif
    cpu->gpr[6] = 31;  /* 0x800FFDE8: 0x2406001F */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004DAA8u);
    cpu->pc = 0x8004DAA8u; return;  /* CPS jal -> 0x8004DAA8 */

block_800FFDEC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFDECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFDECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFDECu, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x25u);
#endif
    cpu->gpr[5] = cpu->gpr[2];  /* move */  /* 0x800FFDEC: 0x00402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFDF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = 0x6500 << 16;  /* 0x65000000 */  /* 0x800FFDF0: 0x3C066500 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000u);
#endif
    cpu->gpr[17] = cpu->gpr[17] + 80;  /* 0x800FFDF4: 0x26310050 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDF4u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[20] + -28136, 16, 0x100000u);  /* 0x800FFDF8: 0x8E909218 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000u);
#endif
    cpu->gpr[17] = cpu->gpr[17] & 0xFFFF;  /* 0x800FFDFC: 0x3231FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x800FFDFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFE00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20400000u);
#endif
    g_debug_last_store_pc = 0x800FFE00u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[22]);  /* 0x800FFE00: 0xAFB60010 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE00u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[18] + 12, 2, 0x40000u);  /* 0x800FFE04: 0x8E42000C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0080u);
#endif
    cpu->gpr[7] = cpu->gpr[17] | cpu->gpr[19];  /* 0x800FFE08: 0x02333825 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200020u);
#endif
    g_debug_last_store_pc = 0x800FFE0Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[21] + -8348, cpu->gpr[5]);  /* 0x800FFE0C: 0xAEA5DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFE10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] + 28;  /* 0x800FFE10: 0x2610001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFE14: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE14u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFE20u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x800FFE1Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[2]);  /* 0x800FFE1C: 0xAFA20014 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004D9BCu);
    cpu->pc = 0x8004D9BCu; return;  /* CPS jal -> 0x8004D9BC */

block_800FFE20:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFE20u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFE20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFE20u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFE20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x800FFE20: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x25u);
#endif
    cpu->gpr[5] = cpu->gpr[2];  /* move */  /* 0x800FFE24: 0x00402821 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x800FFE30u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x41u);
#endif
    cpu->gpr[6] = 31;  /* 0x800FFE2C: 0x2406001F */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8004DAA8u);
    cpu->pc = 0x8004DAA8u; return;  /* CPS jal -> 0x8004DAA8 */

block_800FFE30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FFE30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FFE30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FFE30u, 12u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFE30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200004u);
#endif
    g_debug_last_store_pc = 0x800FFE30u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[21] + -8348, cpu->gpr[2]);  /* 0x800FFE30: 0xAEA2DF64 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE30u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 52, 31, 0x20000000u);  /* 0x800FFE34: 0x8FBF0034 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE34u);
#endif
    cpu->gpr[22] = psx_cyc_load_word(cpu, cpu->gpr[29] + 48, 22, 0x20000000u);  /* 0x800FFE38: 0x8FB60030 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE38u);
#endif
    cpu->gpr[21] = psx_cyc_load_word(cpu, cpu->gpr[29] + 44, 21, 0x20000000u);  /* 0x800FFE3C: 0x8FB5002C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFE40u);
#endif
    cpu->gpr[20] = psx_cyc_load_word(cpu, cpu->gpr[29] + 40, 20, 0x20000000u);  /* 0x800FFE40: 0x8FB40028 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE40u);
#endif
    cpu->gpr[19] = psx_cyc_load_word(cpu, cpu->gpr[29] + 36, 19, 0x20000000u);  /* 0x800FFE44: 0x8FB30024 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE44u);
#endif
    cpu->gpr[18] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 18, 0x20000000u);  /* 0x800FFE48: 0x8FB20020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE48u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 17, 0x20000000u);  /* 0x800FFE4C: 0x8FB1001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FFE50u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 16, 0x20000000u);  /* 0x800FFE50: 0x8FB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 56;  /* 0x800FFE54: 0x27BD0038 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FFE58 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FFE5C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FFE5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FFE58);
    cpu->pc = _jt_800FFE58; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

/* Overlapping-alias body for host func_800FEA08: 1 entries */
void psx_alias_body_800FEA08(CPUState* cpu, uint32_t entry)
{
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x800FEBE0u: goto block_800FEBE0;
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x800FEBE0u, _cont); return;
        }
    }
    switch (entry) {
        case 0x800FEBE0u: goto block_800FEBE0;
        default: cpu->pc = entry; psx_native_bad_entry(cpu, 0x800FEBE0u, entry); return;
    }

block_800FEBE0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x800FEBE0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x800FEBE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x800FEBE0u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBE0u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 31, 0x20000000u);  /* 0x800FEBE0: 0x8FBF001C */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBE0u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 16, 0x20000000u);  /* 0x800FEBE4: 0x8FB00018 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x800FEBE8: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_800FEBEC = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x800FEBF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x800FEBF0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x800FEBF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_800FEBEC);
    cpu->pc = _jt_800FEBEC; return;  /* CPS: jr $ra */
    ;  /* label compatibility */
}

static void func_800FEBE0(CPUState* cpu)
{
    debug_server_log_call_entry(0x800FEBE0u);
    psx_alias_body_800FEA08(cpu, 0x800FEBE0u);  /* alias entry into host func_800FEA08 */
}



typedef struct { uint32_t lo; uint32_t len; } PsxGameCodeRange;
static const PsxGameCodeRange k_psx_fmv_str_ranges[] = {
{0x800FDFE4u, 0x7C8u},
    {0x800FE984u, 0x84u},
    {0x800FEA08u, 0x1ECu},
    {0x800FEA08u, 0x1ECu},
    {0x800FEBF4u, 0xC4u},
    {0x800FECB8u, 0x148u},
    {0x800FEE00u, 0xD0u},
    {0x800FEED0u, 0xC4u},
    {0x800FEF94u, 0x20u},
    {0x800FEFB4u, 0x20u},
    {0x800FF0C4u, 0x6E4u},
    {0x800FF7C0u, 0x70u},
    {0x800FF830u, 0xB0u},
    {0x800FF8E8u, 0x7Cu},
    {0x800FF964u, 0x2C0u},
    {0x800FFC24u, 0xB8u},
    {0x800FFCDCu, 0x184u},
};

typedef void (*PsxGameDispatchFn)(CPUState* cpu);
typedef struct {
    uint32_t addr;
    uint32_t resume_pc;
    uint32_t range_index;
    uint32_t range_count;
    PsxGameDispatchFn fn;
} PsxGameDispatchEntry;

static const PsxGameDispatchEntry k_psx_fmv_str_dispatch[] = {
{0x800FDFE4u, 0x00000000u, 0u, 1u, func_800FDFE4},
    {0x800FE004u, 0x800FE004u, 0u, 1u, func_800FDFE4},
    {0x800FE00Cu, 0x800FE00Cu, 0u, 1u, func_800FDFE4},
    {0x800FE014u, 0x800FE014u, 0u, 1u, func_800FDFE4},
    {0x800FE01Cu, 0x800FE01Cu, 0u, 1u, func_800FDFE4},
    {0x800FE030u, 0x800FE030u, 0u, 1u, func_800FDFE4},
    {0x800FE044u, 0x800FE044u, 0u, 1u, func_800FDFE4},
    {0x800FE054u, 0x800FE054u, 0u, 1u, func_800FDFE4},
    {0x800FE074u, 0x800FE074u, 0u, 1u, func_800FDFE4},
    {0x800FE090u, 0x800FE090u, 0u, 1u, func_800FDFE4},
    {0x800FE0B4u, 0x800FE0B4u, 0u, 1u, func_800FDFE4},
    {0x800FE0C8u, 0x800FE0C8u, 0u, 1u, func_800FDFE4},
    {0x800FE0D8u, 0x800FE0D8u, 0u, 1u, func_800FDFE4},
    {0x800FE0E0u, 0x800FE0E0u, 0u, 1u, func_800FDFE4},
    {0x800FE0E8u, 0x800FE0E8u, 0u, 1u, func_800FDFE4},
    {0x800FE0F0u, 0x800FE0F0u, 0u, 1u, func_800FDFE4},
    {0x800FE0F8u, 0x800FE0F8u, 0u, 1u, func_800FDFE4},
    {0x800FE100u, 0x800FE100u, 0u, 1u, func_800FDFE4},
    {0x800FE108u, 0x800FE108u, 0u, 1u, func_800FDFE4},
    {0x800FE110u, 0x800FE110u, 0u, 1u, func_800FDFE4},
    {0x800FE134u, 0x800FE134u, 0u, 1u, func_800FDFE4},
    {0x800FE16Cu, 0x800FE16Cu, 0u, 1u, func_800FDFE4},
    {0x800FE174u, 0x800FE174u, 0u, 1u, func_800FDFE4},
    {0x800FE17Cu, 0x800FE17Cu, 0u, 1u, func_800FDFE4},
    {0x800FE184u, 0x800FE184u, 0u, 1u, func_800FDFE4},
    {0x800FE18Cu, 0x800FE18Cu, 0u, 1u, func_800FDFE4},
    {0x800FE194u, 0x800FE194u, 0u, 1u, func_800FDFE4},
    {0x800FE1A0u, 0x800FE1A0u, 0u, 1u, func_800FDFE4},
    {0x800FE1B8u, 0x800FE1B8u, 0u, 1u, func_800FDFE4},
    {0x800FE1C0u, 0x800FE1C0u, 0u, 1u, func_800FDFE4},
    {0x800FE1D0u, 0x800FE1D0u, 0u, 1u, func_800FDFE4},
    {0x800FE1D4u, 0x800FE1D4u, 0u, 1u, func_800FDFE4},
    {0x800FE1DCu, 0x800FE1DCu, 0u, 1u, func_800FDFE4},
    {0x800FE1F8u, 0x800FE1F8u, 0u, 1u, func_800FDFE4},
    {0x800FE200u, 0x800FE200u, 0u, 1u, func_800FDFE4},
    {0x800FE21Cu, 0x800FE21Cu, 0u, 1u, func_800FDFE4},
    {0x800FE228u, 0x800FE228u, 0u, 1u, func_800FDFE4},
    {0x800FE264u, 0x800FE264u, 0u, 1u, func_800FDFE4},
    {0x800FE2B0u, 0x800FE2B0u, 0u, 1u, func_800FDFE4},
    {0x800FE2B8u, 0x800FE2B8u, 0u, 1u, func_800FDFE4},
    {0x800FE2C0u, 0x800FE2C0u, 0u, 1u, func_800FDFE4},
    {0x800FE2C8u, 0x800FE2C8u, 0u, 1u, func_800FDFE4},
    {0x800FE2D8u, 0x800FE2D8u, 0u, 1u, func_800FDFE4},
    {0x800FE2E0u, 0x800FE2E0u, 0u, 1u, func_800FDFE4},
    {0x800FE2FCu, 0x800FE2FCu, 0u, 1u, func_800FDFE4},
    {0x800FE304u, 0x800FE304u, 0u, 1u, func_800FDFE4},
    {0x800FE30Cu, 0x800FE30Cu, 0u, 1u, func_800FDFE4},
    {0x800FE314u, 0x800FE314u, 0u, 1u, func_800FDFE4},
    {0x800FE320u, 0x800FE320u, 0u, 1u, func_800FDFE4},
    {0x800FE330u, 0x800FE330u, 0u, 1u, func_800FDFE4},
    {0x800FE338u, 0x800FE338u, 0u, 1u, func_800FDFE4},
    {0x800FE348u, 0x800FE348u, 0u, 1u, func_800FDFE4},
    {0x800FE34Cu, 0x800FE34Cu, 0u, 1u, func_800FDFE4},
    {0x800FE364u, 0x800FE364u, 0u, 1u, func_800FDFE4},
    {0x800FE388u, 0x800FE388u, 0u, 1u, func_800FDFE4},
    {0x800FE3C0u, 0x800FE3C0u, 0u, 1u, func_800FDFE4},
    {0x800FE3D8u, 0x800FE3D8u, 0u, 1u, func_800FDFE4},
    {0x800FE3F0u, 0x800FE3F0u, 0u, 1u, func_800FDFE4},
    {0x800FE3FCu, 0x800FE3FCu, 0u, 1u, func_800FDFE4},
    {0x800FE404u, 0x800FE404u, 0u, 1u, func_800FDFE4},
    {0x800FE40Cu, 0x800FE40Cu, 0u, 1u, func_800FDFE4},
    {0x800FE414u, 0x800FE414u, 0u, 1u, func_800FDFE4},
    {0x800FE42Cu, 0x800FE42Cu, 0u, 1u, func_800FDFE4},
    {0x800FE43Cu, 0x800FE43Cu, 0u, 1u, func_800FDFE4},
    {0x800FE44Cu, 0x800FE44Cu, 0u, 1u, func_800FDFE4},
    {0x800FE454u, 0x800FE454u, 0u, 1u, func_800FDFE4},
    {0x800FE464u, 0x800FE464u, 0u, 1u, func_800FDFE4},
    {0x800FE468u, 0x800FE468u, 0u, 1u, func_800FDFE4},
    {0x800FE480u, 0x800FE480u, 0u, 1u, func_800FDFE4},
    {0x800FE4A8u, 0x800FE4A8u, 0u, 1u, func_800FDFE4},
    {0x800FE4B8u, 0x800FE4B8u, 0u, 1u, func_800FDFE4},
    {0x800FE4C0u, 0x800FE4C0u, 0u, 1u, func_800FDFE4},
    {0x800FE4C4u, 0x800FE4C4u, 0u, 1u, func_800FDFE4},
    {0x800FE4D0u, 0x800FE4D0u, 0u, 1u, func_800FDFE4},
    {0x800FE4D8u, 0x800FE4D8u, 0u, 1u, func_800FDFE4},
    {0x800FE4E8u, 0x800FE4E8u, 0u, 1u, func_800FDFE4},
    {0x800FE4F0u, 0x800FE4F0u, 0u, 1u, func_800FDFE4},
    {0x800FE50Cu, 0x800FE50Cu, 0u, 1u, func_800FDFE4},
    {0x800FE518u, 0x800FE518u, 0u, 1u, func_800FDFE4},
    {0x800FE520u, 0x800FE520u, 0u, 1u, func_800FDFE4},
    {0x800FE528u, 0x800FE528u, 0u, 1u, func_800FDFE4},
    {0x800FE54Cu, 0x800FE54Cu, 0u, 1u, func_800FDFE4},
    {0x800FE558u, 0x800FE558u, 0u, 1u, func_800FDFE4},
    {0x800FE590u, 0x800FE590u, 0u, 1u, func_800FDFE4},
    {0x800FE59Cu, 0x800FE59Cu, 0u, 1u, func_800FDFE4},
    {0x800FE5E4u, 0x800FE5E4u, 0u, 1u, func_800FDFE4},
    {0x800FE630u, 0x800FE630u, 0u, 1u, func_800FDFE4},
    {0x800FE638u, 0x800FE638u, 0u, 1u, func_800FDFE4},
    {0x800FE640u, 0x800FE640u, 0u, 1u, func_800FDFE4},
    {0x800FE664u, 0x800FE664u, 0u, 1u, func_800FDFE4},
    {0x800FE66Cu, 0x800FE66Cu, 0u, 1u, func_800FDFE4},
    {0x800FE67Cu, 0x800FE67Cu, 0u, 1u, func_800FDFE4},
    {0x800FE684u, 0x800FE684u, 0u, 1u, func_800FDFE4},
    {0x800FE694u, 0x800FE694u, 0u, 1u, func_800FDFE4},
    {0x800FE698u, 0x800FE698u, 0u, 1u, func_800FDFE4},
    {0x800FE6ACu, 0x800FE6ACu, 0u, 1u, func_800FDFE4},
    {0x800FE6C8u, 0x800FE6C8u, 0u, 1u, func_800FDFE4},
    {0x800FE6CCu, 0x800FE6CCu, 0u, 1u, func_800FDFE4},
    {0x800FE6D4u, 0x800FE6D4u, 0u, 1u, func_800FDFE4},
    {0x800FE6E4u, 0x800FE6E4u, 0u, 1u, func_800FDFE4},
    {0x800FE6ECu, 0x800FE6ECu, 0u, 1u, func_800FDFE4},
    {0x800FE6FCu, 0x800FE6FCu, 0u, 1u, func_800FDFE4},
    {0x800FE700u, 0x800FE700u, 0u, 1u, func_800FDFE4},
    {0x800FE710u, 0x800FE710u, 0u, 1u, func_800FDFE4},
    {0x800FE728u, 0x800FE728u, 0u, 1u, func_800FDFE4},
    {0x800FE738u, 0x800FE738u, 0u, 1u, func_800FDFE4},
    {0x800FE740u, 0x800FE740u, 0u, 1u, func_800FDFE4},
    {0x800FE74Cu, 0x800FE74Cu, 0u, 1u, func_800FDFE4},
    {0x800FE758u, 0x800FE758u, 0u, 1u, func_800FDFE4},
    {0x800FE760u, 0x800FE760u, 0u, 1u, func_800FDFE4},
    {0x800FE764u, 0x800FE764u, 0u, 1u, func_800FDFE4},
    {0x800FE76Cu, 0x800FE76Cu, 0u, 1u, func_800FDFE4},
    {0x800FE77Cu, 0x800FE77Cu, 0u, 1u, func_800FDFE4},
    {0x800FE784u, 0x800FE784u, 0u, 1u, func_800FDFE4},
    {0x800FE788u, 0x800FE788u, 0u, 1u, func_800FDFE4},
    {0x800FE984u, 0x00000000u, 1u, 1u, func_800FE984},
    {0x800FE9A4u, 0x800FE9A4u, 1u, 1u, func_800FE984},
    {0x800FE9ACu, 0x800FE9ACu, 1u, 1u, func_800FE984},
    {0x800FE9BCu, 0x800FE9BCu, 1u, 1u, func_800FE984},
    {0x800FE9D4u, 0x800FE9D4u, 1u, 1u, func_800FE984},
    {0x800FE9DCu, 0x800FE9DCu, 1u, 1u, func_800FE984},
    {0x800FE9E0u, 0x800FE9E0u, 1u, 1u, func_800FE984},
    {0x800FE9E8u, 0x800FE9E8u, 1u, 1u, func_800FE984},
    {0x800FE9F0u, 0x800FE9F0u, 1u, 1u, func_800FE984},
    {0x800FEA08u, 0x00000000u, 2u, 1u, func_800FEA08},
    {0x800FEA24u, 0x800FEA24u, 2u, 1u, func_800FEA08},
    {0x800FEA2Cu, 0x800FEA2Cu, 2u, 1u, func_800FEA08},
    {0x800FEA30u, 0x800FEA30u, 2u, 1u, func_800FEA08},
    {0x800FEA6Cu, 0x800FEA6Cu, 2u, 1u, func_800FEA08},
    {0x800FEAA0u, 0x800FEAA0u, 2u, 1u, func_800FEA08},
    {0x800FEABCu, 0x800FEABCu, 2u, 1u, func_800FEA08},
    {0x800FEAE0u, 0x800FEAE0u, 2u, 1u, func_800FEA08},
    {0x800FEAE8u, 0x800FEAE8u, 2u, 1u, func_800FEA08},
    {0x800FEAECu, 0x800FEAECu, 2u, 1u, func_800FEA08},
    {0x800FEAFCu, 0x800FEAFCu, 2u, 1u, func_800FEA08},
    {0x800FEB18u, 0x800FEB18u, 2u, 1u, func_800FEA08},
    {0x800FEB60u, 0x800FEB60u, 2u, 1u, func_800FEA08},
    {0x800FEB8Cu, 0x800FEB8Cu, 2u, 1u, func_800FEA08},
    {0x800FEB94u, 0x800FEB94u, 2u, 1u, func_800FEA08},
    {0x800FEBC8u, 0x800FEBC8u, 2u, 1u, func_800FEA08},
    {0x800FEBE0u, 0x00000000u, 3u, 1u, func_800FEBE0},
    {0x800FEBF4u, 0x00000000u, 4u, 1u, func_800FEBF4},
    {0x800FEC18u, 0x800FEC18u, 4u, 1u, func_800FEBF4},
    {0x800FEC24u, 0x800FEC24u, 4u, 1u, func_800FEBF4},
    {0x800FEC2Cu, 0x800FEC2Cu, 4u, 1u, func_800FEBF4},
    {0x800FEC38u, 0x800FEC38u, 4u, 1u, func_800FEBF4},
    {0x800FEC40u, 0x800FEC40u, 4u, 1u, func_800FEBF4},
    {0x800FEC50u, 0x800FEC50u, 4u, 1u, func_800FEBF4},
    {0x800FEC60u, 0x800FEC60u, 4u, 1u, func_800FEBF4},
    {0x800FEC68u, 0x800FEC68u, 4u, 1u, func_800FEBF4},
    {0x800FEC8Cu, 0x800FEC8Cu, 4u, 1u, func_800FEBF4},
    {0x800FEC94u, 0x800FEC94u, 4u, 1u, func_800FEBF4},
    {0x800FEC98u, 0x800FEC98u, 4u, 1u, func_800FEBF4},
    {0x800FECB8u, 0x00000000u, 5u, 1u, func_800FECB8},
    {0x800FECD0u, 0x800FECD0u, 5u, 1u, func_800FECB8},
    {0x800FECE0u, 0x800FECE0u, 5u, 1u, func_800FECB8},
    {0x800FECE8u, 0x800FECE8u, 5u, 1u, func_800FECB8},
    {0x800FECF0u, 0x800FECF0u, 5u, 1u, func_800FECB8},
    {0x800FECF8u, 0x800FECF8u, 5u, 1u, func_800FECB8},
    {0x800FED04u, 0x800FED04u, 5u, 1u, func_800FECB8},
    {0x800FED08u, 0x800FED08u, 5u, 1u, func_800FECB8},
    {0x800FED10u, 0x800FED10u, 5u, 1u, func_800FECB8},
    {0x800FED20u, 0x800FED20u, 5u, 1u, func_800FECB8},
    {0x800FED30u, 0x800FED30u, 5u, 1u, func_800FECB8},
    {0x800FED38u, 0x800FED38u, 5u, 1u, func_800FECB8},
    {0x800FED58u, 0x800FED58u, 5u, 1u, func_800FECB8},
    {0x800FED6Cu, 0x800FED6Cu, 5u, 1u, func_800FECB8},
    {0x800FED84u, 0x800FED84u, 5u, 1u, func_800FECB8},
    {0x800FED8Cu, 0x800FED8Cu, 5u, 1u, func_800FECB8},
    {0x800FED90u, 0x800FED90u, 5u, 1u, func_800FECB8},
    {0x800FED98u, 0x800FED98u, 5u, 1u, func_800FECB8},
    {0x800FEDA0u, 0x800FEDA0u, 5u, 1u, func_800FECB8},
    {0x800FEDA8u, 0x800FEDA8u, 5u, 1u, func_800FECB8},
    {0x800FEDB0u, 0x800FEDB0u, 5u, 1u, func_800FECB8},
    {0x800FEDE8u, 0x800FEDE8u, 5u, 1u, func_800FECB8},
    {0x800FEE00u, 0x00000000u, 6u, 1u, func_800FEE00},
    {0x800FEE20u, 0x800FEE20u, 6u, 1u, func_800FEE00},
    {0x800FEE30u, 0x800FEE30u, 6u, 1u, func_800FEE00},
    {0x800FEE38u, 0x800FEE38u, 6u, 1u, func_800FEE00},
    {0x800FEE40u, 0x800FEE40u, 6u, 1u, func_800FEE00},
    {0x800FEE50u, 0x800FEE50u, 6u, 1u, func_800FEE00},
    {0x800FEE60u, 0x800FEE60u, 6u, 1u, func_800FEE00},
    {0x800FEEA0u, 0x800FEEA0u, 6u, 1u, func_800FEE00},
    {0x800FEEB0u, 0x800FEEB0u, 6u, 1u, func_800FEE00},
    {0x800FEED0u, 0x00000000u, 7u, 1u, func_800FEED0},
    {0x800FEEF8u, 0x800FEEF8u, 7u, 1u, func_800FEED0},
    {0x800FEF14u, 0x800FEF14u, 7u, 1u, func_800FEED0},
    {0x800FEF24u, 0x800FEF24u, 7u, 1u, func_800FEED0},
    {0x800FEF3Cu, 0x800FEF3Cu, 7u, 1u, func_800FEED0},
    {0x800FEF4Cu, 0x800FEF4Cu, 7u, 1u, func_800FEED0},
    {0x800FEF64u, 0x800FEF64u, 7u, 1u, func_800FEED0},
    {0x800FEF78u, 0x800FEF78u, 7u, 1u, func_800FEED0},
    {0x800FEF80u, 0x800FEF80u, 7u, 1u, func_800FEED0},
    {0x800FEF94u, 0x00000000u, 8u, 1u, func_800FEF94},
    {0x800FEFA4u, 0x800FEFA4u, 8u, 1u, func_800FEF94},
    {0x800FEFB4u, 0x00000000u, 9u, 1u, func_800FEFB4},
    {0x800FEFC4u, 0x800FEFC4u, 9u, 1u, func_800FEFB4},
    {0x800FF0C4u, 0x00000000u, 10u, 1u, func_800FF0C4},
    {0x800FF12Cu, 0x800FF12Cu, 10u, 1u, func_800FF0C4},
    {0x800FF148u, 0x800FF148u, 10u, 1u, func_800FF0C4},
    {0x800FF150u, 0x800FF150u, 10u, 1u, func_800FF0C4},
    {0x800FF158u, 0x800FF158u, 10u, 1u, func_800FF0C4},
    {0x800FF16Cu, 0x800FF16Cu, 10u, 1u, func_800FF0C4},
    {0x800FF178u, 0x800FF178u, 10u, 1u, func_800FF0C4},
    {0x800FF180u, 0x800FF180u, 10u, 1u, func_800FF0C4},
    {0x800FF190u, 0x800FF190u, 10u, 1u, func_800FF0C4},
    {0x800FF1B8u, 0x800FF1B8u, 10u, 1u, func_800FF0C4},
    {0x800FF1C0u, 0x800FF1C0u, 10u, 1u, func_800FF0C4},
    {0x800FF1C8u, 0x800FF1C8u, 10u, 1u, func_800FF0C4},
    {0x800FF1D0u, 0x800FF1D0u, 10u, 1u, func_800FF0C4},
    {0x800FF1D8u, 0x800FF1D8u, 10u, 1u, func_800FF0C4},
    {0x800FF1E0u, 0x800FF1E0u, 10u, 1u, func_800FF0C4},
    {0x800FF1E8u, 0x800FF1E8u, 10u, 1u, func_800FF0C4},
    {0x800FF1F0u, 0x800FF1F0u, 10u, 1u, func_800FF0C4},
    {0x800FF1FCu, 0x800FF1FCu, 10u, 1u, func_800FF0C4},
    {0x800FF204u, 0x800FF204u, 10u, 1u, func_800FF0C4},
    {0x800FF20Cu, 0x800FF20Cu, 10u, 1u, func_800FF0C4},
    {0x800FF214u, 0x800FF214u, 10u, 1u, func_800FF0C4},
    {0x800FF224u, 0x800FF224u, 10u, 1u, func_800FF0C4},
    {0x800FF240u, 0x800FF240u, 10u, 1u, func_800FF0C4},
    {0x800FF250u, 0x800FF250u, 10u, 1u, func_800FF0C4},
    {0x800FF268u, 0x800FF268u, 10u, 1u, func_800FF0C4},
    {0x800FF278u, 0x800FF278u, 10u, 1u, func_800FF0C4},
    {0x800FF284u, 0x800FF284u, 10u, 1u, func_800FF0C4},
    {0x800FF290u, 0x800FF290u, 10u, 1u, func_800FF0C4},
    {0x800FF298u, 0x800FF298u, 10u, 1u, func_800FF0C4},
    {0x800FF2A0u, 0x800FF2A0u, 10u, 1u, func_800FF0C4},
    {0x800FF2A8u, 0x800FF2A8u, 10u, 1u, func_800FF0C4},
    {0x800FF2B4u, 0x800FF2B4u, 10u, 1u, func_800FF0C4},
    {0x800FF2BCu, 0x800FF2BCu, 10u, 1u, func_800FF0C4},
    {0x800FF2C4u, 0x800FF2C4u, 10u, 1u, func_800FF0C4},
    {0x800FF2D4u, 0x800FF2D4u, 10u, 1u, func_800FF0C4},
    {0x800FF2DCu, 0x800FF2DCu, 10u, 1u, func_800FF0C4},
    {0x800FF2E4u, 0x800FF2E4u, 10u, 1u, func_800FF0C4},
    {0x800FF2ECu, 0x800FF2ECu, 10u, 1u, func_800FF0C4},
    {0x800FF2F4u, 0x800FF2F4u, 10u, 1u, func_800FF0C4},
    {0x800FF304u, 0x800FF304u, 10u, 1u, func_800FF0C4},
    {0x800FF314u, 0x800FF314u, 10u, 1u, func_800FF0C4},
    {0x800FF31Cu, 0x800FF31Cu, 10u, 1u, func_800FF0C4},
    {0x800FF324u, 0x800FF324u, 10u, 1u, func_800FF0C4},
    {0x800FF32Cu, 0x800FF32Cu, 10u, 1u, func_800FF0C4},
    {0x800FF338u, 0x800FF338u, 10u, 1u, func_800FF0C4},
    {0x800FF348u, 0x800FF348u, 10u, 1u, func_800FF0C4},
    {0x800FF368u, 0x800FF368u, 10u, 1u, func_800FF0C4},
    {0x800FF39Cu, 0x800FF39Cu, 10u, 1u, func_800FF0C4},
    {0x800FF3A4u, 0x800FF3A4u, 10u, 1u, func_800FF0C4},
    {0x800FF3ACu, 0x800FF3ACu, 10u, 1u, func_800FF0C4},
    {0x800FF3B4u, 0x800FF3B4u, 10u, 1u, func_800FF0C4},
    {0x800FF3BCu, 0x800FF3BCu, 10u, 1u, func_800FF0C4},
    {0x800FF3C4u, 0x800FF3C4u, 10u, 1u, func_800FF0C4},
    {0x800FF3D0u, 0x800FF3D0u, 10u, 1u, func_800FF0C4},
    {0x800FF3D8u, 0x800FF3D8u, 10u, 1u, func_800FF0C4},
    {0x800FF3F0u, 0x800FF3F0u, 10u, 1u, func_800FF0C4},
    {0x800FF3F8u, 0x800FF3F8u, 10u, 1u, func_800FF0C4},
    {0x800FF400u, 0x800FF400u, 10u, 1u, func_800FF0C4},
    {0x800FF408u, 0x800FF408u, 10u, 1u, func_800FF0C4},
    {0x800FF410u, 0x800FF410u, 10u, 1u, func_800FF0C4},
    {0x800FF424u, 0x800FF424u, 10u, 1u, func_800FF0C4},
    {0x800FF42Cu, 0x800FF42Cu, 10u, 1u, func_800FF0C4},
    {0x800FF430u, 0x800FF430u, 10u, 1u, func_800FF0C4},
    {0x800FF438u, 0x800FF438u, 10u, 1u, func_800FF0C4},
    {0x800FF440u, 0x800FF440u, 10u, 1u, func_800FF0C4},
    {0x800FF44Cu, 0x800FF44Cu, 10u, 1u, func_800FF0C4},
    {0x800FF45Cu, 0x800FF45Cu, 10u, 1u, func_800FF0C4},
    {0x800FF464u, 0x800FF464u, 10u, 1u, func_800FF0C4},
    {0x800FF474u, 0x800FF474u, 10u, 1u, func_800FF0C4},
    {0x800FF484u, 0x800FF484u, 10u, 1u, func_800FF0C4},
    {0x800FF488u, 0x800FF488u, 10u, 1u, func_800FF0C4},
    {0x800FF490u, 0x800FF490u, 10u, 1u, func_800FF0C4},
    {0x800FF49Cu, 0x800FF49Cu, 10u, 1u, func_800FF0C4},
    {0x800FF4A4u, 0x800FF4A4u, 10u, 1u, func_800FF0C4},
    {0x800FF4B4u, 0x800FF4B4u, 10u, 1u, func_800FF0C4},
    {0x800FF4BCu, 0x800FF4BCu, 10u, 1u, func_800FF0C4},
    {0x800FF4E8u, 0x800FF4E8u, 10u, 1u, func_800FF0C4},
    {0x800FF4F8u, 0x800FF4F8u, 10u, 1u, func_800FF0C4},
    {0x800FF500u, 0x800FF500u, 10u, 1u, func_800FF0C4},
    {0x800FF50Cu, 0x800FF50Cu, 10u, 1u, func_800FF0C4},
    {0x800FF514u, 0x800FF514u, 10u, 1u, func_800FF0C4},
    {0x800FF53Cu, 0x800FF53Cu, 10u, 1u, func_800FF0C4},
    {0x800FF544u, 0x800FF544u, 10u, 1u, func_800FF0C4},
    {0x800FF54Cu, 0x800FF54Cu, 10u, 1u, func_800FF0C4},
    {0x800FF554u, 0x800FF554u, 10u, 1u, func_800FF0C4},
    {0x800FF564u, 0x800FF564u, 10u, 1u, func_800FF0C4},
    {0x800FF56Cu, 0x800FF56Cu, 10u, 1u, func_800FF0C4},
    {0x800FF574u, 0x800FF574u, 10u, 1u, func_800FF0C4},
    {0x800FF578u, 0x800FF578u, 10u, 1u, func_800FF0C4},
    {0x800FF580u, 0x800FF580u, 10u, 1u, func_800FF0C4},
    {0x800FF598u, 0x800FF598u, 10u, 1u, func_800FF0C4},
    {0x800FF5A4u, 0x800FF5A4u, 10u, 1u, func_800FF0C4},
    {0x800FF5BCu, 0x800FF5BCu, 10u, 1u, func_800FF0C4},
    {0x800FF5C4u, 0x800FF5C4u, 10u, 1u, func_800FF0C4},
    {0x800FF5CCu, 0x800FF5CCu, 10u, 1u, func_800FF0C4},
    {0x800FF5D8u, 0x800FF5D8u, 10u, 1u, func_800FF0C4},
    {0x800FF5ECu, 0x800FF5ECu, 10u, 1u, func_800FF0C4},
    {0x800FF5F4u, 0x800FF5F4u, 10u, 1u, func_800FF0C4},
    {0x800FF5FCu, 0x800FF5FCu, 10u, 1u, func_800FF0C4},
    {0x800FF60Cu, 0x800FF60Cu, 10u, 1u, func_800FF0C4},
    {0x800FF62Cu, 0x800FF62Cu, 10u, 1u, func_800FF0C4},
    {0x800FF660u, 0x800FF660u, 10u, 1u, func_800FF0C4},
    {0x800FF668u, 0x800FF668u, 10u, 1u, func_800FF0C4},
    {0x800FF678u, 0x800FF678u, 10u, 1u, func_800FF0C4},
    {0x800FF680u, 0x800FF680u, 10u, 1u, func_800FF0C4},
    {0x800FF6ACu, 0x800FF6ACu, 10u, 1u, func_800FF0C4},
    {0x800FF6B4u, 0x800FF6B4u, 10u, 1u, func_800FF0C4},
    {0x800FF6BCu, 0x800FF6BCu, 10u, 1u, func_800FF0C4},
    {0x800FF6C8u, 0x800FF6C8u, 10u, 1u, func_800FF0C4},
    {0x800FF6D0u, 0x800FF6D0u, 10u, 1u, func_800FF0C4},
    {0x800FF6DCu, 0x800FF6DCu, 10u, 1u, func_800FF0C4},
    {0x800FF6E4u, 0x800FF6E4u, 10u, 1u, func_800FF0C4},
    {0x800FF710u, 0x800FF710u, 10u, 1u, func_800FF0C4},
    {0x800FF718u, 0x800FF718u, 10u, 1u, func_800FF0C4},
    {0x800FF720u, 0x800FF720u, 10u, 1u, func_800FF0C4},
    {0x800FF72Cu, 0x800FF72Cu, 10u, 1u, func_800FF0C4},
    {0x800FF734u, 0x800FF734u, 10u, 1u, func_800FF0C4},
    {0x800FF73Cu, 0x800FF73Cu, 10u, 1u, func_800FF0C4},
    {0x800FF740u, 0x800FF740u, 10u, 1u, func_800FF0C4},
    {0x800FF74Cu, 0x800FF74Cu, 10u, 1u, func_800FF0C4},
    {0x800FF754u, 0x800FF754u, 10u, 1u, func_800FF0C4},
    {0x800FF75Cu, 0x800FF75Cu, 10u, 1u, func_800FF0C4},
    {0x800FF764u, 0x800FF764u, 10u, 1u, func_800FF0C4},
    {0x800FF770u, 0x800FF770u, 10u, 1u, func_800FF0C4},
    {0x800FF77Cu, 0x800FF77Cu, 10u, 1u, func_800FF0C4},
    {0x800FF784u, 0x800FF784u, 10u, 1u, func_800FF0C4},
    {0x800FF788u, 0x800FF788u, 10u, 1u, func_800FF0C4},
    {0x800FF7C0u, 0x00000000u, 11u, 1u, func_800FF7C0},
    {0x800FF7D0u, 0x800FF7D0u, 11u, 1u, func_800FF7C0},
    {0x800FF828u, 0x800FF828u, 11u, 1u, func_800FF7C0},
    {0x800FF830u, 0x00000000u, 12u, 1u, func_800FF830},
    {0x800FF840u, 0x800FF840u, 12u, 1u, func_800FF830},
    {0x800FF858u, 0x800FF858u, 12u, 1u, func_800FF830},
    {0x800FF864u, 0x800FF864u, 12u, 1u, func_800FF830},
    {0x800FF874u, 0x800FF874u, 12u, 1u, func_800FF830},
    {0x800FF880u, 0x800FF880u, 12u, 1u, func_800FF830},
    {0x800FF890u, 0x800FF890u, 12u, 1u, func_800FF830},
    {0x800FF894u, 0x800FF894u, 12u, 1u, func_800FF830},
    {0x800FF8A4u, 0x800FF8A4u, 12u, 1u, func_800FF830},
    {0x800FF8C4u, 0x800FF8C4u, 12u, 1u, func_800FF830},
    {0x800FF8CCu, 0x800FF8CCu, 12u, 1u, func_800FF830},
    {0x800FF8E8u, 0x00000000u, 13u, 1u, func_800FF8E8},
    {0x800FF8F4u, 0x800FF8F4u, 13u, 1u, func_800FF8E8},
    {0x800FF8FCu, 0x800FF8FCu, 13u, 1u, func_800FF8E8},
    {0x800FF91Cu, 0x800FF91Cu, 13u, 1u, func_800FF8E8},
    {0x800FF934u, 0x800FF934u, 13u, 1u, func_800FF8E8},
    {0x800FF954u, 0x800FF954u, 13u, 1u, func_800FF8E8},
    {0x800FF964u, 0x00000000u, 14u, 1u, func_800FF964},
    {0x800FF970u, 0x800FF970u, 14u, 1u, func_800FF964},
    {0x800FF978u, 0x800FF978u, 14u, 1u, func_800FF964},
    {0x800FF980u, 0x800FF980u, 14u, 1u, func_800FF964},
    {0x800FF988u, 0x800FF988u, 14u, 1u, func_800FF964},
    {0x800FF994u, 0x800FF994u, 14u, 1u, func_800FF964},
    {0x800FF99Cu, 0x800FF99Cu, 14u, 1u, func_800FF964},
    {0x800FF9A8u, 0x800FF9A8u, 14u, 1u, func_800FF964},
    {0x800FF9B4u, 0x800FF9B4u, 14u, 1u, func_800FF964},
    {0x800FF9BCu, 0x800FF9BCu, 14u, 1u, func_800FF964},
    {0x800FF9C0u, 0x800FF9C0u, 14u, 1u, func_800FF964},
    {0x800FF9C8u, 0x800FF9C8u, 14u, 1u, func_800FF964},
    {0x800FF9D0u, 0x800FF9D0u, 14u, 1u, func_800FF964},
    {0x800FFA04u, 0x800FFA04u, 14u, 1u, func_800FF964},
    {0x800FFA18u, 0x800FFA18u, 14u, 1u, func_800FF964},
    {0x800FFA30u, 0x800FFA30u, 14u, 1u, func_800FF964},
    {0x800FFA78u, 0x800FFA78u, 14u, 1u, func_800FF964},
    {0x800FFA80u, 0x800FFA80u, 14u, 1u, func_800FF964},
    {0x800FFA90u, 0x800FFA90u, 14u, 1u, func_800FF964},
    {0x800FFAB4u, 0x800FFAB4u, 14u, 1u, func_800FF964},
    {0x800FFAFCu, 0x800FFAFCu, 14u, 1u, func_800FF964},
    {0x800FFB04u, 0x800FFB04u, 14u, 1u, func_800FF964},
    {0x800FFB14u, 0x800FFB14u, 14u, 1u, func_800FF964},
    {0x800FFB2Cu, 0x800FFB2Cu, 14u, 1u, func_800FF964},
    {0x800FFB74u, 0x800FFB74u, 14u, 1u, func_800FF964},
    {0x800FFB7Cu, 0x800FFB7Cu, 14u, 1u, func_800FF964},
    {0x800FFB8Cu, 0x800FFB8Cu, 14u, 1u, func_800FF964},
    {0x800FFBB0u, 0x800FFBB0u, 14u, 1u, func_800FF964},
    {0x800FFBF8u, 0x800FFBF8u, 14u, 1u, func_800FF964},
    {0x800FFC0Cu, 0x800FFC0Cu, 14u, 1u, func_800FF964},
    {0x800FFC1Cu, 0x800FFC1Cu, 14u, 1u, func_800FF964},
    {0x800FFC24u, 0x00000000u, 15u, 1u, func_800FFC24},
    {0x800FFC40u, 0x800FFC40u, 15u, 1u, func_800FFC24},
    {0x800FFC74u, 0x800FFC74u, 15u, 1u, func_800FFC24},
    {0x800FFC8Cu, 0x800FFC8Cu, 15u, 1u, func_800FFC24},
    {0x800FFCCCu, 0x800FFCCCu, 15u, 1u, func_800FFC24},
    {0x800FFCDCu, 0x00000000u, 16u, 1u, func_800FFCDC},
    {0x800FFD50u, 0x800FFD50u, 16u, 1u, func_800FFCDC},
    {0x800FFD60u, 0x800FFD60u, 16u, 1u, func_800FFCDC},
    {0x800FFD98u, 0x800FFD98u, 16u, 1u, func_800FFCDC},
    {0x800FFDA8u, 0x800FFDA8u, 16u, 1u, func_800FFCDC},
    {0x800FFDDCu, 0x800FFDDCu, 16u, 1u, func_800FFCDC},
    {0x800FFDECu, 0x800FFDECu, 16u, 1u, func_800FFCDC},
    {0x800FFE20u, 0x800FFE20u, 16u, 1u, func_800FFCDC},
    {0x800FFE30u, 0x800FFE30u, 16u, 1u, func_800FFCDC},
};

static const PsxGameDispatchEntry* psx_fmv_str_find_entry(uint32_t addr) {
    uint32_t lo = 0;
    uint32_t hi = sizeof(k_psx_fmv_str_dispatch) / sizeof(k_psx_fmv_str_dispatch[0]);
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2;
        uint32_t key = k_psx_fmv_str_dispatch[mid].addr;
        if (addr < key) hi = mid;
        else if (addr > key) lo = mid + 1;
        else return &k_psx_fmv_str_dispatch[mid];
    }
    return 0;
}

static int psx_fmv_str_native_ok(uint32_t addr, const PsxGameDispatchEntry* entry) {
    if (!entry || entry->range_count == 0) return 0;
    const uint8_t *ram = memory_get_ram_ptr();
    if (!ram) return 0;
    for (uint32_t i = 0; i < entry->range_count; i++) {
        const PsxGameCodeRange* r = &k_psx_fmv_str_ranges[entry->range_index + i];
        uint32_t phys = r->lo & 0x1FFFFFFFu;
        if (phys < 0x000FDD58u || phys + r->len > 0x000FDD58u + sizeof(s_fmv_str_rom)) return 0;
        uint32_t offset = phys - 0x000FDD58u;
        if (ram[phys] != s_fmv_str_rom[offset]) return 0;
        if (memcmp(ram + phys, s_fmv_str_rom + offset, r->len) != 0) return 0;
    }
    return 1;
}

int psx_dispatch_fmv_str_compiled(CPUState* cpu, uint32_t addr) {
    const PsxGameDispatchEntry* entry = psx_fmv_str_find_entry(addr);
    if (!entry || entry->range_count == 0) {
        return 0;
    }
    if (!psx_fmv_str_native_ok(addr, entry)) {
        return 0;
    }
    psx_check_interrupts_dispatch_entry(cpu, addr);
    cpu->pc = entry->resume_pc;
    entry->fn(cpu);
    return 1;
}
