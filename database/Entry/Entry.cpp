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

void Entry::encode(tinyxml2::XMLPrinter &xml) const
{
	xml.PushAttribute("id", id.c_str());
	for(Fields::const_iterator i = fields.begin(); i != fields.end(); i++)
	{
		if(i->second->isEmpty()) continue;
		xml.OpenElement(i->first.c_str());
		i->second->encode(xml);
		xml.CloseElement();
	}
}

void Entry::decode(tinyxml2::XMLElement &xml)
{
	for(tinyxml2::XMLElement *e = xml.FirstChildElement(); e; e = e->NextSiblingElement())
	{
		Field *field = fields[e->GetName()];
		If(field == NULL) throw new std::runtime_error("Field does not exist.");
		fields[*field]->decode(*e);
	}
}

Entry* Entry::make(const std::string &name)
{
	if (name == "song")  return new Song();
	if (name == "album") return new Album();
	return NULL;
}