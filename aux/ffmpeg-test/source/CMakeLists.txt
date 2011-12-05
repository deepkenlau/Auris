cmake_minimum_required(VERSION 2.6)
project(ffmpeg-test)

find_package(PkgConfig)

file(GLOB_RECURSE sources RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)
add_executable(ffmpeg-test ${sources})

pkg_check_modules(LIBAVCODEC "libavcodec")
pkg_check_modules(LIBAVFORMAT "libavformat")
pkg_check_modules(LIBAVUTIL "libavutil")
target_link_libraries(
	ffmpeg-test
	${LIBAVCODEC_LIBRARIES}
	${LIBAVFORMAT_LIBRARIES}
	${LIBAVUTIL_LIBRARIES}
)
