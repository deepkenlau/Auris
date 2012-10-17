add_executable(test-http
	HTTP.cpp
	../common/HTTP/HeaderSet.cpp
	../common/HTTP/Request.cpp
)

add_executable(test-database
	Database.cpp
	../database/Database/Database.cpp
	../database/Database/Table.cpp
	../database/Database/Entity.cpp
	../common/tinyxml2.cpp
	../common/sha1.cpp
	../common/FileSystem.cpp
	../common/Path.cpp
)

include_directories(../)

find_package(BoehmGC REQUIRED)
include_directories(${BOEHMGC_INCLUDE_DIRS})

target_link_libraries(test-database ${BOEHMGC_LIBRARIES})