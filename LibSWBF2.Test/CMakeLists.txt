cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0079 NEW)


#[[

Buildfile for testing.  Most setup
is done in the libSWBF2 project,
and the relevant includes, options,
and links are automatically added to each test
that links to SWBF2.

]]

project(libSWBF2_TESTS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#SOURCES
add_executable(lightsTest 	 testLights.cpp)
add_executable(segmentsTest  testSegments.cpp)
add_executable(blendmapTest  testTerrainSplatMap.cpp)
add_executable(heightmapTest testTerrainHeightMap.cpp)
add_executable(entitiesTest  testInstancesEntities.cpp)


#LINKING
target_link_libraries(lightsTest    PUBLIC SWBF2)
target_link_libraries(segmentsTest  PUBLIC SWBF2)
target_link_libraries(blendmapTest  PUBLIC SWBF2)
target_link_libraries(heightmapTest PUBLIC SWBF2)
target_link_libraries(entitiesTest  PUBLIC SWBF2)