/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Table.h"
#include "Database.h"
#include "Commit.h"
#include <sstream>
#include <common/strutil.h>

using database::database::Database;
using database::database::Table;
using std::string;
using std::stringstream;


Table::Table(Commit* c) : commit(c)
{
}

Database* Table::getDatabase() const
{
	return commit->getDatabase();
}

void Table::addEntity(Entity *e)
{
	entities.insert(e);
}

void Table::removeEntity(Entity *e)
{
	entities.erase(e);
}

/** Persists all of the table's entities as well as the table itself. Returns
 * the name of the created object. */
string Table::persist() const
{
	//Iterate through the entities and persist each.
	stringstream str;
	for (Entities::const_iterator ie = entities.begin(); ie != entities.end(); ie++) {
		str << (*ie)->persist() << "\n";
	}

	//Persist the table itself.
	return getDatabase()->persistObject(str.str());
}

void Table::load(const string &hash)
{
	string data = getDatabase()->loadObject(hash);

	//Clear our entities.
	entities.clear();

	//Parse the data line by line.
	size_t lineStart = 0, lineEnd = 0;
	do {
		lineStart = lineEnd+1;
		lineEnd = data.find('\n', lineStart);
		if (lineEnd == string::npos)
			lineEnd = data.length();

		if (lineEnd - lineStart > 0) {
			Entity* e = makeEntity();
			e->load(data.substr(lineStart, lineEnd-lineStart));
			addEntity(e);
		}
	} while (lineEnd < data.length());
}

string Table::describe() const
{
	stringstream str;
	str << "[";
	for (Entities::const_iterator ie = entities.begin(); ie != entities.end(); ie++) {
		str << "\n\t" << strutil::indent((*ie)->describe());
	}
	str << "\n]";
	return str.str();
}