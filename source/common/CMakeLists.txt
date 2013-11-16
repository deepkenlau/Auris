# Copyright (c) 2013 Fabian Schuiki
# This file configures the build of the common class and function library.

add_library(common uuid.cpp)
target_link_libraries(common ${LIBUUID_LIBRARIES})