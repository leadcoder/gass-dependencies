CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(detourcrowd_SRCS
	Source/DetourPathCorridor.cpp
	Source/DetourLocalBoundary.cpp
	Source/DetourObstacleAvoidance.cpp
	Source/DetourPathQueue.cpp
	Source/DetourCrowd.cpp
	Source/DetourProximityGrid.cpp
)

SET(detourcrowd_HDRS
	Include/DetourPathCorridor.h
	Include/DetourCrowd.h
	Include/DetourObstacleAvoidance.h
	Include/DetourLocalBoundary.h
	Include/DetourProximityGrid.h
	Include/DetourPathQueue.h
)

INCLUDE_DIRECTORIES(Include 
	../Detour/Include
	../DetourTileCache
	../Recast/Include
)

ADD_LIBRARY(DetourCrowd ${detourcrowd_SRCS} ${detourcrowd_HDRS})


install(TARGETS DetourCrowd
  RUNTIME DESTINATION ${RC_INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${RC_INSTALL_BIN_DIR}
  ARCHIVE DESTINATION ${RC_INSTALL_LIB_DIR})
 install (FILES  ${detourcrowd_HDRS} DESTINATION include)
