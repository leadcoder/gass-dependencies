cmake_minimum_required(VERSION 2.6)
set(LIB_NAME tinyxml)
set(LIB_TYPE STATIC)
project(${LIB_NAME})

set(SHARED_LIB_EXT so)
if(WIN32)
	set(SHARED_LIB_EXT dll)
else()
	set(SHARED_LIB_EXT so)
endif()
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})
set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")
include_directories(./)

file(
	GLOB_RECURSE
	source_files
	*.h
	*.cpp
)

add_library(
	${LIB_NAME}
	${LIB_TYPE}
	${source_files}
)

#SET_TARGET_PROPERTIES (tinyxml PROPERTIES DEFINE_SYMBOL  "EXPORT" )

INSTALL( FILES tinyxml.h tinystr.h DESTINATION include/ )
#INSTALL( FILES lib/debug/tinyxml.${SHARED_LIB_EXT} DESTINATION lib/ )
if(WIN32)
install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION bin/debug CONFIGURATIONS Debug
  LIBRARY DESTINATION bin/debug CONFIGURATIONS Debug
  ARCHIVE DESTINATION lib/debug CONFIGURATIONS Debug
  )

install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION bin/release CONFIGURATIONS Release
  LIBRARY DESTINATION bin/release CONFIGURATIONS Release
  ARCHIVE DESTINATION lib/release CONFIGURATIONS Release
  )
else()
install(TARGETS ${LIB_NAME}
  RUNTIME DESTINATION bin 
  LIBRARY DESTINATION bin 
  ARCHIVE DESTINATION lib 
  )
endif()
