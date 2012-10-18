#include "Database.h"
#include "Field.h"
#include <sstream>
#include <iostream>

using std::string;
using std::stringstream;

using database::database::Database;
using database::database::Table;
using database::database::Entity;
using database::database::Field;


Entity::Entity(Table* t) : table(t)
{
}

Database* Entity::getDatabase() const
{
	return table->getDatabase();
}

void Entity::setID(const std::string &id)
{
	this->id = id;
}

std::string Entity::getID() const
{
	return id;
}

/** Stores the entity in the database and returns the name of the resulting object. */
string Entity::persist() const
{
	stringstream output;
	output << id << "\n";
	for (Fields::const_iterator it = fields.begin(); it != fields.end(); it++) {
		if (it->second->isEmpty()) continue;
		output << it->first << ": ";
		it->second->encode(output);
		output << "\n";
	}
	return getDatabase()->persistObject(output.str());
}

void Entity::load(const string &hash)
{
	std::cout << "should load entity " << hash << std::endl;
}

/** Returns a human readable string describing the entity. */
string Entity::describe() const
{
	stringstream s;
	s << "{";
	for(Fields::const_iterator it = fields.begin(); it != fields.end(); it++)
		s << "\n\t" << it->first << ": " << it->second->describe();
	s << "\n}";
	return s.str();
}