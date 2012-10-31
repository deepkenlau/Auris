cmake_minimum_required(VERSION 2.6)
project(Auris)

# Default to debug build.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")


# Make the root directory of the project available.
include_directories(.)


# Find the required libraries
find_package(LibUUID REQUIRED)
find_package(BoehmGC REQUIRED)
find_package(LibAV REQUIRED)

include_directories(${BOEHMGC_INCLUDE_DIRS})
include_directories(${LIBUUID_INCLUDE_DIRS})
include_directories(${LIBAV_INCLUDE_DIRS})


# Add the three components
#add_subdirectory(controller)
add_subdirectory(player)
add_subdirectory(database)
add_subdirectory(tests)
