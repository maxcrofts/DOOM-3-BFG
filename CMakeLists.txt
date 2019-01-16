cmake_minimum_required(VERSION 3.6)
project(doom3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CLANG 1)
endif()

if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PLATFORM_NAME "Win64")
    else()
        set(PLATFORM_NAME "Win32")
    endif()

    string(REPLACE " " ";" GENERATOR_LIST ${CMAKE_GENERATOR})
    list(GET GENERATOR_LIST 2 VS_VERSION)
endif()

macro(add_precompiled_header FILES USE_PCH PRECOMPILED_HEADER PRECOMPILED_SOURCE)
    if(MSVC)
        set(SOURCES ${${USE_PCH}})
        list(FILTER SOURCES INCLUDE REGEX ".+\\.(c|cpp|h)$")

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
find_package(OpenGL REQUIRED)

# SDL2
if(WIN32)
    file(GLOB SDL2DIRS "external/SDL2-*")
    list(GET SDL2DIRS 0 SDL2DIR)
    set(ENV{SDL2DIR} "${SDL2DIR}")
endif()

find_package(SDL2 REQUIRED)

if(WIN32)
    list(GET SDL2_LIBRARY 0 SDL2_LIBRARY_HEAD)
    get_filename_component(SDL2_LIBRARY_DIR "${SDL2_LIBRARY_HEAD}" DIRECTORY)
    set(SDL2_DLL "${SDL2_LIBRARY_DIR}/SDL2.dll")
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

    # force W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    # treat warnings as errors for 32-bit builds
    if(CMAKE_SIZEOF_VOID_P EQUAL 4 AND NOT CLANG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    endif()

    # disable warnings introduced after Visual Studio 2013
    if(VS_VERSION GREATER 12)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4091 /wd4121 /wd4244 /wd4456 /wd4457 /wd4458 /wd4459 /wd4467 /wd4595")
    endif()

    # use most-general representation for member pointers
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /vmg")

    # disable some clang specific warnings
    if(CLANG)
        set(CLANG_FLAGS
            "-Wno-bitwise-op-parentheses"
            "-Wno-dangling-else"
            "-Wno-deprecated-declarations"
            "-Wno-ignored-attributes"
            "-Wno-ignored-qualifiers"
            "-Wno-implicit-exception-spec-mismatch"
            "-Wno-inline-new-delete"
            "-Wno-invalid-noreturn"
            "-Wno-logical-op-parentheses"
            "-Wno-missing-braces"
            "-Wno-missing-field-initializers"
            "-Wno-overloaded-virtual"
            "-Wno-parentheses-equality"
            "-Wno-pragma-pack"
            "-Wno-reorder"
            "-Wno-reserved-user-defined-literal"
            "-Wno-shift-negative-value"
            "-Wno-sign-compare"
            "-Wno-switch"
            "-Wno-unknown-pragmas"
            "-Wno-unused-function"
            "-Wno-unused-local-typedef"
            "-Wno-unused-parameter"
            "-Wno-unused-private-field"
            "-Wno-unused-value"
            "-Wno-unused-variable"
            "-Wno-writable-strings"
        )
        string(REPLACE ";" " " CLANG_FLAGS "${CLANG_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# neo
add_subdirectory("neo")