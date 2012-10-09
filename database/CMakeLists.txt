set(DATABASE_EXEC_NAME auris-db)
add_executable(${DATABASE_EXEC_NAME}
	main.cpp
	Server.cpp
	../common/tinyxml2.cpp
)