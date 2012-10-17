#include <cassert>
#include <iostream>
#include <gc_cpp.h>
#include <stdexcept>
#include "../common/tinyxml2.h"
#include "../database/Database/Database.h"

using std::runtime_error;
using database::database::Database;
using database::database::Table;
using database::database::Entity;


int main(int argc, char *argv[])
{
	GC_INIT();
	try {
		//Create a new database.
		Database db("debug_database");

		//Create a new table.
		Table table(&db);

		//Create a new entity.
		Entity entity(&table);
		entity.fields["title"]  = "Fire Hive";
		entity.fields["artist"] = "Knife Party";
		std::string sha = entity.persist();
		std::cout << "stored entity " << sha << std::endl;
	}
	catch (std::runtime_error &e) {
		std::cerr << "runtime error: " << e.what() << std::endl;
	}

	return 0;
}