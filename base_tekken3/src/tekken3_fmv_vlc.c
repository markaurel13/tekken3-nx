/* Auto-generated FMV VLC macroblock decoder for Tekken 3 */
#include "tekken3_fmv_overlay.h"
#include <string.h>

extern void psx_check_interrupts_dispatch_entry(CPUState* cpu, uint32_t resume_pc);
extern uint8_t *memory_get_ram_ptr(void);

static const uint8_t s_fmv_vlc_rom[6144] = {
    0x15u, 0x00u, 0x4Eu, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x36u, 0x01u, 0x0Au, 0x24u, 0x1Cu, 0x00u, 0x40u, 0xA0u,
    0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x09u, 0x01u, 0x09u, 0x24u, 0x1Du, 0x00u, 0x4Du, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu,
    0x18u, 0x80u, 0x0Fu, 0x3Cu, 0x24u, 0x00u, 0x43u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0xFFu, 0x0Cu, 0x3Cu,
    0x25u, 0x00u, 0x4Du, 0xA0u, 0xD8u, 0x4Du, 0xC5u, 0x8Cu, 0xDCu, 0x4Du, 0xE7u, 0x8Du, 0x00u, 0x00u, 0xA3u, 0x8Cu,
    0x36u, 0x00u, 0x02u, 0x24u, 0x08u, 0x00u, 0xA2u, 0xA4u, 0x0Au, 0x00u, 0xABu, 0xA4u, 0x10u, 0x00u, 0xAAu, 0xA4u,
    0x12u, 0x00u, 0xABu, 0xA4u, 0x18u, 0x00u, 0xA2u, 0xA4u, 0x1Au, 0x00u, 0xA9u, 0xA4u, 0x20u, 0x00u, 0xAAu, 0xA4u,
    0x22u, 0x00u, 0xA9u, 0xA4u, 0x00u, 0x00u, 0xE2u, 0x8Cu, 0x24u, 0x18u, 0x6Cu, 0x00u, 0x24u, 0x10u, 0x48u, 0x00u,
    0x25u, 0x18u, 0x62u, 0x00u, 0x00u, 0x00u, 0xA3u, 0xACu, 0x28u, 0x00u, 0xA3u, 0x24u, 0x00u, 0x00u, 0xE2u, 0x8Cu,
    0x24u, 0x28u, 0xA8u, 0x00u, 0xD8u, 0x4Du, 0xC3u, 0xACu, 0x24u, 0x10u, 0x4Cu, 0x00u, 0x25u, 0x10u, 0x45u, 0x00u,
    0x00u, 0x00u, 0xE2u, 0xACu, 0x03u, 0x00u, 0x72u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x07u, 0x00u, 0x51u, 0xA0u, 0xD8u, 0x4Du, 0xC3u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x62u, 0x90u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xFEu, 0x00u, 0x42u, 0x30u, 0x07u, 0x00u, 0x62u, 0xA0u, 0xD8u, 0x4Du, 0xC3u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x62u, 0x90u, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x42u, 0x34u,
    0x07u, 0x00u, 0x62u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x44u, 0xA0u,
    0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x44u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x44u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x0Bu, 0x00u, 0x03u, 0x24u,
    0x16u, 0x00u, 0x43u, 0xA4u, 0x0Eu, 0x00u, 0x50u, 0xA4u, 0x0Cu, 0x00u, 0x40u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x0Du, 0x00u, 0x4Eu, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x04u, 0x00u, 0x03u, 0x24u,
    0x14u, 0x00u, 0x43u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x15u, 0x00u, 0x4Eu, 0xA0u,
    0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Cu, 0x00u, 0x40u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x1Du, 0x00u, 0x4Du, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x24u, 0x00u, 0x43u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x25u, 0x00u, 0x4Du, 0xA0u,
    0xD8u, 0x4Du, 0xC4u, 0x8Cu, 0xDCu, 0x4Du, 0xE5u, 0x8Du, 0x00u, 0x00u, 0x83u, 0x8Cu, 0x3Bu, 0x01u, 0x02u, 0x24u,
    0x08u, 0x00u, 0x8Au, 0xA4u, 0x0Au, 0x00u, 0x8Bu, 0xA4u, 0x10u, 0x00u, 0x82u, 0xA4u, 0x12u, 0x00u, 0x8Bu, 0xA4u,
    0x18u, 0x00u, 0x8Au, 0xA4u, 0x1Au, 0x00u, 0x89u, 0xA4u, 0x20u, 0x00u, 0x82u, 0xA4u, 0x22u, 0x00u, 0x89u, 0xA4u,
    0x00u, 0x00u, 0xA2u, 0x8Cu, 0x24u, 0x18u, 0x6Cu, 0x00u, 0x24u, 0x10u, 0x48u, 0x00u, 0x25u, 0x18u, 0x62u, 0x00u,
    0x00u, 0x00u, 0x83u, 0xACu, 0x28u, 0x00u, 0x83u, 0x24u, 0x00u, 0x00u, 0xA2u, 0x8Cu, 0x24u, 0x20u, 0x88u, 0x00u,
    0xD8u, 0x4Du, 0xC3u, 0xACu, 0x24u, 0x10u, 0x4Cu, 0x00u, 0x25u, 0x10u, 0x44u, 0x00u, 0x00u, 0x00u, 0xA2u, 0xACu,
    0x08u, 0x00u, 0xB2u, 0x8Fu, 0x04u, 0x00u, 0xB1u, 0x8Fu, 0x00u, 0x00u, 0xB0u, 0x8Fu, 0x10u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xD0u, 0xFFu, 0xBDu, 0x27u, 0x28u, 0x00u, 0xBFu, 0xAFu,
    0xF2u, 0xFBu, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x13u, 0x00u, 0x40u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x8Eu, 0xA5u, 0x00u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0xF6u, 0xFBu, 0x01u, 0x0Cu, 0x10u, 0x00u, 0xA4u, 0x27u,
    0x14u, 0x00u, 0xA4u, 0x8Fu, 0x18u, 0x00u, 0xA5u, 0x8Fu, 0x94u, 0xF1u, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0xE9u, 0xF0u, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0x1Cu, 0x00u, 0xA4u, 0x8Fu, 0x20u, 0x00u, 0xA5u, 0x8Fu,
    0x94u, 0xF1u, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0xE9u, 0xF0u, 0x01u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u,
    0x8Eu, 0xA5u, 0x00u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x00u, 0x28u, 0x00u, 0xBFu, 0x8Fu, 0x30u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0xFFu, 0xBDu, 0x27u, 0x21u, 0x48u, 0x80u, 0x00u,
    0x04u, 0x00u, 0xB1u, 0xAFu, 0x21u, 0x88u, 0xA0u, 0x00u, 0x18u, 0x00u, 0xB6u, 0xAFu, 0x28u, 0x18u, 0x16u, 0x24u,
    0x14u, 0x00u, 0xB5u, 0xAFu, 0x2Au, 0x00u, 0x15u, 0x24u, 0x30u, 0x00u, 0xAEu, 0x93u, 0x00u, 0x3Cu, 0x07u, 0x00u,
    0x1Cu, 0x00u, 0xB7u, 0xAFu, 0x10u, 0x00u, 0xB4u, 0xAFu, 0x0Cu, 0x00u, 0xB3u, 0xAFu, 0x08u, 0x00u, 0xB2u, 0xAFu,
    0x08u, 0x00u, 0xE0u, 0x10u, 0x00u, 0x00u, 0xB0u, 0xAFu, 0x15u, 0x00u, 0x0Bu, 0x24u, 0x21u, 0xC8u, 0x00u, 0x00u,
    0x0Cu, 0x00u, 0x18u, 0x24u, 0x08u, 0x00u, 0x12u, 0x24u, 0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x05u, 0x14u, 0x04u, 0x08u,
    0x18u, 0xB9u, 0x4Fu, 0x24u, 0x0Eu, 0x00u, 0x0Bu, 0x24u, 0x24u, 0x00u, 0x19u, 0x24u, 0x12u, 0x00u, 0x18u, 0x24u,
    0x0Cu, 0x00u, 0x12u, 0x24u, 0x0Cu, 0x80u, 0x02u, 0x3Cu, 0x58u, 0xB9u, 0x4Fu, 0x24u, 0x0Cu, 0x00u, 0x13u, 0x24u,
    0x34u, 0x00u, 0xADu, 0x8Fu, 0x18u, 0x80u, 0x06u, 0x3Cu, 0x0Cu, 0x00u, 0x30u, 0x26u, 0xFFu, 0x00u, 0x0Cu, 0x3Cu,
    0xFFu, 0xFFu, 0x8Cu, 0x35u, 0x00u, 0xFFu, 0x14u, 0x3Cu, 0x00u, 0x00u, 0xA2u, 0x91u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xFFu, 0x00u, 0x44u, 0x30u, 0x75u, 0x00u, 0x80u, 0x10u, 0x01u, 0x00u, 0xADu, 0x25u, 0x31u, 0x00u, 0x82u, 0x2Cu,
    0x12u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0x05u, 0x24u, 0x20u, 0x00u, 0x02u, 0x24u, 0x04u, 0x00u, 0x82u, 0x14u,
    0xDFu, 0xFFu, 0x82u, 0x24u, 0x21u, 0x28u, 0x00u, 0x00u, 0x34u, 0x14u, 0x04u, 0x08u, 0x21u, 0x38u, 0x40u, 0x02u,
    0x1Au, 0x00u, 0x4Bu, 0x00u, 0x12u, 0x10u, 0x00u, 0x00u, 0x10u, 0x18u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x00u, 0x53u, 0x00u, 0x12u, 0x40u, 0x00u, 0x00u, 0x21u, 0x10u, 0x8Fu, 0x00u,
    0xDFu, 0xFFu, 0x47u, 0x90u, 0x32u, 0x14u, 0x04u, 0x08u, 0x18u, 0x00u, 0x78u, 0x00u, 0xDEu, 0xFFu, 0x82u, 0x24u,
    0x1Au, 0x00u, 0x4Bu, 0x00u, 0x12u, 0x10u, 0x00u, 0x00u, 0x10u, 0x18u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x00u, 0x53u, 0x00u, 0x12u, 0x40u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x00u, 0x78u, 0x00u, 0x21u, 0x10u, 0x8Fu, 0x00u, 0xDEu, 0xFFu, 0x47u, 0x90u,
    0x21u, 0x40u, 0x19u, 0x01u, 0x12u, 0x50u, 0x00u, 0x00u, 0x4Eu, 0x00u, 0xA0u, 0x10u, 0x09u, 0x00u, 0x02u, 0x24u,
    0xD8u, 0x4Du, 0xC3u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x62u, 0xA0u, 0xD8u, 0x4Du, 0xC3u, 0x8Cu,
    0x2Cu, 0x00u, 0x02u, 0x24u, 0x07u, 0x00u, 0x62u, 0xA0u, 0xD8u, 0x4Du, 0xC3u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x07u, 0x00u, 0x62u, 0x90u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFEu, 0x00u, 0x42u, 0x30u, 0x07u, 0x00u, 0x62u, 0xA0u,
    0xD8u, 0x4Du, 0xC3u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x07u, 0x00u, 0x62u, 0x90u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x02u, 0x00u, 0x42u, 0x34u, 0x07u, 0x00u, 0x62u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x04u, 0x00u, 0x4Eu, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x4Eu, 0xA0u,
    0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x4Eu, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x0Cu, 0x00u, 0x4Au, 0xA0u, 0xD8u, 0x4Du, 0xC3u, 0x8Cu, 0x16u, 0x00u, 0x55u, 0xA4u,
    0x0Eu, 0x00u, 0x56u, 0xA4u, 0x0Du, 0x00u, 0x68u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x21u, 0x20u, 0x47u, 0x01u,
    0x14u, 0x00u, 0x44u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x15u, 0x00u, 0x48u, 0xA0u,
    0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Cu, 0x00u, 0x4Au, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu,
    0x21u, 0x18u, 0x13u, 0x01u, 0x1Du, 0x00u, 0x43u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x24u, 0x00u, 0x44u, 0xA0u, 0xD8u, 0x4Du, 0xC2u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x25u, 0x00u, 0x43u, 0xA0u,
    0xD8u, 0x4Du, 0xC4u, 0x8Cu, 0x21u, 0x10u, 0x27u, 0x01u, 0x10u, 0x00u, 0x82u, 0xA4u, 0x20u, 0x00u, 0x82u, 0xA4u,
    0x0Bu, 0x80u, 0x02u, 0x3Cu, 0x18u, 0x92u, 0x45u, 0x8Cu, 0x00u, 0x00u, 0x83u, 0x8Cu, 0x08u, 0x00u, 0x89u, 0xA4u,
    0x0Au, 0x00u, 0x91u, 0xA4u, 0x12u, 0x00u, 0x91u, 0xA4u, 0x18u, 0x00u, 0x89u, 0xA4u, 0x1Au, 0x00u, 0x90u, 0xA4u,
    0x22u, 0x00u, 0x90u, 0xA4u, 0x18u, 0x00u, 0xA2u, 0x8Cu, 0x24u, 0x18u, 0x74u, 0x00u, 0x24u, 0x10u, 0x4Cu, 0x00u,
    0x25u, 0x18u, 0x62u, 0x00u, 0x28u, 0x00u, 0x82u, 0x24u, 0x00u, 0x00u, 0x83u, 0xACu, 0x18u, 0x00u, 0xA3u, 0x8Cu,
    0x24u, 0x20u, 0x8Cu, 0x00u, 0xD8u, 0x4Du, 0xC2u, 0xACu, 0x24u, 0x18u, 0x74u, 0x00u, 0x25u, 0x18u, 0x64u, 0x00u,
    0x18u, 0x00u, 0xA3u, 0xACu, 0x0Cu, 0x14u, 0x04u, 0x08u, 0x21u, 0x48u, 0x27u, 0x01u, 0x1Cu, 0x00u, 0xB7u, 0x8Fu,
    0x18u, 0x00u, 0xB6u, 0x8Fu, 0x14u, 0x00u, 0xB5u, 0x8Fu, 0x10u, 0x00u, 0xB4u, 0x8Fu, 0x0Cu, 0x00u, 0xB3u, 0x8Fu,
    0x08u, 0x00u, 0xB2u, 0x8Fu, 0x04u, 0x00u, 0xB1u, 0x8Fu, 0x00u, 0x00u, 0xB0u, 0x8Fu, 0x20u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0Fu, 0x08u, 0x24u, 0xFFu, 0xFFu, 0x08u, 0x21u,
    0x10u, 0x80u, 0x06u, 0x3Cu, 0x40u, 0x52u, 0xC6u, 0x24u, 0x00u, 0x00u, 0x89u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x02u, 0x52u, 0x09u, 0x00u, 0x02u, 0x5Cu, 0x09u, 0x00u, 0x02u, 0x66u, 0x09u, 0x00u, 0xFFu, 0x00u, 0x29u, 0x31u,
    0xFFu, 0x00u, 0x4Au, 0x31u, 0xFFu, 0x00u, 0x6Bu, 0x31u, 0x20u, 0x48u, 0x26u, 0x01u, 0x20u, 0x50u, 0x46u, 0x01u,
    0x20u, 0x58u, 0x66u, 0x01u, 0x20u, 0x60u, 0x86u, 0x01u, 0x00u, 0x00u, 0x29u, 0x91u, 0x00u, 0x00u, 0x4Au, 0x91u,
    0x00u, 0x00u, 0x6Bu, 0x91u, 0x00u, 0x00u, 0x8Cu, 0x91u, 0x00u, 0x52u, 0x0Au, 0x00u, 0x00u, 0x5Cu, 0x0Bu, 0x00u,
    0x00u, 0x66u, 0x0Cu, 0x00u, 0x25u, 0x48u, 0x2Au, 0x01u, 0x25u, 0x48u, 0x2Bu, 0x01u, 0x25u, 0x48u, 0x2Cu, 0x01u,
    0x04u, 0x00u, 0x84u, 0x20u, 0x00u, 0x00u, 0xA9u, 0xACu, 0xE4u, 0xFFu, 0x00u, 0x1Du, 0x04u, 0x00u, 0xA5u, 0x20u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0xA0u, 0x10u, 0xFFu, 0xFFu, 0xA5u, 0x20u,
    0x0Fu, 0x00u, 0xA0u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0xE0u, 0x03u, 0xFFu, 0x00u, 0x02u, 0x24u,
    0x10u, 0x80u, 0x05u, 0x3Cu, 0x40u, 0x52u, 0xA5u, 0x24u, 0x00u, 0x00u, 0x03u, 0x24u, 0x18u, 0x00u, 0x44u, 0x00u,
    0x20u, 0x30u, 0xA2u, 0x00u, 0xFFu, 0xFFu, 0x42u, 0x20u, 0x12u, 0x40u, 0x00u, 0x00u, 0x82u, 0x41u, 0x08u, 0x00u,
    0xFAu, 0xFFu, 0x41u, 0x04u, 0x00u, 0x00u, 0xC8u, 0xA0u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x22u, 0x20u, 0x04u, 0x00u, 0xFFu, 0x00u, 0x02u, 0x24u, 0x10u, 0x80u, 0x05u, 0x3Cu, 0x40u, 0x52u, 0xA5u, 0x24u,
    0x00u, 0x00u, 0x03u, 0x24u, 0x18u, 0x00u, 0x44u, 0x00u, 0x20u, 0x30u, 0xA2u, 0x00u, 0xFFu, 0xFFu, 0x42u, 0x20u,
    0x12u, 0x40u, 0x00u, 0x00u, 0x82u, 0x41u, 0x08u, 0x00u, 0xFFu, 0x00u, 0x08u, 0x21u, 0xF9u, 0xFFu, 0x41u, 0x04u,
    0x00u, 0x00u, 0xC8u, 0xA0u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE1u, 0x0Au, 0x34u,
    0xFFu, 0xFFu, 0x09u, 0x24u, 0x00u, 0x00u, 0x88u, 0x8Cu, 0xFFu, 0xFFu, 0x4Au, 0x21u, 0x22u, 0x40u, 0x28u, 0x01u,
    0x00u, 0x00u, 0x88u, 0xACu, 0x04u, 0x00u, 0x84u, 0x20u, 0xFBu, 0xFFu, 0x41u, 0x05u, 0x00u, 0x00u, 0x88u, 0x8Cu,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x21u, 0x18u, 0x00u, 0x00u, 0x11u, 0x80u, 0x07u, 0x3Cu,
    0x48u, 0x34u, 0xE7u, 0x24u, 0x21u, 0x30u, 0x80u, 0x00u, 0xF0u, 0x00u, 0x0Au, 0x24u, 0xFFu, 0xFFu, 0x08u, 0x3Cu,
    0x01u, 0x0Fu, 0x08u, 0x35u, 0x00u, 0x0Fu, 0x09u, 0x24u, 0x00u, 0x00u, 0xE2u, 0x90u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xFFu, 0x00u, 0x45u, 0x30u, 0xF0u, 0x00u, 0xA2u, 0x2Cu, 0x17u, 0x00u, 0x40u, 0x10u, 0x01u, 0x00u, 0xE7u, 0x24u,
    0x0Bu, 0x00u, 0x60u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Au, 0x00u, 0xA0u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x23u, 0x10u, 0xC3u, 0x00u, 0x00u, 0x00u, 0x42u, 0x90u, 0xFFu, 0xFFu, 0xA5u, 0x24u, 0x00u, 0x00u, 0xC2u, 0xA0u,
    0xFBu, 0xFFu, 0xA1u, 0x04u, 0x01u, 0x00u, 0xC6u, 0x24u, 0x47u, 0x15u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x10u, 0x00u, 0xA0u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE2u, 0x90u, 0x01u, 0x00u, 0xE7u, 0x24u,
    0xFFu, 0xFFu, 0xA5u, 0x24u, 0x00u, 0x00u, 0xC2u, 0xA0u, 0xFBu, 0xFFu, 0xA1u, 0x04u, 0x01u, 0x00u, 0xC6u, 0x24u,
    0x47u, 0x15u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0xAAu, 0x10u, 0x21u, 0x18u, 0x00u, 0x00u,
    0x00u, 0x00u, 0xE3u, 0x90u, 0x01u, 0x00u, 0xE7u, 0x24u, 0x00u, 0x12u, 0x05u, 0x00u, 0x25u, 0x10u, 0x43u, 0x00u,
    0x21u, 0x18u, 0x48u, 0x00u, 0xDCu, 0xFFu, 0x69u, 0x14u, 0x04u, 0x00u, 0x05u, 0x24u, 0xFFu, 0x87u, 0x06u, 0x34u,
    0x08u, 0x00u, 0x84u, 0x24u, 0x00u, 0x00u, 0x82u, 0x94u, 0xF8u, 0xFFu, 0x83u, 0x94u, 0x01u, 0x00u, 0xA5u, 0x24u,
    0x26u, 0x10u, 0x43u, 0x00u, 0x00u, 0x00u, 0x82u, 0xA4u, 0x2Au, 0x10u, 0xC5u, 0x00u, 0xF9u, 0xFFu, 0x40u, 0x10u,
    0x02u, 0x00u, 0x84u, 0x24u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xB0u, 0xAFu,
    0x21u, 0x80u, 0x80u, 0x00u, 0x03u, 0x00u, 0x00u, 0x16u, 0x14u, 0x00u, 0xBFu, 0xAFu, 0xF2u, 0x16u, 0x02u, 0x0Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x08u, 0x16u, 0x04u, 0x0Cu, 0x21u, 0x20u, 0x00u, 0x02u, 0x14u, 0x00u, 0xBFu, 0x8Fu,
    0x10u, 0x00u, 0xB0u, 0x8Fu, 0x08u, 0x00u, 0xE0u, 0x03u, 0x18u, 0x00u, 0xBDu, 0x27u, 0x21u, 0x30u, 0x80u, 0x00u,
    0x11u, 0x80u, 0x05u, 0x3Cu, 0x5Cu, 0x42u, 0xA5u, 0x24u, 0x0Fu, 0x00u, 0x03u, 0x24u, 0xFFu, 0xFFu, 0x07u, 0x24u,
    0x00u, 0x00u, 0xA2u, 0x8Cu, 0x04u, 0x00u, 0xA5u, 0x24u, 0xFFu, 0xFFu, 0x63u, 0x24u, 0x00u, 0x00u, 0xC2u, 0xACu,
    0xFBu, 0xFFu, 0x67u, 0x14u, 0x04u, 0x00u, 0xC6u, 0x24u, 0x40u, 0x00u, 0x86u, 0x24u, 0x11u, 0x80u, 0x05u, 0x3Cu,
    0x9Cu, 0x42u, 0xA5u, 0x24u, 0x0Fu, 0x00u, 0x03u, 0x24u, 0xFFu, 0xFFu, 0x07u, 0x24u, 0x00u, 0x00u, 0xA2u, 0x8Cu,
    0x04u, 0x00u, 0xA5u, 0x24u, 0xFFu, 0xFFu, 0x63u, 0x24u, 0x00u, 0x00u, 0xC2u, 0xACu, 0xFBu, 0xFFu, 0x67u, 0x14u,
    0x04u, 0x00u, 0xC6u, 0x24u, 0x80u, 0x00u, 0x86u, 0x24u, 0x11u, 0x80u, 0x05u, 0x3Cu, 0xE0u, 0x42u, 0xA5u, 0x24u,
    0x1Fu, 0x00u, 0x03u, 0x24u, 0xFFu, 0xFFu, 0x07u, 0x24u, 0x00u, 0x00u, 0xA2u, 0x8Cu, 0x04u, 0x00u, 0xA5u, 0x24u,
    0xFFu, 0xFFu, 0x63u, 0x24u, 0x00u, 0x00u, 0xC2u, 0xACu, 0xFBu, 0xFFu, 0x67u, 0x14u, 0x04u, 0x00u, 0xC6u, 0x24u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x21u, 0x10u, 0x80u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xB0u, 0xAFu,
    0x21u, 0x80u, 0x80u, 0x00u, 0x11u, 0x80u, 0x05u, 0x3Cu, 0x5Cu, 0x42u, 0xA5u, 0x24u, 0x0Fu, 0x00u, 0x03u, 0x24u,
    0xFFu, 0xFFu, 0x06u, 0x24u, 0x14u, 0x00u, 0xBFu, 0xAFu, 0x00u, 0x00u, 0x82u, 0x8Cu, 0x04u, 0x00u, 0x84u, 0x24u,
    0xFFu, 0xFFu, 0x63u, 0x24u, 0x00u, 0x00u, 0xA2u, 0xACu, 0xFBu, 0xFFu, 0x66u, 0x14u, 0x04u, 0x00u, 0xA5u, 0x24u,
    0x11u, 0x80u, 0x05u, 0x3Cu, 0x9Cu, 0x42u, 0xA5u, 0x24u, 0x40u, 0x00u, 0x04u, 0x26u, 0x0Fu, 0x00u, 0x03u, 0x24u,
    0xFFu, 0xFFu, 0x06u, 0x24u, 0x00u, 0x00u, 0x82u, 0x8Cu, 0x04u, 0x00u, 0x84u, 0x24u, 0xFFu, 0xFFu, 0x63u, 0x24u,
    0x00u, 0x00u, 0xA2u, 0xACu, 0xFBu, 0xFFu, 0x66u, 0x14u, 0x04u, 0x00u, 0xA5u, 0x24u, 0x11u, 0x80u, 0x04u, 0x3Cu,
    0x58u, 0x42u, 0x84u, 0x24u, 0x44u, 0x16u, 0x04u, 0x0Cu, 0x20u, 0x00u, 0x05u, 0x24u, 0x11u, 0x80u, 0x04u, 0x3Cu,
    0xDCu, 0x42u, 0x84u, 0x24u, 0x44u, 0x16u, 0x04u, 0x0Cu, 0x20u, 0x00u, 0x05u, 0x24u, 0x21u, 0x10u, 0x00u, 0x02u,
    0x14u, 0x00u, 0xBFu, 0x8Fu, 0x10u, 0x00u, 0xB0u, 0x8Fu, 0x08u, 0x00u, 0xE0u, 0x03u, 0x18u, 0x00u, 0xBDu, 0x27u,
    0x00u, 0x00u, 0x82u, 0x94u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u,
    0x01u, 0x00u, 0xA2u, 0x30u, 0x06u, 0x00u, 0x40u, 0x10u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0xFFu, 0xF7u, 0x03u, 0x3Cu,
    0x00u, 0x00u, 0x82u, 0x8Cu, 0xFFu, 0xFFu, 0x63u, 0x34u, 0xBDu, 0x15u, 0x04u, 0x08u, 0x24u, 0x10u, 0x43u, 0x00u,
    0x00u, 0x00u, 0x82u, 0x8Cu, 0x00u, 0x08u, 0x03u, 0x3Cu, 0x25u, 0x10u, 0x43u, 0x00u, 0x00u, 0x00u, 0x82u, 0xACu,
    0x02u, 0x00u, 0xA2u, 0x30u, 0x04u, 0x00u, 0x40u, 0x10u, 0x00u, 0x02u, 0x03u, 0x3Cu, 0x00u, 0x00u, 0x82u, 0x8Cu,
    0xC8u, 0x15u, 0x04u, 0x08u, 0x25u, 0x10u, 0x43u, 0x00u, 0xFFu, 0xFDu, 0x03u, 0x3Cu, 0x00u, 0x00u, 0x82u, 0x8Cu,
    0xFFu, 0xFFu, 0x63u, 0x34u, 0x24u, 0x10u, 0x43u, 0x00u, 0x00u, 0x00u, 0x82u, 0xACu, 0x00u, 0x00u, 0x85u, 0x94u,
    0x44u, 0x16u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xBFu, 0xAFu,
    0x68u, 0x16u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x05u, 0x00u, 0x80u, 0x14u,
    0x10u, 0x00u, 0xBFu, 0xAFu, 0x8Bu, 0x16u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0xE3u, 0x15u, 0x04u, 0x08u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xD5u, 0x16u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x42u, 0x17u, 0x02u, 0x00u,
    0x01u, 0x00u, 0x42u, 0x30u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x05u, 0x00u, 0x80u, 0x14u, 0x10u, 0x00u, 0xBFu, 0xAFu,
    0xB0u, 0x16u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0xF2u, 0x15u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xD5u, 0x16u, 0x04u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x16u, 0x02u, 0x00u, 0x01u, 0x00u, 0x42u, 0x30u,
    0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xE8u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0x21u, 0x28u, 0x80u, 0x00u, 0x0Au, 0x17u, 0x02u, 0x0Cu,
    0x21u, 0x20u, 0x00u, 0x00u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0x21u, 0x28u, 0x80u, 0x00u,
    0x0Au, 0x17u, 0x02u, 0x0Cu, 0x01u, 0x00u, 0x04u, 0x24u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x21u, 0x28u, 0x80u, 0x00u,
    0x06u, 0x00u, 0xA0u, 0x10u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0x01u, 0x00u, 0x02u, 0x24u, 0x1Bu, 0x00u, 0xA2u, 0x10u,
    0x00u, 0x80u, 0x02u, 0x3Cu, 0x3Du, 0x16u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x80u, 0x03u, 0x3Cu,
    0x9Cu, 0x43u, 0x63u, 0x8Cu, 0x00u, 0x80u, 0x02u, 0x3Cu, 0x00u, 0x00u, 0x62u, 0xACu, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x70u, 0x43u, 0x42u, 0x8Cu, 0x11u, 0x80u, 0x04u, 0x3Cu, 0x58u, 0x42u, 0x84u, 0x24u, 0x00u, 0x00u, 0x40u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x7Cu, 0x43u, 0x42u, 0x8Cu, 0x20u, 0x00u, 0x05u, 0x24u, 0x00u, 0x00u, 0x40u, 0xACu,
    0x11u, 0x80u, 0x03u, 0x3Cu, 0x9Cu, 0x43u, 0x63u, 0x8Cu, 0x00u, 0x60u, 0x02u, 0x3Cu, 0x44u, 0x16u, 0x04u, 0x0Cu,
    0x00u, 0x00u, 0x62u, 0xACu, 0x11u, 0x80u, 0x04u, 0x3Cu, 0xDCu, 0x42u, 0x84u, 0x24u, 0x44u, 0x16u, 0x04u, 0x0Cu,
    0x20u, 0x00u, 0x05u, 0x24u, 0x40u, 0x16u, 0x04u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x80u, 0x03u, 0x3Cu,
    0x9Cu, 0x43u, 0x63u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x62u, 0xACu, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x70u, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0xACu, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x7Cu, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0xACu, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x7Cu, 0x43u, 0x42u, 0x8Cu, 0x11u, 0x80u, 0x03u, 0x3Cu, 0x9Cu, 0x43u, 0x63u, 0x8Cu, 0x00u, 0x00u, 0x42u, 0x8Cu,
    0x00u, 0x60u, 0x02u, 0x3Cu, 0x40u, 0x16u, 0x04u, 0x08u, 0x00u, 0x00u, 0x62u, 0xACu, 0x10u, 0x80u, 0x04u, 0x3Cu,
    0x16u, 0xE9u, 0x01u, 0x0Cu, 0xDCu, 0xD9u, 0x84u, 0x24u, 0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0xFFu, 0xBDu, 0x27u, 0x14u, 0x00u, 0xB1u, 0xAFu,
    0x21u, 0x88u, 0x80u, 0x00u, 0x10u, 0x00u, 0xB0u, 0xAFu, 0x18u, 0x00u, 0xBFu, 0xAFu, 0x8Bu, 0x16u, 0x04u, 0x0Cu,
    0x21u, 0x80u, 0xA0u, 0x00u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0xA0u, 0x43u, 0x63u, 0x8Cu, 0x42u, 0x81u, 0x10u, 0x00u,
    0x00u, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x84u, 0x10u, 0x00u, 0x88u, 0x00u, 0x42u, 0x34u, 0x00u, 0x00u, 0x62u, 0xACu,
    0x11u, 0x80u, 0x03u, 0x3Cu, 0x68u, 0x43u, 0x63u, 0x8Cu, 0x04u, 0x00u, 0x22u, 0x26u, 0x00u, 0x00u, 0x62u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x6Cu, 0x43u, 0x42u, 0x8Cu, 0x20u, 0x00u, 0x10u, 0x36u, 0x00u, 0x00u, 0x50u, 0xACu,
    0x11u, 0x80u, 0x03u, 0x3Cu, 0x98u, 0x43u, 0x63u, 0x8Cu, 0x00u, 0x00u, 0x22u, 0x8Eu, 0x00u, 0x01u, 0x04u, 0x3Cu,
    0x00u, 0x00u, 0x62u, 0xACu, 0x11u, 0x80u, 0x02u, 0x3Cu, 0x70u, 0x43u, 0x42u, 0x8Cu, 0x01u, 0x02u, 0x84u, 0x34u,
    0x00u, 0x00u, 0x44u, 0xACu, 0x18u, 0x00u, 0xBFu, 0x8Fu, 0x14u, 0x00u, 0xB1u, 0x8Fu, 0x10u, 0x00u, 0xB0u, 0x8Fu,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x20u, 0x00u, 0xBDu, 0x27u, 0xE0u, 0xFFu, 0xBDu, 0x27u, 0x14u, 0x00u, 0xB1u, 0xAFu,
    0x21u, 0x88u, 0x80u, 0x00u, 0x10u, 0x00u, 0xB0u, 0xAFu, 0x18u, 0x00u, 0xBFu, 0xAFu, 0xB0u, 0x16u, 0x04u, 0x0Cu,
    0x21u, 0x80u, 0xA0u, 0x00u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0xA0u, 0x43u, 0x63u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x62u, 0x8Cu, 0x42u, 0x81u, 0x10u, 0x00u, 0x88u, 0x00u, 0x42u, 0x34u, 0x00u, 0x00u, 0x62u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x7Cu, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x84u, 0x10u, 0x00u, 0x00u, 0x00u, 0x40u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x74u, 0x43u, 0x42u, 0x8Cu, 0x20u, 0x00u, 0x10u, 0x36u, 0x00u, 0x00u, 0x51u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x78u, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x01u, 0x03u, 0x3Cu, 0x00u, 0x00u, 0x50u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x7Cu, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x02u, 0x63u, 0x34u, 0x00u, 0x00u, 0x43u, 0xACu,
    0x18u, 0x00u, 0xBFu, 0x8Fu, 0x14u, 0x00u, 0xB1u, 0x8Fu, 0x10u, 0x00u, 0xB0u, 0x8Fu, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x20u, 0x00u, 0xBDu, 0x27u, 0xE0u, 0xFFu, 0xBDu, 0x27u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0x9Cu, 0x43u, 0x63u, 0x8Cu,
    0x10u, 0x00u, 0x02u, 0x3Cu, 0x18u, 0x00u, 0xBFu, 0xAFu, 0x10u, 0x00u, 0xA2u, 0xAFu, 0x00u, 0x00u, 0x62u, 0x8Cu,
    0x00u, 0x20u, 0x03u, 0x3Cu, 0x24u, 0x10u, 0x43u, 0x00u, 0x17u, 0x00u, 0x40u, 0x10u, 0x21u, 0x10u, 0x00u, 0x00u,
    0xFFu, 0xFFu, 0x04u, 0x24u, 0x10u, 0x00u, 0xA2u, 0x8Fu, 0x00u, 0x00u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x42u, 0x24u,
    0x10u, 0x00u, 0xA2u, 0xAFu, 0x10u, 0x00u, 0xA2u, 0x8Fu, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x44u, 0x14u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x80u, 0x04u, 0x3Cu, 0xDBu, 0x16u, 0x04u, 0x0Cu, 0xF8u, 0xD9u, 0x84u, 0x24u,
    0xACu, 0x16u, 0x04u, 0x08u, 0xFFu, 0xFFu, 0x02u, 0x24u, 0x11u, 0x80u, 0x02u, 0x3Cu, 0x9Cu, 0x43u, 0x42u, 0x8Cu,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x10u, 0x43u, 0x00u,
    0xECu, 0xFFu, 0x40u, 0x14u, 0x21u, 0x10u, 0x00u, 0x00u, 0x18u, 0x00u, 0xBFu, 0x8Fu, 0x20u, 0x00u, 0xBDu, 0x27u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0xFFu, 0xBDu, 0x27u, 0x11u, 0x80u, 0x03u, 0x3Cu,
    0x7Cu, 0x43u, 0x63u, 0x8Cu, 0x10u, 0x00u, 0x02u, 0x3Cu, 0x18u, 0x00u, 0xBFu, 0xAFu, 0x10u, 0x00u, 0xA2u, 0xAFu,
    0x00u, 0x00u, 0x62u, 0x8Cu, 0x00u, 0x01u, 0x03u, 0x3Cu, 0x24u, 0x10u, 0x43u, 0x00u, 0x17u, 0x00u, 0x40u, 0x10u,
    0x21u, 0x10u, 0x00u, 0x00u, 0xFFu, 0xFFu, 0x04u, 0x24u, 0x10u, 0x00u, 0xA2u, 0x8Fu, 0x00u, 0x00u, 0x00u, 0x00u,
    0xFFu, 0xFFu, 0x42u, 0x24u, 0x10u, 0x00u, 0xA2u, 0xAFu, 0x10u, 0x00u, 0xA2u, 0x8Fu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x06u, 0x00u, 0x44u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x80u, 0x04u, 0x3Cu, 0xDBu, 0x16u, 0x04u, 0x0Cu,
    0x08u, 0xDAu, 0x84u, 0x24u, 0xD1u, 0x16u, 0x04u, 0x08u, 0xFFu, 0xFFu, 0x02u, 0x24u, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x7Cu, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x24u, 0x10u, 0x43u, 0x00u, 0xECu, 0xFFu, 0x40u, 0x14u, 0x21u, 0x10u, 0x00u, 0x00u, 0x18u, 0x00u, 0xBFu, 0x8Fu,
    0x20u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x80u, 0x02u, 0x3Cu,
    0x9Cu, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x42u, 0x8Cu, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xE8u, 0xFFu, 0xBDu, 0x27u, 0x21u, 0x28u, 0x80u, 0x00u, 0x10u, 0x80u, 0x04u, 0x3Cu,
    0x18u, 0xDAu, 0x84u, 0x24u, 0x10u, 0x00u, 0xBFu, 0xAFu, 0x16u, 0xE9u, 0x01u, 0x0Cu, 0x00u, 0x00u, 0x00u, 0x00u,
    0x11u, 0x80u, 0x03u, 0x3Cu, 0x9Cu, 0x43u, 0x63u, 0x8Cu, 0x00u, 0x80u, 0x02u, 0x3Cu, 0x00u, 0x00u, 0x62u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x70u, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0xACu,
    0x11u, 0x80u, 0x02u, 0x3Cu, 0x7Cu, 0x43u, 0x42u, 0x8Cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0xACu,
    0x21u, 0x10u, 0x00u, 0x00u, 0x11u, 0x80u, 0x03u, 0x3Cu, 0x7Cu, 0x43u, 0x63u, 0x8Cu, 0x11u, 0x80u, 0x04u, 0x3Cu,
    0x9Cu, 0x43u, 0x84u, 0x8Cu, 0x00u, 0x00u, 0x63u, 0x8Cu, 0x00u, 0x60u, 0x03u, 0x3Cu, 0x00u, 0x00u, 0x83u, 0xACu,
    0x10u, 0x00u, 0xBFu, 0x8Fu, 0x18u, 0x00u, 0xBDu, 0x27u, 0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x80u, 0x08u, 0x3Cu, 0xA8u, 0x43u, 0x08u, 0x25u,
    0xFFu, 0xFFu, 0x81u, 0x20u, 0x04u, 0x00u, 0x20u, 0x18u, 0x00u, 0x00u, 0x02u, 0x8Du, 0x40u, 0x08u, 0x04u, 0x00u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x01u, 0xADu, 0xFFu, 0x00u, 0x01u, 0x3Cu, 0xFFu, 0xFFu, 0x21u, 0x34u,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x00u, 0x00u, 0x01u, 0xADu, 0x11u, 0x80u, 0x08u, 0x3Cu, 0xA8u, 0x43u, 0x08u, 0x25u,
    0x00u, 0x08u, 0xC6u, 0x20u, 0x01u, 0x00u, 0x01u, 0x3Cu, 0x20u, 0x38u, 0xC1u, 0x00u, 0x0Fu, 0x00u, 0x80u, 0x14u,
    0x00u, 0x00u, 0x09u, 0x8Du, 0x11u, 0x80u, 0x08u, 0x3Cu, 0xACu, 0x43u, 0x08u, 0x25u, 0x00u, 0x00u, 0x04u, 0x8Du,
    0x04u, 0x00u, 0x05u, 0x8Du, 0x08u, 0x00u, 0x02u, 0x8Du, 0x0Cu, 0x00u, 0x03u, 0x8Du, 0x10u, 0x00u, 0x0Cu, 0x8Du,
    0x14u, 0x00u, 0x0Du, 0x8Du, 0x18u, 0x00u, 0x0Fu, 0x8Du, 0x1Cu, 0x00u, 0x18u, 0x8Du, 0x20u, 0x00u, 0x19u, 0x8Du,
    0x20u, 0x48u, 0x29u, 0x01u, 0x64u, 0x00u, 0x00u, 0x10u, 0x20u, 0x70u, 0xA9u, 0x00u, 0x20u, 0x68u, 0x00u, 0x00u,
    0x20u, 0x78u, 0x00u, 0x00u, 0x20u, 0xC0u, 0x00u, 0x00u, 0x20u, 0xC8u, 0x00u, 0x00u, 0x20u, 0x48u, 0x29u, 0x01u,
    0x20u, 0x70u, 0xA9u, 0x00u, 0x00u, 0x00u, 0x89u, 0x8Cu, 0x04u, 0x00u, 0x8Cu, 0x94u, 0x06u, 0x00u, 0x8Au, 0x94u,
    0x08u, 0x00u, 0x82u, 0x94u, 0x0Au, 0x00u, 0x83u, 0x94u, 0xFDu, 0xFFu, 0x4Au, 0x21u, 0x02u, 0x00u, 0x40u, 0x05u,
    0x80u, 0x62u, 0x0Cu, 0x00u, 0x01u, 0x00u, 0x0Du, 0x20u, 0x0Cu, 0x00u, 0x84u, 0x20u, 0x00u, 0x14u, 0x02u, 0x00u,
    0x25u, 0x10u, 0x43u, 0x00u, 0x25u, 0x18u, 0x00u, 0x00u, 0x00u, 0x00u, 0xA9u, 0xACu, 0xFFu, 0xFFu, 0x29u, 0x31u,
    0x80u, 0x48u, 0x09u, 0x00u, 0x04u, 0x00u, 0x29u, 0x25u, 0x20u, 0x48u, 0x25u, 0x01u, 0x11u, 0x80u, 0x08u, 0x3Cu,
    0xD0u, 0x43u, 0x08u, 0x25u, 0x00u, 0x00u, 0x09u, 0xADu, 0x02u, 0x00u, 0xA5u, 0x20u, 0x35u, 0x00u, 0xA0u, 0x11u,
    0x82u, 0x45u, 0x02u, 0x00u, 0xFFu, 0x03u, 0x01u, 0x39u, 0x85u, 0x00u, 0x20u, 0x10u, 0x02u, 0x00u, 0xA5u, 0x20u,
    0xFDu, 0xFFu, 0xA1u, 0x21u, 0x02u, 0x00u, 0x20u, 0x04u, 0x00u, 0xFCu, 0xC1u, 0x20u, 0x00u, 0xFCu, 0x21u, 0x20u,
    0x02u, 0x46u, 0x02u, 0x00u, 0x80u, 0x40u, 0x08u, 0x00u, 0x20u, 0x40u, 0x01u, 0x01u, 0x00u, 0x00u, 0x09u, 0x95u,
    0x02u, 0x00u, 0x0Au, 0x95u, 0x24u, 0x40u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x40u, 0x11u, 0x04u, 0x10u, 0x22u, 0x01u,
    0x20u, 0x00u, 0x01u, 0x20u, 0x22u, 0x08u, 0x2Au, 0x00u, 0x06u, 0x40u, 0x22u, 0x00u, 0x04u, 0x00u, 0x40u, 0x04u,
    0x04u, 0x10u, 0x42u, 0x01u, 0xFFu, 0xFFu, 0x0Bu, 0x20u, 0x06u, 0x58u, 0x2Bu, 0x00u, 0x22u, 0x40u, 0x0Bu, 0x01u,
    0x20u, 0x18u, 0x6Au, 0x00u, 0x20u, 0x18u, 0x69u, 0x00u, 0x10u, 0x00u, 0x61u, 0x30u, 0x05u, 0x00u, 0x20u, 0x10u,
    0x0Fu, 0x00u, 0x63u, 0x30u, 0x00u, 0x00u, 0x89u, 0x94u, 0x02u, 0x00u, 0x84u, 0x20u, 0x04u, 0x48u, 0x69u, 0x00u,
    0x25u, 0x10u, 0x49u, 0x00u, 0xFEu, 0xFFu, 0xA1u, 0x21u, 0x08u, 0x00u, 0x20u, 0x1Cu, 0x20u, 0x48u, 0x28u, 0x03u,
    0x04u, 0x00u, 0x20u, 0x10u, 0x20u, 0x48u, 0x08u, 0x03u, 0x20u, 0x48u, 0xE8u, 0x01u, 0x04u, 0x00u, 0x00u, 0x10u,
    0x20u, 0x78u, 0xE8u, 0x01u, 0x02u, 0x00u, 0x00u, 0x10u, 0x20u, 0xC0u, 0x08u, 0x03u, 0x20u, 0xC8u, 0x28u, 0x03u,
    0x80u, 0x48u, 0x09u, 0x00u, 0xFFu, 0x03u, 0x29u, 0x31u, 0x25u, 0x48u, 0x89u, 0x01u, 0x01u, 0x00u, 0xADu, 0x21u,
    0xF9u, 0xFFu, 0xA1u, 0x21u, 0x11u, 0x00u, 0x20u, 0x14u, 0x00u, 0x00u, 0xA9u, 0xA4u, 0x0Fu, 0x00u, 0x00u, 0x10u,
    0xFAu, 0xFFu, 0xADu, 0x21u, 0xFFu, 0x01u, 0x01u, 0x39u, 0x51u, 0x00u, 0x20u, 0x10u, 0x02u, 0x00u, 0xA5u, 0x20u,
    0x80u, 0x12u, 0x02u, 0x00u, 0x0Au, 0x00u, 0x63u, 0x20u, 0x10u, 0x00u, 0x61u, 0x30u, 0x05u, 0x00u, 0x20u, 0x10u,
    0x0Fu, 0x00u, 0x63u, 0x30u, 0x00u, 0x00u, 0x89u, 0x94u, 0x02u, 0x00u, 0x84u, 0x20u, 0x04u, 0x48u, 0x69u, 0x00u,
    0x25u, 0x10u, 0x49u, 0x00u, 0x25u, 0x40u, 0x88u, 0x01u, 0x00u, 0x00u, 0xA8u, 0xA4u, 0x23u, 0x08u, 0xAEu, 0x00u,
    0x54u, 0x00u, 0x21u, 0x04u, 0x02u, 0x00u, 0xA5u, 0x20u, 0xC2u, 0x44u, 0x02u, 0x00u, 0xC0u, 0x40u, 0x08u, 0x00u,
    0x20u, 0x40u, 0x06u, 0x01u, 0x00u, 0x00u, 0x09u, 0x8Du, 0x00u, 0x00u, 0x00u, 0x00u, 0x11u, 0x00u, 0x20u, 0x15u,
    0xFFu, 0x00u, 0x21u, 0x31u, 0x00u, 0x12u, 0x02u, 0x00u, 0x08u, 0x00u, 0x63u, 0x20u, 0x10u, 0x00u, 0x61u, 0x30u,
    0x05u, 0x00u, 0x20u, 0x10u, 0x0Fu, 0x00u, 0x63u, 0x30u, 0x00u, 0x00u, 0x88u, 0x94u, 0x02u, 0x00u, 0x84u, 0x20u,
    0x04u, 0x40u, 0x68u, 0x00u, 0x25u, 0x10u, 0x48u, 0x00u, 0xC2u, 0x45u, 0x02u, 0x00u, 0x80u, 0x40u, 0x08u, 0x00u,
    0x20u, 0x40u, 0x07u, 0x01u, 0x00u, 0x00u, 0x09u, 0x8Du, 0x20u, 0x58u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x10u,
    0xFFu, 0x00u, 0x21u, 0x31u, 0x04u, 0x00u, 0x0Bu, 0x8Du, 0x04u, 0x10u, 0x22u, 0x00u, 0x20u, 0x18u, 0x61u, 0x00u,
    0x10u, 0x00u, 0x61u, 0x30u, 0x05u, 0x00u, 0x20u, 0x10u, 0x0Fu, 0x00u, 0x63u, 0x30u, 0x00u, 0x00u, 0x88u, 0x94u,
    0x02u, 0x00u, 0x84u, 0x20u, 0x04u, 0x40u, 0x68u, 0x00u, 0x25u, 0x10u, 0x48u, 0x00u, 0x02u, 0x4Cu, 0x09u, 0x00u,
    0x1Fu, 0x7Cu, 0x21u, 0x39u, 0x15u, 0x00u, 0x20u, 0x10u, 0x00u, 0xFEu, 0x21u, 0x39u, 0x93u, 0xFFu, 0x20u, 0x10u,
    0x00u, 0x00u, 0xA9u, 0xA4u, 0xD8u, 0xFFu, 0x60u, 0x11u, 0x02u, 0x00u, 0xA5u, 0x20u, 0xFFu, 0xFFu, 0x6Au, 0x31u,
    0x1Fu, 0x7Cu, 0x41u, 0x39u, 0x0Du, 0x00u, 0x20u, 0x10u, 0x00u, 0xFEu, 0x41u, 0x39u, 0x8Bu, 0xFFu, 0x20u, 0x10u,
    0x00u, 0x00u, 0xAAu, 0xA4u, 0x02u, 0x54u, 0x0Bu, 0x00u, 0xCFu, 0xFFu, 0x40u, 0x11u, 0x02u, 0x00u, 0xA5u, 0x20u,
    0x1Fu, 0x7Cu, 0x41u, 0x39u, 0x05u, 0x00u, 0x20u, 0x10u, 0x00u, 0xFEu, 0x41u, 0x39u, 0x83u, 0xFFu, 0x20u, 0x10u,
    0x00u, 0x00u, 0xAAu, 0xA4u, 0xC8u, 0xFFu, 0x00u, 0x10u, 0x02u, 0x00u, 0xA5u, 0x20u, 0x02u, 0x44u, 0x02u, 0x00u,
    0x00u, 0x00u, 0xA8u, 0xA4u, 0x02u, 0x00u, 0xA5u, 0x20u, 0x00u, 0x00u, 0x88u, 0x94u, 0x02u, 0x00u, 0x84u, 0x20u,
    0x00u, 0x14u, 0x02u, 0x00u, 0x04u, 0x40u, 0x68u, 0x00u, 0xBFu, 0xFFu, 0x00u, 0x10u, 0x25u, 0x10u, 0x48u, 0x00u,
    0x11u, 0x80u, 0x08u, 0x3Cu, 0xD0u, 0x43u, 0x08u, 0x25u, 0x00u, 0x00u, 0x09u, 0x8Du, 0x00u, 0xFEu, 0x08u, 0x34u,
    0x23u, 0x08u, 0xA9u, 0x00u, 0x04u, 0x00u, 0x21u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xA8u, 0xA4u,
    0xFBu, 0xFFu, 0x00u, 0x10u, 0x02u, 0x00u, 0xA5u, 0x20u, 0x00u, 0x60u, 0x09u, 0x40u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x02u, 0x00u, 0x01u, 0x3Cu, 0x25u, 0x48u, 0x21u, 0x01u, 0x00u, 0x60u, 0x89u, 0x40u, 0x08u, 0x00u, 0xE0u, 0x03u,
    0x20u, 0x10u, 0x00u, 0x00u, 0x11u, 0x80u, 0x08u, 0x3Cu, 0xACu, 0x43u, 0x08u, 0x25u, 0x00u, 0x00u, 0x04u, 0xADu,
    0x04u, 0x00u, 0x05u, 0xADu, 0x08u, 0x00u, 0x02u, 0xADu, 0x0Cu, 0x00u, 0x03u, 0xADu, 0x10u, 0x00u, 0x0Cu, 0xADu,
    0x14u, 0x00u, 0x0Du, 0xADu, 0x18u, 0x00u, 0x0Fu, 0xADu, 0x1Cu, 0x00u, 0x18u, 0xADu, 0x20u, 0x00u, 0x19u, 0xADu,
    0x08u, 0x00u, 0xE0u, 0x03u, 0x01u, 0x00u, 0x02u, 0x20u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0x01u, 0xE0u, 0x01u,
    0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0x01u, 0xF0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xE0u, 0x01u, 0x10u, 0x00u,
    0xF0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
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
static void func_80104F30(CPUState* cpu);
static void func_80104FA0(CPUState* cpu);
static void func_80105560(CPUState* cpu);
static void func_80105620(CPUState* cpu);
static void func_801056C4(CPUState* cpu);
static void func_80105740(CPUState* cpu);
static void func_80105760(CPUState* cpu);
static void func_8010579C(CPUState* cpu);
static void func_801057D8(CPUState* cpu);
static void func_801057FC(CPUState* cpu);
static void func_80105820(CPUState* cpu);
static void func_80105910(CPUState* cpu);
static void func_801059A0(CPUState* cpu);
static void func_80105A2C(CPUState* cpu);
static void func_80105AC0(CPUState* cpu);
static void func_80105B54(CPUState* cpu);
static void func_80105B6C(CPUState* cpu);
static void func_80105C20(CPUState* cpu);

static void func_80104F30(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80104F40u: goto block_80104F40;
            case 0x80104F50u: goto block_80104F50;
            case 0x80104F58u: goto block_80104F58;
            case 0x80104F68u: goto block_80104F68;
            case 0x80104F70u: goto block_80104F70;
            case 0x80104F80u: goto block_80104F80;
            case 0x80104F88u: goto block_80104F88;
            case 0x80104F90u: goto block_80104F90;
            case 0x80104F48u: goto block_80104F48;
            case 0x80104F30u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80104F30u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80104F30u);
    /* Address: 0x80104F30, Size: 112 bytes, Blocks: 10 */

block_80104F30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F30u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -48;  /* 0x80104F30: 0x27BDFFD0 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80104F34u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 40, cpu->gpr[31]);  /* 0x80104F34: 0xAFBF0028 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F40u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80104F3C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007EFC8u);
    cpu->pc = 0x8007EFC8u; return;  /* CPS jal -> 0x8007EFC8 */

block_80104F40:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F40u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F40u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_80104F40 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80104F44: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F44u);
#endif
    if (_bc_80104F40) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80104F90u);
        goto block_80104F90;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80104F48u);
        goto block_80104F48;  /* not taken */
    }

block_80104F48:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F48u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F48u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F50u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x80104F4C: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80029638u);
    cpu->pc = 0x80029638u; return;  /* CPS jal -> 0x80029638 */

block_80104F50:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F50u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F50u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F58u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000010u);
#endif
    cpu->gpr[4] = cpu->gpr[29] + 16;  /* 0x80104F54: 0x27A40010 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007EFD8u);
    cpu->pc = 0x8007EFD8u; return;  /* CPS jal -> 0x8007EFD8 */

block_80104F58:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F58u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F58u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F58u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 4, 0x20000000u);  /* 0x80104F58: 0x8FA40014 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F58u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 5, 0x20000000u);  /* 0x80104F5C: 0x8FA50018 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F68u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80104F64: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C650u);
    cpu->pc = 0x8007C650u; return;  /* CPS jal -> 0x8007C650 */

block_80104F68:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F68u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F68u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F70u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x80104F6C: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C3A4u);
    cpu->pc = 0x8007C3A4u; return;  /* CPS jal -> 0x8007C3A4 */

block_80104F70:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F70u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F70u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F70u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 4, 0x20000000u);  /* 0x80104F70: 0x8FA4001C */
#ifdef PSX_COSIM
    cosim_instr(0x80104F70u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[29] + 32, 5, 0x20000000u);  /* 0x80104F74: 0x8FA50020 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F80u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80104F7C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C650u);
    cpu->pc = 0x8007C650u; return;  /* CPS jal -> 0x8007C650 */

block_80104F80:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F80u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F80u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F88u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x80104F84: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007C3A4u);
    cpu->pc = 0x8007C3A4u; return;  /* CPS jal -> 0x8007C3A4 */

block_80104F88:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F88u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F88u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80104F90u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x80104F8C: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80029638u);
    cpu->pc = 0x80029638u; return;  /* CPS jal -> 0x80029638 */

block_80104F90:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104F90u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104F90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104F90u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104F90u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 40, 31, 0x20000000u);  /* 0x80104F90: 0x8FBF0028 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 48;  /* 0x80104F94: 0x27BD0030 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80104F98 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80104F9C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80104F9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80104F98);
    cpu->pc = _jt_80104F98; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80104FA0(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80104FE0u: goto block_80104FE0;
            case 0x80104FFCu: goto block_80104FFC;
            case 0x80105014u: goto block_80105014;
            case 0x80105030u: goto block_80105030;
            case 0x80105044u: goto block_80105044;
            case 0x80105050u: goto block_80105050;
            case 0x8010505Cu: goto block_8010505C;
            case 0x80105068u: goto block_80105068;
            case 0x80105094u: goto block_80105094;
            case 0x801050C8u: goto block_801050C8;
            case 0x801050D0u: goto block_801050D0;
            case 0x801050D8u: goto block_801050D8;
            case 0x8010520Cu: goto block_8010520C;
            case 0x80105214u: goto block_80105214;
            case 0x80104FA0u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80104FA0u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80104FA0u);
    /* Address: 0x80104FA0, Size: 672 bytes, Blocks: 15 */

block_80104FA0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104FA0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104FA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104FA0u, 16u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104FA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -32;  /* 0x80104FA0: 0x27BDFFE0 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x211u);
#endif
    cpu->gpr[9] = cpu->gpr[4];  /* move */  /* 0x80104FA4: 0x00804821 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x80104FA8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 4, cpu->gpr[17]);  /* 0x80104FA8: 0xAFB10004 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20021u);
#endif
    cpu->gpr[17] = cpu->gpr[5];  /* move */  /* 0x80104FAC: 0x00A08821 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104FB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20400000u);
#endif
    g_debug_last_store_pc = 0x80104FB0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[22]);  /* 0x80104FB0: 0xAFB60018 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x400001u);
#endif
    cpu->gpr[22] = 6184;  /* 0x80104FB4: 0x24161828 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20200000u);
#endif
    g_debug_last_store_pc = 0x80104FB8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[21]);  /* 0x80104FB8: 0xAFB50014 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200001u);
#endif
    cpu->gpr[21] = 42;  /* 0x80104FBC: 0x2415002A */
#ifdef PSX_COSIM
    cosim_instr(0x80104FBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104FC0u);
#endif
    cpu->gpr[14] = psx_cyc_load_byte(cpu, cpu->gpr[29] + 48, 14, 0x20000000u);  /* 0x80104FC0: 0x93AE0030 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80u);
#endif
    cpu->gpr[7] = cpu->gpr[7] << 16;  /* 0x80104FC4: 0x00073C00 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20800000u);
#endif
    g_debug_last_store_pc = 0x80104FC8u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 28, cpu->gpr[23]);  /* 0x80104FC8: 0xAFB7001C */
#ifdef PSX_COSIM
    cosim_instr(0x80104FC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20100000u);
#endif
    g_debug_last_store_pc = 0x80104FCCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[20]);  /* 0x80104FCC: 0xAFB40010 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104FD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20080000u);
#endif
    g_debug_last_store_pc = 0x80104FD0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 12, cpu->gpr[19]);  /* 0x80104FD0: 0xAFB3000C */
#ifdef PSX_COSIM
    cosim_instr(0x80104FD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20040000u);
#endif
    g_debug_last_store_pc = 0x80104FD4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 8, cpu->gpr[18]);  /* 0x80104FD4: 0xAFB20008 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x81u);
#endif
    int _bc_80104FD8 = (cpu->gpr[7] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x80104FDCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29], cpu->gpr[16]);  /* 0x80104FDC: 0xAFB00000 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FDCu);
#endif
    if (_bc_80104FD8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80104FFCu);
        goto block_80104FFC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80104FE0u);
        goto block_80104FE0;  /* not taken */
    }

block_80104FE0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104FE0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104FE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104FE0u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104FE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x801u);
#endif
    cpu->gpr[11] = 21;  /* 0x80104FE0: 0x240B0015 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000001u);
#endif
    cpu->gpr[25] = cpu->gpr[0];  /* move */  /* 0x80104FE4: 0x0000C821 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000001u);
#endif
    cpu->gpr[24] = 12;  /* 0x80104FE8: 0x2418000C */
#ifdef PSX_COSIM
    cosim_instr(0x80104FE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40001u);
#endif
    cpu->gpr[18] = 8;  /* 0x80104FEC: 0x24120008 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104FF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x80104FF0: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x80104FF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8004u);
#endif
    cpu->gpr[15] = cpu->gpr[2] + -18152;  /* 0x80104FF8: 0x244FB918 */
#ifdef PSX_COSIM
    cosim_instr(0x80104FF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105014u);
    goto block_80105014;  /* j */

block_80104FFC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80104FFCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80104FFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80104FFCu, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80104FFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x801u);
#endif
    cpu->gpr[11] = 14;  /* 0x80104FFC: 0x240B000E */
#ifdef PSX_COSIM
    cosim_instr(0x80104FFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105000u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000001u);
#endif
    cpu->gpr[25] = 36;  /* 0x80105000: 0x24190024 */
#ifdef PSX_COSIM
    cosim_instr(0x80105000u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000001u);
#endif
    cpu->gpr[24] = 18;  /* 0x80105004: 0x24180012 */
#ifdef PSX_COSIM
    cosim_instr(0x80105004u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40001u);
#endif
    cpu->gpr[18] = 12;  /* 0x80105008: 0x2412000C */
#ifdef PSX_COSIM
    cosim_instr(0x80105008u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800C << 16;  /* 0x800C0000 */  /* 0x8010500C: 0x3C02800C */
#ifdef PSX_COSIM
    cosim_instr(0x8010500Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105010u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8004u);
#endif
    cpu->gpr[15] = cpu->gpr[2] + -18088;  /* 0x80105010: 0x244FB958 */
#ifdef PSX_COSIM
    cosim_instr(0x80105010u);
#endif
    /* fall through to block_80105014 */

block_80105014:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105014u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105014u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105014u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105014u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80001u);
#endif
    cpu->gpr[19] = 12;  /* 0x80105014: 0x2413000C */
#ifdef PSX_COSIM
    cosim_instr(0x80105014u);
#endif
    cpu->gpr[13] = psx_cyc_load_word(cpu, cpu->gpr[29] + 52, 13, 0x20000000u);  /* 0x80105018: 0x8FAD0034 */
#ifdef PSX_COSIM
    cosim_instr(0x80105018u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = 0x8018 << 16;  /* 0x80180000 */  /* 0x8010501C: 0x3C068018 */
#ifdef PSX_COSIM
    cosim_instr(0x8010501Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105020u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x30000u);
#endif
    cpu->gpr[16] = cpu->gpr[17] + 12;  /* 0x80105020: 0x2630000C */
#ifdef PSX_COSIM
    cosim_instr(0x80105020u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000u);
#endif
    cpu->gpr[12] = 0x00FF << 16;  /* 0x00FF0000 */  /* 0x80105024: 0x3C0C00FF */
#ifdef PSX_COSIM
    cosim_instr(0x80105024u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000u);
#endif
    cpu->gpr[12] = cpu->gpr[12] | 0xFFFF;  /* 0x80105028: 0x358CFFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105028u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100000u);
#endif
    cpu->gpr[20] = 0xFF00 << 16;  /* 0xFF000000 */  /* 0x8010502C: 0x3C14FF00 */
#ifdef PSX_COSIM
    cosim_instr(0x8010502Cu);
#endif
    /* fall through to block_80105030 */

block_80105030:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105030u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105030u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105030u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105030u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[13], 2, 0x2000u);  /* 0x80105030: 0x91A20000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105030u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105034: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105034u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[4] = cpu->gpr[2] & 0x00FF;  /* 0x80105038: 0x304400FF */
#ifdef PSX_COSIM
    cosim_instr(0x80105038u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    int _bc_8010503C = (cpu->gpr[4] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105040u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000u);
#endif
    cpu->gpr[13] = cpu->gpr[13] + 1;  /* 0x80105040: 0x25AD0001 */
#ifdef PSX_COSIM
    cosim_instr(0x80105040u);
#endif
    if (_bc_8010503C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105214u);
        goto block_80105214;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105044u);
        goto block_80105044;  /* not taken */
    }

block_80105044:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105044u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105044u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105044u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105044u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = (cpu->gpr[4] < (uint32_t)49) ? 1 : 0;  /* 0x80105044: 0x2C820031 */
#ifdef PSX_COSIM
    cosim_instr(0x80105044u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_80105048 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 1;  /* 0x8010504C: 0x24050001 */
#ifdef PSX_COSIM
    cosim_instr(0x8010504Cu);
#endif
    if (_bc_80105048) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105094u);
        goto block_80105094;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105050u);
        goto block_80105050;  /* not taken */
    }

block_80105050:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105050u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105050u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105050u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105050u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 32;  /* 0x80105050: 0x24020020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105050u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    int _bc_80105054 = (cpu->gpr[4] != cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[4] + -33;  /* 0x80105058: 0x2482FFDF */
#ifdef PSX_COSIM
    cosim_instr(0x80105058u);
#endif
    if (_bc_80105054) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105068u);
        goto block_80105068;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x8010505Cu);
        goto block_8010505C;  /* not taken */
    }

block_8010505C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010505Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010505Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010505Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010505Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = cpu->gpr[0];  /* move */  /* 0x8010505C: 0x00002821 */
#ifdef PSX_COSIM
    cosim_instr(0x8010505Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105060u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40081u);
#endif
    cpu->gpr[7] = cpu->gpr[18];  /* move */  /* 0x80105064: 0x02403821 */
#ifdef PSX_COSIM
    cosim_instr(0x80105064u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801050D0u);
    goto block_801050D0;  /* j */

block_80105068:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105068u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105068u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105068u, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105068u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x804u);
#endif
    if (cpu->gpr[11] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[2] / (int32_t)cpu->gpr[11]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[2] % (int32_t)cpu->gpr[11]); } else { cpu->lo = ((int32_t)cpu->gpr[2] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[2]; }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, 37u);
#endif  /* 0x80105068: 0x004B001A */
#ifdef PSX_COSIM
    cosim_instr(0x80105068u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x8010506C: 0x00001012 */
#ifdef PSX_COSIM
    cosim_instr(0x8010506Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105070u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->hi;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x80105070: 0x00001810 */
#ifdef PSX_COSIM
    cosim_instr(0x80105070u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105074: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105074u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105078: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105078u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80004u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[19]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
#endif  /* 0x8010507C: 0x00530018 */
#ifdef PSX_COSIM
    cosim_instr(0x8010507Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105080u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x80105080: 0x00004012 */
#ifdef PSX_COSIM
    cosim_instr(0x80105080u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8014u);
#endif
    cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[15];  /* 0x80105084: 0x008F1021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105084u);
#endif
    cpu->gpr[7] = psx_cyc_load_byte(cpu, cpu->gpr[2] + -33, 7, 0x4u);  /* 0x80105088: 0x9047FFDF */
#ifdef PSX_COSIM
    cosim_instr(0x80105088u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105090u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000008u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[24]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
#endif  /* 0x80105090: 0x00780018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105090u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801050C8u);
    goto block_801050C8;  /* j */

block_80105094:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105094u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105094u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105094u, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105094u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[4] + -34;  /* 0x80105094: 0x2482FFDE */
#ifdef PSX_COSIM
    cosim_instr(0x80105094u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x804u);
#endif
    if (cpu->gpr[11] != 0) { cpu->lo = (uint32_t)((int32_t)cpu->gpr[2] / (int32_t)cpu->gpr[11]); cpu->hi = (uint32_t)((int32_t)cpu->gpr[2] % (int32_t)cpu->gpr[11]); } else { cpu->lo = ((int32_t)cpu->gpr[2] >= 0) ? 0xFFFFFFFF : 1; cpu->hi = (uint32_t)cpu->gpr[2]; }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, 37u);
#endif  /* 0x80105098: 0x004B001A */
#ifdef PSX_COSIM
    cosim_instr(0x80105098u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x8010509C: 0x00001012 */
#ifdef PSX_COSIM
    cosim_instr(0x8010509Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->hi;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x801050A0: 0x00001810 */
#ifdef PSX_COSIM
    cosim_instr(0x801050A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801050A4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801050A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801050A8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801050A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80004u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[2] * (int64_t)(int32_t)cpu->gpr[19]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[2]));
#endif  /* 0x801050AC: 0x00530018 */
#ifdef PSX_COSIM
    cosim_instr(0x801050ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x801050B0: 0x00004012 */
#ifdef PSX_COSIM
    cosim_instr(0x801050B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801050B4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801050B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801050B8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801050B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000008u);
#endif
    { int64_t result = (int64_t)(int32_t)cpu->gpr[3] * (int64_t)(int32_t)cpu->gpr[24]; cpu->lo = (uint32_t)result; cpu->hi = (uint32_t)(result >> 32); }
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_set(cpu, psx_mult_latency_s(cpu->gpr[3]));
#endif  /* 0x801050BC: 0x00780018 */
#ifdef PSX_COSIM
    cosim_instr(0x801050BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8014u);
#endif
    cpu->gpr[2] = cpu->gpr[4] + cpu->gpr[15];  /* 0x801050C0: 0x008F1021 */
#ifdef PSX_COSIM
    cosim_instr(0x801050C0u);
#endif
    cpu->gpr[7] = psx_cyc_load_byte(cpu, cpu->gpr[2] + -34, 7, 0x4u);  /* 0x801050C4: 0x9047FFDE */
#ifdef PSX_COSIM
    cosim_instr(0x801050C4u);
#endif
    /* fall through to block_801050C8 */

block_801050C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801050C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801050C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801050C8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[25];  /* 0x801050C8: 0x01194021 */
#ifdef PSX_COSIM
    cosim_instr(0x801050C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x400u);
#endif
    cpu->gpr[10] = cpu->lo;
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_muldiv_stall(cpu);
#endif  /* 0x801050CC: 0x00005012 */
#ifdef PSX_COSIM
    cosim_instr(0x801050CCu);
#endif
    /* fall through to block_801050D0 */

block_801050D0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801050D0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801050D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801050D0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    int _bc_801050D0 = (cpu->gpr[5] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 9;  /* 0x801050D4: 0x24020009 */
#ifdef PSX_COSIM
    cosim_instr(0x801050D4u);
#endif
    if (_bc_801050D0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x8010520Cu);
        goto block_8010520C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x801050D8u);
        goto block_801050D8;  /* not taken */
    }

block_801050D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801050D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801050D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801050D8u, 77u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050D8u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 3, 0x40u);  /* 0x801050D8: 0x8CC34DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x801050D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801050DC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801050DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x801050E0u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[3] + 3, (uint8_t)cpu->gpr[2]);  /* 0x801050E0: 0xA0620003 */
#ifdef PSX_COSIM
    cosim_instr(0x801050E0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 3, 0x40u);  /* 0x801050E4: 0x8CC34DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x801050E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 44;  /* 0x801050E8: 0x2402002C */
#ifdef PSX_COSIM
    cosim_instr(0x801050E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x801050ECu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[3] + 7, (uint8_t)cpu->gpr[2]);  /* 0x801050EC: 0xA0620007 */
#ifdef PSX_COSIM
    cosim_instr(0x801050ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801050F0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 3, 0x40u);  /* 0x801050F0: 0x8CC34DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x801050F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801050F4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801050F4u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[3] + 7, 2, 0x8u);  /* 0x801050F8: 0x90620007 */
#ifdef PSX_COSIM
    cosim_instr(0x801050F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801050FC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801050FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105100u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x00FE;  /* 0x80105100: 0x304200FE */
#ifdef PSX_COSIM
    cosim_instr(0x80105100u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105104u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[3] + 7, (uint8_t)cpu->gpr[2]);  /* 0x80105104: 0xA0620007 */
#ifdef PSX_COSIM
    cosim_instr(0x80105104u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 3, 0x40u);  /* 0x80105108: 0x8CC34DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105108u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010510C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010510Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105110u);
#endif
    cpu->gpr[2] = psx_cyc_load_byte(cpu, cpu->gpr[3] + 7, 2, 0x8u);  /* 0x80105110: 0x90620007 */
#ifdef PSX_COSIM
    cosim_instr(0x80105110u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105114: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105114u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | 0x0002;  /* 0x80105118: 0x34420002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105118u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x8010511Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[3] + 7, (uint8_t)cpu->gpr[2]);  /* 0x8010511C: 0xA0620007 */
#ifdef PSX_COSIM
    cosim_instr(0x8010511Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105120u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x80105120: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105120u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105124: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105124u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4004u);
#endif
    g_debug_last_store_pc = 0x80105128u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 4, (uint8_t)cpu->gpr[14]);  /* 0x80105128: 0xA04E0004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105128u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x8010512C: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x8010512Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105130u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105130: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105130u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4004u);
#endif
    g_debug_last_store_pc = 0x80105134u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 5, (uint8_t)cpu->gpr[14]);  /* 0x80105134: 0xA04E0005 */
#ifdef PSX_COSIM
    cosim_instr(0x80105134u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x80105138: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105138u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010513C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010513Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105140u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4004u);
#endif
    g_debug_last_store_pc = 0x80105140u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 6, (uint8_t)cpu->gpr[14]);  /* 0x80105140: 0xA04E0006 */
#ifdef PSX_COSIM
    cosim_instr(0x80105140u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x80105144: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105144u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105148: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105148u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x404u);
#endif
    g_debug_last_store_pc = 0x8010514Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 12, (uint8_t)cpu->gpr[10]);  /* 0x8010514C: 0xA04A000C */
#ifdef PSX_COSIM
    cosim_instr(0x8010514Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105150u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 3, 0x40u);  /* 0x80105150: 0x8CC34DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105150u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200004u);
#endif
    g_debug_last_store_pc = 0x80105154u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[2] + 22, (uint16_t)cpu->gpr[21]);  /* 0x80105154: 0xA4550016 */
#ifdef PSX_COSIM
    cosim_instr(0x80105154u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x400004u);
#endif
    g_debug_last_store_pc = 0x80105158u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[2] + 14, (uint16_t)cpu->gpr[22]);  /* 0x80105158: 0xA456000E */
#ifdef PSX_COSIM
    cosim_instr(0x80105158u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x108u);
#endif
    g_debug_last_store_pc = 0x8010515Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[3] + 13, (uint8_t)cpu->gpr[8]);  /* 0x8010515C: 0xA068000D */
#ifdef PSX_COSIM
    cosim_instr(0x8010515Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105160u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x80105160: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105160u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x490u);
#endif
    cpu->gpr[4] = cpu->gpr[10] + cpu->gpr[7];  /* 0x80105164: 0x01472021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105164u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x80105168u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 20, (uint8_t)cpu->gpr[4]);  /* 0x80105168: 0xA0440014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105168u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x8010516C: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x8010516Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105170u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105170: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105170u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    g_debug_last_store_pc = 0x80105174u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 21, (uint8_t)cpu->gpr[8]);  /* 0x80105174: 0xA0480015 */
#ifdef PSX_COSIM
    cosim_instr(0x80105174u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x80105178: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105178u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010517C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010517Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105180u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x404u);
#endif
    g_debug_last_store_pc = 0x80105180u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 28, (uint8_t)cpu->gpr[10]);  /* 0x80105180: 0xA04A001C */
#ifdef PSX_COSIM
    cosim_instr(0x80105180u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x80105184: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105184u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80108u);
#endif
    cpu->gpr[3] = cpu->gpr[8] + cpu->gpr[19];  /* 0x80105188: 0x01131821 */
#ifdef PSX_COSIM
    cosim_instr(0x80105188u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x8010518Cu; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 29, (uint8_t)cpu->gpr[3]);  /* 0x8010518C: 0xA043001D */
#ifdef PSX_COSIM
    cosim_instr(0x8010518Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105190u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x80105190: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105190u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105194: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105194u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x80105198u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 36, (uint8_t)cpu->gpr[4]);  /* 0x80105198: 0xA0440024 */
#ifdef PSX_COSIM
    cosim_instr(0x80105198u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 2, 0x40u);  /* 0x8010519C: 0x8CC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x8010519Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801051A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801051A0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801051A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x801051A4u; psx_store_cycle_barrier(); cpu->write_byte(cpu->gpr[2] + 37, (uint8_t)cpu->gpr[3]);  /* 0x801051A4: 0xA0430025 */
#ifdef PSX_COSIM
    cosim_instr(0x801051A4u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[6] + 19928, 4, 0x40u);  /* 0x801051A8: 0x8CC44DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x801051A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x284u);
#endif
    cpu->gpr[2] = cpu->gpr[9] + cpu->gpr[7];  /* 0x801051AC: 0x01271021 */
#ifdef PSX_COSIM
    cosim_instr(0x801051ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801051B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x801051B0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 16, (uint16_t)cpu->gpr[2]);  /* 0x801051B0: 0xA4820010 */
#ifdef PSX_COSIM
    cosim_instr(0x801051B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x801051B4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 32, (uint16_t)cpu->gpr[2]);  /* 0x801051B4: 0xA4820020 */
#ifdef PSX_COSIM
    cosim_instr(0x801051B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x800B << 16;  /* 0x800B0000 */  /* 0x801051B8: 0x3C02800B */
#ifdef PSX_COSIM
    cosim_instr(0x801051B8u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[2] + -28136, 5, 0x4u);  /* 0x801051BC: 0x8C459218 */
#ifdef PSX_COSIM
    cosim_instr(0x801051BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801051C0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[4], 3, 0x10u);  /* 0x801051C0: 0x8C830000 */
#ifdef PSX_COSIM
    cosim_instr(0x801051C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x210u);
#endif
    g_debug_last_store_pc = 0x801051C4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 8, (uint16_t)cpu->gpr[9]);  /* 0x801051C4: 0xA4890008 */
#ifdef PSX_COSIM
    cosim_instr(0x801051C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010u);
#endif
    g_debug_last_store_pc = 0x801051C8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 10, (uint16_t)cpu->gpr[17]);  /* 0x801051C8: 0xA491000A */
#ifdef PSX_COSIM
    cosim_instr(0x801051C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010u);
#endif
    g_debug_last_store_pc = 0x801051CCu; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 18, (uint16_t)cpu->gpr[17]);  /* 0x801051CC: 0xA4910012 */
#ifdef PSX_COSIM
    cosim_instr(0x801051CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801051D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x210u);
#endif
    g_debug_last_store_pc = 0x801051D0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 24, (uint16_t)cpu->gpr[9]);  /* 0x801051D0: 0xA4890018 */
#ifdef PSX_COSIM
    cosim_instr(0x801051D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10010u);
#endif
    g_debug_last_store_pc = 0x801051D4u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 26, (uint16_t)cpu->gpr[16]);  /* 0x801051D4: 0xA490001A */
#ifdef PSX_COSIM
    cosim_instr(0x801051D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10010u);
#endif
    g_debug_last_store_pc = 0x801051D8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[4] + 34, (uint16_t)cpu->gpr[16]);  /* 0x801051D8: 0xA4900022 */
#ifdef PSX_COSIM
    cosim_instr(0x801051D8u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[5] + 24, 2, 0x20u);  /* 0x801051DC: 0x8CA20018 */
#ifdef PSX_COSIM
    cosim_instr(0x801051DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801051E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100008u);
#endif
    cpu->gpr[3] = cpu->gpr[3] & cpu->gpr[20];  /* 0x801051E0: 0x00741824 */
#ifdef PSX_COSIM
    cosim_instr(0x801051E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1004u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[12];  /* 0x801051E4: 0x004C1024 */
#ifdef PSX_COSIM
    cosim_instr(0x801051E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[3] = cpu->gpr[3] | cpu->gpr[2];  /* 0x801051E8: 0x00621825 */
#ifdef PSX_COSIM
    cosim_instr(0x801051E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    cpu->gpr[2] = cpu->gpr[4] + 40;  /* 0x801051EC: 0x24820028 */
#ifdef PSX_COSIM
    cosim_instr(0x801051ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801051F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    g_debug_last_store_pc = 0x801051F0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[4], cpu->gpr[3]);  /* 0x801051F0: 0xAC830000 */
#ifdef PSX_COSIM
    cosim_instr(0x801051F0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[5] + 24, 3, 0x20u);  /* 0x801051F4: 0x8CA30018 */
#ifdef PSX_COSIM
    cosim_instr(0x801051F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1010u);
#endif
    cpu->gpr[4] = cpu->gpr[4] & cpu->gpr[12];  /* 0x801051F8: 0x008C2024 */
#ifdef PSX_COSIM
    cosim_instr(0x801051F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x44u);
#endif
    g_debug_last_store_pc = 0x801051FCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[6] + 19928, cpu->gpr[2]);  /* 0x801051FC: 0xACC24DD8 */
#ifdef PSX_COSIM
    cosim_instr(0x801051FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105200u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100008u);
#endif
    cpu->gpr[3] = cpu->gpr[3] & cpu->gpr[20];  /* 0x80105200: 0x00741824 */
#ifdef PSX_COSIM
    cosim_instr(0x80105200u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    cpu->gpr[3] = cpu->gpr[3] | cpu->gpr[4];  /* 0x80105204: 0x00641825 */
#ifdef PSX_COSIM
    cosim_instr(0x80105204u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x28u);
#endif
    g_debug_last_store_pc = 0x80105208u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[5] + 24, cpu->gpr[3]);  /* 0x80105208: 0xACA30018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105208u);
#endif
    /* fall through to block_8010520C */

block_8010520C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010520Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010520Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010520Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010520Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105210u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x280u);
#endif
    cpu->gpr[9] = cpu->gpr[9] + cpu->gpr[7];  /* 0x80105210: 0x01274821 */
#ifdef PSX_COSIM
    cosim_instr(0x80105210u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105030u);
    goto block_80105030;  /* j */

block_80105214:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105214u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105214u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105214u, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105214u);
#endif
    cpu->gpr[23] = psx_cyc_load_word(cpu, cpu->gpr[29] + 28, 23, 0x20000000u);  /* 0x80105214: 0x8FB7001C */
#ifdef PSX_COSIM
    cosim_instr(0x80105214u);
#endif
    cpu->gpr[22] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 22, 0x20000000u);  /* 0x80105218: 0x8FB60018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105218u);
#endif
    cpu->gpr[21] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 21, 0x20000000u);  /* 0x8010521C: 0x8FB50014 */
#ifdef PSX_COSIM
    cosim_instr(0x8010521Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105220u);
#endif
    cpu->gpr[20] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 20, 0x20000000u);  /* 0x80105220: 0x8FB40010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105220u);
#endif
    cpu->gpr[19] = psx_cyc_load_word(cpu, cpu->gpr[29] + 12, 19, 0x20000000u);  /* 0x80105224: 0x8FB3000C */
#ifdef PSX_COSIM
    cosim_instr(0x80105224u);
#endif
    cpu->gpr[18] = psx_cyc_load_word(cpu, cpu->gpr[29] + 8, 18, 0x20000000u);  /* 0x80105228: 0x8FB20008 */
#ifdef PSX_COSIM
    cosim_instr(0x80105228u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 4, 17, 0x20000000u);  /* 0x8010522C: 0x8FB10004 */
#ifdef PSX_COSIM
    cosim_instr(0x8010522Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105230u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29], 16, 0x20000000u);  /* 0x80105230: 0x8FB00000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105230u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x80105234: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105234u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105238 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010523C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010523Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105238);
    cpu->pc = _jt_80105238; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105560(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8010557Cu: goto block_8010557C;
            case 0x80105584u: goto block_80105584;
            case 0x80105574u: goto block_80105574;
            case 0x80105560u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105560u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105560u);
    /* Address: 0x80105560, Size: 52 bytes, Blocks: 4 */

block_80105560:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105560u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105560u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105560u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105560u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x80105560: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105560u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x80105564u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);  /* 0x80105564: 0xAFB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105564u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[16] = cpu->gpr[4];  /* move */  /* 0x80105568: 0x00808021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105568u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10001u);
#endif
    int _bc_8010556C = (cpu->gpr[16] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105570u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105570u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);  /* 0x80105570: 0xAFBF0014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105570u);
#endif
    if (_bc_8010556C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x8010557Cu);
        goto block_8010557C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105574u);
        goto block_80105574;  /* not taken */
    }

block_80105574:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105574u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105574u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105574u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105574u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x8010557Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105578: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105578u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80085BC8u);
    cpu->pc = 0x80085BC8u; return;  /* CPS jal -> 0x80085BC8 */

block_8010557C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010557Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010557Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010557Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010557Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105584u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105580u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[4] = cpu->gpr[16];  /* move */  /* 0x80105580: 0x02002021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105580u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105820u);
    cpu->pc = 0x80105820u; return;  /* CPS jal -> 0x80105820 */

block_80105584:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105584u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105584u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105584u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105584u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 31, 0x20000000u);  /* 0x80105584: 0x8FBF0014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105584u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 16, 0x20000000u);  /* 0x80105588: 0x8FB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105588u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_8010558C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105590u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x80105590: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105590u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_8010558C);
    cpu->pc = _jt_8010558C; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105620(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105694u: goto block_80105694;
            case 0x801056A4u: goto block_801056A4;
            case 0x80105640u: goto block_80105640;
            case 0x80105658u: goto block_80105658;
            case 0x8010566Cu: goto block_8010566C;
            case 0x80105684u: goto block_80105684;
            case 0x80105620u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105620u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105620u);
    /* Address: 0x80105620, Size: 152 bytes, Blocks: 7 */

block_80105620:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105620u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105620u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105620u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105620u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x80105620: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105620u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x80105624u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);  /* 0x80105624: 0xAFB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105624u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10011u);
#endif
    cpu->gpr[16] = cpu->gpr[4];  /* move */  /* 0x80105628: 0x00808021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105628u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x8011 << 16;  /* 0x80110000 */  /* 0x8010562C: 0x3C058011 */
#ifdef PSX_COSIM
    cosim_instr(0x8010562Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105630u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 16988;  /* 0x80105630: 0x24A5425C */
#ifdef PSX_COSIM
    cosim_instr(0x80105630u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = 15;  /* 0x80105634: 0x2403000F */
#ifdef PSX_COSIM
    cosim_instr(0x80105634u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x41u);
#endif
    cpu->gpr[6] = -1;  /* 0x80105638: 0x2406FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105638u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x8010563Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[31]);  /* 0x8010563C: 0xAFBF0014 */
#ifdef PSX_COSIM
    cosim_instr(0x8010563Cu);
#endif
    /* fall through to block_80105640 */

block_80105640:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105640u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105640u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105640u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105640u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x80105640: 0x8C820000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105640u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 4;  /* 0x80105644: 0x24840004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105644u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + -1;  /* 0x80105648: 0x2463FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105648u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x8010564Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[5], cpu->gpr[2]);  /* 0x8010564C: 0xACA20000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010564Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105650u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x48u);
#endif
    int _bc_80105650 = (cpu->gpr[3] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 4;  /* 0x80105654: 0x24A50004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105654u);
#endif
    if (_bc_80105650) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105640u);
        goto block_80105640;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105658u);
        goto block_80105658;  /* not taken */
    }

block_80105658:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105658u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105658u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105658u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105658u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105658: 0x3C058011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105658u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 17052;  /* 0x8010565C: 0x24A5429C */
#ifdef PSX_COSIM
    cosim_instr(0x8010565Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105660u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10010u);
#endif
    cpu->gpr[4] = cpu->gpr[16] + 64;  /* 0x80105660: 0x26040040 */
#ifdef PSX_COSIM
    cosim_instr(0x80105660u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = 15;  /* 0x80105664: 0x2403000F */
#ifdef PSX_COSIM
    cosim_instr(0x80105664u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x41u);
#endif
    cpu->gpr[6] = -1;  /* 0x80105668: 0x2406FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105668u);
#endif
    /* fall through to block_8010566C */

block_8010566C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010566Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010566Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010566Cu, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010566Cu);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x8010566C: 0x8C820000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010566Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105670u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 4;  /* 0x80105670: 0x24840004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105670u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + -1;  /* 0x80105674: 0x2463FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105674u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    g_debug_last_store_pc = 0x80105678u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[5], cpu->gpr[2]);  /* 0x80105678: 0xACA20000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105678u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x48u);
#endif
    int _bc_8010567C = (cpu->gpr[3] != cpu->gpr[6]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105680u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 4;  /* 0x80105680: 0x24A50004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105680u);
#endif
    if (_bc_8010567C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x8010566Cu);
        goto block_8010566C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105684u);
        goto block_80105684;  /* not taken */
    }

block_80105684:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105684u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105684u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105684u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105684u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105684: 0x3C048011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105684u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 16984;  /* 0x80105688: 0x24844258 */
#ifdef PSX_COSIM
    cosim_instr(0x80105688u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105694u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105690u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 32;  /* 0x80105690: 0x24050020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105690u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105910u);
    cpu->pc = 0x80105910u; return;  /* CPS jal -> 0x80105910 */

block_80105694:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105694u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105694u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105694u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105694u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105694: 0x3C048011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105694u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 17116;  /* 0x80105698: 0x248442DC */
#ifdef PSX_COSIM
    cosim_instr(0x80105698u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x801056A4u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 32;  /* 0x801056A0: 0x24050020 */
#ifdef PSX_COSIM
    cosim_instr(0x801056A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105910u);
    cpu->pc = 0x80105910u; return;  /* CPS jal -> 0x80105910 */

block_801056A4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801056A4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801056A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801056A4u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10005u);
#endif
    cpu->gpr[2] = cpu->gpr[16];  /* move */  /* 0x801056A4: 0x02001021 */
#ifdef PSX_COSIM
    cosim_instr(0x801056A4u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 31, 0x20000000u);  /* 0x801056A8: 0x8FBF0014 */
#ifdef PSX_COSIM
    cosim_instr(0x801056A8u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 16, 0x20000000u);  /* 0x801056AC: 0x8FB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x801056ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_801056B0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x801056B4: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x801056B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_801056B0);
    cpu->pc = _jt_801056B0; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_801056C4(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105730u: goto block_80105730;
            case 0x801056D4u: goto block_801056D4;
            case 0x801056E8u: goto block_801056E8;
            case 0x801056F4u: goto block_801056F4;
            case 0x80105704u: goto block_80105704;
            case 0x80105710u: goto block_80105710;
            case 0x80105720u: goto block_80105720;
            case 0x801056C4u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x801056C4u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x801056C4u);
    /* Address: 0x801056C4, Size: 124 bytes, Blocks: 8 */

block_801056C4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801056C4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801056C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801056C4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x801056C4: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x801056C4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = cpu->gpr[5] & 0x0001;  /* 0x801056C8: 0x30A20001 */
#ifdef PSX_COSIM
    cosim_instr(0x801056C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_801056CC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x801056D0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x801056D0: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x801056D0u);
#endif
    if (_bc_801056CC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x801056E8u);
        goto block_801056E8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x801056D4u);
        goto block_801056D4;  /* not taken */
    }

block_801056D4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801056D4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801056D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801056D4u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0xF7FF << 16;  /* 0xF7FF0000 */  /* 0x801056D4: 0x3C03F7FF */
#ifdef PSX_COSIM
    cosim_instr(0x801056D4u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x801056D8: 0x8C820000 */
#ifdef PSX_COSIM
    cosim_instr(0x801056D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] | 0xFFFF;  /* 0x801056DC: 0x3463FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x801056DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x801056E4: 0x00431024 */
#ifdef PSX_COSIM
    cosim_instr(0x801056E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801056F4u);
    goto block_801056F4;  /* j */

block_801056E8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801056E8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801056E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801056E8u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056E8u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x801056E8: 0x8C820000 */
#ifdef PSX_COSIM
    cosim_instr(0x801056E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x0800 << 16;  /* 0x08000000 */  /* 0x801056EC: 0x3C030800 */
#ifdef PSX_COSIM
    cosim_instr(0x801056ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];  /* 0x801056F0: 0x00431025 */
#ifdef PSX_COSIM
    cosim_instr(0x801056F0u);
#endif
    /* fall through to block_801056F4 */

block_801056F4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801056F4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801056F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801056F4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801056F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x801056F4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[4], cpu->gpr[2]);  /* 0x801056F4: 0xAC820000 */
#ifdef PSX_COSIM
    cosim_instr(0x801056F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    cpu->gpr[2] = cpu->gpr[5] & 0x0002;  /* 0x801056F8: 0x30A20002 */
#ifdef PSX_COSIM
    cosim_instr(0x801056F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_801056FC = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105700u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x0200 << 16;  /* 0x02000000 */  /* 0x80105700: 0x3C030200 */
#ifdef PSX_COSIM
    cosim_instr(0x80105700u);
#endif
    if (_bc_801056FC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105710u);
        goto block_80105710;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105704u);
        goto block_80105704;  /* not taken */
    }

block_80105704:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105704u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105704u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105704u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105704u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x80105704: 0x8C820000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105704u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];  /* 0x8010570C: 0x00431025 */
#ifdef PSX_COSIM
    cosim_instr(0x8010570Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105720u);
    goto block_80105720;  /* j */

block_80105710:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105710u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105710u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105710u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105710u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0xFDFF << 16;  /* 0xFDFF0000 */  /* 0x80105710: 0x3C03FDFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105710u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[4], 2, 0x10u);  /* 0x80105714: 0x8C820000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105714u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] | 0xFFFF;  /* 0x80105718: 0x3463FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105718u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x8010571C: 0x00431024 */
#ifdef PSX_COSIM
    cosim_instr(0x8010571Cu);
#endif
    /* fall through to block_80105720 */

block_80105720:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105720u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105720u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105720u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105720u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x80105720u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[4], cpu->gpr[2]);  /* 0x80105720: 0xAC820000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105720u);
#endif
    cpu->gpr[5] = psx_cyc_load_half(cpu, cpu->gpr[4], 5, 0x10u);  /* 0x80105724: 0x94850000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105724u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105730u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010572C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010572Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105910u);
    cpu->pc = 0x80105910u; return;  /* CPS jal -> 0x80105910 */

block_80105730:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105730u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105730u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105730u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105730u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x80105730: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105730u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x80105734: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105734u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105738 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010573C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010573Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105738);
    cpu->pc = _jt_80105738; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105740(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105750u: goto block_80105750;
            case 0x80105740u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105740u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105740u);
    /* Address: 0x80105740, Size: 32 bytes, Blocks: 2 */

block_80105740:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105740u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105740u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x80105740: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105740u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105744u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x80105744: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105744u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105750u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010574C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010574Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801059A0u);
    cpu->pc = 0x801059A0u; return;  /* CPS jal -> 0x801059A0 */

block_80105750:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105750u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105750u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105750u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105750u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x80105750: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105750u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x80105754: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105754u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105758 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010575C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010575Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105758);
    cpu->pc = _jt_80105758; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105760(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105774u: goto block_80105774;
            case 0x80105784u: goto block_80105784;
            case 0x8010576Cu: goto block_8010576C;
            case 0x8010577Cu: goto block_8010577C;
            case 0x8010578Cu: goto block_8010578C;
            case 0x80105760u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105760u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105760u);
    /* Address: 0x80105760, Size: 60 bytes, Blocks: 6 */

block_80105760:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105760u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105760u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105760u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105760u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x80105760: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105760u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    int _bc_80105764 = (cpu->gpr[4] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105768u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x80105768: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105768u);
#endif
    if (_bc_80105764) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x8010577Cu);
        goto block_8010577C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x8010576Cu);
        goto block_8010576C;  /* not taken */
    }

block_8010576C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010576Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010576Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010576Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010576Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105774u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105770u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105770: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105770u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105A2Cu);
    cpu->pc = 0x80105A2Cu; return;  /* CPS jal -> 0x80105A2C */

block_80105774:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105774u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105774u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105774u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105774u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105778: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105778u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8010578Cu);
    goto block_8010578C;  /* j */

block_8010577C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010577Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010577Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010577Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010577Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105784u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105780u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105780: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105780u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105B54u);
    cpu->pc = 0x80105B54u; return;  /* CPS jal -> 0x80105B54 */

block_80105784:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105784u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105784u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105784u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105784u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 29;  /* 0x80105784: 0x00021742 */
#ifdef PSX_COSIM
    cosim_instr(0x80105784u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x80105788: 0x30420001 */
#ifdef PSX_COSIM
    cosim_instr(0x80105788u);
#endif
    /* fall through to block_8010578C */

block_8010578C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010578Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010578Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010578Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010578Cu);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x8010578C: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x8010578Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105790u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x80105790: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105790u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105794 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105798: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105798u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105794);
    cpu->pc = _jt_80105794; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_8010579C(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801057B0u: goto block_801057B0;
            case 0x801057C0u: goto block_801057C0;
            case 0x801057A8u: goto block_801057A8;
            case 0x801057B8u: goto block_801057B8;
            case 0x801057C8u: goto block_801057C8;
            case 0x8010579Cu: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x8010579Cu, _cont); return;
        }
    }
    debug_server_log_call_entry(0x8010579Cu);
    /* Address: 0x8010579C, Size: 60 bytes, Blocks: 6 */

block_8010579C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010579Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010579Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010579Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010579Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x8010579C: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x8010579Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    int _bc_801057A0 = (cpu->gpr[4] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x801057A4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x801057A4: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x801057A4u);
#endif
    if (_bc_801057A0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x801057B8u);
        goto block_801057B8;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x801057A8u);
        goto block_801057A8;  /* not taken */
    }

block_801057A8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057A8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057A8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x801057B0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801057AC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801057ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105AC0u);
    cpu->pc = 0x80105AC0u; return;  /* CPS jal -> 0x80105AC0 */

block_801057B0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057B0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057B0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801057B4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801057B4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801057C8u);
    goto block_801057C8;  /* j */

block_801057B8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057B8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057B8u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x801057C0u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801057BC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801057BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105B54u);
    cpu->pc = 0x80105B54u; return;  /* CPS jal -> 0x80105B54 */

block_801057C0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057C0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057C0u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = (uint32_t)cpu->gpr[2] >> 24;  /* 0x801057C0: 0x00021602 */
#ifdef PSX_COSIM
    cosim_instr(0x801057C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] & 0x0001;  /* 0x801057C4: 0x30420001 */
#ifdef PSX_COSIM
    cosim_instr(0x801057C4u);
#endif
    /* fall through to block_801057C8 */

block_801057C8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057C8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057C8u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057C8u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x801057C8: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x801057C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x801057CC: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x801057CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_801057D0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801057D4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801057D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_801057D0);
    cpu->pc = _jt_801057D0; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_801057D8(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801057ECu: goto block_801057EC;
            case 0x801057D8u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x801057D8u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x801057D8u);
    /* Address: 0x801057D8, Size: 36 bytes, Blocks: 2 */

block_801057D8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057D8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057D8u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x801057D8: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x801057D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x801057DCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x801057DC: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x801057DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x31u);
#endif
    cpu->gpr[5] = cpu->gpr[4];  /* move */  /* 0x801057E0: 0x00802821 */
#ifdef PSX_COSIM
    cosim_instr(0x801057E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x801057ECu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = cpu->gpr[0];  /* move */  /* 0x801057E8: 0x00002021 */
#ifdef PSX_COSIM
    cosim_instr(0x801057E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80085C28u);
    cpu->pc = 0x80085C28u; return;  /* CPS jal -> 0x80085C28 */

block_801057EC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057ECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057ECu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057ECu);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x801057EC: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x801057ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x801057F0: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x801057F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_801057F4 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801057F8: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801057F8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_801057F4);
    cpu->pc = _jt_801057F4; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_801057FC(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105810u: goto block_80105810;
            case 0x801057FCu: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x801057FCu, _cont); return;
        }
    }
    debug_server_log_call_entry(0x801057FCu);
    /* Address: 0x801057FC, Size: 36 bytes, Blocks: 2 */

block_801057FC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801057FCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801057FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801057FCu, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801057FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x801057FC: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x801057FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105800u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105800u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x80105800: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105800u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x31u);
#endif
    cpu->gpr[5] = cpu->gpr[4];  /* move */  /* 0x80105804: 0x00802821 */
#ifdef PSX_COSIM
    cosim_instr(0x80105804u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105810u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = 1;  /* 0x8010580C: 0x24040001 */
#ifdef PSX_COSIM
    cosim_instr(0x8010580Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80085C28u);
    cpu->pc = 0x80085C28u; return;  /* CPS jal -> 0x80085C28 */

block_80105810:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105810u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105810u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105810u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105810u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x80105810: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105810u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x80105814: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105814u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105818 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010581C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010581Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105818);
    cpu->pc = _jt_80105818; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105820(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8010588Cu: goto block_8010588C;
            case 0x8010589Cu: goto block_8010589C;
            case 0x80105900u: goto block_80105900;
            case 0x80105830u: goto block_80105830;
            case 0x8010583Cu: goto block_8010583C;
            case 0x80105844u: goto block_80105844;
            case 0x801058A4u: goto block_801058A4;
            case 0x801058F4u: goto block_801058F4;
            case 0x80105820u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105820u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105820u);
    /* Address: 0x80105820, Size: 240 bytes, Blocks: 9 */

block_80105820:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105820u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105820u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105820u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105820u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x80105820: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105820u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x31u);
#endif
    cpu->gpr[5] = cpu->gpr[4];  /* move */  /* 0x80105824: 0x00802821 */
#ifdef PSX_COSIM
    cosim_instr(0x80105824u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    int _bc_80105828 = (cpu->gpr[5] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x8010582Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x8010582C: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x8010582Cu);
#endif
    if (_bc_80105828) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105844u);
        goto block_80105844;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105830u);
        goto block_80105830;  /* not taken */
    }

block_80105830:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105830u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105830u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105830u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105830u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x80105830: 0x24020001 */
#ifdef PSX_COSIM
    cosim_instr(0x80105830u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x24u);
#endif
    int _bc_80105834 = (cpu->gpr[5] == cpu->gpr[2]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8000 << 16;  /* 0x80000000 */  /* 0x80105838: 0x3C028000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105838u);
#endif
    if (_bc_80105834) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x801058A4u);
        goto block_801058A4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x8010583Cu);
        goto block_8010583C;  /* not taken */
    }

block_8010583C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010583Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010583Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010583Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010583Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105840u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105840: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105840u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x801058F4u);
    goto block_801058F4;  /* j */

block_80105844:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105844u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105844u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105844u, 18u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105844u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105844: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105844u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17308, 3, 0x8u);  /* 0x80105848: 0x8C63439C */
#ifdef PSX_COSIM
    cosim_instr(0x80105848u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8000 << 16;  /* 0x80000000 */  /* 0x8010584C: 0x3C028000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010584Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105850u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105850u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x80105850: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105850u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105854: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105854u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17264, 2, 0x4u);  /* 0x80105858: 0x8C424370 */
#ifdef PSX_COSIM
    cosim_instr(0x80105858u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8011 << 16;  /* 0x80110000 */  /* 0x8010585C: 0x3C048011 */
#ifdef PSX_COSIM
    cosim_instr(0x8010585Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105860u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 16984;  /* 0x80105860: 0x24844258 */
#ifdef PSX_COSIM
    cosim_instr(0x80105860u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    g_debug_last_store_pc = 0x80105864u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[0]);  /* 0x80105864: 0xAC400000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105864u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105868: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105868u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17276, 2, 0x4u);  /* 0x8010586C: 0x8C42437C */
#ifdef PSX_COSIM
    cosim_instr(0x8010586Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105870u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 32;  /* 0x80105870: 0x24050020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105870u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    g_debug_last_store_pc = 0x80105874u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[0]);  /* 0x80105874: 0xAC400000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105874u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105878: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105878u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17308, 3, 0x8u);  /* 0x8010587C: 0x8C63439C */
#ifdef PSX_COSIM
    cosim_instr(0x8010587Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105880u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x6000 << 16;  /* 0x60000000 */  /* 0x80105880: 0x3C026000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105880u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x8010588Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105888u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x80105888: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105888u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105910u);
    cpu->pc = 0x80105910u; return;  /* CPS jal -> 0x80105910 */

block_8010588C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010588Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010588Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010588Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010588Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8011 << 16;  /* 0x80110000 */  /* 0x8010588C: 0x3C048011 */
#ifdef PSX_COSIM
    cosim_instr(0x8010588Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105890u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 17116;  /* 0x80105890: 0x248442DC */
#ifdef PSX_COSIM
    cosim_instr(0x80105890u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x8010589Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x21u);
#endif
    cpu->gpr[5] = 32;  /* 0x80105898: 0x24050020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105898u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105910u);
    cpu->pc = 0x80105910u; return;  /* CPS jal -> 0x80105910 */

block_8010589C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010589Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010589Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010589Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010589Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801058A0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105900u);
    goto block_80105900;  /* j */

block_801058A4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801058A4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801058A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801058A4u, 20u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801058A4: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x801058A4u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17308, 3, 0x8u);  /* 0x801058A8: 0x8C63439C */
#ifdef PSX_COSIM
    cosim_instr(0x801058A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801058AC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x801058B0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x801058B0: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801058B4: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x801058B4u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17264, 2, 0x4u);  /* 0x801058B8: 0x8C424370 */
#ifdef PSX_COSIM
    cosim_instr(0x801058B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801058BC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    g_debug_last_store_pc = 0x801058C0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[0]);  /* 0x801058C0: 0xAC400000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801058C4: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x801058C4u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17276, 2, 0x4u);  /* 0x801058C8: 0x8C42437C */
#ifdef PSX_COSIM
    cosim_instr(0x801058C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801058CC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    g_debug_last_store_pc = 0x801058D0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[0]);  /* 0x801058D0: 0xAC400000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801058D4: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x801058D4u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17276, 2, 0x4u);  /* 0x801058D8: 0x8C42437C */
#ifdef PSX_COSIM
    cosim_instr(0x801058D8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801058DC: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x801058DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058E0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17308, 3, 0x8u);  /* 0x801058E0: 0x8C63439C */
#ifdef PSX_COSIM
    cosim_instr(0x801058E0u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2], 2, 0x4u);  /* 0x801058E4: 0x8C420000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x6000 << 16;  /* 0x60000000 */  /* 0x801058E8: 0x3C026000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058E8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x801058F0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x801058F0: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x801058F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105900u);
    goto block_80105900;  /* j */

block_801058F4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801058F4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801058F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801058F4u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801058F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8010 << 16;  /* 0x80100000 */  /* 0x801058F4: 0x3C048010 */
#ifdef PSX_COSIM
    cosim_instr(0x801058F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105900u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + -9764;  /* 0x801058FC: 0x2484D9DC */
#ifdef PSX_COSIM
    cosim_instr(0x801058FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007A458u);
    cpu->pc = 0x8007A458u; return;  /* CPS jal -> 0x8007A458 */

block_80105900:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105900u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105900u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105900u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105900u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x80105900: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105900u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x80105904: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105904u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105908 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x8010590C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x8010590Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105908);
    cpu->pc = _jt_80105908; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105910(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x8010592Cu: goto block_8010592C;
            case 0x80105910u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105910u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105910u);
    /* Address: 0x80105910, Size: 144 bytes, Blocks: 2 */

block_80105910:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105910u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105910u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105910u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105910u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -32;  /* 0x80105910: 0x27BDFFE0 */
#ifdef PSX_COSIM
    cosim_instr(0x80105910u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x80105914u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);  /* 0x80105914: 0xAFB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105914u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[17] = cpu->gpr[4];  /* move */  /* 0x80105918: 0x00808821 */
#ifdef PSX_COSIM
    cosim_instr(0x80105918u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x8010591Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);  /* 0x8010591C: 0xAFB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x8010591Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105920u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105920u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[31]);  /* 0x80105920: 0xAFBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105920u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x8010592Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10021u);
#endif
    cpu->gpr[16] = cpu->gpr[5];  /* move */  /* 0x80105928: 0x00A08021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105928u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105A2Cu);
    cpu->pc = 0x80105A2Cu; return;  /* CPS jal -> 0x80105A2C */

block_8010592C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x8010592Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x8010592Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x8010592Cu, 29u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x8010592Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x8010592C: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x8010592Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105930u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17312, 3, 0x8u);  /* 0x80105930: 0x8C6343A0 */
#ifdef PSX_COSIM
    cosim_instr(0x80105930u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = (uint32_t)cpu->gpr[16] >> 5;  /* 0x80105934: 0x00108142 */
#ifdef PSX_COSIM
    cosim_instr(0x80105934u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x80105938: 0x8C620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105938u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] << 16;  /* 0x8010593C: 0x00108400 */
#ifdef PSX_COSIM
    cosim_instr(0x8010593Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105940u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | 0x0088;  /* 0x80105940: 0x34420088 */
#ifdef PSX_COSIM
    cosim_instr(0x80105940u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105944u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x80105944: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105944u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105948: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105948u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17256, 3, 0x8u);  /* 0x8010594C: 0x8C634368 */
#ifdef PSX_COSIM
    cosim_instr(0x8010594Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105950u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    cpu->gpr[2] = cpu->gpr[17] + 4;  /* 0x80105950: 0x26220004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105950u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105954u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x80105954: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105954u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105958: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105958u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17260, 2, 0x4u);  /* 0x8010595C: 0x8C42436C */
#ifdef PSX_COSIM
    cosim_instr(0x8010595Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105960u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] | 0x0020;  /* 0x80105960: 0x36100020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105960u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x80105964u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[16]);  /* 0x80105964: 0xAC500000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105964u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105968: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105968u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17304, 3, 0x8u);  /* 0x8010596C: 0x8C634398 */
#ifdef PSX_COSIM
    cosim_instr(0x8010596Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105970u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[17], 2, 0x20000u);  /* 0x80105970: 0x8E220000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105970u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x0100 << 16;  /* 0x01000000 */  /* 0x80105974: 0x3C040100 */
#ifdef PSX_COSIM
    cosim_instr(0x80105974u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105978u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x80105978: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105978u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x8010597C: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x8010597Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105980u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17264, 2, 0x4u);  /* 0x80105980: 0x8C424370 */
#ifdef PSX_COSIM
    cosim_instr(0x80105980u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] | 0x0201;  /* 0x80105984: 0x34840201 */
#ifdef PSX_COSIM
    cosim_instr(0x80105984u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    g_debug_last_store_pc = 0x80105988u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[4]);  /* 0x80105988: 0xAC440000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105988u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 31, 0x20000000u);  /* 0x8010598C: 0x8FBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x8010598Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105990u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 17, 0x20000000u);  /* 0x80105990: 0x8FB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105990u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 16, 0x20000000u);  /* 0x80105994: 0x8FB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105994u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105998 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x8010599C: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x8010599Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105998);
    cpu->pc = _jt_80105998; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_801059A0(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x801059BCu: goto block_801059BC;
            case 0x801059A0u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x801059A0u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x801059A0u);
    /* Address: 0x801059A0, Size: 140 bytes, Blocks: 2 */

block_801059A0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801059A0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801059A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801059A0u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801059A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -32;  /* 0x801059A0: 0x27BDFFE0 */
#ifdef PSX_COSIM
    cosim_instr(0x801059A0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20020000u);
#endif
    g_debug_last_store_pc = 0x801059A4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 20, cpu->gpr[17]);  /* 0x801059A4: 0xAFB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x801059A4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20011u);
#endif
    cpu->gpr[17] = cpu->gpr[4];  /* move */  /* 0x801059A8: 0x00808821 */
#ifdef PSX_COSIM
    cosim_instr(0x801059A8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20010000u);
#endif
    g_debug_last_store_pc = 0x801059ACu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[16]);  /* 0x801059AC: 0xAFB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x801059ACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801059B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x801059B0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[31]);  /* 0x801059B0: 0xAFBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x801059B0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x801059BCu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10021u);
#endif
    cpu->gpr[16] = cpu->gpr[5];  /* move */  /* 0x801059B8: 0x00A08021 */
#ifdef PSX_COSIM
    cosim_instr(0x801059B8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105AC0u);
    cpu->pc = 0x80105AC0u; return;  /* CPS jal -> 0x80105AC0 */

block_801059BC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x801059BCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x801059BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x801059BCu, 28u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801059BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801059BC: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x801059BCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801059C0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17312, 3, 0x8u);  /* 0x801059C0: 0x8C6343A0 */
#ifdef PSX_COSIM
    cosim_instr(0x801059C0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x801059C4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x801059C4u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x801059C8: 0x8C620000 */
#ifdef PSX_COSIM
    cosim_instr(0x801059C8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = (uint32_t)cpu->gpr[16] >> 5;  /* 0x801059CC: 0x00108142 */
#ifdef PSX_COSIM
    cosim_instr(0x801059CCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801059D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | 0x0088;  /* 0x801059D0: 0x34420088 */
#ifdef PSX_COSIM
    cosim_instr(0x801059D0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x801059D4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x801059D4: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x801059D4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801059D8: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x801059D8u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17276, 2, 0x4u);  /* 0x801059DC: 0x8C42437C */
#ifdef PSX_COSIM
    cosim_instr(0x801059DCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801059E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] << 16;  /* 0x801059E0: 0x00108400 */
#ifdef PSX_COSIM
    cosim_instr(0x801059E0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    g_debug_last_store_pc = 0x801059E4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[0]);  /* 0x801059E4: 0xAC400000 */
#ifdef PSX_COSIM
    cosim_instr(0x801059E4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801059E8: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x801059E8u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17268, 2, 0x4u);  /* 0x801059EC: 0x8C424374 */
#ifdef PSX_COSIM
    cosim_instr(0x801059ECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x801059F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10000u);
#endif
    cpu->gpr[16] = cpu->gpr[16] | 0x0020;  /* 0x801059F0: 0x36100020 */
#ifdef PSX_COSIM
    cosim_instr(0x801059F0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20004u);
#endif
    g_debug_last_store_pc = 0x801059F4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[17]);  /* 0x801059F4: 0xAC510000 */
#ifdef PSX_COSIM
    cosim_instr(0x801059F4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x801059F8: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x801059F8u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17272, 2, 0x4u);  /* 0x801059FC: 0x8C424378 */
#ifdef PSX_COSIM
    cosim_instr(0x801059FCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x0100 << 16;  /* 0x01000000 */  /* 0x80105A00: 0x3C030100 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10004u);
#endif
    g_debug_last_store_pc = 0x80105A04u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[16]);  /* 0x80105A04: 0xAC500000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105A08: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A08u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17276, 2, 0x4u);  /* 0x80105A0C: 0x8C42437C */
#ifdef PSX_COSIM
    cosim_instr(0x80105A0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] | 0x0200;  /* 0x80105A10: 0x34630200 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105A14u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[3]);  /* 0x80105A14: 0xAC430000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A14u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 31, 0x20000000u);  /* 0x80105A18: 0x8FBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A18u);
#endif
    cpu->gpr[17] = psx_cyc_load_word(cpu, cpu->gpr[29] + 20, 17, 0x20000000u);  /* 0x80105A1C: 0x8FB10014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A20u);
#endif
    cpu->gpr[16] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 16, 0x20000000u);  /* 0x80105A20: 0x8FB00010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105A24 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x80105A28: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105A24);
    cpu->pc = _jt_80105A24; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105A2C(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105A88u: goto block_80105A88;
            case 0x80105A58u: goto block_80105A58;
            case 0x80105A5Cu: goto block_80105A5C;
            case 0x80105A7Cu: goto block_80105A7C;
            case 0x80105A90u: goto block_80105A90;
            case 0x80105AB0u: goto block_80105AB0;
            case 0x80105A2Cu: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105A2Cu, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105A2Cu);
    /* Address: 0x80105A2C, Size: 148 bytes, Blocks: 7 */

block_80105A2C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105A2Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105A2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105A2Cu, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -32;  /* 0x80105A2C: 0x27BDFFE0 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105A30: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A30u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17308, 3, 0x8u);  /* 0x80105A34: 0x8C63439C */
#ifdef PSX_COSIM
    cosim_instr(0x80105A34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x0010 << 16;  /* 0x00100000 */  /* 0x80105A38: 0x3C020010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105A3Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[31]);  /* 0x80105A3C: 0xAFBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x80105A40u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x80105A40: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A40u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x80105A44: 0x8C620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x2000 << 16;  /* 0x20000000 */  /* 0x80105A48: 0x3C032000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x80105A4C: 0x00431024 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_80105A50 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x80105A54: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A54u);
#endif
    if (_bc_80105A50) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105AB0u);
        goto block_80105AB0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105A58u);
        goto block_80105A58;  /* not taken */
    }

block_80105A58:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105A58u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105A58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105A58u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x80105A58: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105A58u);
#endif
    /* fall through to block_80105A5C */

block_80105A5C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105A5Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105A5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105A5Cu, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A5Cu);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 2, 0x20000000u);  /* 0x80105A5C: 0x8FA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105A60: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -1;  /* 0x80105A64: 0x2442FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105A64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x80105A68u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x80105A68: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A68u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 2, 0x20000000u);  /* 0x80105A6C: 0x8FA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105A70: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    int _bc_80105A74 = (cpu->gpr[2] != cpu->gpr[4]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105A78: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A78u);
#endif
    if (_bc_80105A74) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105A90u);
        goto block_80105A90;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105A7Cu);
        goto block_80105A7C;  /* not taken */
    }

block_80105A7C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105A7Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105A7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105A7Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8010 << 16;  /* 0x80100000 */  /* 0x80105A7C: 0x3C048010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105A88u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + -9736;  /* 0x80105A84: 0x2484D9F8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105B6Cu);
    cpu->pc = 0x80105B6Cu; return;  /* CPS jal -> 0x80105B6C */

block_80105A88:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105A88u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105A88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105A88u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x80105A8C: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105A8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105AB0u);
    goto block_80105AB0;  /* j */

block_80105A90:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105A90u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105A90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105A90u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105A90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105A90: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A90u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17308, 2, 0x4u);  /* 0x80105A94: 0x8C42439C */
#ifdef PSX_COSIM
    cosim_instr(0x80105A94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105A98: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A98u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2], 2, 0x4u);  /* 0x80105A9C: 0x8C420000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105A9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105AA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105AA0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x80105AA4: 0x00431024 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_80105AA8 = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x80105AAC: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AACu);
#endif
    if (_bc_80105AA8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105A5Cu);
        goto block_80105A5C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105AB0u);
        goto block_80105AB0;  /* not taken */
    }

block_80105AB0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105AB0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105AB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105AB0u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105AB0u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 31, 0x20000000u);  /* 0x80105AB0: 0x8FBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x80105AB4: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105AB8 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105ABC: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105ABCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105AB8);
    cpu->pc = _jt_80105AB8; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105AC0(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105B1Cu: goto block_80105B1C;
            case 0x80105AECu: goto block_80105AEC;
            case 0x80105AF0u: goto block_80105AF0;
            case 0x80105B10u: goto block_80105B10;
            case 0x80105B24u: goto block_80105B24;
            case 0x80105B44u: goto block_80105B44;
            case 0x80105AC0u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105AC0u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105AC0u);
    /* Address: 0x80105AC0, Size: 148 bytes, Blocks: 7 */

block_80105AC0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105AC0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105AC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105AC0u, 11u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105AC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -32;  /* 0x80105AC0: 0x27BDFFE0 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105AC4: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AC4u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17276, 3, 0x8u);  /* 0x80105AC8: 0x8C63437C */
#ifdef PSX_COSIM
    cosim_instr(0x80105AC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x0010 << 16;  /* 0x00100000 */  /* 0x80105ACC: 0x3C020010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105ACCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105AD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105AD0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 24, cpu->gpr[31]);  /* 0x80105AD0: 0xAFBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x80105AD4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x80105AD4: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AD4u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[3], 2, 0x8u);  /* 0x80105AD8: 0x8C620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x0100 << 16;  /* 0x01000000 */  /* 0x80105ADC: 0x3C030100 */
#ifdef PSX_COSIM
    cosim_instr(0x80105ADCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105AE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x80105AE0: 0x00431024 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_80105AE4 = (cpu->gpr[2] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x80105AE8: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AE8u);
#endif
    if (_bc_80105AE4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105B44u);
        goto block_80105B44;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105AECu);
        goto block_80105AEC;  /* not taken */
    }

block_80105AEC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105AECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105AECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105AECu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105AECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    cpu->gpr[4] = -1;  /* 0x80105AEC: 0x2404FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105AECu);
#endif
    /* fall through to block_80105AF0 */

block_80105AF0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105AF0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105AF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105AF0u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105AF0u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 2, 0x20000000u);  /* 0x80105AF0: 0x8FA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105AF4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] + -1;  /* 0x80105AF8: 0x2442FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105AF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000004u);
#endif
    g_debug_last_store_pc = 0x80105AFCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[2]);  /* 0x80105AFC: 0xAFA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105AFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B00u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 2, 0x20000000u);  /* 0x80105B00: 0x8FA20010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B04: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x14u);
#endif
    int _bc_80105B08 = (cpu->gpr[2] != cpu->gpr[4]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B0C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B0Cu);
#endif
    if (_bc_80105B08) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105B24u);
        goto block_80105B24;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105B10u);
        goto block_80105B10;  /* not taken */
    }

block_80105B10:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105B10u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105B10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105B10u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8010 << 16;  /* 0x80100000 */  /* 0x80105B10: 0x3C048010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105B1Cu;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + -9720;  /* 0x80105B18: 0x2484DA08 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105B6Cu);
    cpu->pc = 0x80105B6Cu; return;  /* CPS jal -> 0x80105B6C */

block_80105B1C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105B1Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105B1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105B1Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = -1;  /* 0x80105B20: 0x2402FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105B20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x80105B44u);
    goto block_80105B44;  /* j */

block_80105B24:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105B24u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105B24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105B24u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105B24: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B24u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17276, 2, 0x4u);  /* 0x80105B28: 0x8C42437C */
#ifdef PSX_COSIM
    cosim_instr(0x80105B28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B2C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B30u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2], 2, 0x4u);  /* 0x80105B30: 0x8C420000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B34: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] & cpu->gpr[3];  /* 0x80105B38: 0x00431024 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    int _bc_80105B3C = (cpu->gpr[2] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x80105B40: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B40u);
#endif
    if (_bc_80105B3C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105AF0u);
        goto block_80105AF0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105B44u);
        goto block_80105B44;  /* not taken */
    }

block_80105B44:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105B44u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105B44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105B44u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B44u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 24, 31, 0x20000000u);  /* 0x80105B44: 0x8FBF0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 32;  /* 0x80105B48: 0x27BD0020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105B4C = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B50: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105B4C);
    cpu->pc = _jt_80105B4C; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105B54(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105B54u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105B54u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105B54u);
    /* Address: 0x80105B54, Size: 24 bytes, Blocks: 1 */

block_80105B54:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105B54u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105B54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105B54u, 6u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105B54: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B54u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17308, 2, 0x4u);  /* 0x80105B58: 0x8C42439C */
#ifdef PSX_COSIM
    cosim_instr(0x80105B58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B5C: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B60u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2], 2, 0x4u);  /* 0x80105B60: 0x8C420000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105B64 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B68: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105B64);
    cpu->pc = _jt_80105B64; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105B6C(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105B88u: goto block_80105B88;
            case 0x80105B6Cu: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105B6Cu, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105B6Cu);
    /* Address: 0x80105B6C, Size: 124 bytes, Blocks: 2 */

block_80105B6C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105B6Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105B6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105B6Cu, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + -24;  /* 0x80105B6C: 0x27BDFFE8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x31u);
#endif
    cpu->gpr[5] = cpu->gpr[4];  /* move */  /* 0x80105B70: 0x00802821 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8010 << 16;  /* 0x80100000 */  /* 0x80105B74: 0x3C048010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + -9704;  /* 0x80105B78: 0x2484DA18 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xA0000000u);
#endif
    g_debug_last_store_pc = 0x80105B7Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[29] + 16, cpu->gpr[31]);  /* 0x80105B7C: 0xAFBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000000u);
#endif
    cpu->gpr[31] = 0x80105B88u;  /* jal link before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105B84: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, 0x8007A458u);
    cpu->pc = 0x8007A458u; return;  /* CPS jal -> 0x8007A458 */

block_80105B88:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105B88u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105B88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105B88u, 24u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105B88: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B88u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17308, 3, 0x8u);  /* 0x80105B8C: 0x8C63439C */
#ifdef PSX_COSIM
    cosim_instr(0x80105B8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105B90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8000 << 16;  /* 0x80000000 */  /* 0x80105B90: 0x3C028000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    g_debug_last_store_pc = 0x80105B94u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[3], cpu->gpr[2]);  /* 0x80105B94: 0xAC620000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105B98: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B98u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17264, 2, 0x4u);  /* 0x80105B9C: 0x8C424370 */
#ifdef PSX_COSIM
    cosim_instr(0x80105B9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105BA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105BA0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    g_debug_last_store_pc = 0x80105BA4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[0]);  /* 0x80105BA4: 0xAC400000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105BA8: 0x3C028011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BA8u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[2] + 17276, 2, 0x4u);  /* 0x80105BAC: 0x8C42437C */
#ifdef PSX_COSIM
    cosim_instr(0x80105BACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105BB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105BB0: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    g_debug_last_store_pc = 0x80105BB4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[2], cpu->gpr[0]);  /* 0x80105BB4: 0xAC400000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x80105BB8: 0x00001021 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105BBC: 0x3C038011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105BC0u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3] + 17276, 3, 0x8u);  /* 0x80105BC0: 0x8C63437C */
#ifdef PSX_COSIM
    cosim_instr(0x80105BC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105BC4: 0x3C048011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BC4u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[4] + 17308, 4, 0x10u);  /* 0x80105BC8: 0x8C84439C */
#ifdef PSX_COSIM
    cosim_instr(0x80105BC8u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[3], 3, 0x8u);  /* 0x80105BCC: 0x8C630000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105BD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = 0x6000 << 16;  /* 0x60000000 */  /* 0x80105BD0: 0x3C036000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x18u);
#endif
    g_debug_last_store_pc = 0x80105BD4u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[4], cpu->gpr[3]);  /* 0x80105BD4: 0xAC830000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BD4u);
#endif
    cpu->gpr[31] = psx_cyc_load_word(cpu, cpu->gpr[29] + 16, 31, 0x20000000u);  /* 0x80105BD8: 0x8FBF0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20000000u);
#endif
    cpu->gpr[29] = cpu->gpr[29] + 24;  /* 0x80105BDC: 0x27BD0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105BE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105BE0 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105BE4: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105BE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105BE0);
    cpu->pc = _jt_80105BE0; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}

static void func_80105C20(CPUState* cpu)
{
#if defined(PSX_ENABLE_BLOCK_CYCLES) && (defined(__GNUC__) || defined(__clang__))
    __attribute__((cleanup(psx_cyc_bb_defer_cleanup))) int _psx_cyc_bb_guard = 1;
    psx_cyc_bb_defer_begin();
#endif
    if (cpu->pc != 0u) {
        uint32_t _cont = cpu->pc; cpu->pc = 0;
        switch (_cont) {
            case 0x80105C3Cu: goto block_80105C3C;
            case 0x80105C74u: goto block_80105C74;
            case 0x80105CACu: goto block_80105CAC;
            case 0x80105CB0u: goto block_80105CB0;
            case 0x80105CE4u: goto block_80105CE4;
            case 0x80105CECu: goto block_80105CEC;
            case 0x80105CF8u: goto block_80105CF8;
            case 0x80105D04u: goto block_80105D04;
            case 0x80105D08u: goto block_80105D08;
            case 0x80105D28u: goto block_80105D28;
            case 0x80105D3Cu: goto block_80105D3C;
            case 0x80105D48u: goto block_80105D48;
            case 0x80105D4Cu: goto block_80105D4C;
            case 0x80105D5Cu: goto block_80105D5C;
            case 0x80105D6Cu: goto block_80105D6C;
            case 0x80105D78u: goto block_80105D78;
            case 0x80105D80u: goto block_80105D80;
            case 0x80105D8Cu: goto block_80105D8C;
            case 0x80105D94u: goto block_80105D94;
            case 0x80105D98u: goto block_80105D98;
            case 0x80105DB4u: goto block_80105DB4;
            case 0x80105DBCu: goto block_80105DBC;
            case 0x80105DC8u: goto block_80105DC8;
            case 0x80105DDCu: goto block_80105DDC;
            case 0x80105DECu: goto block_80105DEC;
            case 0x80105DF4u: goto block_80105DF4;
            case 0x80105E00u: goto block_80105E00;
            case 0x80105E1Cu: goto block_80105E1C;
            case 0x80105E30u: goto block_80105E30;
            case 0x80105E40u: goto block_80105E40;
            case 0x80105E5Cu: goto block_80105E5C;
            case 0x80105E60u: goto block_80105E60;
            case 0x80105E74u: goto block_80105E74;
            case 0x80105E84u: goto block_80105E84;
            case 0x80105E94u: goto block_80105E94;
            case 0x80105E9Cu: goto block_80105E9C;
            case 0x80105EA4u: goto block_80105EA4;
            case 0x80105EB4u: goto block_80105EB4;
            case 0x80105EBCu: goto block_80105EBC;
            case 0x80105EC8u: goto block_80105EC8;
            case 0x80105ED4u: goto block_80105ED4;
            case 0x80105EDCu: goto block_80105EDC;
            case 0x80105EE4u: goto block_80105EE4;
            case 0x80105F08u: goto block_80105F08;
            case 0x80105F18u: goto block_80105F18;
            case 0x80105F24u: goto block_80105F24;
            case 0x80105F30u: goto block_80105F30;
            case 0x80105F4Cu: goto block_80105F4C;
            case 0x80105C20u: break;  /* entry at prologue */
            default: cpu->pc = _cont; psx_native_bad_entry(cpu, 0x80105C20u, _cont); return;
        }
    }
    debug_server_log_call_entry(0x80105C20u);
    /* Address: 0x80105C20, Size: 864 bytes, Blocks: 49 */

block_80105C20:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105C20u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105C20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105C20u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105C20: 0x3C088011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + 17320;  /* 0x80105C24: 0x250843A8 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x40u);
#endif
    cpu->gpr[6] = cpu->gpr[6] + 2048;  /* 0x80105C28: 0x20C60800 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2u);
#endif
    cpu->gpr[1] = 0x0001 << 16;  /* 0x00010000 */  /* 0x80105C2C: 0x3C010001 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xC2u);
#endif
    cpu->gpr[7] = cpu->gpr[6] + cpu->gpr[1];  /* 0x80105C30: 0x00C13820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x11u);
#endif
    int _bc_80105C34 = (cpu->gpr[4] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
    cpu->gpr[9] = psx_cyc_load_word(cpu, cpu->gpr[8], 9, 0x100u);  /* 0x80105C38: 0x8D090000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C38u);
#endif
    if (_bc_80105C34) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105C74u);
        goto block_80105C74;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105C3Cu);
        goto block_80105C3C;  /* not taken */
    }

block_80105C3C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105C3Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105C3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105C3Cu, 14u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105C3C: 0x3C088011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + 17324;  /* 0x80105C40: 0x250843AC */
#ifdef PSX_COSIM
    cosim_instr(0x80105C40u);
#endif
    cpu->gpr[4] = psx_cyc_load_word(cpu, cpu->gpr[8], 4, 0x100u);  /* 0x80105C44: 0x8D040000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C44u);
#endif
    cpu->gpr[5] = psx_cyc_load_word(cpu, cpu->gpr[8] + 4, 5, 0x100u);  /* 0x80105C48: 0x8D050004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C48u);
#endif
    cpu->gpr[2] = psx_cyc_load_word(cpu, cpu->gpr[8] + 8, 2, 0x100u);  /* 0x80105C4C: 0x8D020008 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C50u);
#endif
    cpu->gpr[3] = psx_cyc_load_word(cpu, cpu->gpr[8] + 12, 3, 0x100u);  /* 0x80105C50: 0x8D03000C */
#ifdef PSX_COSIM
    cosim_instr(0x80105C50u);
#endif
    cpu->gpr[12] = psx_cyc_load_word(cpu, cpu->gpr[8] + 16, 12, 0x100u);  /* 0x80105C54: 0x8D0C0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C54u);
#endif
    cpu->gpr[13] = psx_cyc_load_word(cpu, cpu->gpr[8] + 20, 13, 0x100u);  /* 0x80105C58: 0x8D0D0014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C58u);
#endif
    cpu->gpr[15] = psx_cyc_load_word(cpu, cpu->gpr[8] + 24, 15, 0x100u);  /* 0x80105C5C: 0x8D0F0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C60u);
#endif
    cpu->gpr[24] = psx_cyc_load_word(cpu, cpu->gpr[8] + 28, 24, 0x100u);  /* 0x80105C60: 0x8D18001C */
#ifdef PSX_COSIM
    cosim_instr(0x80105C60u);
#endif
    cpu->gpr[25] = psx_cyc_load_word(cpu, cpu->gpr[8] + 32, 25, 0x100u);  /* 0x80105C64: 0x8D190020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->gpr[9] + cpu->gpr[9];  /* 0x80105C68: 0x01294820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105C6C = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4220u);
#endif
    cpu->gpr[14] = cpu->gpr[5] + cpu->gpr[9];  /* 0x80105C70: 0x00A97020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C70u);
#endif
    if (_bc_80105C6C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E00u);
        goto block_80105E00;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105C74u);
        goto block_80105C74;  /* not taken */
    }

block_80105C74:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105C74u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105C74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105C74u, 14u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2001u);
#endif
    cpu->gpr[13] = cpu->gpr[0];  /* move */  /* 0x80105C74: 0x00006820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8001u);
#endif
    cpu->gpr[15] = cpu->gpr[0];  /* move */  /* 0x80105C78: 0x00007820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000001u);
#endif
    cpu->gpr[24] = cpu->gpr[0];  /* move */  /* 0x80105C7C: 0x0000C020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000001u);
#endif
    cpu->gpr[25] = cpu->gpr[0];  /* move */  /* 0x80105C80: 0x0000C820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->gpr[9] + cpu->gpr[9];  /* 0x80105C84: 0x01294820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4220u);
#endif
    cpu->gpr[14] = cpu->gpr[5] + cpu->gpr[9];  /* 0x80105C88: 0x00A97020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C88u);
#endif
    cpu->gpr[9] = psx_cyc_load_word(cpu, cpu->gpr[4], 9, 0x10u);  /* 0x80105C8C: 0x8C890000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105C90u);
#endif
    cpu->gpr[12] = psx_cyc_load_half(cpu, cpu->gpr[4] + 4, 12, 0x10u);  /* 0x80105C90: 0x948C0004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C90u);
#endif
    cpu->gpr[10] = psx_cyc_load_half(cpu, cpu->gpr[4] + 6, 10, 0x10u);  /* 0x80105C94: 0x948A0006 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C94u);
#endif
    cpu->gpr[2] = psx_cyc_load_half(cpu, cpu->gpr[4] + 8, 2, 0x10u);  /* 0x80105C98: 0x94820008 */
#ifdef PSX_COSIM
    cosim_instr(0x80105C98u);
#endif
    cpu->gpr[3] = psx_cyc_load_half(cpu, cpu->gpr[4] + 10, 3, 0x10u);  /* 0x80105C9C: 0x9483000A */
#ifdef PSX_COSIM
    cosim_instr(0x80105C9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x400u);
#endif
    cpu->gpr[10] = cpu->gpr[10] + -3;  /* 0x80105CA0: 0x214AFFFD */
#ifdef PSX_COSIM
    cosim_instr(0x80105CA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x400u);
#endif
    int _bc_80105CA4 = ((int32_t)cpu->gpr[10] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000u);
#endif
    cpu->gpr[12] = cpu->gpr[12] << 10;  /* 0x80105CA8: 0x000C6280 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CA8u);
#endif
    if (_bc_80105CA4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105CB0u);
        goto block_80105CB0;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105CACu);
        goto block_80105CAC;  /* not taken */
    }

block_80105CAC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105CACu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105CACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105CACu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CACu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2001u);
#endif
    cpu->gpr[13] = 1;  /* 0x80105CAC: 0x200D0001 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CACu);
#endif
    /* fall through to block_80105CB0 */

block_80105CB0:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105CB0u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105CB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105CB0u, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 12;  /* 0x80105CB0: 0x2084000C */
#ifdef PSX_COSIM
    cosim_instr(0x80105CB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 16;  /* 0x80105CB4: 0x00021400 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xCu);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[3];  /* 0x80105CB8: 0x00431025 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CB8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x9u);
#endif
    cpu->gpr[3] = cpu->gpr[0];  /* move */  /* 0x80105CBC: 0x00001825 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x220u);
#endif
    g_debug_last_store_pc = 0x80105CC0u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[5], cpu->gpr[9]);  /* 0x80105CC0: 0xACA90000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->gpr[9] & 0xFFFF;  /* 0x80105CC4: 0x3129FFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105CC4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->gpr[9] << 2;  /* 0x80105CC8: 0x00094880 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->gpr[9] + 4;  /* 0x80105CCC: 0x25290004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x220u);
#endif
    cpu->gpr[9] = cpu->gpr[9] + cpu->gpr[5];  /* 0x80105CD0: 0x01254820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105CD4: 0x3C088011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CD4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + 17360;  /* 0x80105CD8: 0x250843D0 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CD8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x300u);
#endif
    g_debug_last_store_pc = 0x80105CDCu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8], cpu->gpr[9]);  /* 0x80105CDC: 0xAD090000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105CE0: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CE0u);
#endif
    /* fall through to block_80105CE4 */

block_80105CE4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105CE4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105CE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105CE4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2001u);
#endif
    int _bc_80105CE4 = (cpu->gpr[13] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[8] = (uint32_t)cpu->gpr[2] >> 22;  /* 0x80105CE8: 0x00024582 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CE8u);
#endif
    if (_bc_80105CE4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DBCu);
        goto block_80105DBC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105CECu);
        goto block_80105CEC;  /* not taken */
    }

block_80105CEC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105CECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105CECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105CECu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x102u);
#endif
    cpu->gpr[1] = cpu->gpr[8] ^ 0x03FF;  /* 0x80105CEC: 0x390103FF */
#ifdef PSX_COSIM
    cosim_instr(0x80105CECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105CF0 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105CF4: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105CF4u);
#endif
    if (_bc_80105CF0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F08u);
        goto block_80105F08;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105CF8u);
        goto block_80105CF8;  /* not taken */
    }

block_80105CF8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105CF8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105CF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105CF8u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105CF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2002u);
#endif
    cpu->gpr[1] = cpu->gpr[13] + -3;  /* 0x80105CF8: 0x21A1FFFD */
#ifdef PSX_COSIM
    cosim_instr(0x80105CF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2u);
#endif
    int _bc_80105CFC = ((int32_t)cpu->gpr[1] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x42u);
#endif
    cpu->gpr[1] = cpu->gpr[6] + -1024;  /* 0x80105D00: 0x20C1FC00 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D00u);
#endif
    if (_bc_80105CFC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D08u);
        goto block_80105D08;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D04u);
        goto block_80105D04;  /* not taken */
    }

block_80105D04:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D04u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D04u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2u);
#endif
    cpu->gpr[1] = cpu->gpr[1] + -1024;  /* 0x80105D04: 0x2021FC00 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D04u);
#endif
    /* fall through to block_80105D08 */

block_80105D08:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D08u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D08u, 8u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[8] = (uint32_t)cpu->gpr[2] >> 24;  /* 0x80105D08: 0x00024602 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] << 2;  /* 0x80105D0C: 0x00084080 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x102u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[1];  /* 0x80105D10: 0x01014020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D10u);
#endif
    cpu->gpr[9] = psx_cyc_load_half(cpu, cpu->gpr[8], 9, 0x100u);  /* 0x80105D14: 0x95090000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D14u);
#endif
    cpu->gpr[10] = psx_cyc_load_half(cpu, cpu->gpr[8] + 2, 10, 0x100u);  /* 0x80105D18: 0x950A0002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x101u);
#endif
    cpu->gpr[8] = cpu->gpr[0] & cpu->gpr[0];  /* 0x80105D1C: 0x00004024 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x401u);
#endif
    int _bc_80105D20 = (cpu->gpr[10] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x204u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << (cpu->gpr[9] & 0x1F);  /* 0x80105D24: 0x01221004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D24u);
#endif
    if (_bc_80105D20) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D4Cu);
        goto block_80105D4C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D28u);
        goto block_80105D28;  /* not taken */
    }

block_80105D28:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D28u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D28u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    cpu->gpr[1] = 32;  /* 0x80105D28: 0x20010020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D28u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x402u);
#endif
    cpu->gpr[1] = cpu->gpr[1] - cpu->gpr[10];  /* 0x80105D2C: 0x002A0822 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D2Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x106u);
#endif
    cpu->gpr[8] = (uint32_t)cpu->gpr[2] >> (cpu->gpr[1] & 0x1F);  /* 0x80105D30: 0x00224006 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    int _bc_80105D34 = ((int32_t)cpu->gpr[2] < 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x404u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << (cpu->gpr[10] & 0x1F);  /* 0x80105D38: 0x01421004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D38u);
#endif
    if (_bc_80105D34) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D48u);
        goto block_80105D48;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D3Cu);
        goto block_80105D3C;  /* not taken */
    }

block_80105D3C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D3Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D3Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x801u);
#endif
    cpu->gpr[11] = -1;  /* 0x80105D3C: 0x200BFFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105D3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x802u);
#endif
    cpu->gpr[11] = (uint32_t)cpu->gpr[11] >> (cpu->gpr[1] & 0x1F);  /* 0x80105D40: 0x002B5806 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x900u);
#endif
    cpu->gpr[8] = cpu->gpr[8] - cpu->gpr[11];  /* 0x80105D44: 0x010B4022 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D44u);
#endif
    /* fall through to block_80105D48 */

block_80105D48:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D48u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D48u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x408u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[10];  /* 0x80105D48: 0x006A1820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D48u);
#endif
    /* fall through to block_80105D4C */

block_80105D4C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D4Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D4Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x208u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[9];  /* 0x80105D4C: 0x00691820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xAu);
#endif
    cpu->gpr[1] = cpu->gpr[3] & 0x0010;  /* 0x80105D50: 0x30610010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105D54 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x80105D58: 0x3063000F */
#ifdef PSX_COSIM
    cosim_instr(0x80105D58u);
#endif
    if (_bc_80105D54) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D6Cu);
        goto block_80105D6C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D5Cu);
        goto block_80105D5C;  /* not taken */
    }

block_80105D5C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D5Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D5Cu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D5Cu);
#endif
    cpu->gpr[9] = psx_cyc_load_half(cpu, cpu->gpr[4], 9, 0x10u);  /* 0x80105D5C: 0x94890000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 2;  /* 0x80105D60: 0x20840002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x208u);
#endif
    cpu->gpr[9] = cpu->gpr[9] << (cpu->gpr[3] & 0x1F);  /* 0x80105D64: 0x00694804 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x204u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[9];  /* 0x80105D68: 0x00491025 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D68u);
#endif
    /* fall through to block_80105D6C */

block_80105D6C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D6Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D6Cu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2002u);
#endif
    cpu->gpr[1] = cpu->gpr[13] + -2;  /* 0x80105D6C: 0x21A1FFFE */
#ifdef PSX_COSIM
    cosim_instr(0x80105D6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2u);
#endif
    int _bc_80105D70 = ((int32_t)cpu->gpr[1] > 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000300u);
#endif
    cpu->gpr[9] = cpu->gpr[25] + cpu->gpr[8];  /* 0x80105D74: 0x03284820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D74u);
#endif
    if (_bc_80105D70) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D94u);
        goto block_80105D94;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D78u);
        goto block_80105D78;  /* not taken */
    }

block_80105D78:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D78u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D78u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105D78 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000300u);
#endif
    cpu->gpr[9] = cpu->gpr[24] + cpu->gpr[8];  /* 0x80105D7C: 0x03084820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D7Cu);
#endif
    if (_bc_80105D78) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D8Cu);
        goto block_80105D8C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D80u);
        goto block_80105D80;  /* not taken */
    }

block_80105D80:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D80u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D80u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8300u);
#endif
    cpu->gpr[9] = cpu->gpr[15] + cpu->gpr[8];  /* 0x80105D80: 0x01E84820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105D84 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8100u);
#endif
    cpu->gpr[15] = cpu->gpr[15] + cpu->gpr[8];  /* 0x80105D88: 0x01E87820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D88u);
#endif
    if (_bc_80105D84) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D98u);
        goto block_80105D98;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D8Cu);
        goto block_80105D8C;  /* not taken */
    }

block_80105D8C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D8Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D8Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D8Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105D8C = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000100u);
#endif
    cpu->gpr[24] = cpu->gpr[24] + cpu->gpr[8];  /* 0x80105D90: 0x0308C020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D90u);
#endif
    if (_bc_80105D8C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D98u);
        goto block_80105D98;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105D94u);
        goto block_80105D94;  /* not taken */
    }

block_80105D94:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D94u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D94u, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000100u);
#endif
    cpu->gpr[25] = cpu->gpr[25] + cpu->gpr[8];  /* 0x80105D94: 0x0328C820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D94u);
#endif
    /* fall through to block_80105D98 */

block_80105D98:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105D98u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105D98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105D98u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105D98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->gpr[9] << 2;  /* 0x80105D98: 0x00094880 */
#ifdef PSX_COSIM
    cosim_instr(0x80105D98u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = cpu->gpr[9] & 0x03FF;  /* 0x80105D9C: 0x312903FF */
#ifdef PSX_COSIM
    cosim_instr(0x80105D9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1200u);
#endif
    cpu->gpr[9] = cpu->gpr[12] | cpu->gpr[9];  /* 0x80105DA0: 0x01894825 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000u);
#endif
    cpu->gpr[13] = cpu->gpr[13] + 1;  /* 0x80105DA4: 0x21AD0001 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2002u);
#endif
    cpu->gpr[1] = cpu->gpr[13] + -7;  /* 0x80105DA8: 0x21A1FFF9 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105DAC = (cpu->gpr[1] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x220u);
#endif
    g_debug_last_store_pc = 0x80105DB0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[9]);  /* 0x80105DB0: 0xA4A90000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DB0u);
#endif
    if (_bc_80105DAC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DF4u);
        goto block_80105DF4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DB4u);
        goto block_80105DB4;  /* not taken */
    }

block_80105DB4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105DB4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105DB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105DB4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105DB4 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000u);
#endif
    cpu->gpr[13] = cpu->gpr[13] + -6;  /* 0x80105DB8: 0x21ADFFFA */
#ifdef PSX_COSIM
    cosim_instr(0x80105DB8u);
#endif
    if (_bc_80105DB4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DF4u);
        goto block_80105DF4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DBCu);
        goto block_80105DBC;  /* not taken */
    }

block_80105DBC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105DBCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105DBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105DBCu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x102u);
#endif
    cpu->gpr[1] = cpu->gpr[8] ^ 0x01FF;  /* 0x80105DBC: 0x390101FF */
#ifdef PSX_COSIM
    cosim_instr(0x80105DBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105DC0 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105DC4: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DC4u);
#endif
    if (_bc_80105DC0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F08u);
        goto block_80105F08;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DC8u);
        goto block_80105DC8;  /* not taken */
    }

block_80105DC8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105DC8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105DC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105DC8u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 10;  /* 0x80105DC8: 0x00021280 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 10;  /* 0x80105DCC: 0x2063000A */
#ifdef PSX_COSIM
    cosim_instr(0x80105DCCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xAu);
#endif
    cpu->gpr[1] = cpu->gpr[3] & 0x0010;  /* 0x80105DD0: 0x30610010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DD0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105DD4 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x80105DD8: 0x3063000F */
#ifdef PSX_COSIM
    cosim_instr(0x80105DD8u);
#endif
    if (_bc_80105DD4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DECu);
        goto block_80105DEC;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105DDCu);
        goto block_80105DDC;  /* not taken */
    }

block_80105DDC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105DDCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105DDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105DDCu, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DDCu);
#endif
    cpu->gpr[9] = psx_cyc_load_half(cpu, cpu->gpr[4], 9, 0x10u);  /* 0x80105DDC: 0x94890000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 2;  /* 0x80105DE0: 0x20840002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x208u);
#endif
    cpu->gpr[9] = cpu->gpr[9] << (cpu->gpr[3] & 0x1F);  /* 0x80105DE4: 0x00694804 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x204u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[9];  /* 0x80105DE8: 0x00491025 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DE8u);
#endif
    /* fall through to block_80105DEC */

block_80105DEC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105DECu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105DECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105DECu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1100u);
#endif
    cpu->gpr[8] = cpu->gpr[12] | cpu->gpr[8];  /* 0x80105DEC: 0x01884025 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x120u);
#endif
    g_debug_last_store_pc = 0x80105DF0u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[8]);  /* 0x80105DF0: 0xA4A80000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DF0u);
#endif
    /* fall through to block_80105DF4 */

block_80105DF4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105DF4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105DF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105DF4u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105DF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4022u);
#endif
    cpu->gpr[1] = cpu->gpr[5] - cpu->gpr[14];  /* 0x80105DF4: 0x00AE0823 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2u);
#endif
    int _bc_80105DF8 = ((int32_t)cpu->gpr[1] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105DFC: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105DFCu);
#endif
    if (_bc_80105DF8) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F4Cu);
        goto block_80105F4C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E00u);
        goto block_80105E00;  /* not taken */
    }

block_80105E00:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E00u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E00u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[8] = (uint32_t)cpu->gpr[2] >> 19;  /* 0x80105E00: 0x000244C2 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] << 3;  /* 0x80105E04: 0x000840C0 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E04u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x140u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[6];  /* 0x80105E08: 0x01064020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E08u);
#endif
    cpu->gpr[9] = psx_cyc_load_word(cpu, cpu->gpr[8], 9, 0x100u);  /* 0x80105E0C: 0x8D090000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105E10: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x201u);
#endif
    int _bc_80105E14 = (cpu->gpr[9] != cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x202u);
#endif
    cpu->gpr[1] = cpu->gpr[9] & 0x00FF;  /* 0x80105E18: 0x312100FF */
#ifdef PSX_COSIM
    cosim_instr(0x80105E18u);
#endif
    if (_bc_80105E14) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E5Cu);
        goto block_80105E5C;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E1Cu);
        goto block_80105E1C;  /* not taken */
    }

block_80105E1C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E1Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E1Cu, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 8;  /* 0x80105E1C: 0x00021200 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E1Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] + 8;  /* 0x80105E20: 0x20630008 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xAu);
#endif
    cpu->gpr[1] = cpu->gpr[3] & 0x0010;  /* 0x80105E24: 0x30610010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105E28 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x80105E2C: 0x3063000F */
#ifdef PSX_COSIM
    cosim_instr(0x80105E2Cu);
#endif
    if (_bc_80105E28) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E40u);
        goto block_80105E40;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E30u);
        goto block_80105E30;  /* not taken */
    }

block_80105E30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E30u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E30u);
#endif
    cpu->gpr[8] = psx_cyc_load_half(cpu, cpu->gpr[4], 8, 0x10u);  /* 0x80105E30: 0x94880000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 2;  /* 0x80105E34: 0x20840002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x108u);
#endif
    cpu->gpr[8] = cpu->gpr[8] << (cpu->gpr[3] & 0x1F);  /* 0x80105E38: 0x00684004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[8];  /* 0x80105E3C: 0x00481025 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E3Cu);
#endif
    /* fall through to block_80105E40 */

block_80105E40:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E40u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E40u, 7u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[8] = (uint32_t)cpu->gpr[2] >> 23;  /* 0x80105E40: 0x000245C2 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] << 2;  /* 0x80105E44: 0x00084080 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E44u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x180u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + cpu->gpr[7];  /* 0x80105E48: 0x01074020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E48u);
#endif
    cpu->gpr[9] = psx_cyc_load_word(cpu, cpu->gpr[8], 9, 0x100u);  /* 0x80105E4C: 0x8D090000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x801u);
#endif
    cpu->gpr[11] = cpu->gpr[0];  /* move */  /* 0x80105E50: 0x00005820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105E54 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x202u);
#endif
    cpu->gpr[1] = cpu->gpr[9] & 0x00FF;  /* 0x80105E58: 0x312100FF */
#ifdef PSX_COSIM
    cosim_instr(0x80105E58u);
#endif
    if (_bc_80105E54) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E60u);
        goto block_80105E60;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E5Cu);
        goto block_80105E5C;  /* not taken */
    }

block_80105E5C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E5Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E5Cu, 1u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E5Cu);
#endif
    cpu->gpr[11] = psx_cyc_load_word(cpu, cpu->gpr[8] + 4, 11, 0x100u);  /* 0x80105E5C: 0x8D0B0004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E5Cu);
#endif
    /* fall through to block_80105E60 */

block_80105E60:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E60u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E60u, 5u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x6u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << (cpu->gpr[1] & 0x1F);  /* 0x80105E60: 0x00221004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xAu);
#endif
    cpu->gpr[3] = cpu->gpr[3] + cpu->gpr[1];  /* 0x80105E64: 0x00611820 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xAu);
#endif
    cpu->gpr[1] = cpu->gpr[3] & 0x0010;  /* 0x80105E68: 0x30610010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105E6C = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8u);
#endif
    cpu->gpr[3] = cpu->gpr[3] & 0x000F;  /* 0x80105E70: 0x3063000F */
#ifdef PSX_COSIM
    cosim_instr(0x80105E70u);
#endif
    if (_bc_80105E6C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E84u);
        goto block_80105E84;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E74u);
        goto block_80105E74;  /* not taken */
    }

block_80105E74:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E74u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E74u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E74u);
#endif
    cpu->gpr[8] = psx_cyc_load_half(cpu, cpu->gpr[4], 8, 0x10u);  /* 0x80105E74: 0x94880000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 2;  /* 0x80105E78: 0x20840002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E78u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x108u);
#endif
    cpu->gpr[8] = cpu->gpr[8] << (cpu->gpr[3] & 0x1F);  /* 0x80105E7C: 0x00684004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E80u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[8];  /* 0x80105E80: 0x00481025 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E80u);
#endif
    /* fall through to block_80105E84 */

block_80105E84:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E84u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E84u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x200u);
#endif
    cpu->gpr[9] = (uint32_t)cpu->gpr[9] >> 16;  /* 0x80105E84: 0x00094C02 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E84u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x202u);
#endif
    cpu->gpr[1] = cpu->gpr[9] ^ 0x7C1F;  /* 0x80105E88: 0x39217C1F */
#ifdef PSX_COSIM
    cosim_instr(0x80105E88u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105E8C = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E90u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x202u);
#endif
    cpu->gpr[1] = cpu->gpr[9] ^ 0xFE00;  /* 0x80105E90: 0x3921FE00 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E90u);
#endif
    if (_bc_80105E8C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EE4u);
        goto block_80105EE4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E94u);
        goto block_80105E94;  /* not taken */
    }

block_80105E94:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E94u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E94u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E94u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105E94 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x220u);
#endif
    g_debug_last_store_pc = 0x80105E98u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[9]);  /* 0x80105E98: 0xA4A90000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105E98u);
#endif
    if (_bc_80105E94) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105CE4u);
        goto block_80105CE4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E9Cu);
        goto block_80105E9C;  /* not taken */
    }

block_80105E9C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105E9Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105E9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105E9Cu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105E9Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x801u);
#endif
    int _bc_80105E9C = (cpu->gpr[11] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EA0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105EA0: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EA0u);
#endif
    if (_bc_80105E9C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E00u);
        goto block_80105E00;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EA4u);
        goto block_80105EA4;  /* not taken */
    }

block_80105EA4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105EA4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105EA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105EA4u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xC00u);
#endif
    cpu->gpr[10] = cpu->gpr[11] & 0xFFFF;  /* 0x80105EA4: 0x316AFFFF */
#ifdef PSX_COSIM
    cosim_instr(0x80105EA4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x402u);
#endif
    cpu->gpr[1] = cpu->gpr[10] ^ 0x7C1F;  /* 0x80105EA8: 0x39417C1F */
#ifdef PSX_COSIM
    cosim_instr(0x80105EA8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105EAC = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EB0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x402u);
#endif
    cpu->gpr[1] = cpu->gpr[10] ^ 0xFE00;  /* 0x80105EB0: 0x3941FE00 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EB0u);
#endif
    if (_bc_80105EAC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EE4u);
        goto block_80105EE4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EB4u);
        goto block_80105EB4;  /* not taken */
    }

block_80105EB4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105EB4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105EB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105EB4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EB4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105EB4 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x420u);
#endif
    g_debug_last_store_pc = 0x80105EB8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[10]);  /* 0x80105EB8: 0xA4AA0000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EB8u);
#endif
    if (_bc_80105EB4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105CE4u);
        goto block_80105CE4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EBCu);
        goto block_80105EBC;  /* not taken */
    }

block_80105EBC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105EBCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105EBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105EBCu, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0xC00u);
#endif
    cpu->gpr[10] = (uint32_t)cpu->gpr[11] >> 16;  /* 0x80105EBC: 0x000B5402 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EBCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EC0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x401u);
#endif
    int _bc_80105EC0 = (cpu->gpr[10] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105EC4: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EC4u);
#endif
    if (_bc_80105EC0) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E00u);
        goto block_80105E00;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EC8u);
        goto block_80105EC8;  /* not taken */
    }

block_80105EC8:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105EC8u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105EC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105EC8u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x402u);
#endif
    cpu->gpr[1] = cpu->gpr[10] ^ 0x7C1F;  /* 0x80105EC8: 0x39417C1F */
#ifdef PSX_COSIM
    cosim_instr(0x80105EC8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105ECC = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105ED0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x402u);
#endif
    cpu->gpr[1] = cpu->gpr[10] ^ 0xFE00;  /* 0x80105ED0: 0x3941FE00 */
#ifdef PSX_COSIM
    cosim_instr(0x80105ED0u);
#endif
    if (_bc_80105ECC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EE4u);
        goto block_80105EE4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105ED4u);
        goto block_80105ED4;  /* not taken */
    }

block_80105ED4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105ED4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105ED4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105ED4u, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105ED4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x3u);
#endif
    int _bc_80105ED4 = (cpu->gpr[1] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x420u);
#endif
    g_debug_last_store_pc = 0x80105ED8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[10]);  /* 0x80105ED8: 0xA4AA0000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105ED8u);
#endif
    if (_bc_80105ED4) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105CE4u);
        goto block_80105CE4;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EDCu);
        goto block_80105EDC;  /* not taken */
    }

block_80105EDC:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105EDCu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105EDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105EDCu, 2u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EDCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105EDC = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EE0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105EE0: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EE0u);
#endif
    if (_bc_80105EDC) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E00u);
        goto block_80105E00;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105EE4u);
        goto block_80105EE4;  /* not taken */
    }

block_80105EE4:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105EE4u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105EE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105EE4u, 9u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[8] = (uint32_t)cpu->gpr[2] >> 16;  /* 0x80105EE4: 0x00024402 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EE4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x120u);
#endif
    g_debug_last_store_pc = 0x80105EE8u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[8]);  /* 0x80105EE8: 0xA4A80000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EE8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105EEC: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EECu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105EF0u);
#endif
    cpu->gpr[8] = psx_cyc_load_half(cpu, cpu->gpr[4], 8, 0x10u);  /* 0x80105EF0: 0x94880000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EF0u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x10u);
#endif
    cpu->gpr[4] = cpu->gpr[4] + 2;  /* 0x80105EF4: 0x20840002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EF4u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x4u);
#endif
    cpu->gpr[2] = cpu->gpr[2] << 16;  /* 0x80105EF8: 0x00021400 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EF8u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x108u);
#endif
    cpu->gpr[8] = cpu->gpr[8] << (cpu->gpr[3] & 0x1F);  /* 0x80105EFC: 0x00684004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105EFCu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F00u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105F00 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    cpu->gpr[2] = cpu->gpr[2] | cpu->gpr[8];  /* 0x80105F04: 0x00481025 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F04u);
#endif
    if (_bc_80105F00) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105E00u);
        goto block_80105E00;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F08u);
        goto block_80105F08;  /* not taken */
    }

block_80105F08:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105F08u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105F08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105F08u, 4u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105F08: 0x3C088011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F08u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + 17360;  /* 0x80105F0C: 0x250843D0 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F0Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F10u);
#endif
    cpu->gpr[9] = psx_cyc_load_word(cpu, cpu->gpr[8], 9, 0x100u);  /* 0x80105F10: 0x8D090000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F10u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x101u);
#endif
    cpu->gpr[8] = 0xFE00;  /* 0x80105F14: 0x3408FE00 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F14u);
#endif
    /* fall through to block_80105F18 */

block_80105F18:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105F18u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105F18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105F18u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x222u);
#endif
    cpu->gpr[1] = cpu->gpr[5] - cpu->gpr[9];  /* 0x80105F18: 0x00A90823 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F18u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2u);
#endif
    int _bc_80105F1C = ((int32_t)cpu->gpr[1] >= 0);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F20u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105F20: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F20u);
#endif
    if (_bc_80105F1C) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F30u);
        goto block_80105F30;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F24u);
        goto block_80105F24;  /* not taken */
    }

block_80105F24:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105F24u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105F24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105F24u, 3u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x120u);
#endif
    g_debug_last_store_pc = 0x80105F24u; psx_store_cycle_barrier(); cpu->write_half(cpu->gpr[5], (uint16_t)cpu->gpr[8]);  /* 0x80105F24: 0xA4A80000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F24u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    int _bc_80105F28 = (cpu->gpr[0] == cpu->gpr[0]);  /* save branch cond before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x20u);
#endif
    cpu->gpr[5] = cpu->gpr[5] + 2;  /* 0x80105F2C: 0x20A50002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F2Cu);
#endif
    if (_bc_80105F28) {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F18u);
        goto block_80105F18;  /* taken */
    } else {
#ifdef PSX_ENABLE_BLOCK_CYCLES
        psx_cyc_bb_defer_flush();
#endif
        psx_check_interrupts_at(cpu, 0x80105F30u);
        goto block_80105F30;  /* not taken */
    }

block_80105F30:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105F30u);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105F30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105F30u, 7u, 1)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    cpu->gpr[9] = cpu->cop0[12];
#ifdef PSX_ENABLE_BLOCK_CYCLES
    cpu->ld_absorb = 0u; cpu->ld_which_t = 9u;
#endif  /* mfc0 (delayed load) */  /* 0x80105F30: 0x40096000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F30u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1u);
#endif
    /* nop */  /* 0x80105F34: 0x00000000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F34u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2u);
#endif
    cpu->gpr[1] = 0x0002 << 16;  /* 0x00020000 */  /* 0x80105F38: 0x3C010002 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F38u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x202u);
#endif
    cpu->gpr[9] = cpu->gpr[9] | cpu->gpr[1];  /* 0x80105F3C: 0x01214825 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F3Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x0u);
#endif
    cpu->cop0[12] = cpu->gpr[9];  /* mtc0 */  /* 0x80105F40: 0x40896000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F40u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105F44 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = cpu->gpr[0];  /* move */  /* 0x80105F48: 0x00001020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F48u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105F44);
    cpu->pc = _jt_80105F44; return;  /* CPS: jr $ra */

block_80105F4C:
#ifndef PSX_NO_DEBUG_TOOLS
    debug_server_cyc_observe(0x80105F4Cu);
#endif
#ifdef PSX_COSIM
    cosim_block(0x80105F4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    if (psx_slice_block(cpu, 0x80105F4Cu, 13u, 0)) return;
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = 0x8011 << 16;  /* 0x80110000 */  /* 0x80105F4C: 0x3C088011 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F4Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x100u);
#endif
    cpu->gpr[8] = cpu->gpr[8] + 17324;  /* 0x80105F50: 0x250843AC */
#ifdef PSX_COSIM
    cosim_instr(0x80105F50u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x110u);
#endif
    g_debug_last_store_pc = 0x80105F54u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8], cpu->gpr[4]);  /* 0x80105F54: 0xAD040000 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F54u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x120u);
#endif
    g_debug_last_store_pc = 0x80105F58u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 4, cpu->gpr[5]);  /* 0x80105F58: 0xAD050004 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F58u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x104u);
#endif
    g_debug_last_store_pc = 0x80105F5Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 8, cpu->gpr[2]);  /* 0x80105F5C: 0xAD020008 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F5Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x108u);
#endif
    g_debug_last_store_pc = 0x80105F60u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 12, cpu->gpr[3]);  /* 0x80105F60: 0xAD03000C */
#ifdef PSX_COSIM
    cosim_instr(0x80105F60u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1100u);
#endif
    g_debug_last_store_pc = 0x80105F64u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 16, cpu->gpr[12]);  /* 0x80105F64: 0xAD0C0010 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F64u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2100u);
#endif
    g_debug_last_store_pc = 0x80105F68u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 20, cpu->gpr[13]);  /* 0x80105F68: 0xAD0D0014 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F68u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x8100u);
#endif
    g_debug_last_store_pc = 0x80105F6Cu; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 24, cpu->gpr[15]);  /* 0x80105F6C: 0xAD0F0018 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F6Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_icache_fetch(cpu, 0x80105F70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x1000100u);
#endif
    g_debug_last_store_pc = 0x80105F70u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 28, cpu->gpr[24]);  /* 0x80105F70: 0xAD18001C */
#ifdef PSX_COSIM
    cosim_instr(0x80105F70u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x2000100u);
#endif
    g_debug_last_store_pc = 0x80105F74u; psx_store_cycle_barrier(); cpu->write_word(cpu->gpr[8] + 32, cpu->gpr[25]);  /* 0x80105F74: 0xAD190020 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F74u);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x80000001u);
#endif
    uint32_t _jt_80105F78 = cpu->gpr[31];  /* latch indirect target before delay slot */
    /* delay slot (always executes) */
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_step(cpu, 0x5u);
#endif
    cpu->gpr[2] = 1;  /* 0x80105F7C: 0x20020001 */
#ifdef PSX_COSIM
    cosim_instr(0x80105F7Cu);
#endif
#ifdef PSX_ENABLE_BLOCK_CYCLES
    psx_cyc_bb_defer_flush();
#endif
    psx_check_interrupts_at(cpu, _jt_80105F78);
    cpu->pc = _jt_80105F78; return;  /* CPS: jr $ra */
    ;  /* label compatibility: C requires a statement after the last label */
}



typedef struct { uint32_t lo; uint32_t len; } PsxGameCodeRange;
static const PsxGameCodeRange k_psx_fmv_vlc_ranges[] = {
{0x80104F30u, 0x70u},
    {0x80104FA0u, 0x2A0u},
    {0x80105560u, 0x34u},
    {0x80105620u, 0x98u},
    {0x801056C4u, 0x7Cu},
    {0x80105740u, 0x20u},
    {0x80105760u, 0x3Cu},
    {0x8010579Cu, 0x3Cu},
    {0x801057D8u, 0x24u},
    {0x801057FCu, 0x24u},
    {0x80105820u, 0xF0u},
    {0x80105910u, 0x90u},
    {0x801059A0u, 0x8Cu},
    {0x80105A2Cu, 0x94u},
    {0x80105AC0u, 0x94u},
    {0x80105B54u, 0x18u},
    {0x80105B6Cu, 0x7Cu},
    {0x80105C20u, 0x360u},
};

typedef void (*PsxGameDispatchFn)(CPUState* cpu);
typedef struct {
    uint32_t addr;
    uint32_t resume_pc;
    uint32_t range_index;
    uint32_t range_count;
    PsxGameDispatchFn fn;
} PsxGameDispatchEntry;

static const PsxGameDispatchEntry k_psx_fmv_vlc_dispatch[] = {
{0x80104F30u, 0x00000000u, 0u, 1u, func_80104F30},
    {0x80104F40u, 0x80104F40u, 0u, 1u, func_80104F30},
    {0x80104F48u, 0x80104F48u, 0u, 1u, func_80104F30},
    {0x80104F50u, 0x80104F50u, 0u, 1u, func_80104F30},
    {0x80104F58u, 0x80104F58u, 0u, 1u, func_80104F30},
    {0x80104F68u, 0x80104F68u, 0u, 1u, func_80104F30},
    {0x80104F70u, 0x80104F70u, 0u, 1u, func_80104F30},
    {0x80104F80u, 0x80104F80u, 0u, 1u, func_80104F30},
    {0x80104F88u, 0x80104F88u, 0u, 1u, func_80104F30},
    {0x80104F90u, 0x80104F90u, 0u, 1u, func_80104F30},
    {0x80104FA0u, 0x00000000u, 1u, 1u, func_80104FA0},
    {0x80104FE0u, 0x80104FE0u, 1u, 1u, func_80104FA0},
    {0x80104FFCu, 0x80104FFCu, 1u, 1u, func_80104FA0},
    {0x80105014u, 0x80105014u, 1u, 1u, func_80104FA0},
    {0x80105030u, 0x80105030u, 1u, 1u, func_80104FA0},
    {0x80105044u, 0x80105044u, 1u, 1u, func_80104FA0},
    {0x80105050u, 0x80105050u, 1u, 1u, func_80104FA0},
    {0x8010505Cu, 0x8010505Cu, 1u, 1u, func_80104FA0},
    {0x80105068u, 0x80105068u, 1u, 1u, func_80104FA0},
    {0x80105094u, 0x80105094u, 1u, 1u, func_80104FA0},
    {0x801050C8u, 0x801050C8u, 1u, 1u, func_80104FA0},
    {0x801050D0u, 0x801050D0u, 1u, 1u, func_80104FA0},
    {0x801050D8u, 0x801050D8u, 1u, 1u, func_80104FA0},
    {0x8010520Cu, 0x8010520Cu, 1u, 1u, func_80104FA0},
    {0x80105214u, 0x80105214u, 1u, 1u, func_80104FA0},
    {0x80105560u, 0x00000000u, 2u, 1u, func_80105560},
    {0x80105574u, 0x80105574u, 2u, 1u, func_80105560},
    {0x8010557Cu, 0x8010557Cu, 2u, 1u, func_80105560},
    {0x80105584u, 0x80105584u, 2u, 1u, func_80105560},
    {0x80105620u, 0x00000000u, 3u, 1u, func_80105620},
    {0x80105640u, 0x80105640u, 3u, 1u, func_80105620},
    {0x80105658u, 0x80105658u, 3u, 1u, func_80105620},
    {0x8010566Cu, 0x8010566Cu, 3u, 1u, func_80105620},
    {0x80105684u, 0x80105684u, 3u, 1u, func_80105620},
    {0x80105694u, 0x80105694u, 3u, 1u, func_80105620},
    {0x801056A4u, 0x801056A4u, 3u, 1u, func_80105620},
    {0x801056C4u, 0x00000000u, 4u, 1u, func_801056C4},
    {0x801056D4u, 0x801056D4u, 4u, 1u, func_801056C4},
    {0x801056E8u, 0x801056E8u, 4u, 1u, func_801056C4},
    {0x801056F4u, 0x801056F4u, 4u, 1u, func_801056C4},
    {0x80105704u, 0x80105704u, 4u, 1u, func_801056C4},
    {0x80105710u, 0x80105710u, 4u, 1u, func_801056C4},
    {0x80105720u, 0x80105720u, 4u, 1u, func_801056C4},
    {0x80105730u, 0x80105730u, 4u, 1u, func_801056C4},
    {0x80105740u, 0x00000000u, 5u, 1u, func_80105740},
    {0x80105750u, 0x80105750u, 5u, 1u, func_80105740},
    {0x80105760u, 0x00000000u, 6u, 1u, func_80105760},
    {0x8010576Cu, 0x8010576Cu, 6u, 1u, func_80105760},
    {0x80105774u, 0x80105774u, 6u, 1u, func_80105760},
    {0x8010577Cu, 0x8010577Cu, 6u, 1u, func_80105760},
    {0x80105784u, 0x80105784u, 6u, 1u, func_80105760},
    {0x8010578Cu, 0x8010578Cu, 6u, 1u, func_80105760},
    {0x8010579Cu, 0x00000000u, 7u, 1u, func_8010579C},
    {0x801057A8u, 0x801057A8u, 7u, 1u, func_8010579C},
    {0x801057B0u, 0x801057B0u, 7u, 1u, func_8010579C},
    {0x801057B8u, 0x801057B8u, 7u, 1u, func_8010579C},
    {0x801057C0u, 0x801057C0u, 7u, 1u, func_8010579C},
    {0x801057C8u, 0x801057C8u, 7u, 1u, func_8010579C},
    {0x801057D8u, 0x00000000u, 8u, 1u, func_801057D8},
    {0x801057ECu, 0x801057ECu, 8u, 1u, func_801057D8},
    {0x801057FCu, 0x00000000u, 9u, 1u, func_801057FC},
    {0x80105810u, 0x80105810u, 9u, 1u, func_801057FC},
    {0x80105820u, 0x00000000u, 10u, 1u, func_80105820},
    {0x80105830u, 0x80105830u, 10u, 1u, func_80105820},
    {0x8010583Cu, 0x8010583Cu, 10u, 1u, func_80105820},
    {0x80105844u, 0x80105844u, 10u, 1u, func_80105820},
    {0x8010588Cu, 0x8010588Cu, 10u, 1u, func_80105820},
    {0x8010589Cu, 0x8010589Cu, 10u, 1u, func_80105820},
    {0x801058A4u, 0x801058A4u, 10u, 1u, func_80105820},
    {0x801058F4u, 0x801058F4u, 10u, 1u, func_80105820},
    {0x80105900u, 0x80105900u, 10u, 1u, func_80105820},
    {0x80105910u, 0x00000000u, 11u, 1u, func_80105910},
    {0x8010592Cu, 0x8010592Cu, 11u, 1u, func_80105910},
    {0x801059A0u, 0x00000000u, 12u, 1u, func_801059A0},
    {0x801059BCu, 0x801059BCu, 12u, 1u, func_801059A0},
    {0x80105A2Cu, 0x00000000u, 13u, 1u, func_80105A2C},
    {0x80105A58u, 0x80105A58u, 13u, 1u, func_80105A2C},
    {0x80105A5Cu, 0x80105A5Cu, 13u, 1u, func_80105A2C},
    {0x80105A7Cu, 0x80105A7Cu, 13u, 1u, func_80105A2C},
    {0x80105A88u, 0x80105A88u, 13u, 1u, func_80105A2C},
    {0x80105A90u, 0x80105A90u, 13u, 1u, func_80105A2C},
    {0x80105AB0u, 0x80105AB0u, 13u, 1u, func_80105A2C},
    {0x80105AC0u, 0x00000000u, 14u, 1u, func_80105AC0},
    {0x80105AECu, 0x80105AECu, 14u, 1u, func_80105AC0},
    {0x80105AF0u, 0x80105AF0u, 14u, 1u, func_80105AC0},
    {0x80105B10u, 0x80105B10u, 14u, 1u, func_80105AC0},
    {0x80105B1Cu, 0x80105B1Cu, 14u, 1u, func_80105AC0},
    {0x80105B24u, 0x80105B24u, 14u, 1u, func_80105AC0},
    {0x80105B44u, 0x80105B44u, 14u, 1u, func_80105AC0},
    {0x80105B54u, 0x00000000u, 15u, 1u, func_80105B54},
    {0x80105B6Cu, 0x00000000u, 16u, 1u, func_80105B6C},
    {0x80105B88u, 0x80105B88u, 16u, 1u, func_80105B6C},
    {0x80105C20u, 0x00000000u, 17u, 1u, func_80105C20},
    {0x80105C3Cu, 0x80105C3Cu, 17u, 1u, func_80105C20},
    {0x80105C74u, 0x80105C74u, 17u, 1u, func_80105C20},
    {0x80105CACu, 0x80105CACu, 17u, 1u, func_80105C20},
    {0x80105CB0u, 0x80105CB0u, 17u, 1u, func_80105C20},
    {0x80105CE4u, 0x80105CE4u, 17u, 1u, func_80105C20},
    {0x80105CECu, 0x80105CECu, 17u, 1u, func_80105C20},
    {0x80105CF8u, 0x80105CF8u, 17u, 1u, func_80105C20},
    {0x80105D04u, 0x80105D04u, 17u, 1u, func_80105C20},
    {0x80105D08u, 0x80105D08u, 17u, 1u, func_80105C20},
    {0x80105D28u, 0x80105D28u, 17u, 1u, func_80105C20},
    {0x80105D3Cu, 0x80105D3Cu, 17u, 1u, func_80105C20},
    {0x80105D48u, 0x80105D48u, 17u, 1u, func_80105C20},
    {0x80105D4Cu, 0x80105D4Cu, 17u, 1u, func_80105C20},
    {0x80105D5Cu, 0x80105D5Cu, 17u, 1u, func_80105C20},
    {0x80105D6Cu, 0x80105D6Cu, 17u, 1u, func_80105C20},
    {0x80105D78u, 0x80105D78u, 17u, 1u, func_80105C20},
    {0x80105D80u, 0x80105D80u, 17u, 1u, func_80105C20},
    {0x80105D8Cu, 0x80105D8Cu, 17u, 1u, func_80105C20},
    {0x80105D94u, 0x80105D94u, 17u, 1u, func_80105C20},
    {0x80105D98u, 0x80105D98u, 17u, 1u, func_80105C20},
    {0x80105DB4u, 0x80105DB4u, 17u, 1u, func_80105C20},
    {0x80105DBCu, 0x80105DBCu, 17u, 1u, func_80105C20},
    {0x80105DC8u, 0x80105DC8u, 17u, 1u, func_80105C20},
    {0x80105DDCu, 0x80105DDCu, 17u, 1u, func_80105C20},
    {0x80105DECu, 0x80105DECu, 17u, 1u, func_80105C20},
    {0x80105DF4u, 0x80105DF4u, 17u, 1u, func_80105C20},
    {0x80105E00u, 0x80105E00u, 17u, 1u, func_80105C20},
    {0x80105E1Cu, 0x80105E1Cu, 17u, 1u, func_80105C20},
    {0x80105E30u, 0x80105E30u, 17u, 1u, func_80105C20},
    {0x80105E40u, 0x80105E40u, 17u, 1u, func_80105C20},
    {0x80105E5Cu, 0x80105E5Cu, 17u, 1u, func_80105C20},
    {0x80105E60u, 0x80105E60u, 17u, 1u, func_80105C20},
    {0x80105E74u, 0x80105E74u, 17u, 1u, func_80105C20},
    {0x80105E84u, 0x80105E84u, 17u, 1u, func_80105C20},
    {0x80105E94u, 0x80105E94u, 17u, 1u, func_80105C20},
    {0x80105E9Cu, 0x80105E9Cu, 17u, 1u, func_80105C20},
    {0x80105EA4u, 0x80105EA4u, 17u, 1u, func_80105C20},
    {0x80105EB4u, 0x80105EB4u, 17u, 1u, func_80105C20},
    {0x80105EBCu, 0x80105EBCu, 17u, 1u, func_80105C20},
    {0x80105EC8u, 0x80105EC8u, 17u, 1u, func_80105C20},
    {0x80105ED4u, 0x80105ED4u, 17u, 1u, func_80105C20},
    {0x80105EDCu, 0x80105EDCu, 17u, 1u, func_80105C20},
    {0x80105EE4u, 0x80105EE4u, 17u, 1u, func_80105C20},
    {0x80105F08u, 0x80105F08u, 17u, 1u, func_80105C20},
    {0x80105F18u, 0x80105F18u, 17u, 1u, func_80105C20},
    {0x80105F24u, 0x80105F24u, 17u, 1u, func_80105C20},
    {0x80105F30u, 0x80105F30u, 17u, 1u, func_80105C20},
    {0x80105F4Cu, 0x80105F4Cu, 17u, 1u, func_80105C20},
};

static const PsxGameDispatchEntry* psx_fmv_vlc_find_entry(uint32_t addr) {
    uint32_t lo = 0;
    uint32_t hi = sizeof(k_psx_fmv_vlc_dispatch) / sizeof(k_psx_fmv_vlc_dispatch[0]);
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2;
        uint32_t key = k_psx_fmv_vlc_dispatch[mid].addr;
        if (addr < key) hi = mid;
        else if (addr > key) lo = mid + 1;
        else return &k_psx_fmv_vlc_dispatch[mid];
    }
    return 0;
}

static int psx_fmv_vlc_native_ok(uint32_t addr, const PsxGameDispatchEntry* entry) {
    if (!entry || entry->range_count == 0) return 0;
    const uint8_t *ram = memory_get_ram_ptr();
    if (!ram) return 0;
    for (uint32_t i = 0; i < entry->range_count; i++) {
        const PsxGameCodeRange* r = &k_psx_fmv_vlc_ranges[entry->range_index + i];
        uint32_t phys = r->lo & 0x1FFFFFFFu;
        if (phys < 0x00104D58u || phys + r->len > 0x00104D58u + sizeof(s_fmv_vlc_rom)) return 0;
        uint32_t offset = phys - 0x00104D58u;
        if (ram[phys] != s_fmv_vlc_rom[offset]) return 0;
        if (memcmp(ram + phys, s_fmv_vlc_rom + offset, r->len) != 0) return 0;
    }
    return 1;
}

int psx_dispatch_fmv_vlc_compiled(CPUState* cpu, uint32_t addr) {
    const PsxGameDispatchEntry* entry = psx_fmv_vlc_find_entry(addr);
    if (!entry || entry->range_count == 0) {
        return 0;
    }
    if (!psx_fmv_vlc_native_ok(addr, entry)) {
        return 0;
    }
    psx_check_interrupts_dispatch_entry(cpu, addr);
    cpu->pc = entry->resume_pc;
    entry->fn(cpu);
    return 1;
}
