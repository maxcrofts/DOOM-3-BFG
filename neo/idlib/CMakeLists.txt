set(IDLIB_FILES
    "BitMsg.cpp"
    "BitMsg.h"
    "Callback.h"
    "CommandLink.cpp"
    "DataQueue.h"
    "Dict.cpp"
    "Dict.h"
    "Heap.cpp"
    "Heap.h"
    "LangDict.cpp"
    "LangDict.h"
    "Lib.cpp"
    "Lib.h"
    "MapFile.cpp"
    "MapFile.h"
    "ParallelJobList.cpp"
    "ParallelJobList.h"
    "ParallelJobList_JobHeaders.h"
    #"precompiled.cpp"
    "precompiled.h"
    "RectAllocator.cpp"
    "SoftwareCache.cpp"
    "SoftwareCache.h"
    "Swap.h"
    "Thread.cpp"
    "Thread.h"
    "Timer.cpp"
    "Timer.h"
)

set(IDLIB_BV_FILES
    "bv/Bounds.cpp"
    "bv/Bounds.h"
    "bv/Box.cpp"
    "bv/Box.h"
    "bv/Sphere.cpp"
    "bv/Sphere.h"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_BV_FILES})

set(IDLIB_CONTAINERS_FILES
    "containers/Array.h"
    "containers/BinSearch.h"
    "containers/BTree.h"
    "containers/HashIndex.cpp"
    "containers/HashIndex.h"
    "containers/HashTable.h"
    "containers/Hierarchy.h"
    "containers/LinkList.h"
    "containers/List.h"
    "containers/PlaneSet.h"
    "containers/Queue.h"
    "containers/Sort.h"
    "containers/Stack.h"
    "containers/StaticList.h"
    "containers/StrList.h"
    "containers/StrPool.h"
    "containers/VectorSet.h"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_CONTAINERS_FILES})

set(IDLIB_GEOMETRY_FILES
    "geometry/DrawVert.cpp"
    "geometry/DrawVert.h"
    "geometry/DrawVert_intrinsics.h"
    "geometry/JointTransform.cpp"
    "geometry/JointTransform.h"
    "geometry/RenderMatrix.cpp"
    "geometry/RenderMatrix.h"
    "geometry/Surface.cpp"
    "geometry/Surface.h"
    "geometry/Surface_Patch.cpp"
    "geometry/Surface_Patch.h"
    "geometry/Surface_Polytope.cpp"
    "geometry/Surface_Polytope.h"
    "geometry/Surface_SweptSpline.cpp"
    "geometry/Surface_SweptSpline.h"
    "geometry/TraceModel.cpp"
    "geometry/TraceModel.h"
    "geometry/Winding.cpp"
    "geometry/Winding.h"
    "geometry/Winding2D.cpp"
    "geometry/Winding2D.h"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_GEOMETRY_FILES})

set(IDLIB_HASHING_FILES
    "hashing/CRC32.cpp"
    "hashing/CRC32.h"
    "hashing/MD4.cpp"
    "hashing/MD4.h"
    "hashing/MD5.cpp"
    "hashing/MD5.h"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_HASHING_FILES})

set(IDLIB_MATH_FILES
    "math/Angles.cpp"
    "math/Angles.h"
    "math/Complex.cpp"
    "math/Complex.h"
    "math/Curve.h"
    "math/Extrapolate.h"
    "math/Interpolate.h"
    "math/Lcp.cpp"
    "math/Lcp.h"
    "math/Math.cpp"
    "math/Math.h"
    "math/Matrix.cpp"
    "math/Matrix.h"
    "math/MatX.cpp"
    "math/MatX.h"
    "math/Ode.cpp"
    "math/Ode.h"
    "math/Plane.cpp"
    "math/Plane.h"
    "math/Pluecker.cpp"
    "math/Pluecker.h"
    "math/Polynomial.cpp"
    "math/Polynomial.h"
    "math/Quat.cpp"
    "math/Quat.h"
    "math/Random.h"
    "math/Rotation.cpp"
    "math/Rotation.h"
    "math/Simd.cpp"
    "math/Simd.h"
    "math/Simd_Generic.cpp"
    "math/Simd_Generic.h"
    "math/Simd_SSE.cpp"
    "math/Simd_SSE.h"
    "math/Vector.cpp"
    "math/Vector.h"
    "math/VecX.cpp"
    "math/VecX.h"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_MATH_FILES})

set(IDLIB_SYS_FILES
    "sys/sys_alloc_tags.h"
    "sys/sys_assert.cpp"
    "sys/sys_assert.h"
    "sys/sys_atomic.cpp"
    "sys/sys_builddefines.h"
    "sys/sys_defines.h"
    "sys/sys_filesystem.h"
    "sys/sys_includes.h"
    "sys/sys_intrinsics.h"
    "sys/sys_threading.h"
    "sys/sys_types.h"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_SYS_FILES})

set(IDLIB_SYS_SDL_FILES
    "sys/sdl/sdl_thread.cpp"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_SYS_SDL_FILES})

set(IDLIB_TEXT_FILES
    "Base64.cpp"
    "Base64.h"
    "CmdArgs.cpp"
    "CmdArgs.h"
    "Lexer.cpp"
    "Lexer.h"
    "Parser.cpp"
    "Parser.h"
    "Str.cpp"
    "Str.h"
    "StrStatic.h"
    "Token.cpp"
    "Token.h"
)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_TEXT_FILES})

set(IDLIB_USE_PRECOMPILED_HEADER ${IDLIB_FILES})
list(REMOVE_ITEM IDLIB_USE_PRECOMPILED_HEADER
    "idlib/SoftwareCache.cpp"
    "idlib/geometry/RenderMatrix.cpp"
)

add_precompiled_header(IDLIB_FILES IDLIB_USE_PRECOMPILED_HEADER "precompiled.h" "precompiled.cpp")

source_group("" FILES ${IDLIB_FILES})
source_group("BV" FILES ${IDLIB_BV_FILES})
source_group("Containers" FILES ${IDLIB_CONTAINERS_FILES})
source_group("Geometry" FILES ${IDLIB_GEOMETRY_FILES})
source_group("Hashing" FILES ${IDLIB_HASHING_FILES})
source_group("Math" FILES ${IDLIB_MATH_FILES})
source_group("Text" FILES ${IDLIB_TEXT_FILES})
source_group("Sys" FILES ${IDLIB_SYS_FILES})
source_group("Sys\\SDL" FILES ${IDLIB_SYS_SDL_FILES})

add_library(idLib ${IDLIB_FILES})
target_include_directories(idLib PUBLIC ${SDL2_INCLUDE_DIR})
