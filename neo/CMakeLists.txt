cmake_minimum_required(VERSION 2.8.12)
project(doom3)

# OpenGL
find_package(OpenGL)

# SDL2
find_path(SDL2_INCLUDE_DIR
    SDL.h
    PATHS "../external/SDL2-2.0.5/include"
)

get_filename_component(SDL2_DIR "${SDL2_INCLUDE_DIR}/.." ABSOLUTE)

find_library(SDL2_LIB
    SDL2
    PATHS "${SDL2_DIR}/lib/x86"
)
find_library(SDL2MAIN_LIB
    SDL2main
    PATHS "${SDL2_DIR}/lib/x86"
)

set(SDL2_BIN "${SDL2_DIR}/lib/x86/SDL2.dll")

# libjpeg-turbo
find_path(LIBJPEG_TURBO_INCLUDE_DIR
    jpeglib.h
    PATHS "C:/libjpeg-turbo/include" "../external/libjpeg-turbo-1.5.1-vc/include"
)

get_filename_component(LIBJPEG_TURBO_DIR "${LIBJPEG_TURBO_INCLUDE_DIR}/.." ABSOLUTE)

find_library(LIBJPEG_TURBO
    jpeg
    PATHS "${LIBJPEG_TURBO_DIR}/lib"
)

set(LIBJPEG_TURBO_BIN "${LIBJPEG_TURBO_DIR}/bin/jpeg62.dll")

# DirectX
find_path(DIRECTX_INCLUDE_DIRS
    d3d9.h
    PATHS "$ENV{DXSDK_DIR}/Include" "$ENV{PROGRAMFILES}/Microsoft DirectX SDK*/Include"
)

get_filename_component(DIRECTX_DIR "${DIRECTX_INCLUDE_DIRS}/.." ABSOLUTE)

set(DIRECTX_LIBRARY_PATHS "${DIRECTX_DIR}/Lib/x86" "${DIRECTX_DIR}/Lib")

find_library(X3DAUDIO_LIB
    X3DAudio
    PATHS ${DIRECTX_LIBRARY_PATHS}
)

if(WIN32)
    set(PLATFORM_NAME "Win32")
endif()

if(MSVC)
    # force static runtime libraries
    foreach(flag
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
    )
        string(REPLACE "/MD"  "/MT" "${flag}" "${${flag}}")
        set("${flag}" "${${flag}} /EHsc")
    endforeach()
endif()

# doomclassic
add_subdirectory("../doomclassic" "../doomclassic")

# compiler flags
if(MSVC)
    # force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    # treat warnings as errors
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")

    # build fails with Visual Studio 2015 when /WX is enabled
    if(MSVC_VERSION GREATER 1800)
        # use Visual Studio 2013 warnings
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wv:18")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# we only want debug and release targets
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# set build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/${PLATFORM_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/${PLATFORM_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/${PLATFORM_NAME}")

# idLib
add_subdirectory("idlib")
source_group("" FILES ${IDLIB_FILES})
source_group("BV" FILES ${IDLIB_BV_FILES})
source_group("Containers" FILES ${IDLIB_CONTAINERS_FILES})
source_group("Geometry" FILES ${IDLIB_GEOMETRY_FILES})
source_group("Hashing" FILES ${IDLIB_HASHING_FILES})
source_group("Math" FILES ${IDLIB_MATH_FILES})
source_group("Text" FILES ${IDLIB_TEXT_FILES})
source_group("Sys" FILES ${IDLIB_SYS_FILES})
source_group("Sys\\Win32" FILES ${IDLIB_SYS_WIN32_FILES})
add_library(idLib ${IDLIB_FILES})

# Doom3BFG
set(DOOM3BFG_AAS_FILES
    "aas/AASFile.cpp"
    "aas/AASFile.h"
    "aas/AASFile_local.h"
    "aas/AASFile_optimize.cpp"
    "aas/AASFile_sample.cpp"
    "aas/AASFileManager.cpp"
    "aas/AASFileManager.h"
)
source_group("AAS" FILES ${DOOM3BFG_AAS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_AAS_FILES})

set(DOOM3BFG_CM_FILES
    "cm/CollisionModel.h"
    "cm/CollisionModel_contacts.cpp"
    "cm/CollisionModel_contents.cpp"
    "cm/CollisionModel_debug.cpp"
    "cm/CollisionModel_files.cpp"
    "cm/CollisionModel_load.cpp"
    "cm/CollisionModel_local.h"
    "cm/CollisionModel_rotate.cpp"
    "cm/CollisionModel_trace.cpp"
    "cm/CollisionModel_translate.cpp"
)
source_group("CM" FILES ${DOOM3BFG_CM_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_CM_FILES})

set(DOOM3BFG_FRAMEWORK_FILES
    "framework/BuildVersion.h"
    "framework/CmdSystem.cpp"
    "framework/CmdSystem.h"
    "framework/Common.cpp"
    "framework/Common.h"
    "framework/Common_demos.cpp"
    "framework/Common_dialog.cpp"
    "framework/Common_dialog.h"
    "framework/common_frame.cpp"
    "framework/Common_load.cpp"
    "framework/Common_local.h"
    "framework/Common_localize.cpp"
    "framework/Common_menu.cpp"
    "framework/Common_network.cpp"
    "framework/Common_printf.cpp"
    "framework/Compressor.cpp"
    "framework/Compressor.h"
    "framework/Console.cpp"
    "framework/Console.h"
    "framework/ConsoleHistory.cpp"
    "framework/ConsoleHistory.h"
    "framework/CVarSystem.cpp"
    "framework/CVarSystem.h"
    "framework/DebugGraph.cpp"
    "framework/DebugGraph.h"
    "framework/DemoChecksum.h"
    "framework/DemoFile.cpp"
    "framework/DemoFile.h"
    "framework/EditField.cpp"
    "framework/EditField.h"
    "framework/EventLoop.cpp"
    "framework/EventLoop.h"
    "framework/File.cpp"
    "framework/File.h"
    "framework/File_Manifest.cpp"
    "framework/File_Manifest.h"
    "framework/File_Resource.cpp"
    "framework/File_Resource.h"
    "framework/File_SaveGame.cpp"
    "framework/File_SaveGame.h"
    "framework/FileSystem.cpp"
    "framework/FileSystem.h"
    "framework/KeyInput.cpp"
    "framework/KeyInput.h"
    "framework/Licensee.h"
    "framework/miniz.cpp"
    "framework/miniz.h"
    "framework/PlayerProfile.cpp"
    "framework/PlayerProfile.h"
    "framework/precompiled.cpp"
    "framework/Serializer.h"
    "framework/TokenParser.cpp"
    "framework/TokenParser.h"
    "framework/Unzip.cpp"
    "framework/Unzip.h"
    "framework/UsercmdGen.cpp"
    "framework/UsercmdGen.h"
    "framework/Zip.cpp"
    "framework/Zip.h"
)
source_group("Framework" FILES ${DOOM3BFG_FRAMEWORK_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_FRAMEWORK_FILES})

set(DOOM3BFG_FRAMEWORK_DECLS_FILES
    "framework/DeclAF.cpp"
    "framework/DeclAF.h"
    "framework/DeclEntityDef.cpp"
    "framework/DeclEntityDef.h"
    "framework/DeclFX.cpp"
    "framework/DeclFX.h"
    "framework/DeclManager.cpp"
    "framework/DeclManager.h"
    "framework/DeclParticle.cpp"
    "framework/DeclParticle.h"
    "framework/DeclPDA.cpp"
    "framework/DeclPDA.h"
    "framework/DeclSkin.cpp"
    "framework/DeclSkin.h"
    "framework/DeclTable.cpp"
    "framework/DeclTable.h"
)
source_group("Framework\\Decls" FILES ${DOOM3BFG_FRAMEWORK_DECLS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_FRAMEWORK_DECLS_FILES})

set(DOOM3BFG_RENDERER_FILES
    "renderer/AutoRender.cpp"
    "renderer/AutoRender.h"
    "renderer/AutoRenderBink.cpp"
    "renderer/AutoRenderBink.h"
    "renderer/BinaryImage.cpp"
    "renderer/BinaryImage.h"
    "renderer/BinaryImageData.h"
    "renderer/BufferObject.cpp"
    "renderer/BufferObject.h"
    "renderer/Cinematic.cpp"
    "renderer/Cinematic.h"
    "renderer/Font.cpp"
    "renderer/Font.h"
    "renderer/GLMatrix.cpp"
    "renderer/GLMatrix.h"
    "renderer/GLState.h"
    "renderer/GraphicsAPIWrapper.h"
    "renderer/GuiModel.cpp"
    "renderer/GuiModel.h"
    "renderer/Image.h"
    "renderer/Image_files.cpp"
    "renderer/Image_intrinsic.cpp"
    "renderer/Image_load.cpp"
    "renderer/Image_process.cpp"
    "renderer/Image_program.cpp"
    "renderer/ImageManager.cpp"
    "renderer/ImageOpts.h"
    "renderer/Interaction.cpp"
    "renderer/Interaction.h"
    "renderer/Material.cpp"
    "renderer/Material.h"
    "renderer/Model.cpp"
    "renderer/Model.h"
    "renderer/Model_ase.cpp"
    "renderer/Model_ase.h"
    "renderer/Model_beam.cpp"
    "renderer/Model_liquid.cpp"
    "renderer/Model_local.h"
    "renderer/Model_lwo.cpp"
    "renderer/Model_lwo.h"
    "renderer/Model_ma.cpp"
    "renderer/Model_ma.h"
    "renderer/Model_md3.cpp"
    "renderer/Model_md3.h"
    "renderer/Model_md5.cpp"
    "renderer/Model_prt.cpp"
    "renderer/Model_sprite.cpp"
    "renderer/ModelDecal.cpp"
    "renderer/ModelDecal.h"
    "renderer/ModelManager.cpp"
    "renderer/ModelManager.h"
    "renderer/ModelOverlay.cpp"
    "renderer/ModelOverlay.h"
    "renderer/RenderContext.h"
    "renderer/RenderEntity.cpp"
    "renderer/RenderLog.cpp"
    "renderer/RenderLog.h"
    "renderer/RenderProgs.cpp"
    "renderer/RenderProgs.h"
    "renderer/RenderProgs_GLSL.cpp"
    "renderer/RenderSystem.cpp"
    "renderer/RenderSystem.h"
    "renderer/RenderSystem_init.cpp"
    "renderer/RenderTexture.h"
    "renderer/RenderWorld.cpp"
    "renderer/RenderWorld.h"
    "renderer/RenderWorld_defs.cpp"
    "renderer/RenderWorld_demo.cpp"
    "renderer/RenderWorld_load.cpp"
    "renderer/RenderWorld_local.h"
    "renderer/RenderWorld_portals.cpp"
    "renderer/ResolutionScale.cpp"
    "renderer/ResolutionScale.h"
    "renderer/ScreenRect.cpp"
    "renderer/ScreenRect.h"
    "renderer/simplex.h"
    "renderer/tr_backend_draw.cpp"
    "renderer/tr_backend_rendertools.cpp"
    "renderer/tr_frontend_addlights.cpp"
    "renderer/tr_frontend_addmodels.cpp"
    "renderer/tr_frontend_deform.cpp"
    "renderer/tr_frontend_guisurf.cpp"
    "renderer/tr_frontend_main.cpp"
    "renderer/tr_frontend_subview.cpp"
    "renderer/tr_local.h"
    "renderer/tr_trace.cpp"
    "renderer/tr_trisurf.cpp"
    "renderer/VertexCache.cpp"
    "renderer/VertexCache.h"
)
source_group("Renderer" FILES ${DOOM3BFG_RENDERER_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_FILES})

set(DOOM3BFG_RENDERER_COLOR_FILES
    "renderer/Color/ColorSpace.cpp"
    "renderer/Color/ColorSpace.h"
)
source_group("Renderer\\Color" FILES ${DOOM3BFG_RENDERER_COLOR_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_COLOR_FILES})

set(DOOM3BFG_RENDERER_DXT_FILES
    "renderer/DXT/DXTCodec.h"
    "renderer/DXT/DXTCodec_local.h"
    "renderer/DXT/DXTDecoder.cpp"
    "renderer/DXT/DXTEncoder.cpp"
    "renderer/DXT/DXTEncoder_SSE2.cpp"
)
source_group("Renderer\\DXT" FILES ${DOOM3BFG_RENDERER_DXT_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_DXT_FILES})

set(DOOM3BFG_RENDERER_JOBS_FILES
    "renderer/jobs/ShadowShared.cpp"
    "renderer/jobs/ShadowShared.h"
)
source_group("Renderer\\Jobs" FILES ${DOOM3BFG_RENDERER_JOBS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_FILES})

set(DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES
    "renderer/jobs/dynamicshadowvolume/DynamicShadowVolume.cpp"
    "renderer/jobs/dynamicshadowvolume/DynamicShadowVolume.h"
    "renderer/jobs/dynamicshadowvolume/DynamicShadowVolume_local.h"
)
source_group("Renderer\\Jobs\\DynamicShadowVolume" FILES ${DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES})

set(DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES
    "renderer/jobs/prelightshadowvolume/PreLightShadowVolume.cpp"
    "renderer/jobs/prelightshadowvolume/PreLightShadowVolume.h"
    "renderer/jobs/prelightshadowvolume/PreLightShadowVolume_local.h"
)
source_group("Renderer\\Jobs\\PreLightShadowVolume" FILES ${DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES})

set(DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES
    "renderer/jobs/staticshadowvolume/StaticShadowVolume.cpp"
    "renderer/jobs/staticshadowvolume/StaticShadowVolume.h"
    "renderer/jobs/staticshadowvolume/StaticShadowVolume_local.h"
)
source_group("Renderer\\Jobs\\StaticShadowVolume" FILES ${DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES})

set(DOOM3BFG_RENDERER_OPENGL_FILES
    "renderer/OpenGL/gl_backend.cpp"
    "renderer/OpenGL/gl_GraphicsAPIWrapper.cpp"
    "renderer/OpenGL/gl_image.cpp"
    "renderer/OpenGL/glext.h"
    "renderer/OpenGL/qgl.h"
    "renderer/OpenGL/qgl_linked.h"
    "renderer/OpenGL/wglext.h"
)
source_group("Renderer\\OpenGL" FILES ${DOOM3BFG_RENDERER_OPENGL_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_OPENGL_FILES})

set(DOOM3BFG_SOUND_FILES
    "sound/snd_emitter.cpp"
    "sound/snd_local.h"
    "sound/snd_shader.cpp"
    "sound/snd_system.cpp"
    "sound/snd_world.cpp"
    "sound/sound.h"
    "sound/SoundVoice.cpp"
    "sound/SoundVoice.h"
    "sound/WaveFile.cpp"
    "sound/WaveFile.h"
)
source_group("Sound" FILES ${DOOM3BFG_SOUND_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SOUND_FILES})

set(DOOM3BFG_SOUND_XAUDIO2_FILES
    "sound/XAudio2/XA2_SoundHardware.cpp"
    "sound/XAudio2/XA2_SoundHardware.h"
    "sound/XAudio2/XA2_SoundSample.cpp"
    "sound/XAudio2/XA2_SoundSample.h"
    "sound/XAudio2/XA2_SoundVoice.cpp"
    "sound/XAudio2/XA2_SoundVoice.h"
)
source_group("Sound\\XAudio2" FILES ${DOOM3BFG_SOUND_XAUDIO2_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SOUND_XAUDIO2_FILES})

set(DOOM3BFG_SWF_FILES
    "swf/SWF.h"
    "swf/SWF_Bitstream.cpp"
    "swf/SWF_Bitstream.h"
    "swf/SWF_Dictionary.cpp"
    "swf/SWF_Enums.h"
    "swf/SWF_Events.cpp"
    "swf/SWF_Image.cpp"
    "swf/SWF_Load.cpp"
    "swf/SWF_Main.cpp"
    "swf/SWF_Names.cpp"
    "swf/SWF_ParmList.cpp"
    "swf/SWF_ParmList.h"
    "swf/SWF_PlaceObject.cpp"
    "swf/SWF_Render.cpp"
    "swf/SWF_ScriptFunction.cpp"
    "swf/SWF_ScriptFunction.h"
    "swf/SWF_ScriptObject.cpp"
    "swf/SWF_ScriptObject.h"
    "swf/SWF_ScriptVar.cpp"
    "swf/SWF_ScriptVar.h"
    "swf/SWF_ShapeParser.cpp"
    "swf/SWF_ShapeParser.h"
    "swf/SWF_Shapes.cpp"
    "swf/SWF_Sounds.cpp"
    "swf/SWF_SpriteInstance.cpp"
    "swf/SWF_SpriteInstance.h"
    "swf/SWF_Sprites.cpp"
    "swf/SWF_Sprites.h"
    "swf/SWF_Text.cpp"
    "swf/SWF_TextInstance.cpp"
    "swf/SWF_TextInstance.h"
    "swf/SWF_Types.h"
    "swf/SWF_Zlib.cpp"
)
source_group("SWF" FILES ${DOOM3BFG_SWF_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SWF_FILES})

set(DOOM3BFG_SYS_FILES
    "sys/LightweightCompression.cpp"
    "sys/LightweightCompression.h"
    "sys/PacketProcessor.cpp"
    "sys/PacketProcessor.h"
    "sys/Snapshot.cpp"
    "sys/Snapshot.h"
    "sys/Snapshot_Jobs.cpp"
    "sys/Snapshot_Jobs.h"
    "sys/SnapshotProcessor.cpp"
    "sys/SnapshotProcessor.h"
    "sys/sys_achievements.cpp"
    "sys/sys_achievements.h"
    "sys/sys_dedicated_server_search.cpp"
    "sys/sys_dedicated_server_search.h"
    "sys/sys_lobby.cpp"
    "sys/sys_lobby.h"
    "sys/sys_lobby_backend.h"
    "sys/sys_lobby_backend_direct.cpp"
    "sys/sys_lobby_backend_direct.h"
    "sys/sys_lobby_migrate.cpp"
    "sys/sys_lobby_snapshot.cpp"
    "sys/sys_lobby_users.cpp"
    "sys/sys_local.cpp"
    "sys/sys_local.h"
    "sys/sys_localuser.cpp"
    "sys/sys_localuser.h"
    "sys/sys_profile.cpp"
    "sys/sys_profile.h"
    "sys/sys_public.h"
    "sys/sys_savegame.cpp"
    "sys/sys_savegame.h"
    "sys/sys_session.h"
    "sys/sys_session_callbacks.cpp"
    "sys/sys_session_local.cpp"
    "sys/sys_session_local.h"
    "sys/sys_session_savegames.cpp"
    "sys/sys_session_savegames.h"
    "sys/sys_signin.cpp"
    "sys/sys_signin.h"
    "sys/sys_stats.h"
    "sys/sys_stats_misc.h"
    "sys/sys_voicechat.cpp"
    "sys/sys_voicechat.h"
)
source_group("Sys" FILES ${DOOM3BFG_SYS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_FILES})

set(DOOM3BFG_SYS_SDL_FILES
    "sys/sdl/sdl_achievements.cpp"
    "sys/sdl/sdl_achievements.h"
    "sys/sdl/sdl_glimp.cpp"
    "sys/sdl/sdl_input.cpp"
    "sys/sdl/sdl_input.h"
    "sys/sdl/sdl_local.h"
    "sys/sdl/sdl_localuser.cpp"
    "sys/sdl/sdl_localuser.h"
    "sys/sdl/sdl_main.cpp"
    "sys/sdl/sdl_savegame.cpp"
    "sys/sdl/sdl_session_local.cpp"
    "sys/sdl/sdl_shared.cpp"
    "sys/sdl/sdl_signin.cpp"
    "sys/sdl/sdl_signin.h"
)
source_group("Sys\\SDL" FILES ${DOOM3BFG_SYS_SDL_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_SDL_FILES})

if(WIN32)
    set(DOOM3BFG_SYS_RC_FILES
        "sys/win32/rc/doom.rc"
        "sys/win32/rc/doom_resource.h"
    )
    source_group("Sys\\RC" FILES ${DOOM3BFG_SYS_RC_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_RC_FILES})

    set(DOOM3BFG_SYS_RC_RES_FILES
        "sys/win32/rc/res/doom.ico"
    )
    source_group("Sys\\RC\\res" FILES ${DOOM3BFG_SYS_RC_RES_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_RC_RES_FILES})

    set(DOOM3BFG_SYS_WIN32_FILES
        "sys/win32/win_cpu.cpp"
        "sys/win32/win_net.cpp"
        "sys/win32/win_syscon.cpp"
    )
    source_group("Sys\\Win32" FILES ${DOOM3BFG_SYS_WIN32_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_WIN32_FILES})
endif()

set(DOOM3BFG_UI_FILES
    "ui/BindWindow.cpp"
    "ui/BindWindow.h"
    "ui/ChoiceWindow.cpp"
    "ui/ChoiceWindow.h"
    "ui/DeviceContext.cpp"
    "ui/DeviceContext.h"
    "ui/EditWindow.cpp"
    "ui/EditWindow.h"
    "ui/FieldWindow.cpp"
    "ui/FieldWindow.h"
    "ui/GameBearShootWindow.cpp"
    "ui/GameBearShootWindow.h"
    "ui/GameBustOutWindow.cpp"
    "ui/GameBustOutWindow.h"
    "ui/GameSSDWindow.cpp"
    "ui/GameSSDWindow.h"
    "ui/GuiScript.cpp"
    "ui/GuiScript.h"
    "ui/ListGUI.cpp"
    "ui/ListGUI.h"
    "ui/ListGUILocal.h"
    "ui/ListWindow.cpp"
    "ui/ListWindow.h"
    "ui/Rectangle.h"
    "ui/RegExp.cpp"
    "ui/RegExp.h"
    "ui/RenderWindow.cpp"
    "ui/RenderWindow.h"
    "ui/SimpleWindow.cpp"
    "ui/SimpleWindow.h"
    "ui/SliderWindow.cpp"
    "ui/SliderWindow.h"
    "ui/UserInterface.cpp"
    "ui/UserInterface.h"
    "ui/UserInterfaceLocal.h"
    "ui/Window.cpp"
    "ui/Window.h"
    "ui/Winvar.cpp"
    "ui/Winvar.h"
)
source_group("Ui" FILES ${DOOM3BFG_UI_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_UI_FILES})

# Game-d3xp
add_subdirectory("d3xp")
source_group("Game-d3xp" FILES ${GAME_D3XP_ROOT_FILES})
source_group("Game-d3xp\\AI" FILES ${GAME_D3XP_AI_FILES})
source_group("Game-d3xp\\Animation" FILES ${GAME_D3XP_ANIMATION_FILES})
source_group("Game-d3xp\\GameSys" FILES ${GAME_D3XP_GAMESYS_FILES})
source_group("Game-d3xp\\Physics" FILES ${GAME_D3XP_PHYSICS_FILES})
source_group("Game-d3xp\\Script" FILES ${GAME_D3XP_SCRIPT_FILES})
source_group("Game-d3xp\\Menus" FILES ${GAME_D3XP_MENUS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${GAME_D3XP_FILES})

add_executable(Doom3BFG WIN32 ${DOOM3BFG_FILES})

target_include_directories(Doom3BFG PUBLIC ${SDL2_INCLUDE_DIR} ${LIBJPEG_TURBO_INCLUDE_DIR} ${DIRECTX_INCLUDE_DIRS})

target_link_libraries(Doom3BFG idLib doomclassic timidity)
target_link_libraries(Doom3BFG ${SDL2_LIB} ${SDL2MAIN_LIB} ${LIBJPEG_TURBO} ${OPENGL_LIBRARIES} ${X3DAUDIO_LIB})

if(WIN32)
target_link_libraries(Doom3BFG Dbghelp.lib winmm.lib)
endif()

# copy dlls to target dir
foreach(dll
    ${SDL2_BIN}
    ${LIBJPEG_TURBO_BIN}
)
    add_custom_command(
        TARGET Doom3BFG
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${dll} $<TARGET_FILE_DIR:Doom3BFG>
    )
endforeach()
