cmake_minimum_required(VERSION 2.6)
project(SonusVeritas)

# Server
file(GLOB_RECURSE sources RELATIVE ${CMAKE_SOURCE_DIR} *.cpp)
add_executable(server ${sources})
