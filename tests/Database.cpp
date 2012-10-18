#include <cassert>
#include <iostream>
#include <gc_cpp.h>
#include "../common/tinyxml2.h"
#include "../database/Table.h"
#include "../database/Entry/Song.h"
#include "../database/Entry/Album.h"
#include "../database/Database.h"

#define assert_equal(a,b) if (a != b) { std::cerr << #a << ": expected " << b << ", got " << a << std::endl; return 1; }

int main(int argc, char *argv[])
{
	GC_INIT();

	Database::Table table;
	Database::Entry::Song *e = new Database::Entry::Song;
	e->setID("3F2F1089-3E6C-4030-A8A7-FCD27294CD54");
	e->title = "Fire Hive";
	e->artist = "Knife Party";
	table.addEntry(e);
	//std::cout << "created table " << table.describe() << std::endl;
	/*(*e)["title"]  = "Fire Hive";
	(*e)["artist"] = "Knive Party";
	(*e)["rating"] = 0.98;
	(*e)["count"]  = 687;*/

	//Serialize the table to disk.
	FILE * f = fopen("table.xml", "w");
	tinyxml2::XMLPrinter xml(f);
	xml.OpenElement("table");
	table.encode(xml);
	xml.CloseElement();
	fclose(f);

	//Read the table from disk.
	tinyxml2::XMLDocument xmlr;
	assert_equal(xmlr.LoadFile("table.xml"), 0);
	Database::Table rt;
	tinyxml2::XMLElement *root = xmlr.RootElement();
	if (!root) {
		std::cerr << "no root element!" << std::endl;
		return 1;
	}
	rt.decode(*root);
	//std::cout << "read table " << rt.describe() << std::endl;

	Database::Database db("metadata.xml");

	Database::Entry::Song *s = new Database::Entry::Song;
	s->setID("a");
	s->title = "Internet Friends";
	s->artist = "Knife Party";
	db.songTable.addEntry(s);

	Database::Entry::Album *a = new Database::Entry::Album;
	a->setID("b");
	a->title = "Rage Valley";
	a->artist = "Knife Party";
	db.albumTable.addEntry(a);

	db.store();

	return 0;
}