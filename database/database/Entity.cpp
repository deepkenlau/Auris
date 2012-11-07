#include "Database.h"
#include "Field.h"
#include "Error.h"
#include <sstream>
#include <iostream>
#include <common/strutil.h>

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
	std::string data = getDatabase()->loadObject(hash);

	//Parse the entity id.
	size_t nl = data.find('\n');
	if (nl == string::npos) {
		throw new Error("Entity has no ID: " + data);
	}
	id = data.substr(0, nl);

	//Parse the fields.
	strutil::Dictionary dict = strutil::parseDictionary(data.substr(nl+1));
	for (Fields::const_iterator it = fields.begin(); it != fields.end(); it++) {
		if (dict.count(it->first)) {
			*it->second = dict[it->first];
		} else {
			it->second->clear();
		}
	}
}

/** Returns a human readable string describing the entity. */
string Entity::describe() const
{
	stringstream s;
	s << id << " {";
	for (Fields::const_iterator it = fields.begin(); it != fields.end(); it++)
		s << "\n\t" << it->first << ": " << it->second->describe();
	s << "\n}";
	return s.str();
}

void Entity::encode(coding::Encoder *encoder) const
{
	for (Fields::const_iterator it = fields.begin(); it != fields.end(); it++)
		encoder->add(it->second->describe(), it->first);
}

void Entity::decode(coding::Decoder *decoder)
{

}