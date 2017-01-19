set(IDLIB_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/BitMsg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BitMsg.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Callback.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CommandLink.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/DataQueue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dict.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Dict.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Heap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Heap.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/LangDict.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/LangDict.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Lib.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Lib.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/MapFile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/MapFile.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ParallelJobList.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ParallelJobList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ParallelJobList_JobHeaders.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/precompiled.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/precompiled.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/RectAllocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SoftwareCache.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SoftwareCache.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Swap.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thread.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Thread.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Timer.h"
)

set(IDLIB_BV_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/bv/Bounds.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bv/Bounds.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/bv/Box.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bv/Box.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/bv/Sphere.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/bv/Sphere.h"
)
set(IDLIB_BV_FILES ${IDLIB_BV_FILES} PARENT_SCOPE)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_BV_FILES})

set(IDLIB_CONTAINERS_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/Array.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/BinSearch.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/BTree.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/HashIndex.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/HashIndex.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/HashTable.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/Hierarchy.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/LinkList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/List.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/PlaneSet.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/Queue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/Sort.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/Stack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/StaticList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/StrList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/StrPool.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/containers/VectorSet.h"
)
set(IDLIB_CONTAINERS_FILES ${IDLIB_CONTAINERS_FILES} PARENT_SCOPE)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_CONTAINERS_FILES})

set(IDLIB_GEOMETRY_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/DrawVert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/DrawVert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/DrawVert_intrinsics.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/JointTransform.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/JointTransform.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/RenderMatrix.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/RenderMatrix.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Patch.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Patch.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Polytope.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_Polytope.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_SweptSpline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Surface_SweptSpline.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/TraceModel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/TraceModel.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding2D.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/geometry/Winding2D.h"
)
set(IDLIB_GEOMETRY_FILES ${IDLIB_GEOMETRY_FILES} PARENT_SCOPE)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_GEOMETRY_FILES})

set(IDLIB_HASHING_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/hashing/CRC32.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hashing/CRC32.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD4.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD4.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD5.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/hashing/MD5.h"
)
set(IDLIB_HASHING_FILES ${IDLIB_HASHING_FILES} PARENT_SCOPE)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_HASHING_FILES})

set(IDLIB_MATH_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Angles.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Angles.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Complex.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Complex.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Curve.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Extrapolate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Interpolate.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Lcp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Lcp.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Math.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Math.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Matrix.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Matrix.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/MatX.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/MatX.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Ode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Ode.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Plane.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Plane.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Pluecker.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Pluecker.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Polynomial.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Polynomial.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Quat.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Quat.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Random.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Rotation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Rotation.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Simd.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Simd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_Generic.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_Generic.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Simd_SSE.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Vector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/Vector.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/VecX.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/VecX.h"
)
set(IDLIB_MATH_FILES ${IDLIB_MATH_FILES} PARENT_SCOPE)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_MATH_FILES})

set(IDLIB_SYS_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_alloc_tags.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_assert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_assert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_builddefines.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_defines.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_filesystem.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_includes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_intrinsics.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_threading.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sys/sys_types.h"
)
set(IDLIB_SYS_FILES ${IDLIB_SYS_FILES} PARENT_SCOPE)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_SYS_FILES})

if(WIN32)
    set(IDLIB_SYS_WIN32_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/sys/win32/win_thread.cpp"
    )
    set(IDLIB_SYS_WIN32_FILES ${IDLIB_SYS_WIN32_FILES} PARENT_SCOPE)
    set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_SYS_WIN32_FILES})
endif()

set(IDLIB_TEXT_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Base64.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Base64.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/CmdArgs.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/CmdArgs.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Lexer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Lexer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Parser.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Str.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Str.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/StrStatic.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Token.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Token.h"
)
set(IDLIB_TEXT_FILES ${IDLIB_TEXT_FILES} PARENT_SCOPE)
set(IDLIB_FILES ${IDLIB_FILES} ${IDLIB_TEXT_FILES})

set(IDLIB_FILES ${IDLIB_FILES} PARENT_SCOPE)
