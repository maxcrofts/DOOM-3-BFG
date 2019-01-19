# compiler flags
if(MSVC)
    # enable multi-processor compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # disable warnings introduced after Visual Studio 2013
    if(MSVC_VERSION GREATER 1800)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4467 /wd4595")
    endif()

    # disable some clang specific warnings
    if(CLANG)
        set(CLANG_FLAGS
            "-Wno-braced-scalar-init"
            "-Wno-deprecated-declarations"
            "-Wno-ignored-attributes"
            "-Wno-implicit-exception-spec-mismatch"
            "-Wno-inline-new-delete"
            "-Wno-microsoft-include"
            "-Wno-missing-braces"
            "-Wno-overloaded-virtual"
            "-Wno-parentheses-equality"
            "-Wno-pragma-pack"
            "-Wno-reorder"
            "-Wno-reserved-user-defined-literal"
            "-Wno-sometimes-uninitialized"
            "-Wno-switch"
            "-Wno-unused-const-variable"
            "-Wno-unused-function"
            "-Wno-unused-local-typedef"
            "-Wno-unused-variable"
            "-Wno-writable-strings"
        )
        string(REPLACE ";" " " CLANG_FLAGS "${CLANG_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-everything")
endif()

# doomclassic
set(DOOMCLASSIC_FILES
    "doom/am_map.cpp"
    "doom/am_map.h"
    "doom/constructs.h"
    "doom/d_englsh.h"
    "doom/d_event.h"
    "doom/d_french.h"
    "doom/d_items.cpp"
    "doom/d_items.h"
    "doom/d_main.cpp"
    "doom/d_main.h"
    "doom/d_net.cpp"
    "doom/d_net.h"
    "doom/d_player.h"
    "doom/d_textur.h"
    "doom/d_think.h"
    "doom/d_ticcmd.h"
    "doom/defs.h"
    "doom/doomdata.h"
    "doom/doomdef.h"
    "doom/doominterface.cpp"
    "doom/doominterface.h"
    "doom/doomlib.cpp"
    "doom/doomlib.h"
    "doom/doomstat.h"
    "doom/doomtype.h"
    "doom/dstrings.cpp"
    "doom/dstrings.h"
    "doom/f_finale.cpp"
    "doom/f_finale.h"
    "doom/f_wipe.cpp"
    "doom/f_wipe.h"
    "doom/g_game.cpp"
    "doom/g_game.h"
    "doom/globaldata.cpp"
    "doom/globaldata.h"
    "doom/hu_lib.cpp"
    "doom/hu_lib.h"
    "doom/hu_stuff.cpp"
    "doom/hu_stuff.h"
    "doom/i_input.cpp"
    "doom/i_net.cpp"
    "doom/i_net.h"
    "doom/i_sound.cpp"
    "doom/i_sound.h"
    "doom/i_system.cpp"
    "doom/i_system.h"
    "doom/i_video.cpp"
    "doom/i_video.h"
    "doom/info.cpp"
    "doom/info.h"
    "doom/m_argv.cpp"
    "doom/m_argv.h"
    "doom/m_bbox.cpp"
    "doom/m_bbox.h"
    "doom/m_cheat.cpp"
    "doom/m_cheat.h"
    "doom/m_fixed.cpp"
    "doom/m_fixed.h"
    "doom/m_menu.cpp"
    "doom/m_menu.h"
    "doom/m_misc.cpp"
    "doom/m_misc.h"
    "doom/m_random.cpp"
    "doom/m_random.h"
    "doom/m_swap.h"
    "doom/mus2midi.cpp"
    "doom/p_ceilng.cpp"
    "doom/p_doors.cpp"
    "doom/p_enemy.cpp"
    "doom/p_floor.cpp"
    "doom/p_inter.cpp"
    "doom/p_inter.h"
    "doom/p_lights.cpp"
    "doom/p_local.h"
    "doom/p_map.cpp"
    "doom/p_maputl.cpp"
    "doom/p_mobj.cpp"
    "doom/p_mobj.h"
    "doom/p_plats.cpp"
    "doom/p_pspr.cpp"
    "doom/p_pspr.h"
    "doom/p_saveg.cpp"
    "doom/p_saveg.h"
    "doom/p_setup.cpp"
    "doom/p_setup.h"
    "doom/p_sight.cpp"
    "doom/p_spec.cpp"
    "doom/p_spec.h"
    "doom/p_switch.cpp"
    "doom/p_telept.cpp"
    "doom/p_tick.cpp"
    "doom/p_tick.h"
    "doom/p_user.cpp"
    "doom/Precompiled.h"
    "doom/r_bsp.cpp"
    "doom/r_bsp.h"
    "doom/r_data.cpp"
    "doom/r_data.h"
    "doom/r_defs.h"
    "doom/r_draw.cpp"
    "doom/r_draw.h"
    "doom/r_local.h"
    "doom/r_main.cpp"
    "doom/r_main.h"
    "doom/r_plane.cpp"
    "doom/r_plane.h"
    "doom/r_segs.cpp"
    "doom/r_segs.h"
    "doom/r_sky.cpp"
    "doom/r_sky.h"
    "doom/r_state.h"
    "doom/r_things.cpp"
    "doom/r_things.h"
    "doom/s_sound.cpp"
    "doom/s_sound.h"
    "doom/sounds.cpp"
    "doom/sounds.h"
    "doom/st_lib.cpp"
    "doom/st_lib.h"
    "doom/st_stuff.cpp"
    "doom/st_stuff.h"
    "doom/structs.h"
    "doom/tables.cpp"
    "doom/tables.h"
    "doom/typedefs.h"
    "doom/v_video.cpp"
    "doom/v_video.h"
    "doom/vars.h"
    "doom/w_wad.cpp"
    "doom/w_wad.h"
    "doom/wi_stuff.cpp"
    "doom/wi_stuff.h"
    "doom/z_zone.cpp"
    "doom/z_zone.h"
)
source_group("Doom" FILES ${DOOMCLASSIC_FILES})

add_precompiled_header(DOOMCLASSIC_FILES DOOMCLASSIC_FILES "Precompiled.h" "doom/Precompiled.cpp")
source_group("Doom" FILES "doom/Precompiled.cpp")

add_library(doomclassic ${DOOMCLASSIC_FILES})
target_include_directories(doomclassic PUBLIC "../neo" ${SDL2_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR})

if(MSVC)
    # disable warning C4146: unary minus operator applied to unsigned type, result still unsigned
    target_compile_options(doomclassic PRIVATE "/wd4146")
endif()

add_subdirectory("timidity")
