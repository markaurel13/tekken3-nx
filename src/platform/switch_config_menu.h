#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Inicialización del menú con la ruta de configuración y la ruta del ejecutable NRO
void switch_config_menu_init(const char* config_path, const char* nro_path, int ssaa, bool widescreen, bool unlock_all);

// Sincronizar valores actuales
void switch_config_menu_sync_values(int ssaa, bool widescreen, bool unlock_all);

// Solicitud y consulta de apertura del menú
void switch_config_menu_request_open(void);
bool switch_config_menu_is_requested(void);
bool switch_config_menu_is_open(void);

// Bucle modal que congela la ejecución del juego y maneja la interacción con el menú
void switch_config_menu_run_modal(void);

// Obtener el buffer ARGB8888 de la imagen del menú para renderizado en Deko3D
bool switch_config_menu_get_image(const uint32_t** out_pixels, int* out_w, int* out_h);

// Notificar avance de frame al watchdog mientras el menú está abierto
void switch_notify_menu_frame(void);

// Solicitar auto-reinicio ordenado del juego
void switch_request_app_restart(const char* nro_path);

#ifdef __cplusplus
}
#endif
