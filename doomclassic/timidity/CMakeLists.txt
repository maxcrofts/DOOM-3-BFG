# timidity
add_library(timidity ${TIMIDITY_FILES})
target_include_directories(timidity PUBLIC ${SDL2_INCLUDE_DIR})
if(MSVC)
    target_compile_options(timidity PRIVATE /WX-)
endif()

target_sources(timidity PRIVATE
    "common.cpp"
    "common.h"
    "config.h"
    "controls.cpp"
    "controls.h"
    "filter.cpp"
    "filter.h"
    "instrum.cpp"
    "instrum.h"
    "mix.cpp"
    "mix.h"
    "output.cpp"
    "output.h"
    "playmidi.cpp"
    "playmidi.h"
    "readmidi.cpp"
    "readmidi.h"
    "resample.cpp"
    "resample.h"
    "sdl_a.cpp"
    "sdl_c.cpp"
    "structs.h"
    "tables.cpp"
    "tables.h"
    "timidity.cpp"
    "timidity.h"
)

get_target_property(TIMIDITY_SOURCES timidity SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TIMIDITY_SOURCES})
