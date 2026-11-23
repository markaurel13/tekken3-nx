# POST_BUILD (dev/tmst_inject.cmake, addon.cmake's TMST_STAGED): the folder
# SRC the add-on's setup made, copied to DST beside the game; DST is emptied
# first, so that nothing stale stays. SRC not made (the setup had no disc for
# it): DST left empty, the part that reads it says so in the game's log.
#
#   cmake -DSRC=<folder> -DDST=<folder> -P tmst_stage_dir.cmake

file(REMOVE_RECURSE "${DST}")
if(IS_DIRECTORY "${SRC}")
    file(COPY "${SRC}/" DESTINATION "${DST}")
else()
    message(STATUS "TMST: ${SRC} not made (Setup TMST.cmd), nothing staged in ${DST}")
endif()
