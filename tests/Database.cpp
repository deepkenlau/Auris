#include <cassert>
#include <iostream>
#include <gc_cpp.h>
#include <stdexcept>
#include <database/Database/Database.h>
#include <database/Database/Commit.h>
#include <database/Database/Song.h>
#include <common/Error.h>

using std::runtime_error;
using database::database::Database;
using database::database::Commit;
using database::database::Table;
using database::database::ConcreteTable;
using database::database::Entity;
using database::database::Song;


int main(int argc, char *argv[])
{
	GC_INIT();
	try {
		//Create a new database and try to load it.
		Database db(Path("debug_database"));
		db.load();

		//Fetch the songs table.
		ConcreteTable<Song> &songs = db.getSongs();

		std::cout << "Songs: " << songs.describe() << std::endl;

		//Create a new entity.
		Song *song = songs.newEntity();
		song->setID("abcdefg");
		song->title = "Fire Hive";
		song->artist = "Knife Party";
		song->album = "Rage Valley";
		db.commit();
	}
	catch (std::runtime_error &e) {
		std::cerr << "runtime error: " << e.what() << std::endl;
	}
	catch (Error *e) {
		std::cerr << "*** " << e->what() << std::endl;
	}

	return 0;
}