/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Entry.h"
#include "Field.h"
#include <sstream>
using namespace Database::Entry;


Entry::ID Entry::getID() const
{
	return id;
}

void Entry::setID(ID id)
{
	this->id = id;
}

std::string Entry::describe() const
{
	std::stringstream s;
	for(Fields::const_iterator it = fields.begin(); it != fields.end(); it++)
		s << it->first << ": " << it->second->describe() << std::endl;
	return s.str();
}