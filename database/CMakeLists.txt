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
	../common/Path.cpp
	../common/FileSystem.cpp
	../common/sha1.cpp
	../common/sha1_utils.cpp
	../common/uuid.cpp

	database/Database.cpp
	database/Table.cpp
	database/Commit.cpp
	database/Entity.cpp
	database/Field.cpp

	media/Store.cpp
	library/Library.cpp
	library/Song.cpp
)
include_directories(../)

find_package(BoehmGC REQUIRED)
target_link_libraries(${DATABASE_EXEC_NAME} ${BOEHMGC_LIBRARIES})
include_directories(${BOEHMGC_INCLUDE_DIRS})

find_package(LibAV REQUIRED)
target_link_libraries(${DATABASE_EXEC_NAME} ${LIBAV_LIBRARIES})
include_directories(${LIBAV_INCLUDE_DIRS})