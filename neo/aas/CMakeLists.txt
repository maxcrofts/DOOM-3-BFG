set(DOOM3BFG_AAS_FILES
    "AASFile.cpp"
    "AASFile.h"
    "AASFile_local.h"
    "AASFile_optimize.cpp"
    "AASFile_sample.cpp"
    "AASFileManager.cpp"
    "AASFileManager.h"
)
prepend(DOOM3BFG_AAS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/")
set(DOOM3BFG_AAS_FILES ${DOOM3BFG_AAS_FILES} PARENT_SCOPE)
