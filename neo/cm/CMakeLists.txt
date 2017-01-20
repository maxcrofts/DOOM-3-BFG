set(DOOM3BFG_CM_FILES
    "CollisionModel.h"
    "CollisionModel_contacts.cpp"
    "CollisionModel_contents.cpp"
    "CollisionModel_debug.cpp"
    "CollisionModel_files.cpp"
    "CollisionModel_load.cpp"
    "CollisionModel_local.h"
    "CollisionModel_rotate.cpp"
    "CollisionModel_trace.cpp"
    "CollisionModel_translate.cpp"
)
prepend(DOOM3BFG_CM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_CM_FILES ${DOOM3BFG_CM_FILES} PARENT_SCOPE)
