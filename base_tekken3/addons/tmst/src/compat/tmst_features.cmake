# Build-time feature detection: which parts of the Expanded's plugin API this
# release has. Each found item becomes a definition TMST_HAS_<NAME>=1 for the
# add-on's sources only; src/compat/ turns a missing one into a mod that
# switches itself off instead of a build error. Names, not version numbers.

function(tmst_detect_features out)
    set(_header "${CMAKE_SOURCE_DIR}/psxrecomp/runtime/include/mod_plugins.h")
    set(_defs "")
    if(EXISTS "${_header}")
        file(READ "${_header}" _api)
        foreach(_pair
                "ACTIVATION_PLUGIN|psx_mod_register_activation_plugin"
                "VBLANK_PLUGIN|psx_mod_register_vblank_plugin"
                "GUEST_MEMORY|psx_mod_alloc_guest_memory"
                "HIGH_VRAM_UPLOAD|psx_mod_gpu_high_vram_upload"
                "VRAM_WRITE_OBSERVER|psx_mod_add_vram_write_observer")
            string(REPLACE "|" ";" _p "${_pair}")
            list(GET _p 0 _name)
            list(GET _p 1 _sym)
            string(FIND "${_api}" "${_sym}(" _at)
            if(NOT _at EQUAL -1)
                list(APPEND _defs "TMST_HAS_${_name}=1")
            endif()
        endforeach()
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/tekken3_game_options.h")
        file(READ "${CMAKE_SOURCE_DIR}/src/tekken3_game_options.h" _opts)
        string(FIND "${_opts}" "tekken3_game_options_add(" _at)
        if(NOT _at EQUAL -1)
            list(APPEND _defs "TMST_HAS_GAME_OPTIONS=1")
        endif()
    endif()
    # GAME OPTION's BGM SELECT with RANDOM (src/tekken3_bgm_select.c): the
    # fight's music version, for the Tekken 2 stages' and the tournaments'
    # pieces (src/core/tmst_music.c tmst_music_version, issue #9).
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/tekken3_bgm_select.h")
        file(READ "${CMAKE_SOURCE_DIR}/src/tekken3_bgm_select.h" _bgm)
        string(FIND "${_bgm}" "int tekken3_bgm_select_version(void)" _at)
        if(NOT _at EQUAL -1)
            list(APPEND _defs "TMST_HAS_BGM_SELECT=1")
        endif()
    endif()
    # The stage engine and the STAGE line (Tekken 2 Content's Stages wraps
    # these functions, called from other files of theirs).
    set(_engine 1)
    foreach(_pair
            "tekken3_stage_source.h|tekken3_stage_source(void)"
            "tekken3_stage_source.h|tekken3_stage_ttt_stage(void)"
            "tekken3_stage_source.h|tekken3_arcade_stages_source_ready(int source)"
            "tekken3_stage_select.h|tekken3_stage_select_fight(unsigned mode)"
            "tekken3_stage_select.h|tekken3_stage_select_input(int player, uint16_t buttons)"
            "tekken3_stage_select.h|tekken3_stage_select_line(const char **label"
            "tekken3_stage_select.h|tekken3_stage_select_arena(void)"
            "tekken3_stage_select.h|tekken3_stage_select_music(void)"
            "tekken3_stage_select.h|tekken3_stage_select_choice(unsigned mode, int *source")
        string(REPLACE "|" ";" _p "${_pair}")
        list(GET _p 0 _file)
        list(GET _p 1 _decl)
        set(_text "")
        if(EXISTS "${CMAKE_SOURCE_DIR}/src/${_file}")
            file(READ "${CMAKE_SOURCE_DIR}/src/${_file}" _text)
        endif()
        string(FIND "${_text}" "${_decl}" _at)
        if(_at EQUAL -1)
            set(_engine 0)
        endif()
    endforeach()
    if(_engine)
        list(APPEND _defs "TMST_HAS_STAGE_ENGINE=1")
    endif()
    # GAME OPTION's STAGES row given TEKKEN 2 (issue #11): the engine, GAME
    # OPTION's rows and the stage source's decision, called from other files.
    set(_text "")
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/tekken3_stage_source.h")
        file(READ "${CMAKE_SOURCE_DIR}/src/tekken3_stage_source.h" _text)
    endif()
    string(FIND "${_text}" "void tekken3_stage_source_decide(unsigned mode, int player, const char *guest, int arena)" _at1)
    string(FIND "${_text}" "int tekken3_stage_source_load_arena(void)" _at2)
    if(_engine AND "TMST_HAS_GAME_OPTIONS=1" IN_LIST _defs AND NOT _at1 EQUAL -1 AND NOT _at2 EQUAL -1)
        list(APPEND _defs "TMST_HAS_STAGE_OPTION=1")
    endif()
    # Unknown's wolf (src/tekken3_ttt1_wolf.c, built with TTT1 Characters):
    # its two entry points, wrapped to keep the wolf to Unknown's own fights
    # (src/compat/tmst_compat.c, docs/COMPAT.md).
    set(_text "")
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/tekken3_ttt1_wolf.c")
        file(READ "${CMAKE_SOURCE_DIR}/src/tekken3_ttt1_wolf.c" _text)
    endif()
    string(FIND "${_text}" "void tekken3_ttt1_wolf_after_actor(CPUState *cpu,uint32_t actor)" _at1)
    string(FIND "${_text}" "int tekken3_ttt1_wolf_high_vram_used(int x,int y,int w,int h)" _at2)
    if(NOT _at1 EQUAL -1 AND NOT _at2 EQUAL -1 AND (NOT DEFINED TEKKEN3_TTT1_CHARACTERS OR TEKKEN3_TTT1_CHARACTERS))
        list(APPEND _defs "TMST_HAS_UNKNOWN_WOLF=1")
    endif()
    set(${out} "${_defs}" PARENT_SCOPE)
endfunction()
