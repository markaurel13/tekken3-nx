# Developer shim for Expanded releases before the add-on hook (1.4.1 and
# older; docs/API_GAPS.md G11, closed in 1.5.0 by dev/tmst_hook.cmake): adds this add-on to the game's build without
# touching any of the Expanded's files.
#
#   cmake -S <expanded> -B <build> -DCMAKE_PROJECT_INCLUDE=<this file> ...
#
# CMAKE_PROJECT_INCLUDE runs after every project() call (the fetched
# dependencies' too): only the top-level project counts. The add-on itself
# is set up by a deferred call, once the Expanded's whole CMakeLists has run
# and psx-runtime exists. Everything it adds comes from ../addon.cmake.

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR OR DEFINED TMST_INJECTED
   OR NOT EXISTS "${CMAKE_SOURCE_DIR}/game.toml")
    return()
endif()
set(TMST_INJECTED 1)
get_filename_component(TMST_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

function(tmst_setup)
    # An Expanded with its own add-on hook (1.5.0 on) builds the add-on itself
    # (dev/tmst_hook.cmake): the shim, still in an old cache, adds nothing.
    get_property(_hooked GLOBAL PROPERTY TMST_ADDON_HOOK)
    if(_hooked OR COMMAND tekken3_addon_chain)
        message(STATUS "TMST: the Expanded's add-on hook builds the add-on; the shim is left out")
        return()
    endif()
    include("${TMST_DIR}/addon.cmake")
    if(NOT TARGET psx-runtime)
        message(WARNING "TMST: no psx-runtime target in this Expanded build; the add-on is left out.")
        return()
    endif()

    set(_sources "")
    foreach(_s IN LISTS TMST_SOURCES)
        list(APPEND _sources "${TMST_DIR}/${_s}")
    endforeach()
    set(_includes "")
    foreach(_i IN LISTS TMST_INCLUDE_DIRS)
        list(APPEND _includes "${TMST_DIR}/${_i}")
    endforeach()
    target_sources(psx-runtime PRIVATE ${_sources})
    # Only our sources see our include folders and definitions.
    execute_process(COMMAND git -C "${CMAKE_SOURCE_DIR}" describe --tags --always
        OUTPUT_VARIABLE _rev OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    if(NOT _rev)
        set(_rev "unknown")
    endif()
    include("${TMST_DIR}/src/compat/tmst_features.cmake")
    tmst_detect_features(_features)
    set_source_files_properties(${_sources} PROPERTIES
        INCLUDE_DIRECTORIES "${_includes}"
        COMPILE_DEFINITIONS "TMST_VERSION=\"${TMST_VERSION}\";TMST_UPSTREAM_REV=\"${_rev}\";TMST_TESTED_WITH=\"${TMST_TESTED_WITH}\";${_features}")

    # macOS (no --wrap in ld64), or trying that there: the preprocessor's way.
    if(APPLE OR TMST_EMULATE_WRAPS)
        include("${TMST_DIR}/dev/tmst_wraps.cmake")
        tmst_emulate_wraps()
    else()
        foreach(_w IN LISTS TMST_WRAPS)
            string(REPLACE "|" ";" _parts "${_w}")
            list(GET _parts 0 _sym)
            list(GET _parts 1 _file)
            # A third field: the TMST_HAS_ feature the wrap needs (absent: left out).
            list(LENGTH _parts _n)
            if(_n GREATER 2)
                list(GET _parts 2 _need)
                if(NOT "TMST_HAS_${_need}=1" IN_LIST _features)
                    message(STATUS "TMST: no ${_need} in this Expanded, ${_sym} not wrapped")
                    continue()
                endif()
            endif()
            if(COMMAND tekken3_wrap_symbol)
                # The Expanded's own helper: the same --wrap, emulated on macOS.
                tekken3_wrap_symbol(${_sym} WRAPPER "${TMST_DIR}/${_file}")
            else()
                target_link_options(psx-runtime PRIVATE "-Wl,--wrap=${_sym}")
            endif()
        endforeach()
        # Chains: symbols the Expanded may wrap itself. Wrapped in this build (its
        # link options say so; TTT1 Characters off, some are not): its object's
        # __wrap_ is renamed before the link (dev/tmst_chain.py). Not wrapped: we
        # wrap it, and our source gets TMST_BRIDGE_<sym>, under which it defines
        # __tmst_chain_<sym> as a call of __real_<sym> (the game's own).
        get_target_property(_their_links psx-runtime LINK_OPTIONS)
        get_property(_their_syms GLOBAL PROPERTY TEKKEN3_WRAP_SYMS)
        set(_renamed "")
        set(_bridges "")
        foreach(_c IN LISTS TMST_CHAINS)
            string(REPLACE "|" ";" _parts "${_c}")
            list(GET _parts 0 _sym)
            list(GET _parts 1 _file)
            list(LENGTH _parts _n)
            if(_n GREATER 2)
                list(GET _parts 2 _need)
                if(NOT "TMST_HAS_${_need}=1" IN_LIST _features)
                    message(STATUS "TMST: no ${_need} in this Expanded, ${_sym} not chained")
                    continue()
                endif()
            endif()
            # -Wl, or LINKER: (the runtime's own, e.g. Android's fopen)
            if("-Wl,--wrap=${_sym}" IN_LIST _their_links OR "LINKER:--wrap=${_sym}" IN_LIST _their_links
               OR "${_sym}" IN_LIST _their_syms)
                list(APPEND _renamed ${_sym})
            else()
                if(COMMAND tekken3_wrap_symbol)
                    tekken3_wrap_symbol(${_sym} WRAPPER "${TMST_DIR}/${_file}")
                else()
                    target_link_options(psx-runtime PRIVATE "-Wl,--wrap=${_sym}")
                endif()
                list(APPEND _bridges "TMST_BRIDGE_${_sym}=1")
            endif()
        endforeach()
    endif()
    if(_bridges)
        set_property(SOURCE ${_sources} APPEND PROPERTY COMPILE_DEFINITIONS ${_bridges})
    endif()
    if(_renamed)
        get_filename_component(_bin "${CMAKE_C_COMPILER}" DIRECTORY)
        find_program(TMST_OBJCOPY NAMES llvm-objcopy objcopy HINTS "${_bin}" NO_DEFAULT_PATH)
        find_program(TMST_OBJCOPY NAMES llvm-objcopy objcopy)
        find_program(TMST_NM NAMES llvm-nm nm HINTS "${_bin}" NO_DEFAULT_PATH)
        find_program(TMST_NM NAMES llvm-nm nm)
        add_custom_command(TARGET psx-runtime PRE_LINK
            COMMAND "${Python3_EXECUTABLE}" "${TMST_DIR}/dev/tmst_chain.py"
                --objcopy "${TMST_OBJCOPY}" --nm "${TMST_NM}"
                --objdir "${CMAKE_BINARY_DIR}/CMakeFiles/psx-runtime.dir"
                --state "${CMAKE_BINARY_DIR}/tmst_chain_state.json" ${_renamed}
            COMMENT "TMST: chaining in front of the Expanded's wrappers: ${_renamed}"
            VERBATIM)
    endif()

    # After the Expanded's own POST_BUILD steps, which recreate mods/.
    foreach(_p IN LISTS TMST_PACKAGES)
        add_custom_command(TARGET psx-runtime POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:psx-runtime>/mods/packages/${_p}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${TMST_DIR}/mods/${_p}"
                "$<TARGET_FILE_DIR:psx-runtime>/mods/packages/${_p}"
            COMMENT "TMST: staging ${_p}"
            VERBATIM)
    endforeach()
    # Folders the setup made (workspace/), copied beside the game; a folder
    # not made yet leaves nothing there (dev/tmst_stage_dir.cmake).
    foreach(_s IN LISTS TMST_STAGED)
        string(REPLACE "|" ";" _parts "${_s}")
        list(GET _parts 0 _from)
        list(GET _parts 1 _to)
        add_custom_command(TARGET psx-runtime POST_BUILD
            COMMAND ${CMAKE_COMMAND} "-DSRC=${TMST_DIR}/${_from}" "-DDST=$<TARGET_FILE_DIR:psx-runtime>/${_to}"
                -P "${TMST_DIR}/dev/tmst_stage_dir.cmake"
            COMMENT "TMST: staging ${_to}"
            VERBATIM)
    endforeach()
    message(STATUS "TMST ${TMST_VERSION}: ${CMAKE_SOURCE_DIR} at ${_rev}; features: ${_features}")
endfunction()

cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL tmst_setup)
