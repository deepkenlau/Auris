cmake_minimum_required(VERSION 2.6)
project(Auris)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

# Add the three components
#add_subdirectory(controller)
#add_subdirectory(player)
add_subdirectory(database)
add_subdirectory(tests)