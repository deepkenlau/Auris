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

void Entry::serialize(tinyxml2::XMLPrinter &xml) const
{
	xml.PushAttribute("id", id.c_str());
	for(Fields::const_iterator i = fields.begin(); i != fields.end(); i++)
	{
		if(i->second->isEmpty()) continue;
		xml.OpenElement(i->first.c_str());
		i->second->serialize(xml);
		xml.CloseElement();
	}
}