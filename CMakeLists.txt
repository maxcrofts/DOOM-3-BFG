if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-source build. Please make a build subdirectory and run CMake from there.")
endif()

cmake_minimum_required(VERSION 3.8)
project(doom3)

macro(target_pch TARGET PRECOMPILED_HEADER PRECOMPILED_SOURCE)
    if(MSVC)
        if(CLANG)
            get_filename_component(PRECOMPILED_SOURCE_ABS "${PRECOMPILED_SOURCE}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
            get_filename_component(PRECOMPILED_SOURCE_DIR "${PRECOMPILED_SOURCE_ABS}" DIRECTORY)
            set(PCH "${PRECOMPILED_SOURCE_DIR}/${PRECOMPILED_HEADER}")
        else()
            set(PCH "${PRECOMPILED_HEADER}")
        endif()

        get_target_property(SOURCES ${TARGET} SOURCES)
        list(FILTER SOURCES INCLUDE REGEX ".+\\.(c|cpp|h)$")

        set_source_files_properties(${PRECOMPILED_SOURCE} PROPERTIES COMPILE_FLAGS "/Yc\"${PCH}\"")
        set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS "/Yu\"${PCH}\" /FI\"${PCH}\"")

        # add precompiled header to sources
        target_sources(${TARGET} PRIVATE ${PRECOMPILED_SOURCE})
    endif()
endmacro()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED NO)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CLANG 1)
endif()

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
        # ignore sensitive to packing warnings
        add_compile_options(/wd4121)
        # allow nameless structs
        add_compile_options(/wd4201)
        # ignore unreferenced local functions
        add_compile_options(/wd4505)
        
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
        add_compile_options(-Wall -Wextra)
    endif()
    
    # disable some warnings
    add_compile_options(
        -Wno-cast-function-type
        -Wno-class-memaccess
        -Wno-dangling-else
        -Wno-deprecated-copy
        -Wno-deprecated-declarations
        -Wno-ignored-attributes
        -Wno-ignored-qualifiers
        -Wno-implicit-fallthrough
        -Wno-misleading-indentation
        -Wno-missing-braces
        -Wno-missing-field-initializers
        -Wno-overloaded-virtual
        -Wno-parentheses
        -Wno-reorder
        -Wno-shift-negative-value
        -Wno-sign-compare
        -Wno-switch
        -Wno-trigraphs
        -Wno-unknown-pragmas
        -Wno-unused-but-set-parameter
        -Wno-unused-but-set-variable
        -Wno-unused-function
        -Wno-unused-local-typedefs
        -Wno-unused-parameter
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-write-strings
    )

    if(CLANG)
        add_compile_options(
            -Wno-implicit-exception-spec-mismatch
            -Wno-inline-new-delete
            -Wno-invalid-noreturn
            -Wno-parentheses-equality
            -Wno-pragma-pack
            -Wno-reserved-user-defined-literal
            -Wno-unknown-warning-option
            -Wno-unused-private-field
        )
    endif()
endif()

# add defines if certain C++11 features are supported
# emulates write_compiler_detection_header() but doesn't create a file
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<COMPILE_FEATURES:cxx_constexpr>:ID_COMPILER_CXX_CONSTEXPR>
    $<$<COMPILE_FEATURES:cxx_nullptr>:ID_COMPILER_CXX_NULLPTR>
)

# OpenGL
find_package(OpenGL REQUIRED)

# SDL2
if(MSVC)
    # try to find SDL2
    file(GLOB SDL2_DIRS "${CMAKE_BINARY_DIR}/SDL2-*")
    list(LENGTH SDL2_DIRS SDL2_DIRS_LENGTH)
    if(SDL2_DIRS_LENGTH GREATER 0)
        list(GET SDL2_DIRS 0 SDL2_DIR)
    else() # if we can't find it...
        # download it
        set(SDL2_VERSION "2.0.9")
        file(DOWNLOAD "https://www.libsdl.org/release/SDL2-devel-${SDL2_VERSION}-VC.zip" "${CMAKE_BINARY_DIR}/SDL2-devel-${SDL2_VERSION}-VC.zip")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/SDL2-devel-${SDL2_VERSION}-VC.zip")
        set(SDL2_DIR "${CMAKE_BINARY_DIR}/SDL2-${SDL2_VERSION}")
    endif()

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SDL2_LIBDIR "${SDL2_DIR}/lib/x64")
    else()
        set(SDL2_LIBDIR "${SDL2_DIR}/lib/x86")
    endif()

    set(SDL2_LIBRARIES "${SDL2_LIBDIR}/SDL2.lib;${SDL2_LIBDIR}/SDL2main.lib")
    set(SDL2_INCLUDE_DIRS "${SDL2_DIR}/include")
    set(SDL2_DLL "${SDL2_LIBDIR}/SDL2.dll")
else()
    find_package(SDL2 REQUIRED)
endif()

# doomclassic
add_subdirectory("doomclassic")

# neo
add_subdirectory("neo")
