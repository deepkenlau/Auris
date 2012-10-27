add_executable(test-http
	HTTP.cpp
	../common/HTTP/HeaderSet.cpp
	../common/HTTP/Request.cpp
	../common/Error.cpp
)

add_executable(test-database
	Database.cpp
	../database/Database/Database.cpp
	../database/Database/Commit.cpp
	../database/Database/Table.cpp
	../database/Database/Entity.cpp
	../database/Database/Field.cpp
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

include_directories(../)

find_package(BoehmGC REQUIRED)
include_directories(${BOEHMGC_INCLUDE_DIRS})

target_link_libraries(test-database ${BOEHMGC_LIBRARIES})
target_link_libraries(test-http ${BOEHMGC_LIBRARIES})
