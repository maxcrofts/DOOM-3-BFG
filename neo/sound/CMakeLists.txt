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

set(DOOM3BFG_SOUND_XAUDIO2_FILES
    "XAudio2/XA2_SoundHardware.cpp"
    "XAudio2/XA2_SoundHardware.h"
    "XAudio2/XA2_SoundSample.cpp"
    "XAudio2/XA2_SoundSample.h"
    "XAudio2/XA2_SoundVoice.cpp"
    "XAudio2/XA2_SoundVoice.h"
)
prepend(DOOM3BFG_SOUND_XAUDIO2_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_SOUND_XAUDIO2_FILES ${DOOM3BFG_SOUND_XAUDIO2_FILES} PARENT_SCOPE)
