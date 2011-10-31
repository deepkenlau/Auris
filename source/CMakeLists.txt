cmake_minimum_required(VERSION 2.6)
project(Auris)

# Server
file(GLOB_RECURSE sources RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)
add_executable(aurisd ${sources})

find_package (Threads)
target_link_libraries (aurisd ${CMAKE_THREAD_LIBS_INIT})

# Automatically generate the documentation using Doxygen.
find_package(Doxygen)
if (DOXYGEN_FOUND)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../doc/
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif (DOXYGEN_FOUND)

