# idLib
add_library(idLib STATIC ${IDLIB_FILES})
target_include_directories(idLib PUBLIC ${SDL2_INCLUDE_DIRS})

target_sources(idLib PRIVATE
    "bv/Bounds.cpp"
    "bv/Bounds.h"
    "bv/Box.cpp"
    "bv/Box.h"
    "bv/Sphere.cpp"
    "bv/Sphere.h"
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
    "geometry/DrawVert.cpp"
    "geometry/DrawVert.h"
    "geometry/DrawVert_intrinsics.h"
    "geometry/JointTransform.cpp"
    "geometry/JointTransform.h"
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
    "hashing/CRC32.cpp"
    "hashing/CRC32.h"
    "hashing/MD4.cpp"
    "hashing/MD4.h"
    "hashing/MD5.cpp"
    "hashing/MD5.h"
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
    "sys/sdl/sdl_thread.cpp"
    "sys/sys_alloc_tags.h"
    "sys/sys_assert.cpp"
    "sys/sys_assert.h"
    "sys/sys_builddefines.h"
    "sys/sys_defines.h"
    "sys/sys_filesystem.h"
    "sys/sys_includes.h"
    "sys/sys_intrinsics.h"
    "sys/sys_threading.h"
    "sys/sys_types.h"
    "text/Base64.cpp"
    "text/Base64.h"
    "text/CmdArgs.cpp"
    "text/CmdArgs.h"
    "text/Lexer.cpp"
    "text/Lexer.h"
    "text/Parser.cpp"
    "text/Parser.h"
    "text/Str.cpp"
    "text/Str.h"
    "text/StrStatic.h"
    "text/Token.cpp"
    "text/Token.h"
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
    "precompiled.h"
    "RectAllocator.cpp"
    "Swap.h"
    "Thread.cpp"
    "Thread.h"
    "Timer.cpp"
    "Timer.h"
)

target_pch(idLib "precompiled.h" "precompiled.cpp")

target_sources(idLib PRIVATE
    "geometry/RenderMatrix.cpp"
    "geometry/RenderMatrix.h"
    "SoftwareCache.cpp"
    "SoftwareCache.h"
)

get_target_property(IDLIB_SOURCES idLib SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${IDLIB_SOURCES})
