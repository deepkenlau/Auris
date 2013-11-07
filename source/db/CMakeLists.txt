# Copyright © 2013 Fabian Schuiki
# This file configures the build of the database library. It contains all the
# code associated with creating, maintaining and using a database.

add_library(db Database.cpp)

add_executable(dbtool tool.cpp)
target_link_libraries(dbtool db ${Boost_LIBRARIES})