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

install(DIRECTORY auris/ DESTINATION include/auris CONFIGURATIONS Release)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")
find_package(Boost REQUIRED COMPONENTS filesystem system program_options)
if (NOT APPLE)
	find_package(LibUUID REQUIRED)
endif()
include_directories(${Boost_INCLUDE_DIRS} ${LibUUID_INCLUDE_DIRS} .)
add_subdirectory(tools)
