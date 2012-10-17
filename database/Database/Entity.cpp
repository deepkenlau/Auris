#include "Database.h"
#include <sstream>

using std::string;
using std::stringstream;

using database::database::Table;
using database::database::Entity;


Entity::Entity(Table* t) : table(t)
{
}

string Entity::persist() const
{
	stringstream output;
	for (Fields::const_iterator it = fields.begin(); it != fields.end(); it++) {
		output << it->first << ": " << it->second << "\n";
	}
	return table->getDatabase()->persistObject(output.str());
}