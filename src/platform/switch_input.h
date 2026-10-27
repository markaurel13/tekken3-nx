#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

// Funciones de interfacing en C
bool switch_input_init(void);
void switch_input_update(void);
void switch_input_show_controller_applet(void);
bool switch_input_is_exit_requested(void);
bool switch_input_check_immediate_exit(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// Máscaras de bits para mandos de PS1 (activo en nivel bajo / 0 = pulsado)
enum PS1ButtonBits : uint16_t {
    PS1_SELECT   = (1 << 0),
    PS1_L3       = (1 << 1),
    PS1_R3       = (1 << 2),
    PS1_START    = (1 << 3),
    PS1_UP       = (1 << 4),
    PS1_RIGHT    = (1 << 5),
    PS1_DOWN     = (1 << 6),
    PS1_LEFT     = (1 << 7),
    PS1_L2       = (1 << 8),
    PS1_R2       = (1 << 9),
    PS1_L1       = (1 << 10),
    PS1_R1       = (1 << 11),
    PS1_TRIANGLE = (1 << 12),
    PS1_CIRCLE   = (1 << 13),
    PS1_CROSS    = (1 << 14),
    PS1_SQUARE   = (1 << 15),
};

class SwitchInput {
public:
    static constexpr int MaxPlayers = 2;

    static SwitchInput& Instance();

    bool Init();
    void Update();
    void ShowControllerApplet();
    bool IsExitRequested() const { return m_exitRequested; }

private:
    SwitchInput() = default;
    ~SwitchInput() = default;

    uint16_t TranslatePadState(const PadState& pad, int playerIdx,
                               uint8_t& outLx, uint8_t& outLy,
                               uint8_t& outRx, uint8_t& outRy);

    PadState m_pads[MaxPlayers]{};
    bool m_initialized = false;
    bool m_exitRequested = false;
};

#endif
