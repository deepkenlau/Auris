set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

set(DATABASE_EXEC_NAME auris-db)
add_executable(${DATABASE_EXEC_NAME}
	main.cpp
	Server.cpp
	../common/Socket.cpp
	../common/tinyxml2.cpp
)

find_package(BoehmGC REQUIRED)
target_link_libraries(${DATABASE_EXEC_NAME} ${BOEHMGC_LIBRARIES})
include_directories(${BOEHMGC_INCLUDE_DIRS})