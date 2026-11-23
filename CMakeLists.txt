cmake_minimum_required(VERSION 3.20)

# Toolchain devkitA64 para Nintendo Switch
set(DEVKITPRO "/opt/devkitpro" CACHE PATH "Ruta de devkitPro")
set(CMAKE_TOOLCHAIN_FILE "${DEVKITPRO}/cmake/Switch.cmake")

project(tekken3_nx LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Opciones de compilación para ARM Cortex-A57 de Nintendo Switch
# Nota: devkitA64 Switch.cmake define -march=armv8-a+crc+crypto, por lo que usamos -mtune=cortex-a57
# para evitar el conflicto/warning de cc1 con -mcpu=cortex-a57.
add_compile_options(
    -O3
    -mtune=cortex-a57
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-label
    -D__SWITCH__=1
    -DNDEBUG=1
    -DPSX_NO_DEBUG_TOOLS=1
    -DPSX_NATIVE_BUILD=1
    -DPSX_ENABLE_BLOCK_CYCLES=1
    -DPSX_HAS_GAME_DISPATCH=1
    -DPSX_HAS_OVERLAY_DISPATCH=1
    -DTEKKEN3_RUNTIME=1
    -DTEKKEN3_TTT1_CHARACTERS=1
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)

# Opción para Link-Time Optimization (LTO)
option(ENABLE_LTO "Habilitar Link-Time Optimization (-flto)" OFF)
if(ENABLE_LTO)
    message(STATUS "Link-Time Optimization (LTO) ACTIVADO")
    add_compile_options(-flto)
    add_link_options(-flto)
endif()

set(PSXRECOMP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/psxrecomp")

# Recopilar todos los shards de código C generados de Tekken 3
file(GLOB TEKKEN3_SHARDS "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_*.c")
# Excluir SLUS_004.02_full_04.c porque tekken3_ttt1_push_generated.c compila su versión con hooks de colisión TTT1
list(REMOVE_ITEM TEKKEN3_SHARDS "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_04.c")

# Generación automática de código Python (Fase 1 de Tekken 3 Expanded)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(TEKKEN3_FIGHT_CAMERA_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/tekken3_fight_camera_generated.c")
add_custom_command(
    OUTPUT "${TEKKEN3_FIGHT_CAMERA_SOURCE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/tools/build_fight_camera.py"
        --source "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_26.c"
        --exe "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/disc/SLUS_004.02"
        --output "${TEKKEN3_FIGHT_CAMERA_SOURCE}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/tools/build_fight_camera.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_26.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/disc/SLUS_004.02"
    VERBATIM
)

set(TEKKEN3_ARCADE_CAMERAS_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/tekken3_arcade_cameras_generated.c")
add_custom_command(
    OUTPUT "${TEKKEN3_ARCADE_CAMERAS_SOURCE}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/tools/build_arcade_cameras.py"
        --generated "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated"
        --exe "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/disc/SLUS_004.02"
        --output "${TEKKEN3_ARCADE_CAMERAS_SOURCE}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/tools/build_arcade_cameras.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_12.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_27.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/disc/SLUS_004.02"
    VERBATIM
)

set(TTT1_COMBAT_HOOKS "${CMAKE_CURRENT_BINARY_DIR}/tekken3_ttt1_combat_generated.c")
set(TTT1_PUSH_HOOKS "${CMAKE_CURRENT_BINARY_DIR}/tekken3_ttt1_push_generated.c")
add_custom_command(
    OUTPUT "${TTT1_COMBAT_HOOKS}" "${TTT1_PUSH_HOOKS}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/tools/build_ttt1_combat_hooks.py"
        --root "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3"
        --output "${TTT1_COMBAT_HOOKS}"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/tools/build_ttt1_combat_hooks.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_02.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_03.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_04.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated/SLUS_004.02_full_09.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/disc/SLUS_004.02"
    VERBATIM
)

# Opción para compilar shards con -O2 en vez de -O3 (evita 'I-cache thrashing' en los 48 KB de L1I del Cortex-A57)
option(TEKKEN3_SHARDS_O2 "Compilar shards generados con -O2 para optimizar L1I cache" ON)
if(TEKKEN3_SHARDS_O2)
    message(STATUS "Optimizando shards generados (TEKKEN3_SHARDS) con -O2 (L1I cache friendly)")
    set_source_files_properties(
        ${TEKKEN3_SHARDS}
        "${TEKKEN3_FIGHT_CAMERA_SOURCE}"
        "${TEKKEN3_ARCADE_CAMERAS_SOURCE}"
        "${TTT1_COMBAT_HOOKS}"
        "${TTT1_PUSH_HOOKS}"
        PROPERTIES COMPILE_OPTIONS "-O2"
    )
endif()

# Módulos C de Tekken 3 Expanded (Fase 1)
set(TEKKEN3_EXPANDED_SOURCES
    base_tekken3/src/tekken3_widescreen_mod.c
    base_tekken3/src/tekken3_fight_camera.c
    base_tekken3/src/tekken3_nina_skin_mod.c
    base_tekken3/src/tekken3_xiaoyu_skin_mod.c
    base_tekken3/src/tekken3_anna_skin_mod.c
    base_tekken3/src/tekken3_kuma_skin_mod.c
    base_tekken3/src/tekken3_unlock_characters_mod.c
    base_tekken3/src/tekken3_difficulty_mod.c
    base_tekken3/src/tekken3_main_menu.c
    base_tekken3/src/tekken3_game_options.c
    base_tekken3/src/tekken3_stage_source.c
    base_tekken3/src/tekken3_stage_select.c
    base_tekken3/src/tekken3_bgm_select.c
    base_tekken3/src/tekken3_new_skin_mods.c
    base_tekken3/src/tekken3_selector_timer_mod.c
    base_tekken3/src/tekken3_high_vram_test.c
    base_tekken3/src/tekken3_arcade_stages.c
    base_tekken3/src/tekken3_arcade_cameras.c
    base_tekken3/src/tekken3_ttt1_mod.c
    base_tekken3/src/tekken3_ttt1_renderer.c
    base_tekken3/src/tekken3_ttt1_motion.c
    base_tekken3/src/tekken3_ttt1_combat.c
    base_tekken3/src/tekken3_jin_omen.c
    base_tekken3/src/tekken3_ttt1_wolf.c
    base_tekken3/src/tekken3_ttt1_ps2_laser.c
    base_tekken3/src/tekken3_ttt1_ps2_weapons.c
    base_tekken3/src/tekken3_ttt1_voices.c
    base_tekken3/src/tekken3_ttt_music.c
    base_tekken3/src/tekken3_ttt1_sfx.c
    base_tekken3/src/tekken3_ttt1_selector.c
    base_tekken3/src/tekken3_native_moves.c
    base_tekken3/src/tekken3_dr_camera.c
    base_tekken3/src/tekken3_dr_content.c
    base_tekken3/src/tekken3_ttt1_roster.c
    base_tekken3/src/tekken3_cine_hooks.c
    base_tekken3/src/tekken3_cine.c
    base_tekken3/src/tekken3_cine_floor.c
    base_tekken3/src/tekken3_cine_near.c
    base_tekken3/src/tekken3_cine_trigger.c
    base_tekken3/src/tekken3_cine_scene.c
    base_tekken3/src/tekken3_cine_swap.c
    base_tekken3/src/tekken3_ttt1_records.c
    base_tekken3/src/tekken3_fmv_overlay.c
    base_tekken3/src/tekken3_fmv_str.c
    base_tekken3/src/tekken3_fmv_vlc.c
    base_tekken3/src/tekken3_fmv_diag.c
    "${TEKKEN3_FIGHT_CAMERA_SOURCE}"
    "${TEKKEN3_ARCADE_CAMERAS_SOURCE}"
    "${TTT1_COMBAT_HOOKS}"
    "${TTT1_PUSH_HOOKS}"
)

# Add-on TMST (Jukebox, Photo Mode, Tournament Mode, VS Mode+, Tekken 2 Content)
set(TMST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/addons/tmst")
set(TMST_SOURCES
    ${TMST_DIR}/src/core/tmst_core.c
    ${TMST_DIR}/src/core/tmst_ui_font.c
    ${TMST_DIR}/src/core/tmst_ui_font_data.c
    ${TMST_DIR}/src/core/tmst_overlay.c
    ${TMST_DIR}/src/core/tmst_audio.c
    ${TMST_DIR}/src/core/tmst_music.c
    ${TMST_DIR}/src/core/tmst_menu.c
    ${TMST_DIR}/src/core/tmst_input.c
    ${TMST_DIR}/src/core/tmst_spu.c
    ${TMST_DIR}/src/core/tmst_pause.c
    ${TMST_DIR}/src/core/tmst_camera.c
    ${TMST_DIR}/src/core/tmst_png.c
    ${TMST_DIR}/src/core/tmst_files.c
    ${TMST_DIR}/src/compat/tmst_compat.c
    ${TMST_DIR}/src/jukebox/tmst_jukebox.c
    ${TMST_DIR}/src/jukebox/tmst_adpcm.c
    ${TMST_DIR}/src/photo/tmst_photo.c
    ${TMST_DIR}/src/photo/tmst_photo_fx.c
    ${TMST_DIR}/src/tournament/tmst_tournament_pools.c
    ${TMST_DIR}/src/tournament/tmst_bracket.c
    ${TMST_DIR}/src/tournament/tmst_tournament_progress.c
    ${TMST_DIR}/src/tournament/tmst_tournament_stages.c
    ${TMST_DIR}/src/tournament/tmst_tournament_cine.c
    ${TMST_DIR}/src/tournament/tmst_tournament_art.c
    ${TMST_DIR}/src/tournament/tmst_tournament_screens.c
    ${TMST_DIR}/src/tournament/tmst_tournament_story.c
    ${TMST_DIR}/src/tournament/tmst_tournament_options.c
    ${TMST_DIR}/src/tournament/tmst_tournament.c
    ${TMST_DIR}/src/vsplus/tmst_vsplus.c
    ${TMST_DIR}/src/vsplus/tmst_vsplus_options.c
    ${TMST_DIR}/src/tekken2/tmst_tekken2.c
    ${TMST_DIR}/src/tekken2/tmst_tekken2_stages.c
    ${TMST_DIR}/src/tekken2/tmst_tekken2_mirror.c
    ${TMST_DIR}/src/tekken2/tmst_tekken2_extras.c
    ${TMST_DIR}/src/tekken2/tmst_tekken2_trace.c
    ${TMST_DIR}/src/tekken2/tmst_tekken2_anchor.c
    ${TMST_DIR}/src/tekken2/tmst_tekken2_wireframe.c
)
set_source_files_properties(${TMST_SOURCES} PROPERTIES
    INCLUDE_DIRECTORIES "${TMST_DIR}/src/core;${TMST_DIR}/src/compat;${TMST_DIR}/src/tournament"
    COMPILE_DEFINITIONS "TMST_VERSION=\"1.0.0\";TMST_UPSTREAM_REV=\"v1.5.1\";TMST_TESTED_WITH=\"v1.4.1 v1.5.0 v1.5.1\";TMST_HAS_ACTIVATION_PLUGIN=1;TMST_HAS_VBLANK_PLUGIN=1;TMST_HAS_GUEST_MEMORY=1;TMST_HAS_HIGH_VRAM_UPLOAD=1;TMST_HAS_VRAM_WRITE_OBSERVER=1;TMST_HAS_GAME_OPTIONS=1;TMST_HAS_BGM_SELECT=1;TMST_HAS_STAGE_ENGINE=1;TMST_HAS_STAGE_OPTION=1;TMST_HAS_UNKNOWN_WOLF=1;TMST_BRIDGE_fopen=1;TMST_BRIDGE_func_800780BC=1;TMST_BRIDGE_func_8004A6BC=1;TMST_BRIDGE_func_8003A818=1;TMST_BRIDGE_func_800826E8=1"
)

# Fuentes del Runtime de psxrecomp
set(PSX_RUNTIME_SOURCES
    ${PSXRECOMP_ROOT}/runtime/src/memory.c
    ${PSXRECOMP_ROOT}/runtime/src/dma.c
    ${PSXRECOMP_ROOT}/runtime/src/timers.c
    ${PSXRECOMP_ROOT}/runtime/src/interrupts.c
    ${PSXRECOMP_ROOT}/runtime/src/sio.c
    ${PSXRECOMP_ROOT}/runtime/src/spu.c
    ${PSXRECOMP_ROOT}/runtime/src/audio_trace.c
    ${PSXRECOMP_ROOT}/runtime/src/memcard.c
    ${PSXRECOMP_ROOT}/runtime/src/gpu.c
    ${PSXRECOMP_ROOT}/runtime/src/gpu_render.c
    ${PSXRECOMP_ROOT}/runtime/src/gpu_sw_renderer.c
    ${PSXRECOMP_ROOT}/runtime/src/gpu_vram_dirty.c
    ${PSXRECOMP_ROOT}/runtime/src/gte.cpp
    ${PSXRECOMP_ROOT}/runtime/src/ws_ui_group.c
    ${PSXRECOMP_ROOT}/runtime/src/widescreen_layer_classifier.c
    ${PSXRECOMP_ROOT}/runtime/src/widescreen_layer_layout.c
    ${PSXRECOMP_ROOT}/runtime/src/ws_aspect_cone_math.c
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_selector_layout.c
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_force_background.c
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_ball_background.c
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_outfits.c
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_sound_options.c
    ${PSXRECOMP_ROOT}/runtime/src/host_osd.c
    ${PSXRECOMP_ROOT}/runtime/src/mdec.c
    ${PSXRECOMP_ROOT}/runtime/src/cdrom.c
    ${PSXRECOMP_ROOT}/runtime/src/host_time.c
    ${PSXRECOMP_ROOT}/runtime/src/dirty_ram_interp.c
    ${PSXRECOMP_ROOT}/runtime/src/fntrace.c
    ${PSXRECOMP_ROOT}/runtime/src/parity_trace.c
    ${PSXRECOMP_ROOT}/runtime/src/device_trace.c
    ${PSXRECOMP_ROOT}/runtime/src/stub_interpreter.c
    ${PSXRECOMP_ROOT}/runtime/src/cue_sheet.cpp
    ${PSXRECOMP_ROOT}/runtime/src/iso_reader.cpp
    ${PSXRECOMP_ROOT}/runtime/src/iso_reader_c.cpp
    ${PSXRECOMP_ROOT}/runtime/src/psx_bios_backend.c
    ${PSXRECOMP_ROOT}/runtime/src/traps.c
    ${PSXRECOMP_ROOT}/runtime/src/psx_cycles.c
    ${PSXRECOMP_ROOT}/runtime/src/psx_icache.c
    ${PSXRECOMP_ROOT}/runtime/src/bios_hle.c
    ${PSXRECOMP_ROOT}/runtime/src/bios_hle_plan.c
    ${PSXRECOMP_ROOT}/runtime/src/starvation_ring.c
    ${PSXRECOMP_ROOT}/runtime/src/data_shards.c
    ${PSXRECOMP_ROOT}/runtime/src/load_accel.c
    ${PSXRECOMP_ROOT}/runtime/src/crc32.c
    ${PSXRECOMP_ROOT}/runtime/src/psx_sha256.c
    ${PSXRECOMP_ROOT}/runtime/src/disc_path.cpp
    ${PSXRECOMP_ROOT}/runtime/src/disc_identity.cpp
    src/platform/psx_bios_registry.c
    src/platform/switch_fiber.c
    src/platform/libchdr/chd_stub.c
    src/platform/mod_runtime_stub.c
    ${PSXRECOMP_ROOT}/generated/OpenBIOS_dispatch.c
    ${PSXRECOMP_ROOT}/generated/OpenBIOS_full.c
)

set_source_files_properties(
    ${PSXRECOMP_ROOT}/runtime/src/host_osd.c
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_outfits.c
    PROPERTIES COMPILE_DEFINITIONS "RECOMP_LAUNCHER=1;TEKKEN3_LAUNCHER=1;PSX_SDL_NO_RENDER=1"
)

# Generar dinámicamente build_version.h con Git Hash y timestamp de compilación en cada build
add_custom_target(generate_build_version ALL
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_build_version.py" "${CMAKE_CURRENT_BINARY_DIR}/build_version.h"
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/build_version.h"
    COMMENT "Generando build_version.h con Git hash y timestamp..."
)

# Definir ejecutable principal ELF
add_executable(tekken3_nx
    src/main.cpp
    src/tek_block_prof.c
    src/renderer/deko3d_renderer.cpp
    src/platform/switch_audio.cpp
    src/platform/switch_file_io.c
    src/platform/switch_input.cpp
    src/platform/switch_config_menu.cpp
    base_tekken3/generated/SLUS_004.02_dispatch.c
    base_tekken3/generated/overlays_static.c
    ${TEKKEN3_SHARDS}
    ${TEKKEN3_EXPANDED_SOURCES}
    ${TMST_SOURCES}
    ${PSX_RUNTIME_SOURCES}
)

add_dependencies(tekken3_nx generate_build_version)

target_include_directories(tekken3_nx PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3
    ${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/src
    ${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated
    ${PSXRECOMP_ROOT}/generated
    ${PSXRECOMP_ROOT}/runtime/include
    ${DEVKITPRO}/portlibs/switch/include
    ${DEVKITPRO}/libnx/include
)

target_link_directories(tekken3_nx PRIVATE
    ${DEVKITPRO}/portlibs/switch/lib
)

target_link_libraries(tekken3_nx PRIVATE
    nx
    deko3d
    z
    m
)

# Hooks de enlazado -Wl,--wrap=... de Tekken 3 Expanded (Fase 1)
set(TEKKEN3_WRAP_SYMBOLS
    func_80064E10
    func_80028CD4
    func_800753BC
    func_8006AE14
    func_80048E58
    func_80049848
    func_8006D014
    func_80048374
    func_80048240
    func_80048208
    func_8004610C
    func_80081FD8
    func_80046270
    func_80046460
    func_80046464
    func_800465BC
    func_800686B0
    func_8006870C
    func_8006953C
    func_80069414
    func_80036CAC
    func_80035B28
    func_80035BC0
    func_80035190
    func_80035CE8
    func_8003AD10
    func_80037CBC
    func_80034844
    func_8006A3BC
    func_8003C8F0
    func_8002F3AC
    func_8003F098
    func_8002DE24
    func_8002D9E8
    func_8007745C
    func_80077524
    func_8004AC24
    func_8004ACB4
    func_80040C90
    func_80040DF4
    func_80041CB4
    func_800757A8
    func_8008D658
    func_8003CB84
    spu_render
    spu_set_host_mix_volume
    spu_init
    spu_snapshot_read
    func_8002D178
    func_8002D07C
    func_80038B4C
    func_800389C0
    func_8002D264
    func_8002E8D0
    func_8002D8B4
    func_8002E0BC
    func_8002CC28
    func_8002EE94
    func_800449B4
    func_800458C8
    func_8002CD7C
    func_80059890
    func_80056764
    func_8002C580
    func_8002A914
    func_8004F2DC
    func_8007E0B8
    tekken3_outfits_input
    host_osd_outfit_image
    host_osd_draw_sdl
    func_8006C618
    func_8006C63C
    func_80063EEC
    func_8004FF74
    func_8004E000
    func_8002A40C
    func_8006D7CC
    func_80052938
    func_80052940
    func_8004B790
    func_8004B928
    func_800377F8
    func_80050450
    func_8004F8F8
    func_8004FA60
    func_8006C23C
    func_8003B4F8
    func_80031BFC
    func_8003172C
    func_800707F4
    func_80052AD0
    func_80052958
    func_80052990
    func_8006BF20
    func_8003626C
    func_80036294
    func_800362C4
    func_8003F044
    func_8002D1DC
    func_800293BC
    func_800513C0
    func_80051464
    func_8005155C
    func_80051660
    func_8004439C
    func_8004CD80
    func_8004BEC0
    func_8004C240
    func_8005138C
    func_8004CC08
    func_8004F3B0
    func_8004F0E4
    func_8004CE54
    func_8002D45C
    func_8003352C
    func_80037B28
    func_80064080
    func_80070E08
    func_80071110
    func_80076884
    func_80076C28
    # TMST add-on wraps
    gpu_ws_prepass_linked_list
    psx_mod_gpu_high_vram_upload
    tekken3_ttt1_wolf_after_actor
    tekken3_ttt1_wolf_high_vram_used
    tekken3_stage_source
    tekken3_stage_ttt_stage
    tekken3_stage_select_fight
    tekken3_stage_select_input
    tekken3_stage_select_line
    tekken3_stage_select_arena
    tekken3_stage_select_music
    tekken3_stage_select_choice
    tekken3_game_options_add
    tekken3_game_options_value
    tekken3_stage_source_decide
    tekken3_stage_source_load_arena
    tekken3_arcade_stages_thunder_mix
    tekken3_arcade_stages_thunder_stop
    # TMST add-on bridged chains (not previously wrapped on Switch)
    fopen
    func_800780BC
    func_8004A6BC
    func_8003A818
    func_800826E8
)
foreach(_sym IN LISTS TEKKEN3_WRAP_SYMBOLS)
    target_link_options(tekken3_nx PRIVATE "-Wl,--wrap=${_sym}")
endforeach()

# Encadenar wrappers de TMST delante de los wrappers existentes de Tekken 3 Expanded
add_custom_command(TARGET tekken3_nx PRE_LINK
    COMMAND "${Python3_EXECUTABLE}" "${TMST_DIR}/dev/tmst_chain.py"
        --objcopy "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-objcopy"
        --nm "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-nm"
        --objdir "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/tekken3_nx.dir"
        --state "${CMAKE_CURRENT_BINARY_DIR}/tmst_chain_state.json"
        func_8006AE14
        func_80028CD4
        tekken3_outfits_input
        spu_render
        spu_set_host_mix_volume
        func_80048240
        func_8004F0E4
        func_8004439C
        func_80050450
        func_8006D014
        func_80049848
        func_80036CAC
        func_80064E10
    COMMENT "Encadenando wrappers TMST con objcopy..."
    VERBATIM
)

# Generar metadata NACP para Horizon OS
nx_generate_nacp(tekken3_nx.nacp
    NAME "Tekken 3: Definitive Edition"
    AUTHOR "markaurel13"
    VERSION "1.5.1"
)

# Empaquetar el ELF final en formato Homebrew .NRO con RomFS e icono
nx_create_nro(tekken3_nx
    NACP tekken3_nx.nacp
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.jpg"
    ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
)
