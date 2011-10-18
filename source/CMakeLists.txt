cmake_minimum_required(VERSION 2.6)
project(Auris)

# Server
file(GLOB_RECURSE sources RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)
add_executable(aurisd ${sources})
