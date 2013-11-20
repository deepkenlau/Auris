# Copyright (c) 2013 Fabian Schuiki
# This file configures the build of the various command line tools of Auris.

add_executable(db-add db-add.cpp)
add_executable(db-meta db-meta.cpp)
add_executable(db-blob db-blob.cpp)
add_executable(db-list db-list.cpp)
add_executable(db-hash-object db-hash-object.cpp)
add_executable(db-cat-object db-cat-object.cpp)
add_executable(db-rev-list db-rev-list.cpp)

target_link_libraries(db-add ${Boost_LIBRARIES} common)
target_link_libraries(db-meta ${Boost_LIBRARIES} common)
target_link_libraries(db-blob ${Boost_LIBRARIES} common)
target_link_libraries(db-list ${Boost_LIBRARIES} common)
target_link_libraries(db-hash-object ${Boost_LIBRARIES} common)
target_link_libraries(db-cat-object ${Boost_LIBRARIES} common)
target_link_libraries(db-rev-list ${Boost_LIBRARIES} common)
