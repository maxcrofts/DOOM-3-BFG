set(DOOM3BFG_RENDERER_FILES
    "AutoRender.cpp"
    "AutoRender.h"
    "AutoRenderBink.cpp"
    "AutoRenderBink.h"
    "BinaryImage.cpp"
    "BinaryImage.h"
    "BinaryImageData.h"
    "BufferObject.cpp"
    "BufferObject.h"
    "Cinematic.cpp"
    "Cinematic.h"
    "Font.cpp"
    "Font.h"
    "GLMatrix.cpp"
    "GLMatrix.h"
    "GLState.h"
    "GraphicsAPIWrapper.h"
    "GuiModel.cpp"
    "GuiModel.h"
    "Image.h"
    "Image_files.cpp"
    "Image_intrinsic.cpp"
    "Image_load.cpp"
    "Image_process.cpp"
    "Image_program.cpp"
    "ImageManager.cpp"
    "ImageOpts.h"
    "Interaction.cpp"
    "Interaction.h"
    "Material.cpp"
    "Material.h"
    "Model.cpp"
    "Model.h"
    "Model_ase.cpp"
    "Model_ase.h"
    "Model_beam.cpp"
    "Model_liquid.cpp"
    "Model_local.h"
    "Model_lwo.cpp"
    "Model_lwo.h"
    "Model_ma.cpp"
    "Model_ma.h"
    "Model_md3.cpp"
    "Model_md3.h"
    "Model_md5.cpp"
    "Model_prt.cpp"
    "Model_sprite.cpp"
    "ModelDecal.cpp"
    "ModelDecal.h"
    "ModelManager.cpp"
    "ModelManager.h"
    "ModelOverlay.cpp"
    "ModelOverlay.h"
    "RenderContext.h"
    "RenderEntity.cpp"
    "RenderLog.cpp"
    "RenderLog.h"
    "RenderProgs.cpp"
    "RenderProgs.h"
    "RenderProgs_GLSL.cpp"
    "RenderSystem.cpp"
    "RenderSystem.h"
    "RenderSystem_init.cpp"
    "RenderTexture.h"
    "RenderWorld.cpp"
    "RenderWorld.h"
    "RenderWorld_defs.cpp"
    "RenderWorld_demo.cpp"
    "RenderWorld_load.cpp"
    "RenderWorld_local.h"
    "RenderWorld_portals.cpp"
    "ResolutionScale.cpp"
    "ResolutionScale.h"
    "ScreenRect.cpp"
    "ScreenRect.h"
    "simplex.h"
    "tr_backend_draw.cpp"
    "tr_backend_rendertools.cpp"
    "tr_frontend_addlights.cpp"
    "tr_frontend_addmodels.cpp"
    "tr_frontend_deform.cpp"
    "tr_frontend_guisurf.cpp"
    "tr_frontend_main.cpp"
    "tr_frontend_subview.cpp"
    "tr_local.h"
    "tr_trace.cpp"
    "tr_trisurf.cpp"
    "VertexCache.cpp"
    "VertexCache.h"
)
prepend(DOOM3BFG_RENDERER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_FILES ${DOOM3BFG_RENDERER_FILES} PARENT_SCOPE)

set(DOOM3BFG_RENDERER_COLOR_FILES
    "Color/ColorSpace.cpp"
    "Color/ColorSpace.h"
)
prepend(DOOM3BFG_RENDERER_COLOR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_COLOR_FILES ${DOOM3BFG_RENDERER_COLOR_FILES} PARENT_SCOPE)

set(DOOM3BFG_RENDERER_DXT_FILES
    "DXT/DXTCodec.h"
    "DXT/DXTCodec_local.h"
    "DXT/DXTDecoder.cpp"
    "DXT/DXTEncoder.cpp"
    "DXT/DXTEncoder_SSE2.cpp"
)
prepend(DOOM3BFG_RENDERER_DXT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_DXT_FILES ${DOOM3BFG_RENDERER_DXT_FILES} PARENT_SCOPE)

set(DOOM3BFG_RENDERER_JOBS_FILES
    "jobs/ShadowShared.cpp"
    "jobs/ShadowShared.h"
)
prepend(DOOM3BFG_RENDERER_JOBS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_JOBS_FILES ${DOOM3BFG_RENDERER_JOBS_FILES} PARENT_SCOPE)

set(DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES
    "jobs/dynamicshadowvolume/DynamicShadowVolume.cpp"
    "jobs/dynamicshadowvolume/DynamicShadowVolume.h"
    "jobs/dynamicshadowvolume/DynamicShadowVolume_local.h"
)
prepend(DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES ${DOOM3BFG_RENDERER_JOBS_DYNAMICSHADOWVOLUME_FILES} PARENT_SCOPE)

set(DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES
    "jobs/prelightshadowvolume/PreLightShadowVolume.cpp"
    "jobs/prelightshadowvolume/PreLightShadowVolume.h"
    "jobs/prelightshadowvolume/PreLightShadowVolume_local.h"
)
prepend(DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES ${DOOM3BFG_RENDERER_JOBS_PRELIGHTSHADOWVOLUME_FILES} PARENT_SCOPE)

set(DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES
    "jobs/staticshadowvolume/StaticShadowVolume.cpp"
    "jobs/staticshadowvolume/StaticShadowVolume.h"
    "jobs/staticshadowvolume/StaticShadowVolume_local.h"
)
prepend(DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES ${DOOM3BFG_RENDERER_JOBS_STATICSHADOWVOLUME_FILES} PARENT_SCOPE)

set(DOOM3BFG_RENDERER_OPENGL_FILES
    "OpenGL/gl_backend.cpp"
    "OpenGL/gl_GraphicsAPIWrapper.cpp"
    "OpenGL/gl_image.cpp"
    "OpenGL/glext.h"
    "OpenGL/qgl.h"
    "OpenGL/qgl_linked.h"
    "OpenGL/wglext.h"
)
prepend(DOOM3BFG_RENDERER_OPENGL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_RENDERER_OPENGL_FILES ${DOOM3BFG_RENDERER_OPENGL_FILES} PARENT_SCOPE)
