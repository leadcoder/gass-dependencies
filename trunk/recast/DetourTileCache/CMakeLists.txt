CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(detourtilecache_SRCS 
	Source/DetourTileCache.cpp
	Source/DetourTileCacheBuilder.cpp
)

SET(detourtilecache_HDRS
	Include/DetourTileCache.h
	Include/DetourTileCacheBuilder.h
)

INCLUDE_DIRECTORIES(Include 
	../Detour/Include
	../Recast/Include
)

ADD_LIBRARY(DetourTileCache ${detourtilecache_SRCS} ${detourtilecache_HDRS})


install(TARGETS DetourTileCache
  RUNTIME DESTINATION ${RC_INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${RC_INSTALL_BIN_DIR}
  ARCHIVE DESTINATION ${RC_INSTALL_LIB_DIR})
 install (FILES  ${detourtilecache_HDRS} DESTINATION include)