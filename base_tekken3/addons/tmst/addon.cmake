# TMST add-on for Tekken 3 Expanded: what the game's build takes from this
# folder. Lists only, read by both ways in: the Expanded's own add-on hook
# (1.5.0 on, its addons/README.md; dev/tmst_hook.cmake, at the end of this
# file) and, for older releases, dev/tmst_inject.cmake. Paths are relative to
# this folder.

set(TMST_ID "tmst")
set(TMST_VERSION "1.0.0")
# The Expanded's setup rebuilds when this file changes: bump the version at
# every update.
# Expanded releases this add-on was tested with (docs/COMPAT.md). Another
# release still builds and runs: the add-on detects features, not versions,
# and only notes the release as untested in the log.
set(TMST_TESTED_WITH "v1.4.1 v1.5.0")

# C sources linked into the game (psx-runtime).
set(TMST_SOURCES
    src/core/tmst_core.c
    src/core/tmst_ui_font.c
    src/core/tmst_ui_font_data.c
    src/core/tmst_overlay.c
    src/core/tmst_audio.c
    src/core/tmst_music.c
    src/core/tmst_menu.c
    src/core/tmst_input.c
    src/core/tmst_spu.c
    src/core/tmst_pause.c
    src/core/tmst_camera.c
    src/core/tmst_png.c
    src/core/tmst_files.c
    src/compat/tmst_compat.c
    src/jukebox/tmst_jukebox.c
    src/jukebox/tmst_adpcm.c
    src/photo/tmst_photo.c
    src/photo/tmst_photo_fx.c
    src/tournament/tmst_tournament_pools.c
    src/tournament/tmst_bracket.c
    src/tournament/tmst_tournament_progress.c
    src/tournament/tmst_tournament_stages.c
    src/tournament/tmst_tournament_cine.c
    src/tournament/tmst_tournament_art.c
    src/tournament/tmst_tournament_screens.c
    src/tournament/tmst_tournament_story.c
    src/tournament/tmst_tournament_options.c
    src/tournament/tmst_tournament.c
    src/vsplus/tmst_vsplus.c
    src/vsplus/tmst_vsplus_options.c
    src/tekken2/tmst_tekken2.c
    src/tekken2/tmst_tekken2_stages.c
    src/tekken2/tmst_tekken2_mirror.c
    src/tekken2/tmst_tekken2_extras.c
    src/tekken2/tmst_tekken2_trace.c
    src/tekken2/tmst_tekken2_anchor.c
    src/tekken2/tmst_tekken2_wireframe.c
)
set(TMST_INCLUDE_DIRS
    src/core
    src/compat
    src/tournament
)

# Symbols wrapped with -Wl,--wrap, each "symbol|file defining __wrap_symbol",
# optionally "|NAME": only when the build found TMST_HAS_NAME (the file then
# defines the wrapper under it). Only symbols the Expanded does not wrap
# itself (docs/COMPAT.md).
set(TMST_WRAPS
    "gpu_ws_prepass_linked_list|src/core/tmst_overlay.c"
    "psx_mod_gpu_high_vram_upload|src/core/tmst_overlay.c"
    "psx_sdl_audio_open|src/core/tmst_audio.c"
    "tekken3_ttt1_wolf_after_actor|src/compat/tmst_compat.c|UNKNOWN_WOLF"
    "tekken3_ttt1_wolf_high_vram_used|src/compat/tmst_compat.c|UNKNOWN_WOLF"
    "tekken3_stage_source|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_stage_ttt_stage|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_stage_select_fight|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_stage_select_input|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_stage_select_line|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_stage_select_arena|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_stage_select_music|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_stage_select_choice|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "tekken3_game_options_add|src/tekken2/tmst_tekken2_stages.c|STAGE_OPTION"
    "tekken3_game_options_value|src/tekken2/tmst_tekken2_stages.c|STAGE_OPTION"
    "tekken3_stage_source_decide|src/tekken2/tmst_tekken2_stages.c|STAGE_OPTION"
    "tekken3_stage_source_load_arena|src/tekken2/tmst_tekken2_stages.c|STAGE_OPTION"
    "tekken3_arcade_stages_thunder_mix|src/tekken2/tmst_tekken2_extras.c|STAGE_ENGINE"
    "tekken3_arcade_stages_thunder_stop|src/tekken2/tmst_tekken2_extras.c|STAGE_ENGINE"
)

# macOS (ld64 has no --wrap; dev/tmst_wraps.cmake emulates it): wrapped
# symbols that no source of the game defines (the system's), each
# "symbol|the Expanded's source whose calls go to our wrapper|header put before
# it (-include)". The other wraps need nothing more there.
set(TMST_APPLE_CALLERS
    "fopen|src/tekken3_arcade_stages.c|src/compat/tmst_redirect_fopen.h"
)

# Symbols the Expanded may wrap itself (guest routines and runtime
# functions), each "symbol|file defining __wrap_symbol", optionally "|NAME"
# (only with TMST_HAS_NAME, as for TMST_WRAPS): ours runs first and calls
# __tmst_chain_symbol (theirs). Wrapped in the build: the build renames
# their wrapper (dev/tmst_chain.py). Not wrapped there (another release, or
# TTT1 Characters off): we wrap it, and the file defines __tmst_chain_ under
# TMST_BRIDGE_symbol as a call of __real_symbol. fopen: the Expanded's
# Android runtime wraps it (the APK's files, android_apk_file.c), the PC ones not.
set(TMST_CHAINS
    "fopen|src/tekken2/tmst_tekken2_stages.c|STAGE_ENGINE"
    "func_8006AE14|src/core/tmst_music.c"
    "func_80028CD4|src/core/tmst_menu.c"
    "tekken3_outfits_input|src/core/tmst_input.c"
    "spu_render|src/core/tmst_spu.c"
    "spu_set_host_mix_volume|src/core/tmst_spu.c"
    "func_800780BC|src/core/tmst_pause.c"
    "func_80048240|src/core/tmst_camera.c"
    "func_8004A6BC|src/core/tmst_camera.c"
    "func_8003A818|src/core/tmst_camera.c"
    "func_8004F0E4|src/tournament/tmst_tournament.c"
    "func_8004439C|src/tournament/tmst_tournament.c"
    "func_80050450|src/tournament/tmst_tournament.c"
    "func_800826E8|src/tekken2/tmst_tekken2_stages.c"
    "func_8006D014|src/tekken2/tmst_tekken2_mirror.c"
    "func_80049848|src/tekken2/tmst_tekken2_stages.c"
    "func_80036CAC|src/tekken2/tmst_tekken2_mirror.c"
    "func_80064E10|src/tekken2/tmst_tekken2_wireframe.c"
)

# Packages staged into the game's mods/packages (mods/<id>/<version>/manifest.toml).
set(TMST_PACKAGES
    tmst.jukebox
    tmst.photo-mode
    tmst.tournament
    tmst.vs-plus
    tmst.tekken2
)

# Folders the setup makes, staged beside the game at each build, each
# "folder here|folder beside the game" (dev/tmst_stage_dir.cmake).
# mods/tmst-data: what the game reads from addons/tmst (src/core/tmst_files.h
# tmst_files_addon), copied under mods/ because the Expanded's own Android APK
# holds only the game's mods/ (about 1 MB; the music stays out, ~800 MB:
# setup/tmst_android.py puts it in).
set(TMST_STAGED
    "workspace/t2-stages/classic|mods/t2-stages/classic"
    "workspace/t2-stages/remake|mods/t2-stages/remake"
    "data/tournament|mods/tmst-data/data/tournament"
    "data/photo|mods/tmst-data/data/photo"
    "workspace/jukebox|mods/tmst-data/workspace/jukebox"
)

# Python steps run after the Expanded's setup (setup/tmst_setup.py).
set(TMST_SETUP setup/tmst_setup.py)

# The Expanded's own add-on hook (TEKKEN3_ADDON_API): this add-on built in by
# it, unless dev/build.py --without (or tmst_android.py --without) asked not to.
if(DEFINED TEKKEN3_ADDON_API AND NOT TMST_WITHOUT)
    include("${CMAKE_CURRENT_LIST_DIR}/dev/tmst_hook.cmake")
endif()
# The step the Expanded's Update & play runs before each build (--no-build).
set(TEKKEN3_ADDON_SETUP setup/tmst_setup.py)
