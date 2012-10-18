#include <cassert>
#include <iostream>
#include <gc_cpp.h>
#include <stdexcept>
#include <database/Database/Database.h>
#include <database/Database/Commit.h>
#include <database/Database/Song.h>

using std::runtime_error;
using database::database::Database;
using database::database::Commit;
using database::database::Table;
using database::database::Entity;
using database::database::Song;


int main(int argc, char *argv[])
{
	GC_INIT();
	try {
		//Create a new database.
		Database db("debug_database");

		//Create a new commit.
		Commit commit(&db);

		//Create a new entity.
		Song song(&commit.songs);
		song.setID("abcdefg");
		song.title = "Fire Hive";
		song.artist = "Knife Party";
		commit.songs.addEntity(&song);

		std::cout << "initial commit: " << commit.persist() << std::endl;

		song.album = "Rage Valley";
		std::cout << "first change: " << commit.persist() << std::endl;
	}
	catch (std::runtime_error &e) {
		std::cerr << "runtime error: " << e.what() << std::endl;
	}

	return 0;
}