#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "mod_plugins.h"
#include "mod_memory.h"
#include "gpu.h"
#include "fntrace.h"
#include "dirty_ram_interp.h"
#include "psx_sdl.h"
#include "tekken3_main_menu.h"
#include "tekken3_sound_options.h"
#include "tekken3_outfits.h"

extern uint8_t  psx_read_byte(uint32_t addr);
extern uint16_t psx_read_half(uint32_t addr);
extern uint32_t psx_read_word(uint32_t addr);
extern void     psx_write_byte(uint32_t addr, uint8_t val);
extern void     psx_write_half(uint32_t addr, uint16_t val);
extern void     psx_write_word(uint32_t addr, uint32_t val);
extern uint32_t psx_mod_memory_alloc(uint32_t size, uint32_t alignment);

#define MAX_MOD_PLUGINS 64

typedef struct {
    char id[96];
    PSXModActivationCallback callback;
} ModActivationEntry;

typedef struct {
    char id[96];
    PSXModVBlankCallback callback;
} ModVBlankEntry;

typedef struct {
    char id[96];
    uint32_t address;
    PSXModFunctionEntryCallback callback;
} ModFuncEntry;

static ModActivationEntry s_act_plugins[MAX_MOD_PLUGINS];
static int s_act_count = 0;

static ModVBlankEntry s_vblank_plugins[MAX_MOD_PLUGINS];
static int s_vblank_count = 0;

static ModFuncEntry s_func_plugins[MAX_MOD_PLUGINS];
static int s_func_count = 0;

static int s_plugins_activated = 0;

/* Filter out debug/test or intrusive plugins that should stay off by default on Switch */
static int is_plugin_enabled_on_switch(const char *id) {
    if (!id || !*id) return 0;
    if (strcmp(id, "tekken3.high-vram-test") == 0) return 0;
    if (strcmp(id, "tekken3.selector-timer") == 0) return 0;
    if (strcmp(id, "tekken3.true-widescreen") == 0) return 0;
    return 1;
}

int psx_mod_register_activation_plugin(const char* id, PSXModActivationCallback callback) {
    if (!id || !*id || !callback || s_act_count >= MAX_MOD_PLUGINS) return 0;
    snprintf(s_act_plugins[s_act_count].id, sizeof(s_act_plugins[0].id), "%s", id);
    s_act_plugins[s_act_count].callback = callback;
    s_act_count++;
    return 1;
}

int psx_mod_register_vblank_plugin(const char* id, PSXModVBlankCallback callback) {
    if (!id || !*id || !callback || s_vblank_count >= MAX_MOD_PLUGINS) return 0;
    snprintf(s_vblank_plugins[s_vblank_count].id, sizeof(s_vblank_plugins[0].id), "%s", id);
    s_vblank_plugins[s_vblank_count].callback = callback;
    s_vblank_count++;
    return 1;
}

int psx_mod_register_function_entry_plugin(const char* id, uint32_t address, PSXModFunctionEntryCallback callback) {
    if (!id || !*id || !address || !callback || s_func_count >= MAX_MOD_PLUGINS) return 0;
    snprintf(s_func_plugins[s_func_count].id, sizeof(s_func_plugins[0].id), "%s", id);
    s_func_plugins[s_func_count].address = address;
    s_func_plugins[s_func_count].callback = callback;
    s_func_count++;
    return 1;
}

void psx_mod_function_entry(CPUState* cpu, uint32_t address) {
    if (!cpu) return;
    for (int i = 0; i < s_func_count; i++) {
        if (s_func_plugins[i].address == address) {
            s_func_plugins[i].callback(cpu, address);
        }
    }
}

static void log_dir_contents(const char *dir_path, int max_items) {
    DIR *d = opendir(dir_path);
    if (!d) {
        fprintf(stdout, "[ModDiag] opendir('%s') FAILED: errno=%d (%s)\n",
                dir_path, errno, strerror(errno));
        fflush(stdout);
        return;
    }
    fprintf(stdout, "[ModDiag] Contenido de '%s':", dir_path);
    int count = 0;
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        if (count < max_items) {
            fprintf(stdout, " [%s]", ent->d_name);
        }
        count++;
    }
    closedir(d);
    fprintf(stdout, " (total=%d entradas)\n", count);
    fflush(stdout);
}

void mod_runtime_activate_plugins(void) {
    if (s_plugins_activated) return;
    s_plugins_activated = 1;
    log_dir_contents("sdmc:/switch", 20);
    log_dir_contents("sdmc:/switch/tekken3", 25);
    log_dir_contents("sdmc:/switch/tekken3/mods", 25);
    log_dir_contents("sdmc:/switch/tekken3/mods/ttt1", 15);
    log_dir_contents("sdmc:/switch/tekken3/mods/arcade-stages", 15);
    for (int i = 0; i < s_act_count; i++) {
        if (is_plugin_enabled_on_switch(s_act_plugins[i].id)) {
            s_act_plugins[i].callback();
        }
    }
}

void mod_runtime_on_vblank(void) {
    if (!s_plugins_activated) {
        mod_runtime_activate_plugins();
    }
    tekken3_outfits_sync();
    tekken3_outfits_tick();
    tekken3_main_menu_vblank();
    tekken3_sound_options_tick();
    for (int i = 0; i < s_vblank_count; i++) {
        if (is_plugin_enabled_on_switch(s_vblank_plugins[i].id)) {
            s_vblank_plugins[i].callback();
        }
    }
}

void mod_runtime_on_dispatch(uint32_t target) {
    (void)target;
}

void mod_runtime_patch_disc_sector(uint32_t lba, int raw_sector, uint8_t* buffer, int size) {
    (void)lba; (void)raw_sector; (void)buffer; (void)size;
}

int psx_mod_game_started(void) {
    return fntrace_is_game_started();
}

int psx_mod_option_value(const char* package_id, const char* feature_id,
                         const char* option_id, char* out, uint32_t out_size) {
    (void)package_id; (void)feature_id; (void)option_id;
    if (out && out_size > 0) out[0] = '\0';
    return 0;
}

uint8_t psx_mod_read_byte(uint32_t address) {
    return psx_read_byte(address);
}

void psx_mod_write_byte(uint32_t address, uint8_t value) {
    psx_write_byte(address, value);
}

uint16_t psx_mod_read_half(uint32_t address) {
    return psx_read_half(address);
}

void psx_mod_write_half(uint32_t address, uint16_t value) {
    psx_write_half(address, value);
}

uint32_t psx_mod_read_word(uint32_t address) {
    return psx_read_word(address);
}

void psx_mod_write_word(uint32_t address, uint32_t value) {
    psx_write_word(address, value);
}

void psx_mod_write_code_word(uint32_t address, uint32_t value) {
    psx_write_word(address, value);
    dirty_ram_mark_executable_range(address & 0x1FFFFFFFu, 4u);
}

uint32_t psx_mod_alloc_guest_memory(uint32_t size, uint32_t alignment) {
    return psx_mod_memory_alloc(size, alignment);
}

uint32_t psx_mod_alloc_gpu_dma_memory(uint32_t size, uint32_t alignment) {
    return psx_mod_gpu_dma_memory_alloc(size, alignment);
}

int32_t psx_mod_widescreen_x_margin(void) {
    return (int32_t)psx_ws_x_margin();
}

int psx_mod_precision_wanted(void) {
    return 0;
}

int psx_mod_geometry_wanted(void) {
    return 0;
}

void psx_mod_precise_word(uint32_t address, uint32_t value, int32_t x16, int32_t y16, uint32_t z) {
    (void)address; (void)value; (void)x16; (void)y16; (void)z;
}

void psx_mod_audio_state_register(uint32_t (*bytes)(void),
                                  void (*write)(uint8_t* out),
                                  int (*read)(const uint8_t* in, uint32_t len)) {
    (void)bytes; (void)write; (void)read;
}

int psx_mod_set_fixed_display_aspect(uint32_t numerator, uint32_t denominator) {
    (void)numerator; (void)denominator;
    return 1;
}

/* GL texture pack & outfit catalog registration */
static void ensure_outfit_catalog_loaded(void) {
    static int s_loaded = 0;
    if (s_loaded) return;
    s_loaded = 1;
    (void)tekken3_outfits_load_catalog("sdmc:/switch/tekken3/mods/outfit-menu/catalog.txt");
}

void gl_renderer_set_skin_texture_pack(const char *directory) {
    ensure_outfit_catalog_loaded();
    if (directory && *directory) tekken3_outfits_set_available(T3_SKIN_NINA, 1);
}
void gl_renderer_set_xiaoyu_texture_pack(const char *directory) {
    ensure_outfit_catalog_loaded();
    if (directory && *directory) tekken3_outfits_set_available(T3_SKIN_XIAOYU, 1);
}
void gl_renderer_set_anna_texture_pack(const char *directory) {
    ensure_outfit_catalog_loaded();
    if (directory && *directory) tekken3_outfits_set_available(T3_SKIN_ANNA, 1);
}
void gl_renderer_set_kuma_texture_pack(const char *directory) {
    ensure_outfit_catalog_loaded();
    if (directory && *directory) tekken3_outfits_set_available(T3_SKIN_KUMA, 1);
}
void gl_renderer_set_character_texture_pack(int skin, const char *directory) {
    ensure_outfit_catalog_loaded();
    if (directory && *directory) tekken3_outfits_set_available(skin, 1);
}
