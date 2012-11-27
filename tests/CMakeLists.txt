add_executable(test-http
	HTTP.cpp
	../common/HTTP/HeaderSet.cpp
	../common/HTTP/Request.cpp
	../common/Error.cpp
)

add_executable(test-database
	Database.cpp
	../database/database/Database.cpp
	../database/database/Commit.cpp
	../database/database/Table.cpp
	../database/database/Entity.cpp
	../database/database/Field.cpp
	../common/tinyxml2.cpp
	../common/sha1.cpp
	../common/sha1_utils.cpp
	../common/FileSystem.cpp
	../common/Path.cpp
	../common/Error.cpp
)

add_executable(test-mediastore
	Mediastore.cpp
	../database/media/Store.cpp
	../common/sha1.cpp
	../common/sha1_utils.cpp
	../common/FileSystem.cpp
	../common/Path.cpp
)

add_executable(test-coding
	Coding.cpp
	../common/coding/Decoder.cpp
	../common/coding/XMLDecoder.cpp
	../common/Error.cpp
	../common/tinyxml2.cpp
)

add_executable(test-libav
	LibAV.cpp
)

target_link_libraries(test-database ${BOEHMGC_LIBRARIES})
target_link_libraries(test-http ${BOEHMGC_LIBRARIES})
target_link_libraries(test-coding ${BOEHMGC_LIBRARIES})
target_link_libraries(test-libav ${BOEHMGC_LIBRARIES} ${LIBAV_LIBRARIES})
