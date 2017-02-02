# idLib
add_subdirectory("idlib")

# Game-d3xp
add_subdirectory("d3xp")

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
source_group("Sys\\Stub" FILES ${DOOM3BFG_SYS_STUB_FILES})
set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_STUB_FILES})

if(WIN32)
    source_group("Sys\\RC" FILES ${DOOM3BFG_SYS_RC_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_RC_FILES})
    source_group("Sys\\RC\\res" FILES ${DOOM3BFG_SYS_RC_RES_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_RC_RES_FILES})
    source_group("Sys\\Win32" FILES ${DOOM3BFG_SYS_WIN32_FILES})
    set(DOOM3BFG_FILES ${DOOM3BFG_FILES} ${DOOM3BFG_SYS_WIN32_FILES})
endif()

# precompiled headers
set(DOOM3BFG_USE_PRECOMPILED_HEADER ${DOOM3BFG_FILES})
list(REMOVE_ITEM DOOM3BFG_USE_PRECOMPILED_HEADER
    "${CMAKE_SOURCE_DIR}/framework/miniz.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/ShadowShared.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/dynamicshadowvolume/DynamicShadowVolume.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/prelightshadowvolume/PreLightShadowVolume.cpp"
    "${CMAKE_SOURCE_DIR}/renderer/jobs/staticshadowvolume/StaticShadowVolume.cpp"
)

add_precompiled_header(DOOM3BFG_FILES DOOM3BFG_USE_PRECOMPILED_HEADER "../idlib/precompiled.h" "framework/precompiled.cpp")
source_group("Framework" FILES "framework/precompiled.cpp")

add_executable(Doom3BFG WIN32 ${DOOM3BFG_FILES} $<TARGET_OBJECTS:Game-d3xp>)
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
