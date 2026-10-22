#include "switch_input.h"
#include <cstdio>
#include <cstring>

// Declaraciones de psxrecomp SIO
extern "C" {
    void sio_set_pad_connected(int slot, int connected);
    void sio_set_pad_state_slot(int slot, uint16_t buttons);
    void sio_set_pad_sticks(int slot, uint8_t lx, uint8_t ly, uint8_t rx, uint8_t ry);
}

SwitchInput& SwitchInput::Instance() {
    static SwitchInput s_instance;
    return s_instance;
}

bool SwitchInput::Init() {
    if (m_initialized) return true;
    printf("[SwitchInput] Inicializando sistema de mandos (1 mando completo por jugador, hasta 2 jugadores)...\n");

    // Exigir mandos completos por jugador: Handheld, Pro Controller / FullKey, o par de Joy-Cons (JoyDual).
    // Se desactivan JoyLeft/JoyRight individuales para que cada jugador disponga de todos los botones de Tekken 3.
    padConfigureInput(MaxPlayers, HidNpadStyleTag_NpadFullKey | HidNpadStyleTag_NpadHandheld | HidNpadStyleTag_NpadJoyDual);
    hidSetNpadJoyHoldType(HidNpadJoyHoldType_Vertical);

    // Jugador 1 (Handheld o primer mando completo conectado)
    padInitializeDefault(&m_pads[0]);

    // Jugador 2 (Segundo mando completo conectado automáticamente)
    padInitialize(&m_pads[1], HidNpadIdType_No2);

    m_initialized = true;
    m_exitRequested = false;
    printf("[SwitchInput] Mandos listos (1 mando completo en P1; P2 se activa automáticamente al conectar un 2do mando).\n");
    return true;
}

void SwitchInput::ShowControllerApplet() {
    printf("[SwitchInput] Abriendo menú nativo de conexión de mandos (Change Grip/Order)...\n");

    HidLaControllerSupportArg args{};
    hidLaCreateControllerSupportArg(&args);
    args.hdr.player_count_min = 1;
    args.hdr.player_count_max = 2;
    args.hdr.enable_single_mode = false;
    args.hdr.enable_take_over_connection = 1;
    args.hdr.enable_permit_joy_dual = 1;

    HidLaControllerSupportResultInfo resInfo{};
    hidLaShowControllerSupportForSystem(&resInfo, &args, false);

    printf("[SwitchInput] Menú cerrado. Jugadores conectados: %d\n", resInfo.player_count);
}

uint16_t SwitchInput::TranslatePadState(const PadState& pad, int playerIdx,
                                       uint8_t& outLx, uint8_t& outLy,
                                       uint8_t& outRx, uint8_t& outRy) {
    u64 kHeld = padGetButtons(&pad);

    // En PS1 los botones activos son 0 (activo en nivel bajo), por lo que 0xFFFF = ninguno pulsado
    uint16_t ps1State = 0xFFFF;

    // Tekken 3 Layout estándar de combate:
    // Botón B  -> PS1 Cross (X)     - Patada Izquierda (LK)
    // Botón A  -> PS1 Circle (O)    - Patada Derecha (RK)
    // Botón Y  -> PS1 Square ([])   - Puño Izquierdo (LP)
    // Botón X  -> PS1 Triangle (/\) - Puño Derecho (RP)
    if (kHeld & HidNpadButton_B) ps1State &= ~PS1_CROSS;
    if (kHeld & HidNpadButton_A) ps1State &= ~PS1_CIRCLE;
    if (kHeld & HidNpadButton_Y) ps1State &= ~PS1_SQUARE;
    if (kHeld & HidNpadButton_X) ps1State &= ~PS1_TRIANGLE;

    // D-Pad y Stick Izquierdo
    if (kHeld & HidNpadButton_Up)    ps1State &= ~PS1_UP;
    if (kHeld & HidNpadButton_Down)  ps1State &= ~PS1_DOWN;
    if (kHeld & HidNpadButton_Left)  ps1State &= ~PS1_LEFT;
    if (kHeld & HidNpadButton_Right) ps1State &= ~PS1_RIGHT;

    // Gatillos y hombros
    if (kHeld & HidNpadButton_L)  ps1State &= ~PS1_L1;
    if (kHeld & HidNpadButton_ZL) ps1State &= ~PS1_L2;
    if (kHeld & HidNpadButton_R)  ps1State &= ~PS1_R1;
    if (kHeld & HidNpadButton_ZR) ps1State &= ~PS1_R2;

    // Botones de Joy-Con horizontal (SL / SR)
    if (kHeld & (HidNpadButton_StickL | HidNpadButton_StickR)) {
        // En Joy-Con individual o click de stick
        if (kHeld & HidNpadButton_StickL) ps1State &= ~PS1_L3;
        if (kHeld & HidNpadButton_StickR) ps1State &= ~PS1_R3;
    }

    // Start y Select
    if (kHeld & HidNpadButton_Plus)  ps1State &= ~PS1_START;
    if (kHeld & HidNpadButton_Minus) ps1State &= ~PS1_SELECT;

    // Lectura de sticks analógicos
    HidAnalogStickState stickL = padGetStickPos(&pad, 0);
    HidAnalogStickState stickR = padGetStickPos(&pad, 1);

    // Mapear de [-32768, 32767] a [0, 255] con centro en 128
    outLx = (uint8_t)(((stickL.x + 32768) >> 8) & 0xFF);
    outLy = (uint8_t)(((32767 - stickL.y) >> 8) & 0xFF); // Eje Y invertido
    outRx = (uint8_t)(((stickR.x + 32768) >> 8) & 0xFF);
    outRy = (uint8_t)(((32767 - stickR.y) >> 8) & 0xFF);

    // Inclusión de zonas muertas del stick para mover el D-Pad en Tekken
    if (stickL.y > 16000)  ps1State &= ~PS1_UP;
    if (stickL.y < -16000) ps1State &= ~PS1_DOWN;
    if (stickL.x < -16000) ps1State &= ~PS1_LEFT;
    if (stickL.x > 16000)  ps1State &= ~PS1_RIGHT;

    (void)playerIdx;
    return ps1State;
}

void SwitchInput::Update() {
    if (!m_initialized) return;

    static bool s_p2ConnectedLast = false;

    for (int i = 0; i < MaxPlayers; i++) {
        padUpdate(&m_pads[i]);
        bool connected = padIsConnected(&m_pads[i]);

        if (i == 0) {
            // El puerto 1 de PS1 siempre está conectado al Jugador 1
            sio_set_pad_connected(0, 1);
        } else {
            // El puerto 2 de PS1 se conecta/desconecta dinámicamente cuando se detecta un 2do mando completo
            if (connected != s_p2ConnectedLast) {
                s_p2ConnectedLast = connected;
                printf("[SwitchInput] Mando de Jugador 2 %s.\n", connected ? "CONECTADO" : "DESCONECTADO");
            }
            sio_set_pad_connected(1, connected ? 1 : 0);
        }

        if (connected || i == 0) {
            uint8_t lx = 128, ly = 128, rx = 128, ry = 128;
            uint16_t ps1Buttons = connected ? TranslatePadState(m_pads[i], i, lx, ly, rx, ry) : 0xFFFF;

            // Enviar estado de botones y sticks a la simulación SIO de PS1
            sio_set_pad_state_slot(i, ps1Buttons);
            sio_set_pad_sticks(i, lx, ly, rx, ry);
        }
    }

    // Combinación de salida rápida / retorno a Homebrew Menu: Plus (+) y Minus (-) a la vez en P1
    u64 p1Held = padGetButtons(&m_pads[0]);
    if ((p1Held & HidNpadButton_Plus) && (p1Held & HidNpadButton_Minus)) {
        m_exitRequested = true;
    }

    // Combinación para abrir menú de cambio de mandos en plena partida: ZL + ZR + Minus
    u64 p1Down = padGetButtonsDown(&m_pads[0]);
    if ((p1Held & HidNpadButton_ZL) && (p1Held & HidNpadButton_ZR) && (p1Down & HidNpadButton_Minus)) {
        ShowControllerApplet();
    }
}

// Wrapper C
extern "C" {

bool switch_input_init(void) {
    return SwitchInput::Instance().Init();
}

void switch_input_update(void) {
    SwitchInput::Instance().Update();
}

void switch_input_show_controller_applet(void) {
    SwitchInput::Instance().ShowControllerApplet();
}

bool switch_input_is_exit_requested(void) {
    return SwitchInput::Instance().IsExitRequested();
}

}
