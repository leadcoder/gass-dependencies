cmake_minimum_required(VERSION 2.6)
set(LIB_NAME Hydrax)
#set(LIB_TYPE SHARED)
set(LIB_TYPE STATIC)

project(${LIB_NAME})
set(CMAKE_DEBUG_POSTFIX  "_d" CACHE STRING "add a postfix, usually d on windows")
#add_definitions()
set(OGRE_DIR $ENV{OGRE_HOME} CACHE PATH "Ogre 3D folder")
set(BOOST_DIR $ENV{BOOST_HOME} CACHE PATH "Boost folder")
set (OGRE_SDK_INCLUDE_DIRS "${OGRE_DIR}/include/OGRE"
		"${OGRE_DIR}/include/OGRE/Plugins/OctreeSceneManager")

set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax/Modules
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax/Modules/ProjectedGrid
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax/Modules/RadialGrid
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax/Modules/SimpleGrid
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax/Noise
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax/Noise/FFT
	${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax/Noise/Perlin
	${OGRE_SDK_INCLUDE_DIRS}
	${BOOST_DIR})

file(GLOB_RECURSE SOURCE_FILES_CPP ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE SOURCE_FILES_H ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
set(ALL_SOURCE_FILES ${SOURCE_FILES_CPP} ${SOURCE_FILES_H})

link_directories("${OGRE_DIR}/lib")

add_library(
	${LIB_NAME}
	${LIB_TYPE}
	${ALL_SOURCE_FILES}
)

set(PROJ_LINK_LIBRARIES optimized OgreMain 
	debug OgreMain_d 
	)
target_link_libraries(${LIB_NAME} ${PROJ_LINK_LIBRARIES})

add_definitions(-DHYDRAX_EXPORTS -DHYDRAX_LIB)

#INSTALL( FILES ${SOURCE_FILES_H} DESTINATION include/ )
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/Hydrax DESTINATION include)
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
  
  IF(MSVC)
  INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/lib/Debug
    DESTINATION lib
    FILES_MATCHING
    PATTERN *.pdb
)
ENDIF(MSVC)
