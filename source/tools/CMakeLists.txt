# Copyright (c) 2013 Fabian Schuiki
# This file configures the build of the various command line tools of Auris.

add_executable(adb-add db-add.cpp)
target_link_libraries(adb-add ${Boost_LIBRARIES})