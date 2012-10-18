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
	entities_lock.lock();
	entities.insert(e);
	entities_lock.unlock();
}

void Table::removeEntity(Entity *e)
{
	entities_lock.lock();
	entities.erase(e);
	entities_lock.unlock();
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