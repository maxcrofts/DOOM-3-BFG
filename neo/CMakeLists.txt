cmake_minimum_required(VERSION 2.8.12)
project(doom3)

if(WIN32)
    set(PLATFORM_NAME "Win32")

    string(REPLACE " " ";" GENERATOR_LIST ${CMAKE_GENERATOR})
    list(GET GENERATOR_LIST 2 VS_VERSION)
endif()

# prepends CONTENT to each element of LIST
function(prepend LIST CONTENT)
  foreach(THING ${${LIST}})
    set(MODIFIED ${MODIFIED} ${CONTENT}${THING})
  endforeach()
  set(${LIST} ${MODIFIED} PARENT_SCOPE)
endfunction()

macro(add_precompiled_header FILES USE_PCH PRECOMPILED_HEADER PRECOMPILED_SOURCE)
    if(MSVC)
        set(SOURCES ${${USE_PCH}})

        set_source_files_properties(${PRECOMPILED_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc\"${PRECOMPILED_HEADER}\"")
        set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS "/Yu\"${PRECOMPILED_HEADER}\" /FI\"${PRECOMPILED_HEADER}\"")

        # add precompiled header to FILES
        list(APPEND ${FILES} ${PRECOMPILED_SOURCE})
    endif()
endmacro()

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

# OpenGL
find_package(OpenGL)

# OpenAL
find_path(OPENAL_INCLUDE_DIR
    al.h
    PATHS "../external/openal-soft-1.17.2-bin/include/AL"
)

get_filename_component(OPENAL_INCLUDE_DIR "${OPENAL_INCLUDE_DIR}/.." ABSOLUTE)

get_filename_component(OPENAL_DIR "${OPENAL_INCLUDE_DIR}/.." ABSOLUTE)

find_library(OPENAL_LIB
    OpenAL32
    PATHS "${OPENAL_DIR}/libs/Win32"
)

set(OPENAL_BIN "${OPENAL_DIR}/bin/Win32/OpenAL32.dll")

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

# doomclassic
add_subdirectory("../doomclassic" "../doomclassic")

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
source_group("Sys\\SDL" FILES ${IDLIB_SYS_SDL_FILES})

# Game-d3xp
add_subdirectory("d3xp")
source_group("" FILES ${GAME_D3XP_FILES})
source_group("AI" FILES ${GAME_D3XP_AI_FILES})
source_group("Animation" FILES ${GAME_D3XP_ANIMATION_FILES})
source_group("GameSys" FILES ${GAME_D3XP_GAMESYS_FILES})
source_group("Physics" FILES ${GAME_D3XP_PHYSICS_FILES})
source_group("Script" FILES ${GAME_D3XP_SCRIPT_FILES})
source_group("Menus" FILES ${GAME_D3XP_MENUS_FILES})

# Doom3BFG
add_subdirectory("aas")
source_group("AAS" FILES ${DOOM3BFG_AAS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_AAS_FILES})

add_subdirectory("cm")
source_group("CM" FILES ${DOOM3BFG_CM_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_CM_FILES})

add_subdirectory("framework")
source_group("Framework" FILES ${DOOM3BFG_FRAMEWORK_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_FRAMEWORK_FILES})
source_group("Framework\\Decls" FILES ${DOOM3BFG_FRAMEWORK_DECLS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_FRAMEWORK_DECLS_FILES})

add_subdirectory("renderer")
source_group("Renderer" FILES ${DOOM3BFG_RENDERER_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_FILES})
source_group("Renderer\\Color" FILES ${DOOM3BFG_RENDERER_COLOR_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_COLOR_FILES})
source_group("Renderer\\DXT" FILES ${DOOM3BFG_RENDERER_DXT_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_DXT_FILES})
source_group("Renderer\\Jobs" FILES ${DOOM3BFG_RENDERER_JOBS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_FILES})
source_group("Renderer\\Jobs\\DynamicShadowVolume" FILES ${DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES})
source_group("Renderer\\Jobs\\PreLightShadowVolume" FILES ${DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES})
source_group("Renderer\\Jobs\\StaticShadowVolume" FILES ${DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES})
source_group("Renderer\\OpenGL" FILES ${DOOM3BFG_RENDERER_OPENGL_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_RENDERER_OPENGL_FILES})

add_subdirectory("sound")
source_group("Sound" FILES ${DOOM3BFG_SOUND_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SOUND_FILES})
source_group("Sound\\OpenAL" FILES ${DOOM3BFG_SOUND_OPENAL_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SOUND_OPENAL_FILES})

add_subdirectory("swf")
source_group("SWF" FILES ${DOOM3BFG_SWF_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SWF_FILES})

add_subdirectory("ui")
source_group("Ui" FILES ${DOOM3BFG_UI_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_UI_FILES})

add_subdirectory("sys")
source_group("Sys" FILES ${DOOM3BFG_SYS_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_FILES})
source_group("Sys\\SDL" FILES ${DOOM3BFG_SYS_SDL_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_SDL_FILES})

if(WIN32)
    source_group("Sys\\RC" FILES ${DOOM3BFG_SYS_RC_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_RC_FILES})
    source_group("Sys\\RC\\res" FILES ${DOOM3BFG_SYS_RC_RES_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_RC_RES_FILES})
    source_group("Sys\\Win32" FILES ${DOOM3BFG_SYS_WIN32_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_WIN32_FILES})
endif()

# compiler flags
if(MSVC)
    # enable multi-processor compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    # treat warnings as errors
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")

    # build fails with Visual Studio 2015 when /WX is enabled
    if(VS_VERSION GREATER 12)
        # use Visual Studio 2013 warnings
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wv:18 /wd4244 /wd4091")
    endif()

    # OpenAL fix
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# we only want debug and release targets
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# set build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/${PLATFORM_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/${PLATFORM_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/${PLATFORM_NAME}")

# precompiled headers
set(IDLIB_USE_PRECOMPILED_HEADER ${IDLIB_FILES})
list(REMOVE_ITEM IDLIB_USE_PRECOMPILED_HEADER
    "${CMAKE_SOURCE_DIR}/idlib/SoftwareCache.cpp"
    "${CMAKE_SOURCE_DIR}/idlib/geometry/RenderMatrix.cpp"
)

set(DOOM3BFG_USE_PRECOMPILED_HEADER ${DOOM3BFG_FILES})
list(REMOVE_ITEM DOOM3BFG_USE_PRECOMPILED_HEADER
    "${CMAKE_SOURCE_DIR}/framework/miniz.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/ShadowShared.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/dynamicshadowvolume/DynamicShadowVolume.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/prelightshadowvolume/PreLightShadowVolume.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/staticshadowvolume/StaticShadowVolume.cpp"
)

add_precompiled_header(IDLIB_FILES IDLIB_USE_PRECOMPILED_HEADER "precompiled.h" "idlib/precompiled.cpp")
add_precompiled_header(GAME_D3XP_FILES GAME_D3XP_FILES "../idlib/precompiled.h" "d3xp/precompiled.cpp")
add_precompiled_header(DOOM3BFG_FILES DOOM3BFG_USE_PRECOMPILED_HEADER "../idlib/precompiled.h" "framework/precompiled.cpp")
source_group("" FILES "idlib/precompiled.cpp")
source_group("" FILES "d3xp/precompiled.cpp")
source_group("Framework" FILES "framework/precompiled.cpp")

add_library(idLib ${IDLIB_FILES})
add_library(Game-d3xp OBJECT ${GAME_D3XP_FILES})
add_executable(Doom3BFG WIN32 ${DOOM3BFG_FILES} $<TARGET_OBJECTS:Game-d3xp>)

target_include_directories(idLib PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(Game-d3xp PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(Doom3BFG PUBLIC ${SDL2_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR})
target_link_libraries(Doom3BFG idLib doomclassic timidity)
target_link_libraries(Doom3BFG ${SDL2_LIB} ${SDL2MAIN_LIB} ${OPENGL_LIBRARIES} ${OPENAL_LIB})

if(WIN32)
    target_link_libraries(Doom3BFG Dbghelp.lib winmm.lib)
endif()

# copy dlls to target dir
foreach(dll
    ${SDL2_BIN}
    ${OPENAL_BIN}
)
    add_custom_command(
        TARGET Doom3BFG
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${dll} $<TARGET_FILE_DIR:Doom3BFG>
    )
endforeach()
