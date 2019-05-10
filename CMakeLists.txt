if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-source build. Please make a build subdirectory and run CMake from there.")
endif()

cmake_minimum_required(VERSION 3.6)
project(doom3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CLANG 1)
endif()

macro(add_precompiled_header FILES USE_PCH PRECOMPILED_HEADER PRECOMPILED_SOURCE)
    if(MSVC)
        if(CLANG)
            get_filename_component(PRECOMPILED_SOURCE_ABS "${PRECOMPILED_SOURCE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
            get_filename_component(PRECOMPILED_SOURCE_DIR "${PRECOMPILED_SOURCE_ABS}" DIRECTORY)
            set(PCH "${PRECOMPILED_SOURCE_DIR}/${PRECOMPILED_HEADER}")
        else()
            set(PCH "${PRECOMPILED_HEADER}")
        endif()

        set(SOURCES ${${USE_PCH}})
        list(FILTER SOURCES INCLUDE REGEX ".+\\.(c|cpp|h)$")

        set_source_files_properties(${PRECOMPILED_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc\"${PCH}\"")
        set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS "/Yu\"${PCH}\" /FI\"${PCH}\"")

        # add precompiled header to FILES
        list(APPEND ${FILES} ${PRECOMPILED_SOURCE})
    endif()
endmacro()

# MSVC compile options
if(MSVC)
    # force static runtime libraries
    foreach(flag
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
    )
        string(REPLACE "/MD"  "/MT" "${flag}" "${${flag}}")
        set("${flag}" "${${flag}} /EHsc")
    endforeach()
    
    # enable multi-processor compilation
    add_compile_options(/MP)

    # use warning level 4
    add_compile_options(/W4)

    # use most-general representation for member pointers
    add_compile_options(/vmg)

    if(NOT CLANG) # not clang-cl
        # allow nameless structs
        add_compile_options(/wd4201)
        
        # treat warnings as errors for 32-bit builds
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            add_compile_options(/WX)
        endif()

        # disable warnings introduced after Visual Studio 2013
        if(MSVC_VERSION GREATER 1800)
            add_compile_options(/wd4091 /wd4121 /wd4244 /wd4456 /wd4457 /wd4458 /wd4459 /wd4467 /wd4595)
        endif()
    endif()
endif()

# GCC compile options
if(CLANG OR NOT MSVC)
    if(NOT MSVC) # not clang-cl
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        add_compile_options(-Wall -Wextra)
    endif()
    
    # disable some warnings
    add_compile_options(
        -Wno-bitwise-op-parentheses
        -Wno-dangling-else
        -Wno-deprecated-declarations
        -Wno-ignored-attributes
        -Wno-ignored-qualifiers
        -Wno-implicit-exception-spec-mismatch
        -Wno-inline-new-delete
        -Wno-invalid-noreturn
        -Wno-logical-op-parentheses
        -Wno-missing-braces
        -Wno-missing-field-initializers
        -Wno-overloaded-virtual
        -Wno-parentheses-equality
        -Wno-pragma-pack
        -Wno-reorder
        -Wno-reserved-user-defined-literal
        -Wno-shift-negative-value
        -Wno-sign-compare
        -Wno-switch
        -Wno-trigraphs
        -Wno-unknown-pragmas
        -Wno-unknown-warning-option
        -Wno-unused-function
        -Wno-unused-local-typedef
        -Wno-unused-parameter
        -Wno-unused-private-field
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-writable-strings
    )
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

# doomclassic
add_subdirectory("doomclassic")

# neo
add_subdirectory("neo")
