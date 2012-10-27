set(DATABASE_EXEC_NAME auris-db)
add_executable(${DATABASE_EXEC_NAME}
	main.cpp
	Connection.cpp
	Server.cpp
	../common/Socket.cpp
	../common/tinyxml2.cpp
	../common/Error.cpp
	../common/HTTP/Request.cpp
	../common/HTTP/Response.cpp
	../common/HTTP/HeaderSet.cpp
	Table.cpp
	Entry/Entry.cpp
	Entry/Field.cpp
)
include_directories(../)

find_package(BoehmGC REQUIRED)
target_link_libraries(${DATABASE_EXEC_NAME} ${BOEHMGC_LIBRARIES})
include_directories(${BOEHMGC_INCLUDE_DIRS})