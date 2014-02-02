# Copyright (c) 2013 Fabian Schuiki
# This file configures the build of the various command line tools of Auris.

add_executable(auris-add db-add.cpp)
add_executable(auris-meta db-meta.cpp)
add_executable(auris-blob db-blob.cpp)
add_executable(auris-list db-list.cpp)
add_executable(auris-hash-object db-hash-object.cpp)
add_executable(auris-cat-object db-cat-object.cpp)
add_executable(auris-rev-list db-rev-list.cpp)

set(commonlibs ${Boost_LIBRARIES} ${LIBUUID_LIBRARIES})

target_link_libraries(auris-add ${commonlibs})
target_link_libraries(auris-meta ${commonlibs})
target_link_libraries(auris-blob ${commonlibs})
target_link_libraries(auris-list ${commonlibs})
target_link_libraries(auris-hash-object ${commonlibs})
target_link_libraries(auris-cat-object ${commonlibs})
target_link_libraries(auris-rev-list ${commonlibs})

add_custom_target(
	auris-dummy ALL
	${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/auris-dummy ${CMAKE_CURRENT_BINARY_DIR}/auris
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/auris-dummy)

install(TARGETS
	auris-add
	auris-meta
	auris-blob
	auris-list
	auris-hash-object
	auris-cat-object
	auris-rev-list
	RUNTIME DESTINATION bin
	CONFIGURATIONS Release)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/auris DESTINATION bin CONFIGURATIONS Release)