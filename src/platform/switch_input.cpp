#include "switch_input.h"
#include "switch_config_menu.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

// Declaraciones de psxrecomp SIO
extern "C" {
    void sio_set_pad_connected(int slot, int connected);
    void sio_set_pad_state_slot(int slot, uint16_t buttons);
    void sio_set_pad_sticks(int slot, uint8_t lx, uint8_t ly, uint8_t rx, uint8_t ry);
    void tekken3_outfits_connect(int player, int connected);
    uint16_t tekken3_outfits_input(int player, uint16_t buttons);
    void host_osd_fps_toggle(void);
}

SwitchInput& SwitchInput::Instance() {
    static SwitchInput s_instance;
    return s_instance;
}

bool SwitchInput::Init() {
    if (m_initialized) return true;
    printf("[SwitchInput] Inicializando sistema de mandos con soporte para Joy-Cons individuales y duales...\n");

    // Permitir todos los tipos de mandos: Handheld, Pro Controller / FullKey, par de Joy-Cons (JoyDual),
    // y Joy-Cons individuales horizontales (JoyLeft / JoyRight).
    padConfigureInput(MaxPlayers, HidNpadStyleTag_NpadFullKey | HidNpadStyleTag_NpadHandheld |
                                  HidNpadStyleTag_NpadJoyDual | HidNpadStyleTag_NpadJoyLeft |
                                  HidNpadStyleTag_NpadJoyRight);
    hidSetNpadJoyHoldType(HidNpadJoyHoldType_Horizontal);

    // Jugador 1 (Handheld o primer mando conectado)
    padInitializeDefault(&m_pads[0]);

    // Jugador 2 (Segundo mando conectado automáticamente)
    padInitialize(&m_pads[1], HidNpadIdType_No2);

    m_initialized = true;
    m_exitRequested = false;
    printf("[SwitchInput] Mandos listos (Soporte Joy-Cons individuales horizontales activado).\n");
    return true;
}

void SwitchInput::ShowControllerApplet() {
    printf("[SwitchInput] Abriendo menú nativo de conexión de mandos (Change Grip/Order)...\n");

    // Asegurar que el modo de sujeción horizontal esté configurado
    hidSetNpadJoyHoldType(HidNpadJoyHoldType_Horizontal);

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

void SwitchInput::SuppressGameInput(int frames) {
    m_suppress_frames = frames;
    sio_set_pad_state_slot(0, 0xFFFF);
    sio_set_pad_state_slot(1, 0xFFFF);
}

uint16_t SwitchInput::TranslatePadState(const PadState& pad, int playerIdx,
                                       uint8_t& outLx, uint8_t& outLy,
                                       uint8_t& outRx, uint8_t& outRy) {
    u64 kHeld = padGetButtons(&pad);
    u32 style = padGetStyleSet(&pad);
    bool isSingleJoy = (style & (HidNpadStyleTag_NpadJoyLeft | HidNpadStyleTag_NpadJoyRight)) != 0;

    // En PS1 los botones activos son 0 (activo en nivel bajo), por lo que 0xFFFF = ninguno pulsado
    uint16_t ps1State = 0xFFFF;

    if (isSingleJoy) {
        if (style & HidNpadStyleTag_NpadJoyRight) {
            // Joy-Con Derecho individual en horizontal:
            // Mapeo solicitado por el usuario:
            // Y: Triángulo (RP)
            // B: Cuadrado (LP)
            // A: Equis (LK)
            // X: Círculo (RK)
            if (kHeld & HidNpadButton_Y) ps1State &= ~PS1_TRIANGLE;
            if (kHeld & HidNpadButton_B) ps1State &= ~PS1_SQUARE;
            if (kHeld & HidNpadButton_A) ps1State &= ~PS1_CROSS;
            if (kHeld & HidNpadButton_X) ps1State &= ~PS1_CIRCLE;
        } else {
            // Joy-Con Izquierdo individual (flechas d-pad):
            // Definido por el usuario en posición VERTICAL exacta de la cruceta:
            // Flecha Arriba    -> Cuadrado (LP)
            // Flecha Abajo     -> Círculo (RK)
            // Flecha Izquierda -> Equis / Cross (LK)
            // Flecha Derecha   -> Triángulo (RP)
            if (kHeld & HidNpadButton_Up)    ps1State &= ~PS1_SQUARE;
            if (kHeld & HidNpadButton_Down)  ps1State &= ~PS1_CIRCLE;
            if (kHeld & HidNpadButton_Left)  ps1State &= ~PS1_CROSS;
            if (kHeld & HidNpadButton_Right) ps1State &= ~PS1_TRIANGLE;
        }

        // Gatillos SL y SR asignados a L2 y R2
        if (kHeld & (HidNpadButton_LeftSL | HidNpadButton_RightSL)) ps1State &= ~PS1_L2;
        if (kHeld & (HidNpadButton_LeftSR | HidNpadButton_RightSR)) ps1State &= ~PS1_R2;

        // Botón + o - como Start
        if (kHeld & (HidNpadButton_Plus | HidNpadButton_Minus)) ps1State &= ~PS1_START;
    } else {
        // Layout estándar para mandos completos (Handheld, Pro Controller, JoyDual):
        if (kHeld & HidNpadButton_B) ps1State &= ~PS1_CROSS;
        if (kHeld & HidNpadButton_A) ps1State &= ~PS1_CIRCLE;
        if (kHeld & HidNpadButton_Y) ps1State &= ~PS1_SQUARE;
        if (kHeld & HidNpadButton_X) ps1State &= ~PS1_TRIANGLE;

        if (kHeld & HidNpadButton_Up)    ps1State &= ~PS1_UP;
        if (kHeld & HidNpadButton_Down)  ps1State &= ~PS1_DOWN;
        if (kHeld & HidNpadButton_Left)  ps1State &= ~PS1_LEFT;
        if (kHeld & HidNpadButton_Right) ps1State &= ~PS1_RIGHT;

        if (kHeld & HidNpadButton_L)  ps1State &= ~PS1_L1;
        if (kHeld & HidNpadButton_ZL) ps1State &= ~PS1_L2;
        if (kHeld & HidNpadButton_R)  ps1State &= ~PS1_R1;
        if (kHeld & HidNpadButton_ZR) ps1State &= ~PS1_R2;

        if (kHeld & HidNpadButton_Plus)  ps1State &= ~PS1_START;
        if (kHeld & HidNpadButton_Minus) ps1State &= ~PS1_SELECT;
        if (kHeld & HidNpadButton_StickL) ps1State &= ~PS1_L3;
        if (kHeld & HidNpadButton_StickR) ps1State &= ~PS1_R3;
    }

    // Lectura de sticks analógicos
    HidAnalogStickState stickL = padGetStickPos(&pad, 0);
    HidAnalogStickState stickR = padGetStickPos(&pad, 1);

    int32_t stick_x = stickL.x;
    int32_t stick_y = stickL.y;

    if (isSingleJoy) {
        if (style & HidNpadStyleTag_NpadJoyRight) {
            // Joy-Con Derecho en horizontal (girado 90° en sentido horario):
            // Arriba en horizontal = izquierda en vertical (-X_raw) -> Y_horiz = -X_raw
            // Abajo en horizontal = derecha en vertical (+X_raw)   -> Y_horiz = -X_raw
            // Izquierda en horizontal = abajo en vertical (-Y_raw) -> X_horiz = Y_raw
            // Derecha en horizontal = arriba en vertical (+Y_raw)  -> X_horiz = Y_raw
            int32_t raw_x = (stickR.x != 0 || stickR.y != 0) ? stickR.x : stickL.x;
            int32_t raw_y = (stickR.x != 0 || stickR.y != 0) ? stickR.y : stickL.y;
            stick_x = raw_y;
            stick_y = -raw_x;
        } else {
            // Joy-Con Izquierdo en horizontal (girado 90° en sentido anti-horario):
            // Arriba en horizontal = derecha en vertical (+X_raw) -> Y_horiz = X_raw
            // Abajo en horizontal = izquierda en vertical (-X_raw) -> Y_horiz = X_raw
            // Izquierda en horizontal = arriba en vertical (+Y_raw) -> X_horiz = -Y_raw
            // Derecha en horizontal = abajo en vertical (-Y_raw) -> X_horiz = -Y_raw
            int32_t raw_x = stickL.x;
            int32_t raw_y = stickL.y;
            stick_x = -raw_y;
            stick_y = raw_x;
        }

        // En Joy-Con horizontal, las direcciones del D-Pad se activan con el stick correctamente rotado
        if (stick_y > 16000)  ps1State &= ~PS1_UP;
        if (stick_y < -16000) ps1State &= ~PS1_DOWN;
        if (stick_x < -16000) ps1State &= ~PS1_LEFT;
        if (stick_x > 16000)  ps1State &= ~PS1_RIGHT;

        static uint32_t s_joy_log_cnt = 0;
        if ((stick_x != 0 || stick_y != 0) && (++s_joy_log_cnt % 60 == 1)) {
            printf("[JoyStick] P%d (%s): raw=(%d,%d) -> rot=(%d,%d) ps1=0x%04x\n",
                   playerIdx + 1, (style & HidNpadStyleTag_NpadJoyRight) ? "JoyRight" : "JoyLeft",
                   (style & HidNpadStyleTag_NpadJoyRight) ? stickR.x : stickL.x,
                   (style & HidNpadStyleTag_NpadJoyRight) ? stickR.y : stickL.y,
                   stick_x, stick_y, (unsigned)ps1State);
            fflush(stdout);
        }
    } else {
        // Zonas muertas del stick para asegurar registro en D-Pad en mandos completos
        if (stick_y > 16000)  ps1State &= ~PS1_UP;
        if (stick_y < -16000) ps1State &= ~PS1_DOWN;
        if (stick_x < -16000) ps1State &= ~PS1_LEFT;
        if (stick_x > 16000)  ps1State &= ~PS1_RIGHT;
    }

    // Mapear de [-32768, 32767] a [0, 255] con centro en 128 para PS1 SIO
    outLx = (uint8_t)(((stick_x + 32768) >> 8) & 0xFF);
    outLy = (uint8_t)(((32767 - stick_y) >> 8) & 0xFF); // Eje Y invertido (arriba = 0, abajo = 255)
    outRx = (uint8_t)(((stickR.x + 32768) >> 8) & 0xFF);
    outRy = (uint8_t)(((32767 - stickR.y) >> 8) & 0xFF);

    (void)playerIdx;
    return ps1State;
}

void SwitchInput::Update() {
    if (!m_initialized) return;

    if (m_suppress_frames > 0) {
        m_suppress_frames--;
        sio_set_pad_state_slot(0, 0xFFFF);
        sio_set_pad_state_slot(1, 0xFFFF);
        return;
    }

    u64 start_tick = armGetSystemTick();
    static bool s_p2ConnectedLast = false;
    static uint64_t s_update_count = 0;
    static uint64_t s_total_update_ns = 0;
    static uint64_t s_max_update_ns = 0;

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

            tekken3_outfits_connect(i, (connected || i == 0) ? 1 : 0);
            ps1Buttons = tekken3_outfits_input(i, ps1Buttons);

            // Enviar estado de botones y sticks a la simulación SIO de PS1
            sio_set_pad_state_slot(i, ps1Buttons);
            sio_set_pad_sticks(i, lx, ly, rx, ry);
        } else {
            tekken3_outfits_connect(i, 0);
        }
    }

    // NOTA: Se eliminó la combinación de salida Plus (+) + Minus (-) porque en Tekken 3
    // Start + Select es una combinación nativa del juego (ej. reiniciar posición / menú en Practice Mode)
    // y el cierre de la aplicación se maneja de forma nativa mediante el botón HOME de la Switch.

    // Combinación para abrir menú de cambio de mandos en cualquier momento:
    // Mando completo: ZL + ZR + Minus  o  L + R + Minus
    // Joy-Con individual horizontal: SL + SR + Minus/Plus
    u64 p1Held = padGetButtons(&m_pads[0]);
    u64 p1Down = padGetButtonsDown(&m_pads[0]);
    bool combo_full = ((p1Held & HidNpadButton_ZL) && (p1Held & HidNpadButton_ZR) && (p1Down & HidNpadButton_Minus)) ||
                      ((p1Held & HidNpadButton_L) && (p1Held & HidNpadButton_R) && (p1Down & HidNpadButton_Minus));
    bool combo_single = (p1Held & (HidNpadButton_LeftSL | HidNpadButton_RightSL)) &&
                        (p1Held & (HidNpadButton_LeftSR | HidNpadButton_RightSR)) &&
                        (p1Down & (HidNpadButton_Minus | HidNpadButton_Plus));
    if (combo_full || combo_single) {
        ShowControllerApplet();
    }

    // Combinación para abrir menú de configuración dinámica (config.txt):
    // Mando completo: ZL + ZR + Plus  o  L + R + Plus
    // Joy-Con individual horizontal: SL + SR + Plus
    // Acceso alternativo por conveniencia: Mantener Minus (-) durante ~1.15 segundos (70 frames)
    bool cfg_combo_full = ((p1Held & HidNpadButton_ZL) && (p1Held & HidNpadButton_ZR) && (p1Down & HidNpadButton_Plus)) ||
                          ((p1Held & HidNpadButton_L) && (p1Held & HidNpadButton_R) && (p1Down & HidNpadButton_Plus));
    bool cfg_combo_single = (p1Held & (HidNpadButton_LeftSL | HidNpadButton_RightSL)) &&
                            (p1Held & (HidNpadButton_LeftSR | HidNpadButton_RightSR)) &&
                            (p1Down & HidNpadButton_Plus);

    static uint32_t s_minus_hold_frames = 0;
    if (p1Held & HidNpadButton_Minus) {
        s_minus_hold_frames++;
    } else {
        s_minus_hold_frames = 0;
    }
    bool cfg_minus_hold = (s_minus_hold_frames == 70);

    if (cfg_combo_full || cfg_combo_single || cfg_minus_hold) {
        switch_config_menu_request_open();
    }

    // Combinación para alternar el contador de FPS en pantalla:
    // Minus (-) + presionar ambos sticks analógicos (L3 + R3)
    bool minus_held = (p1Held & HidNpadButton_Minus) != 0;
    bool stick_l_held = (p1Held & HidNpadButton_StickL) != 0;
    bool stick_r_held = (p1Held & HidNpadButton_StickR) != 0;
    if (minus_held && stick_l_held && (p1Down & HidNpadButton_StickR)) {
        host_osd_fps_toggle();
    } else if (minus_held && stick_r_held && (p1Down & HidNpadButton_StickL)) {
        host_osd_fps_toggle();
    } else if (stick_l_held && stick_r_held && (p1Down & HidNpadButton_Minus)) {
        host_osd_fps_toggle();
    }

    u64 end_tick = armGetSystemTick();
    uint64_t update_ns = armTicksToNs(end_tick - start_tick);
    s_update_count++;
    s_total_update_ns += update_ns;
    if (update_ns > s_max_update_ns) s_max_update_ns = update_ns;
    if (s_update_count <= 5 || (s_update_count % 120) == 0) {
        double avg_ns = s_update_count ? (double)s_total_update_ns / (double)s_update_count : 0.0;
        printf("[InputPerf] samples=%llu avg_ns=%.0f max_ns=%llu p2=%d exit=%d\n",
               (unsigned long long)s_update_count,
               avg_ns,
               (unsigned long long)s_max_update_ns,
               s_p2ConnectedLast ? 1 : 0,
               m_exitRequested ? 1 : 0);
    }
}

void SwitchInput::PollMenuInput(uint64_t& outDown, uint64_t& outHeld, int32_t& stickX, int32_t& stickY) {
    outDown = 0;
    outHeld = 0;
    stickX = 0;
    stickY = 0;
    for (int i = 0; i < MaxPlayers; i++) {
        padUpdate(&m_pads[i]);
        if (!padIsConnected(&m_pads[i]) && i > 0) continue;
        u64 kd = padGetButtonsDown(&m_pads[i]);
        u64 kh = padGetButtons(&m_pads[i]);
        u32 style = padGetStyleSet(&m_pads[i]);
        bool isSingleJoy = (style & (HidNpadStyleTag_NpadJoyLeft | HidNpadStyleTag_NpadJoyRight)) != 0;

        HidAnalogStickState s0 = padGetStickPos(&m_pads[i], 0);
        HidAnalogStickState s1 = padGetStickPos(&m_pads[i], 1);
        int32_t raw_x = (s1.x != 0 || s1.y != 0) ? s1.x : s0.x;
        int32_t raw_y = (s1.y != 0 || s1.y != 0) ? s1.y : s0.y;

        int32_t rot_x = raw_x;
        int32_t rot_y = raw_y;
        if (isSingleJoy) {
            if (style & HidNpadStyleTag_NpadJoyRight) {
                rot_x = raw_y;
                rot_y = -raw_x;
            } else {
                rot_x = -raw_y;
                rot_y = raw_x;
            }
        }

        outDown |= kd;
        outHeld |= kh;
        if (std::abs(rot_x) > std::abs(stickX)) stickX = rot_x;
        if (std::abs(rot_y) > std::abs(stickY)) stickY = rot_y;
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

void switch_input_poll_menu(uint64_t* outDown, uint64_t* outHeld, int32_t* stickX, int32_t* stickY) {
    if (outDown && outHeld && stickX && stickY) {
        SwitchInput::Instance().PollMenuInput(*outDown, *outHeld, *stickX, *stickY);
    }
}

void switch_input_show_controller_applet(void) {
    SwitchInput::Instance().ShowControllerApplet();
}

void switch_input_suppress_game_input(int frames) {
    SwitchInput::Instance().SuppressGameInput(frames);
}

bool switch_input_is_exit_requested(void) {
    return false;
}

bool switch_input_check_immediate_exit(void) {
    return false;
}

}
