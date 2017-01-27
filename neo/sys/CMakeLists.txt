set(DOOM3BFG_SYS_FILES
    "LightweightCompression.cpp"
    "LightweightCompression.h"
    "PacketProcessor.cpp"
    "PacketProcessor.h"
    "Snapshot.cpp"
    "Snapshot.h"
    "Snapshot_Jobs.cpp"
    "Snapshot_Jobs.h"
    "SnapshotProcessor.cpp"
    "SnapshotProcessor.h"
    "sys_achievements.cpp"
    "sys_achievements.h"
    "sys_cpu.cpp"
    "sys_dedicated_server_search.cpp"
    "sys_dedicated_server_search.h"
    "sys_lobby.cpp"
    "sys_lobby.h"
    "sys_lobby_backend.h"
    "sys_lobby_backend_direct.cpp"
    "sys_lobby_backend_direct.h"
    "sys_lobby_migrate.cpp"
    "sys_lobby_snapshot.cpp"
    "sys_lobby_users.cpp"
    "sys_local.cpp"
    "sys_local.h"
    "sys_localuser.cpp"
    "sys_localuser.h"
    "sys_profile.cpp"
    "sys_profile.h"
    "sys_public.h"
    "sys_savegame.cpp"
    "sys_savegame.h"
    "sys_session.h"
    "sys_session_callbacks.cpp"
    "sys_session_local.cpp"
    "sys_session_local.h"
    "sys_session_savegames.cpp"
    "sys_session_savegames.h"
    "sys_signin.cpp"
    "sys_signin.h"
    "sys_stats.h"
    "sys_stats_misc.h"
    "sys_voicechat.cpp"
    "sys_voicechat.h"
)
prepend(DOOM3BFG_SYS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_SYS_FILES ${DOOM3BFG_SYS_FILES} PARENT_SCOPE)

set(DOOM3BFG_SYS_SDL_FILES
    "sdl/sdl_achievements.cpp"
    "sdl/sdl_achievements.h"
    "sdl/sdl_glimp.cpp"
    "sdl/sdl_input.cpp"
    "sdl/sdl_input.h"
    "sdl/sdl_local.h"
    "sdl/sdl_localuser.cpp"
    "sdl/sdl_localuser.h"
    "sdl/sdl_main.cpp"
    "sdl/sdl_savegame.cpp"
    "sdl/sdl_session_local.cpp"
    "sdl/sdl_shared.cpp"
    "sdl/sdl_signin.cpp"
    "sdl/sdl_signin.h"
)
prepend(DOOM3BFG_SYS_SDL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_SYS_SDL_FILES ${DOOM3BFG_SYS_SDL_FILES} PARENT_SCOPE)

if(WIN32)
    set(DOOM3BFG_SYS_RC_FILES
        "win32/rc/doom.rc"
        "win32/rc/doom_resource.h"
    )
    prepend(DOOM3BFG_SYS_RC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
    set(DOOM3BFG_SYS_RC_FILES ${DOOM3BFG_SYS_RC_FILES} PARENT_SCOPE)

    set(DOOM3BFG_SYS_RC_RES_FILES
        "win32/rc/res/doom.ico"
    )
    prepend(DOOM3BFG_SYS_RC_RES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
    set(DOOM3BFG_SYS_RC_RES_FILES ${DOOM3BFG_SYS_RC_RES_FILES} PARENT_SCOPE)

    set(DOOM3BFG_SYS_WIN32_FILES
        "win32/win_net.cpp"
        "win32/win_syscon.cpp"
    )
    prepend(DOOM3BFG_SYS_WIN32_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
    set(DOOM3BFG_SYS_WIN32_FILES ${DOOM3BFG_SYS_WIN32_FILES} PARENT_SCOPE)
endif()
