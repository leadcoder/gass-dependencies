CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(recast_utils_SRCS
	Source/ChunkyTriMesh.cpp
	Source/InputGeom.cpp
	Source/MeshLoaderObj.cpp
	Source/TiledNavigationMeshSerializer.cpp
	Source/TiledNavigationMeshBuilder.cpp
)

SET(recast_utils_HDRS
	Include/ChunkyTriMesh.h
	Include/InputGeom.h
	Include/MeshLoaderObj.h
	Include/TiledNavigationMeshSerializer.h
	Include/TiledNavigationMeshBuilder.h
)

INCLUDE_DIRECTORIES(Include 
	../Detour/Include
	../DetourTileCache/Include
	../Recast/Include)

ADD_LIBRARY(RecastUtils ${recast_utils_SRCS} ${recast_utils_HDRS})


install(TARGETS RecastUtils
  RUNTIME DESTINATION ${RC_INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${RC_INSTALL_BIN_DIR}
  ARCHIVE DESTINATION ${RC_INSTALL_LIB_DIR})
 install (FILES  ${recast_utils_HDRS} DESTINATION include)