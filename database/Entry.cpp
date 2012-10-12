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