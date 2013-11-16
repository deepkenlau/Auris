# Copyright (c) 2013 Fabian Schuiki
# This file configures the build of the various command line tools of Auris.

add_executable(db-add db-add.cpp)
add_executable(db-meta db-meta.cpp)
target_link_libraries(db-add ${Boost_LIBRARIES} common)
target_link_libraries(db-meta ${Boost_LIBRARIES} common)