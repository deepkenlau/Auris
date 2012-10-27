cmake_minimum_required(VERSION 2.6)
project(Auris)

# Default to debug build.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

# Add the three components
#add_subdirectory(controller)
#add_subdirectory(player)
add_subdirectory(database)
add_subdirectory(tests)