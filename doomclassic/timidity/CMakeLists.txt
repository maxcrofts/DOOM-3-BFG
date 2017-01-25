# timidity
set(TIMIDITY_FILES
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
source_group("" FILES ${TIMIDITY_FILES})

add_library(timidity ${TIMIDITY_FILES})
target_include_directories(timidity PUBLIC ${SDL2_INCLUDE_DIR})
