# The add-on's wraps and chains without -Wl,--wrap, with the preprocessor
# (included by dev/tmst_inject.cmake, run inside its tmst_setup()):
#
#   - macOS: Apple's ld64 has no --wrap. The Expanded emulates its own wraps
#     in tekken3_apply_wraps(), which has run before the add-on's deferred
#     setup, so the add-on does the same for its own here.
#   - TMST_EMULATE_WRAPS=ON (Windows, Linux): the same emulation, to try it
#     where it can be run (dev/build.py --emulate-wraps). The Expanded's own
#     wraps stay --wrap there, except those the add-on chains.
#
# How (as the Expanded's tools/resolve_wrap_targets.py): the source that
# defines sym is compiled with sym=__real_sym, our wrapper's with
# __wrap_sym=sym; every other call of sym then reaches our wrapper. A chain
# in front of the Expanded's wrapper: theirs with __wrap_sym=__tmst_chain_sym
# instead of __wrap_sym=sym. A system function (fopen): its callers include
# a header that sends them to our wrapper (addon.cmake's TMST_APPLE_CALLERS).
# dev/tmst_wraps.py finds the sources. Calls inside the defining source keep
# reaching the routine itself: --wrap leaves those too, once the compiler
# has inlined them (release builds).
#
# Sets _bridges and _renamed for tmst_setup() (the chains renamed in the
# objects, dev/tmst_chain.py: only with TMST_EMULATE_WRAPS, for a wrapper the
# build has not made yet at configure time).

macro(tmst_emulate_wraps)
    set(_renamed "")
    set(_bridges "")
    # The Expanded's own wraps.
    set(_tw_theirs "")
    if(APPLE)
        get_property(_tw_theirs GLOBAL PROPERTY TEKKEN3_WRAP_SYMS)
    else()
        get_target_property(_tw_links psx-runtime LINK_OPTIONS)
        if(_tw_links)
            foreach(_tw_l IN LISTS _tw_links)
                if(_tw_l MATCHES "^(-Wl,|LINKER:)--wrap=(.+)$")
                    list(APPEND _tw_theirs "${CMAKE_MATCH_2}")
                endif()
            endforeach()
        endif()
    endif()

    set(_tw_caller_syms "")
    foreach(_tw_c IN LISTS TMST_APPLE_CALLERS)
        string(REPLACE "|" ";" _tw_p "${_tw_c}")
        list(GET _tw_p 0 _tw_s)
        list(APPEND _tw_caller_syms ${_tw_s})
    endforeach()

    # Ours: "sym|file" for wraps, chains in front of theirs, chains alone.
    set(_tw_own "")
    set(_tw_chain "")
    set(_tw_callers "")
    set(_tw_syms "")
    foreach(_tw_w IN LISTS TMST_WRAPS)
        string(REPLACE "|" ";" _tw_p "${_tw_w}")
        list(GET _tw_p 0 _tw_s)
        list(GET _tw_p 1 _tw_f)
        list(LENGTH _tw_p _tw_n)
        if(_tw_n GREATER 2)
            list(GET _tw_p 2 _tw_need)
            if(NOT "TMST_HAS_${_tw_need}=1" IN_LIST _features)
                message(STATUS "TMST: no ${_tw_need} in this Expanded, ${_tw_s} not wrapped")
                continue()
            endif()
        endif()
        if(_tw_s IN_LIST _tw_caller_syms)
            list(APPEND _tw_callers "${_tw_s}|${_tw_f}")
        else()
            list(APPEND _tw_own "${_tw_s}|${_tw_f}")
            list(APPEND _tw_syms ${_tw_s})
        endif()
    endforeach()
    foreach(_tw_w IN LISTS TMST_CHAINS)
        string(REPLACE "|" ";" _tw_p "${_tw_w}")
        list(GET _tw_p 0 _tw_s)
        list(GET _tw_p 1 _tw_f)
        list(LENGTH _tw_p _tw_n)
        if(_tw_n GREATER 2)
            list(GET _tw_p 2 _tw_need)
            if(NOT "TMST_HAS_${_tw_need}=1" IN_LIST _features)
                message(STATUS "TMST: no ${_tw_need} in this Expanded, ${_tw_s} not chained")
                continue()
            endif()
        endif()
        if(_tw_s IN_LIST _tw_theirs)
            list(APPEND _tw_chain "${_tw_s}|${_tw_f}")
            list(APPEND _tw_syms ${_tw_s})
        else()
            # Not wrapped there: as our own wrap (a system function: its
            # callers sent to ours), and our source makes __tmst_chain_sym a
            # call of __real_sym (TMST_BRIDGE_sym).
            if(_tw_s IN_LIST _tw_caller_syms)
                list(APPEND _tw_callers "${_tw_s}|${_tw_f}")
            else()
                list(APPEND _tw_own "${_tw_s}|${_tw_f}")
                list(APPEND _tw_syms ${_tw_s})
            endif()
            list(APPEND _bridges "TMST_BRIDGE_${_tw_s}=1")
        endif()
    endforeach()

    # Where they are defined, among the game's sources.
    get_target_property(_tw_srcs psx-runtime SOURCES)
    get_target_property(_tw_dir psx-runtime SOURCE_DIR)
    set(_tw_text "")
    foreach(_tw_src IN LISTS _tw_srcs)
        if(_tw_src MATCHES "\\$<")
            continue()
        endif()
        if(NOT IS_ABSOLUTE "${_tw_src}")
            set(_tw_src "${_tw_dir}/${_tw_src}")
        endif()
        string(APPEND _tw_text "${_tw_src}\n")
    endforeach()
    file(WRITE "${CMAKE_BINARY_DIR}/tmst_wrap_sources.txt" "${_tw_text}")
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" "${TMST_DIR}/dev/tmst_wraps.py"
                --sources "${CMAKE_BINARY_DIR}/tmst_wrap_sources.txt" --ours "${TMST_DIR}" ${_tw_syms}
        OUTPUT_VARIABLE _tw_out ERROR_VARIABLE _tw_err RESULT_VARIABLE _tw_rc
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT _tw_rc EQUAL 0)
        message(FATAL_ERROR "TMST: dev/tmst_wraps.py failed (${_tw_rc}):\n${_tw_err}")
    endif()
    if(_tw_err)
        message(STATUS "TMST: ${_tw_err}")
    endif()
    string(REPLACE "\n" ";" _tw_lines "${_tw_out}")
    set(_tw_weak "")
    foreach(_tw_line IN LISTS _tw_lines)
        string(REPLACE "|" ";" _tw_p "${_tw_line}")
        list(GET _tw_p 0 _tw_s)
        list(GET _tw_p 1 _tw_a)
        if(_tw_s STREQUAL "weak")
            list(APPEND _tw_weak ${_tw_a})
            continue()
        endif()
        list(GET _tw_p 2 _tw_b)
        set(_tw_def_${_tw_s} "${_tw_a}")
        set(_tw_wrap_${_tw_s} "${_tw_b}")
    endforeach()

    foreach(_tw_e IN LISTS _tw_own)
        string(REPLACE "|" ";" _tw_p "${_tw_e}")
        list(GET _tw_p 0 _tw_s)
        list(GET _tw_p 1 _tw_f)
        set(_tw_ours "${TMST_DIR}/${_tw_f}")
        if(_tw_def_${_tw_s})
            set_property(SOURCE "${_tw_def_${_tw_s}}" APPEND PROPERTY COMPILE_DEFINITIONS "${_tw_s}=__real_${_tw_s}")
            set_property(SOURCE "${_tw_ours}" APPEND PROPERTY COMPILE_DEFINITIONS "__wrap_${_tw_s}=${_tw_s}")
        else()
            # Left out, but the game still links: our wrapper unused, its
            # call of the routine the routine itself.
            message(WARNING "TMST: no source of this build defines ${_tw_s} (or more than one): not wrapped")
            set_property(SOURCE "${_tw_ours}" APPEND PROPERTY COMPILE_DEFINITIONS "__real_${_tw_s}=${_tw_s}")
        endif()
    endforeach()

    foreach(_tw_e IN LISTS _tw_chain)
        string(REPLACE "|" ";" _tw_p "${_tw_e}")
        list(GET _tw_p 0 _tw_s)
        list(GET _tw_p 1 _tw_f)
        set(_tw_ours "${TMST_DIR}/${_tw_f}")
        if(APPLE)
            # Their wrapper, as tekken3_apply_wraps() found it.
            get_property(_tw_theirs_w GLOBAL PROPERTY TEKKEN3_WRAP_WRAPPER_${_tw_s})
            if(NOT _tw_theirs_w)
                set(_tw_theirs_w "${_tw_wrap_${_tw_s}}")
            endif()
            if(_tw_theirs_w)
                get_source_file_property(_tw_defs "${_tw_theirs_w}" COMPILE_DEFINITIONS)
                if(NOT _tw_defs)
                    set(_tw_defs "")
                endif()
                list(REMOVE_ITEM _tw_defs "__wrap_${_tw_s}=${_tw_s}")
                list(APPEND _tw_defs "__wrap_${_tw_s}=__tmst_chain_${_tw_s}")
                set_source_files_properties("${_tw_theirs_w}" PROPERTIES COMPILE_DEFINITIONS "${_tw_defs}")
                set_property(SOURCE "${_tw_ours}" APPEND PROPERTY COMPILE_DEFINITIONS "__wrap_${_tw_s}=${_tw_s}")
            else()
                message(WARNING "TMST: the Expanded's wrapper of ${_tw_s} not found: not chained")
                set_property(SOURCE "${_tw_ours}" APPEND PROPERTY COMPILE_DEFINITIONS "__tmst_chain_${_tw_s}=${_tw_s}")
            endif()
        elseif(_tw_def_${_tw_s} AND _tw_wrap_${_tw_s})
            # Their --wrap taken off: both emulated, ours in front.
            get_target_property(_tw_lo psx-runtime LINK_OPTIONS)
            list(REMOVE_ITEM _tw_lo "-Wl,--wrap=${_tw_s}" "LINKER:--wrap=${_tw_s}")
            set_property(TARGET psx-runtime PROPERTY LINK_OPTIONS "${_tw_lo}")
            set_property(SOURCE "${_tw_def_${_tw_s}}" APPEND PROPERTY COMPILE_DEFINITIONS "${_tw_s}=__real_${_tw_s}")
            set_property(SOURCE "${_tw_wrap_${_tw_s}}" APPEND PROPERTY COMPILE_DEFINITIONS "__wrap_${_tw_s}=__tmst_chain_${_tw_s}")
            set_property(SOURCE "${_tw_ours}" APPEND PROPERTY COMPILE_DEFINITIONS "__wrap_${_tw_s}=${_tw_s}")
        else()
            message(STATUS "TMST: ${_tw_s}: the Expanded's wrapper is made by the build, chained in the objects")
            list(APPEND _renamed ${_tw_s})
        endif()
    endforeach()

    # System functions: their callers sent to our wrapper.
    foreach(_tw_e IN LISTS _tw_callers)
        string(REPLACE "|" ";" _tw_p "${_tw_e}")
        list(GET _tw_p 0 _tw_s)
        list(GET _tw_p 1 _tw_f)
        set(_tw_ours "${TMST_DIR}/${_tw_f}")
        set_property(SOURCE "${_tw_ours}" APPEND PROPERTY COMPILE_DEFINITIONS "__real_${_tw_s}=${_tw_s}")
        foreach(_tw_c IN LISTS TMST_APPLE_CALLERS)
            string(REPLACE "|" ";" _tw_p "${_tw_c}")
            list(GET _tw_p 0 _tw_cs)
            if(NOT _tw_cs STREQUAL _tw_s)
                continue()
            endif()
            list(GET _tw_p 1 _tw_caller)
            list(GET _tw_p 2 _tw_header)
            set(_tw_caller "${CMAKE_SOURCE_DIR}/${_tw_caller}")
            if(EXISTS "${_tw_caller}")
                set_property(SOURCE "${_tw_caller}" APPEND PROPERTY COMPILE_OPTIONS "-include" "${TMST_DIR}/${_tw_header}")
            else()
                message(WARNING "TMST: ${_tw_caller} not in this Expanded: its ${_tw_s} not wrapped")
            endif()
        endforeach()
    endforeach()

    # ld64: a weak declaration may stay undefined (an Expanded without TTT1, ...).
    if(APPLE)
        foreach(_tw_name IN LISTS _tw_weak)
            target_link_options(psx-runtime PRIVATE "-Wl,-U,_${_tw_name}")
        endforeach()
    endif()
    list(LENGTH _tw_own _tw_n1)
    list(LENGTH _tw_chain _tw_n2)
    list(LENGTH _tw_callers _tw_n3)
    message(STATUS "TMST: --wrap emulated: ${_tw_n1} wrap(s), ${_tw_n2} chain(s), ${_tw_n3} caller redirect(s)")
endmacro()
