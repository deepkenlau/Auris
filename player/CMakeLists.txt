set(PLAYER_EXEC_NAME auris-player)
add_executable(${PLAYER_EXEC_NAME}
	main.cpp
	Connection.cpp
	Player.cpp
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

)
include_directories(../)

find_package(BoehmGC REQUIRED)
target_link_libraries(${PLAYER_EXEC_NAME} ${BOEHMGC_LIBRARIES})
include_directories(${BOEHMGC_INCLUDE_DIRS})

find_package(LibAV REQUIRED)
target_link_libraries(${PLAYER_EXEC_NAME} ${LIBAV_LIBRARIES})
include_directories(${LIBAV_INCLUDE_DIRS})
message(dirs ${LIBAV_INCLUDE_DIRS})
