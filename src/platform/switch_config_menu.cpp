#include "switch_config_menu.h"
#include "switch_audio.h"
#include "switch_input.h"
#include "renderer/deko3d_renderer.h"

#include <switch.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

// Fuentes 8x8 de dominio público
#include "psxrecomp/runtime/src/host_osd_font.inl"

static constexpr int MENU_W = 820;
static constexpr int MENU_H = 440;
static uint32_t s_menu_img[MENU_W * MENU_H];

static char s_config_path[512] = "sdmc:/switch/tekken3/config.txt";
static char s_nro_path[512]    = "sdmc:/switch/tekken3/tekken3.nro";

static bool s_menu_requested = false;
static bool s_menu_open      = false;
static int  s_selected_item  = 0; // 0..4

static int  s_orig_ssaa   = 2;
static int  s_cur_ssaa    = 2;
static bool s_orig_wide   = false;
static bool s_cur_wide    = false;
static bool s_orig_unlock = false;
static bool s_cur_unlock  = false;

static bool s_restarting = false;
static const char* s_status_msg = nullptr;
static int s_status_timer = 0;
static uint32_t s_anim_counter = 0;

void switch_config_menu_init(const char* config_path, const char* nro_path, int ssaa, bool widescreen, bool unlock_all) {
    if (config_path && config_path[0] != '\0') {
        strncpy(s_config_path, config_path, sizeof(s_config_path) - 1);
    } else {
        strncpy(s_config_path, "sdmc:/switch/tekken3/config.txt", sizeof(s_config_path) - 1);
    }

    if (nro_path && nro_path[0] != '\0') {
        if (strncmp(nro_path, "sdmc:/", 6) == 0) {
            strncpy(s_nro_path, nro_path, sizeof(s_nro_path) - 1);
        } else if (nro_path[0] == '/') {
            snprintf(s_nro_path, sizeof(s_nro_path), "sdmc:%s", nro_path);
        } else {
            snprintf(s_nro_path, sizeof(s_nro_path), "sdmc:/switch/tekken3/%s", nro_path);
        }
    } else {
        if (access("sdmc:/switch/tekken3/tekken3.nro", F_OK) == 0) {
            strncpy(s_nro_path, "sdmc:/switch/tekken3/tekken3.nro", sizeof(s_nro_path) - 1);
        } else if (access("sdmc:/switch/tekken3/tekken3_nx.nro", F_OK) == 0) {
            strncpy(s_nro_path, "sdmc:/switch/tekken3/tekken3_nx.nro", sizeof(s_nro_path) - 1);
        } else {
            strncpy(s_nro_path, "sdmc:/switch/tekken3/tekken3.nro", sizeof(s_nro_path) - 1);
        }
    }

    switch_config_menu_sync_values(ssaa, widescreen, unlock_all);
    printf("[ConfigMenu] Initialized: config='%s', nro='%s', ssaa=%d, wide=%d, unlock=%d\n",
           s_config_path, s_nro_path, ssaa, widescreen ? 1 : 0, unlock_all ? 1 : 0);
}

void switch_config_menu_sync_values(int ssaa, bool widescreen, bool unlock_all) {
    s_orig_ssaa   = (ssaa >= 1 && ssaa <= 3) ? ssaa : 2;
    s_cur_ssaa    = s_orig_ssaa;
    s_orig_wide   = widescreen;
    s_cur_wide    = s_orig_wide;
    s_orig_unlock = unlock_all;
    s_cur_unlock  = s_orig_unlock;
}

void switch_config_menu_request_open(void) {
    s_menu_requested = true;
}

bool switch_config_menu_is_requested(void) {
    return s_menu_requested;
}

bool switch_config_menu_is_open(void) {
    return s_menu_open;
}

bool switch_config_menu_get_image(const uint32_t** out_pixels, int* out_w, int* out_h) {
    if (!s_menu_open || !out_pixels || !out_w || !out_h) return false;
    *out_pixels = s_menu_img;
    *out_w = MENU_W;
    *out_h = MENU_H;
    return true;
}

// -----------------------------------------------------------------------------
// Primitivas de renderizado de texto y rectángulos para ARGB8888
// -----------------------------------------------------------------------------
static void fill_rect(int rx, int ry, int rw, int rh, uint32_t color) {
    int x1 = rx < 0 ? 0 : rx;
    int y1 = ry < 0 ? 0 : ry;
    int x2 = rx + rw;
    int y2 = ry + rh;
    if (x2 > MENU_W) x2 = MENU_W;
    if (y2 > MENU_H) y2 = MENU_H;
    for (int y = y1; y < y2; y++) {
        uint32_t* row = &s_menu_img[y * MENU_W];
        for (int x = x1; x < x2; x++) {
            row[x] = color;
        }
    }
}

static void draw_rect_border(int rx, int ry, int rw, int rh, uint32_t color, int border_thick) {
    fill_rect(rx, ry, rw, border_thick, color);
    fill_rect(rx, ry + rh - border_thick, rw, border_thick, color);
    fill_rect(rx, ry, border_thick, rh, color);
    fill_rect(rx + rw - border_thick, ry, border_thick, rh, color);
}

static void draw_char(int x, int y, char c, uint32_t color, int scale) {
    if (c < 32 || c > 126) c = '?';
    const uint8_t* g = FONT8X8[c - 32];
    for (int r = 0; r < 8; r++) {
        uint8_t row_bits = g[r];
        for (int col = 0; col < 8; col++) {
            if (!(row_bits & (1u << col))) continue;
            for (int dy = 0; dy < scale; dy++) {
                int py = y + r * scale + dy;
                if (py < 0 || py >= MENU_H) continue;
                for (int dx = 0; dx < scale; dx++) {
                    int px = x + col * scale + dx;
                    if (px < 0 || px >= MENU_W) continue;
                    s_menu_img[py * MENU_W + px] = color;
                }
            }
        }
    }
}

static void draw_text(int x, int y, const char* str, uint32_t color, int scale, bool shadow = true) {
    int cur_x = x;
    while (*str) {
        if (shadow) {
            draw_char(cur_x + scale, y + scale, *str, 0xFF000000u, scale);
        }
        draw_char(cur_x, y, *str, color, scale);
        cur_x += 8 * scale;
        str++;
    }
}

// -----------------------------------------------------------------------------
// Guardar archivo config.txt
// -----------------------------------------------------------------------------
static bool save_config_file(int ssaa, bool wide, bool unlock) {
    FILE* f = fopen(s_config_path, "w");
    if (!f) {
        f = fopen("sdmc:/switch/tekken3/config.txt", "w");
        if (f) strncpy(s_config_path, "sdmc:/switch/tekken3/config.txt", sizeof(s_config_path) - 1);
    }
    if (!f) {
        printf("[ConfigMenu] ERROR: Could not open '%s' for writing!\n", s_config_path);
        return false;
    }

    fprintf(f,
        "# ==============================================================================\n"
        "# Tekken 3 (USA) - Graphics & Game Configuration for Nintendo Switch\n"
        "# ==============================================================================\n"
        "# Place this file in the same directory as 'tekken3_nx.nro' on your microSD card\n"
        "# (e.g. sdmc:/switch/tekken3/config.txt).\n"
        "# If this file is missing, the game will automatically create it with default values.\n"
        "#\n"
        "# ------------------------------------------------------------------------------\n"
        "# Internal Supersampling Anti-Aliasing (SSAA):\n"
        "#   1 = 1x PS1 Native (1024x512)\n"
        "#   2 = 2x HD (2048x1024) [RECOMMENDED]\n"
        "#       -> Sharp edges, optimal configuration\n"
        "#   3 = 3x Ultra HD (3072x1536)\n"
        "#       -> Maximum polygon edge definition for 4K / large TV screens\n"
        "# ------------------------------------------------------------------------------\n"
        "supersampling = %d\n\n"
        "# ------------------------------------------------------------------------------\n"
        "# Screen Aspect Ratio:\n"
        "#   OFF = 4:3 Original with black sidebars (Pillarbox) [RECOMMENDED]\n"
        "#         -> Preserves authentic retro proportions for characters and stages.\n"
        "#   ON  = 16:9 True Widescreen (Native-Wide Sidecar)\n"
        "#         -> Real panoramic 16:9 3D view with expanded FOV, correct geometry,\n"
        "#            and non-stretched characters/stages (PC Tekken3Recompiled feature).\n"
        "# ------------------------------------------------------------------------------\n"
        "widescreen_16_9 = %s\n\n"
        "# ------------------------------------------------------------------------------\n"
        "# Unlock All Characters & Modes:\n"
        "#   0 = OFF: Authentic progression. Loads only characters unlocked in your save/memory card. [DEFAULT]\n"
        "#   1 = ON: Automatically unlocks all characters, secret costumes, and modes.\n"
        "#       WARNING: If you save your game while ON (1), your memory card will record\n"
        "#       100%% progression with all fighters and modes permanently unlocked.\n"
        "# ------------------------------------------------------------------------------\n"
        "unlocked_all_characters = %d\n",
        ssaa,
        wide ? "ON" : "OFF",
        unlock ? 1 : 0
    );

    fflush(f);
    fsync(fileno(f));
    fclose(f);
    printf("[ConfigMenu] Saved '%s' successfully (SSAA=%d, Wide=%d, Unlock=%d).\n",
           s_config_path, ssaa, wide ? 1 : 0, unlock ? 1 : 0);
    return true;
}

// -----------------------------------------------------------------------------
// Generación visual del menú (Rasterize)
// -----------------------------------------------------------------------------
static void switch_config_menu_rasterize(void) {
    s_anim_counter++;

    // Fondo translúcido oscuro con tono pizarra elegante (93% opaco)
    fill_rect(0, 0, MENU_W, MENU_H, 0xEE121824u);

    // Marco exterior e interior
    draw_rect_border(0, 0, MENU_W, MENU_H, 0xFF00E5FFu, 3);
    draw_rect_border(4, 4, MENU_W - 8, MENU_H - 8, 0xFF005577u, 1);

    // Barra de título superior
    fill_rect(6, 6, MENU_W - 12, 38, 0xFF1B2838u);
    fill_rect(6, 44, MENU_W - 12, 2, 0xFF00E5FFu);
    draw_text(178, 16, "TEKKEN 3 - CONFIGURATION MENU", 0xFFFFD700u, 2);

    // Opciones (coordenadas espaciadas y sin solapamiento)
    const int item_y[5] = { 66, 110, 154, 220, 264 };

    // Si una fila está seleccionada, dibujar franja de resalte
    if (s_selected_item >= 0 && s_selected_item < 5) {
        int ry = item_y[s_selected_item] - 4;
        fill_rect(16, ry, MENU_W - 32, 28, 0xFF1E3A5Fu);
        draw_rect_border(16, ry, MENU_W - 32, 28, 0xFF00D4FFu, 1);
    }

    // Item 0: Supersampling (SSAA)
    {
        bool sel = (s_selected_item == 0);
        draw_text(36, item_y[0], sel ? "> Supersampling :" : "  Supersampling :", sel ? 0xFFFFFFFFu : 0xFFB0C4DEu, 2);
        const char* val_str = "< 2x HD [Optimal]  >";
        if (s_cur_ssaa == 1) val_str = "< 1x Native (PS1)  >";
        else if (s_cur_ssaa == 3) val_str = "< 3x Ultra HD (4K) >";
        uint32_t val_color = sel ? 0xFFFFE066u : 0xFF55FFFFu;
        draw_text(380, item_y[0], val_str, val_color, 2);
    }

    // Item 1: Relación de Aspecto
    {
        bool sel = (s_selected_item == 1);
        draw_text(36, item_y[1], sel ? "> Aspect Ratio  :" : "  Aspect Ratio  :", sel ? 0xFFFFFFFFu : 0xFFB0C4DEu, 2);
        const char* val_str = s_cur_wide ? "< 16:9 Widescreen    >" : "< 4:3 Original (4:3) >";
        uint32_t val_color = sel ? 0xFFFFE066u : 0xFF55FFFFu;
        draw_text(380, item_y[1], val_str, val_color, 2);
    }

    // Item 2: Desbloquear Todo
    {
        bool sel = (s_selected_item == 2);
        draw_text(36, item_y[2], sel ? "> Unlock All    :" : "  Unlock All    :", sel ? 0xFFFFFFFFu : 0xFFB0C4DEu, 2);
        const char* val_str = s_cur_unlock ? "< 1: ON (All 100%)   >" : "< 0: OFF (MemCard)   >";
        uint32_t val_color = sel ? 0xFFFFE066u : 0xFF55FFFFu;
        draw_text(380, item_y[2], val_str, val_color, 2);
    }

    // Línea separadora previa a botones de acción
    fill_rect(24, 198, MENU_W - 48, 1, 0xFF334455u);

    // Item 3: [ SAVE & APPLY ]
    {
        bool sel = (s_selected_item == 3);
        const char* label = sel ? "--> [ SAVE & APPLY ] <--" : "    [ SAVE & APPLY ]    ";
        uint32_t color = sel ? 0xFF55FF55u : 0xFF88CC88u;
        draw_text(210, item_y[3], label, color, 2);
    }

    // Item 4: [ CANCEL & EXIT ]
    {
        bool sel = (s_selected_item == 4);
        const char* label = sel ? "--> [ CANCEL & EXIT ] <--" : "    [ CANCEL & EXIT ]    ";
        uint32_t color = sel ? 0xFFFF6B6Bu : 0xFFCC8888u;
        draw_text(202, item_y[4], label, color, 2);
    }

    // Línea separadora inferior
    fill_rect(24, 308, MENU_W - 48, 1, 0xFF334455u);

    // Sección de instrucciones (a escala 1x para máxima nitidez y amplitud)
    draw_text(36, 320, "D-Pad / Left Stick: Navigate cursor    |    Left / Right: Change selected setting", 0xFFAAAAAAu, 1);
    draw_text(36, 340, "A Button: Confirm / Toggle value        |    B Button / ZL+ZR+Plus: Cancel & Exit", 0xFFAAAAAAu, 1);
    draw_text(36, 362, "* NOTE: If SSAA, Aspect Ratio or Fighters are modified, Tekken 3 will automatically", 0xFFFFB84Du, 1);
    draw_text(36, 378, "  restart to apply the new hardware configuration immediately.", 0xFFFFB84Du, 1);

    // Mensaje de estado temporal
    if (s_status_msg) {
        fill_rect(30, 402, MENU_W - 60, 24, 0xFF003322u);
        draw_rect_border(30, 402, MENU_W - 60, 24, 0xFF00FF88u, 1);
        draw_text(40, 408, s_status_msg, 0xFF00FFAAu, 1);
    }

    // Cuadro modal centrado si se está ejecutando el auto-reinicio
    if (s_restarting) {
        fill_rect(50, 110, MENU_W - 100, 190, 0xF80E1724u);
        draw_rect_border(50, 110, MENU_W - 100, 190, 0xFFFFD700u, 3);
        draw_text(122, 138, "SAVING CONFIGURATION & RESTARTING...", 0xFFFFD700u, 2);
        draw_text(130, 185, "Tekken 3 will restart automatically", 0xFFFFFFFFu, 2);
        draw_text(130, 220, "to apply the new engine settings...", 0xFF00E5FFu, 2);
    }
}

// -----------------------------------------------------------------------------
// Bucle modal del menú (ejecutado mientras el juego está pausado)
// -----------------------------------------------------------------------------
void switch_config_menu_run_modal(void) {
    s_menu_requested = false;
    s_menu_open = true;
    s_restarting = false;
    s_status_msg = nullptr;
    s_status_timer = 0;

    printf("[ConfigMenu] Menu opened. Game paused at VBlank.\n");

    // Limpiar y suprimir inputs residuales al abrir el menú
    switch_input_suppress_game_input(10);

    // Pausar audio limpio sin zumbidos ni underruns
    switch_audio_set_paused(true);

    int stick_repeat_delay = 0;

    while (s_menu_open) {
        // Control de salida del sistema oficial (botón HOME o suspensión)
        if (!appletMainLoop()) {
            s_menu_open = false;
            break;
        }

        if (appletGetFocusState() != AppletFocusState_InFocus) {
            svcSleepThread(16000000ULL);
            continue;
        }

        // Notificar avance de frame para evitar falsas alertas del watchdog
        switch_notify_menu_frame();

        // Lectura de mandos
        uint64_t kDown = 0;
        uint64_t kHeld = 0;
        int32_t stickX = 0;
        int32_t stickY = 0;
        switch_input_poll_menu(&kDown, &kHeld, &stickX, &stickY);

        // Control de repetición de stick analógico
        bool stick_up    = (stickY > 18000);
        bool stick_down  = (stickY < -18000);
        bool stick_left  = (stickX < -18000);
        bool stick_right = (stickX > 18000);

        bool nav_up = false, nav_down = false, nav_left = false, nav_right = false;

        if (kDown & (HidNpadButton_Up | HidNpadButton_LeftSL)) nav_up = true;
        if (kDown & (HidNpadButton_Down | HidNpadButton_LeftSR)) nav_down = true;
        if (kDown & HidNpadButton_Left) nav_left = true;
        if (kDown & HidNpadButton_Right) nav_right = true;

        if (!stick_up && !stick_down && !stick_left && !stick_right) {
            stick_repeat_delay = 0;
        } else {
            if (stick_repeat_delay == 0) {
                if (stick_up) nav_up = true;
                if (stick_down) nav_down = true;
                if (stick_left) nav_left = true;
                if (stick_right) nav_right = true;
                stick_repeat_delay = 18; // ~300ms antes del primer repeat
            } else {
                stick_repeat_delay--;
                if (stick_repeat_delay == 1) {
                    stick_repeat_delay = 7; // repeat continuo cada ~110ms
                    if (stick_up) nav_up = true;
                    if (stick_down) nav_down = true;
                    if (stick_left) nav_left = true;
                    if (stick_right) nav_right = true;
                }
            }
        }

        // Navegación vertical
        if (nav_up) {
            s_selected_item = (s_selected_item + 4) % 5;
        }
        if (nav_down) {
            s_selected_item = (s_selected_item + 1) % 5;
        }

        // Cambio de opciones (Izquierda / Derecha)
        if (nav_left) {
            if (s_selected_item == 0) {
                if (s_cur_ssaa > 1) s_cur_ssaa--;
                else s_cur_ssaa = 3;
            } else if (s_selected_item == 1) {
                s_cur_wide = !s_cur_wide;
            } else if (s_selected_item == 2) {
                s_cur_unlock = !s_cur_unlock;
            }
        }
        if (nav_right) {
            if (s_selected_item == 0) {
                if (s_cur_ssaa < 3) s_cur_ssaa++;
                else s_cur_ssaa = 1;
            } else if (s_selected_item == 1) {
                s_cur_wide = !s_cur_wide;
            } else if (s_selected_item == 2) {
                s_cur_unlock = !s_cur_unlock;
            }
        }

        // Botón Confirmar (A)
        bool btn_confirm = (kDown & (HidNpadButton_A | HidNpadButton_RightSL)) != 0;
        if (btn_confirm) {
            if (s_selected_item == 0) {
                if (s_cur_ssaa < 3) s_cur_ssaa++;
                else s_cur_ssaa = 1;
            } else if (s_selected_item == 1) {
                s_cur_wide = !s_cur_wide;
            } else if (s_selected_item == 2) {
                s_cur_unlock = !s_cur_unlock;
            } else if (s_selected_item == 3) {
                // [ SAVE & APPLY ]
                bool settings_changed = (s_cur_ssaa != s_orig_ssaa) ||
                                       (s_cur_wide != s_orig_wide) ||
                                       (s_cur_unlock != s_orig_unlock);

                save_config_file(s_cur_ssaa, s_cur_wide, s_cur_unlock);

                if (settings_changed) {
                    s_restarting = true;
                    switch_config_menu_rasterize();
                    Deko3DRenderer::Instance().RenderDisplay(nullptr, 1280, 0, 0, 1280, 720);
                    svcSleepThread(1200000000ULL); // 1.2 segundos para mostrar el diálogo en pantalla
                    switch_request_app_restart(s_nro_path);
                    return;
                } else {
                    s_status_msg = "No settings modified. Resuming game...";
                    switch_config_menu_rasterize();
                    Deko3DRenderer::Instance().RenderDisplay(nullptr, 1280, 0, 0, 1280, 720);
                    svcSleepThread(500000000ULL); // 0.5s
                    s_menu_open = false;
                    break;
                }
            } else if (s_selected_item == 4) {
                // [ CANCEL & EXIT ]
                s_cur_ssaa = s_orig_ssaa;
                s_cur_wide = s_orig_wide;
                s_cur_unlock = s_orig_unlock;
                s_menu_open = false;
                break;
            }
        }

        // Botón Cancelar (B) o cerrar con atajo
        bool btn_cancel = (kDown & (HidNpadButton_B | HidNpadButton_RightSR)) != 0;
        bool toggle_combo = ((kHeld & HidNpadButton_ZL) && (kHeld & HidNpadButton_ZR) && (kDown & HidNpadButton_Plus)) ||
                            ((kHeld & HidNpadButton_L) && (kHeld & HidNpadButton_R) && (kDown & HidNpadButton_Plus));

        if (btn_cancel || toggle_combo) {
            s_cur_ssaa = s_orig_ssaa;
            s_cur_wide = s_orig_wide;
            s_cur_unlock = s_orig_unlock;
            s_menu_open = false;
            break;
        }

        // Rasterizar imagen del menú
        switch_config_menu_rasterize();

        // Renderizar y presentar frame con el menú superpuesto sobre el combate congelado
        Deko3DRenderer::Instance().RenderDisplay(nullptr, 1280, 0, 0, 1280, 720);

        // Temporizador de mensaje de estado
        if (s_status_msg && s_status_timer > 0) {
            s_status_timer--;
            if (s_status_timer == 0) s_status_msg = nullptr;
        }

        // Pacing a 60 FPS
        svcSleepThread(16666667ULL);
    }

    // Reanudar audio al salir del menú
    switch_audio_set_paused(false);

    // Esperar a que se suelten los botones de confirmación/cancelación para que el juego no reciba la pulsación
    uint64_t wait_down = 0, wait_held = 0;
    int32_t wait_sx = 0, wait_sy = 0;
    int release_timeout = 25; // máx ~400ms
    while (release_timeout-- > 0) {
        switch_input_poll_menu(&wait_down, &wait_held, &wait_sx, &wait_sy);
        if (!(wait_held & (HidNpadButton_A | HidNpadButton_B | HidNpadButton_Plus | HidNpadButton_Minus |
                           HidNpadButton_LeftSL | HidNpadButton_RightSL | HidNpadButton_LeftSR | HidNpadButton_RightSR))) {
            break;
        }
        svcSleepThread(16000000ULL);
    }

    // Suprimir envío de botones a PS1 durante 10 frames (~160ms) tras reanudar
    switch_input_suppress_game_input(10);
    printf("[ConfigMenu] Menu closed. Resuming game (inputs flushed)...\n");
}
