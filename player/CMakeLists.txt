set(PLAYER_EXEC_NAME auris-player)
add_executable(${PLAYER_EXEC_NAME}
	main.cpp
	Connection.cpp
	Player.cpp
	Session.cpp
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
find_package(LibAO REQUIRED)
include_directories(${LIBAO_INCLUDE_DIR})
target_link_libraries(${PLAYER_EXEC_NAME} ${BOEHMGC_LIBRARIES} ${LIBAV_LIBRARIES} ${LIBUUID_LIBRARIES} ${LIBAO_LIBRARY})

