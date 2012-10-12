/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Entry.h"
using namespace Database;


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
	for(Fields::iterator it = fields.begin(); it != fields.end(); it++)
		s << it->first << ": " << it->second->describe() << endl;
	return s.str();
}