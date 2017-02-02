cmake_minimum_required(VERSION 2.8.12)
project(doom3)

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM_NAME "Win64")
    else()
        set(PLATFORM_NAME "Win32")
    endif()

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
    PATHS "external/openal-soft-1.17.2-bin/include/AL"
)

get_filename_component(OPENAL_INCLUDE_DIR "${OPENAL_INCLUDE_DIR}/.." ABSOLUTE)

get_filename_component(OPENAL_DIR "${OPENAL_INCLUDE_DIR}/.." ABSOLUTE)

find_library(OPENAL_LIB
    OpenAL32
    PATHS "${OPENAL_DIR}/libs/${PLATFORM_NAME}"
)

set(OPENAL_BIN "${OPENAL_DIR}/bin/${PLATFORM_NAME}/OpenAL32.dll")

# SDL2
find_path(SDL2_INCLUDE_DIR
    SDL.h
    PATHS "external/SDL2-2.0.5/include"
)

get_filename_component(SDL2_DIR "${SDL2_INCLUDE_DIR}/.." ABSOLUTE)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    find_library(SDL2_LIB
        SDL2
        PATHS "${SDL2_DIR}/lib/x64"
    )
    find_library(SDL2MAIN_LIB
        SDL2main
        PATHS "${SDL2_DIR}/lib/x64"
    )

    set(SDL2_BIN "${SDL2_DIR}/lib/x64/SDL2.dll")
else()
    find_library(SDL2_LIB
        SDL2
        PATHS "${SDL2_DIR}/lib/x86"
    )
    find_library(SDL2MAIN_LIB
        SDL2main
        PATHS "${SDL2_DIR}/lib/x86"
    )

    set(SDL2_BIN "${SDL2_DIR}/lib/x86/SDL2.dll")
endif()

# set build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${PLATFORM_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${PLATFORM_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/${PLATFORM_NAME}")

# doomclassic
add_subdirectory("doomclassic")

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

# neo
add_subdirectory("neo")
