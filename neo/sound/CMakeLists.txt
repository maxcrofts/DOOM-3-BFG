set(DOOM3BFG_SOUND_FILES
    "snd_emitter.cpp"
    "snd_local.h"
    "snd_shader.cpp"
    "snd_system.cpp"
    "snd_world.cpp"
    "sound.h"
    "SoundVoice.cpp"
    "SoundVoice.h"
    "WaveFile.cpp"
    "WaveFile.h"
)
prepend(DOOM3BFG_SOUND_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_SOUND_FILES ${DOOM3BFG_SOUND_FILES} PARENT_SCOPE)

set(DOOM3BFG_SOUND_OPENAL_FILES
    "OpenAL/AL_SoundHardware.cpp"
    "OpenAL/AL_SoundHardware.h"
    "OpenAL/AL_SoundSample.cpp"
    "OpenAL/AL_SoundSample.h"
    "OpenAL/AL_SoundVoice.cpp"
    "OpenAL/AL_SoundVoice.h"
)
prepend(DOOM3BFG_SOUND_OPENAL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_SOUND_OPENAL_FILES ${DOOM3BFG_SOUND_OPENAL_FILES} PARENT_SCOPE)
