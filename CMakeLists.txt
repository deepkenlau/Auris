# Copyright © 2013 Fabian Schuiki
# This is the root build configuration for the Auris project. It looks up
# common libraries and pulls in the source directories that need to be built.

cmake_minimum_required(VERSION 2.8)
project(Auris)


# Default to debug build.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions("-DBUILD_DEBUG")
else()
	add_definitions("-DBUILD_RELEASE")
endif()

# Add the modules directory to the CMake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")


# Find the required packages and configure the include directories.
find_package(Boost REQUIRED COMPONENTS filesystem system program_options)
find_package(LibUUID REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${LibUUID_INCLUDE_DIRS})
include_directories(source)


add_subdirectory(source)