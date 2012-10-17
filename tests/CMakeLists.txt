add_executable(test-http
	HTTP.cpp
	../common/HTTP/HeaderSet.cpp
	../common/HTTP/Request.cpp
)

add_executable(test-database
	Database.cpp
	../database/Table.cpp
	../database/Entry/Entry.cpp
	../database/Entry/Field.cpp
	../common/tinyxml2.cpp
)

find_package(BoehmGC REQUIRED)
include_directories(${BOEHMGC_INCLUDE_DIRS})

target_link_libraries(test-database ${BOEHMGC_LIBRARIES})