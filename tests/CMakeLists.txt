enable_testing()
add_test(test-http ${CMAKE_CURRENT_BINARY_DIR}/test-http)

add_executable(test-http
	HTTP.cpp
	../common/HTTP/HeaderSet.cpp
)