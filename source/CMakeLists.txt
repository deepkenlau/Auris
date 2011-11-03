cmake_minimum_required(VERSION 2.6)
project(Auris)

find_package(PkgConfig)

# Server
file(GLOB_RECURSE sources RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)
add_executable(aurisd ${sources})

find_package(Threads)
target_link_libraries(aurisd ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenSSL)
target_link_libraries(aurisd ${OPENSSL_LIBRARIES})

pkg_check_modules(UUID uuid)
if (UUID_FOUND)
	target_link_libraries(aurisd ${UUID_LIBRARIES})
endif (UUID_FOUND)

pkg_check_modules(LIBCONFIG "libconfig++")
target_link_libraries(aurisd ${LIBCONFIG_LIBRARIES})

# Automatically generate the documentation using Doxygen.
find_package(Doxygen)
if (DOXYGEN_FOUND)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../doc/
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif (DOXYGEN_FOUND)

