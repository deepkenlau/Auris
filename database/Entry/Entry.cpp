/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Entry.h"
#include "Field.h"
#include "Song.h"
#include "Album.h"
#include <sstream>
#include <stdexcept>
using namespace Database::Entry;
using std::runtime_error;
using std::string;


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
		Field *field = fields[e->Name()];
		if (field == NULL) {
			throw new runtime_error((string("Trying to decode field ") + e->Name()) + "which does not exist.");
		}
		fields[*field]->decode(*e);
	}
}

Entry* Entry::make(const std::string &type)
{
	if (type == "song")  return new Song();
	if (type == "album") return new Album();
	return NULL;
}