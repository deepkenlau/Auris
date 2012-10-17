set(DATABASE_EXEC_NAME auris-db)
add_executable(${DATABASE_EXEC_NAME}
	main.cpp
	Connection.cpp
	Server.cpp
	../common/Socket.cpp
	../common/tinyxml2.cpp
	Table.cpp
	Entry/Entry.cpp
	Entry/Field.cpp
)

find_package(BoehmGC REQUIRED)
target_link_libraries(${DATABASE_EXEC_NAME} ${BOEHMGC_LIBRARIES})
include_directories(${BOEHMGC_INCLUDE_DIRS})