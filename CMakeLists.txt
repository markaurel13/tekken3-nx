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

# Opción para compilar shards con -O2 en vez de -O3 (evita 'I-cache thrashing' en los 48 KB de L1I del Cortex-A57)
option(TEKKEN3_SHARDS_O2 "Compilar shards generados con -O2 para optimizar L1I cache" ON)
if(TEKKEN3_SHARDS_O2)
    message(STATUS "Optimizando shards generados (TEKKEN3_SHARDS) con -O2 (L1I cache friendly)")
    set_source_files_properties(${TEKKEN3_SHARDS} PROPERTIES COMPILE_OPTIONS "-O2")
endif()

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
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_outfits.c
    ${PSXRECOMP_ROOT}/runtime/src/tekken3_sound_options.c
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
    ${PSXRECOMP_ROOT}/runtime/src/disc_path.cpp
    ${PSXRECOMP_ROOT}/runtime/src/disc_identity.cpp
    src/platform/psx_bios_registry.c
    src/platform/switch_fiber.c
    src/platform/libchdr/chd_stub.c
    src/platform/mod_runtime_stub.c
    ${PSXRECOMP_ROOT}/generated/OpenBIOS_dispatch.c
    ${PSXRECOMP_ROOT}/generated/OpenBIOS_full.c
)

# Definir ejecutable principal ELF
add_executable(tekken3_nx
    src/main.cpp
    src/tek_block_prof.c
    src/renderer/deko3d_renderer.cpp
    src/platform/switch_audio.cpp
    src/platform/switch_input.cpp
    base_tekken3/generated/SLUS_004.02_dispatch.c
    base_tekken3/generated/overlays_static.c
    ${TEKKEN3_SHARDS}
    ${PSX_RUNTIME_SOURCES}
)

target_include_directories(tekken3_nx PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3
    ${CMAKE_CURRENT_SOURCE_DIR}/base_tekken3/generated
    ${PSXRECOMP_ROOT}/generated
    ${PSXRECOMP_ROOT}/runtime/include
    ${DEVKITPRO}/libnx/include
)

target_link_libraries(tekken3_nx PRIVATE
    nx
    deko3d
    m
)

# Generar metadata NACP para Horizon OS
nx_generate_nacp(tekken3_nx.nacp
    NAME "Tekken 3"
    AUTHOR "markaurel13"
    VERSION "1.0.0"
)

# Empaquetar el ELF final en formato Homebrew .NRO con RomFS e icono
nx_create_nro(tekken3_nx
    NACP tekken3_nx.nacp
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.jpg"
    ROMFS "${CMAKE_CURRENT_SOURCE_DIR}/romfs"
)
