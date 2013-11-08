/* Copyright © 2013 Fabian Schuiki */
#include <db/Database.hpp>
#include <iostream>

using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using auris::db::Database;

void print_usage(const char *argv0)
{
	cout << "usage: " << argv0 << " PATH COMMAND\n";
}

int main(int argc, char *argv[])
{
	try {
		if (argc != 3) {
			print_usage(*argv);
			return 1;
		}

		// Initialize the database and dump some information.
		Database db(argv[1]);
		
		Database::TrackList tl = db.getTrackList();

	} catch (std::exception &e) {
		cerr << e.what() << endl;
		return 2;
	}
	return 0;
}