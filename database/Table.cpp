/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Table.h"
#include "../common/strutil.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace Database;
using std::runtime_error;
using std::string;
using std::stringstream;


void Table::addEntry(Entry::Entry *e)
{
	entries_lock.lock();
	entries.insert(e);
	entriesByID[e->getID()] = e;
	entries_lock.unlock();
}

void Table::removeEntry(Entry::Entry *e)
{
	entries_lock.lock();
	entries.erase(e);
	entriesByID.erase(e->getID());
	entries_lock.unlock();
}

/** Returns the entry with the given ID, or null if none is found. */
Entry::Entry* Table::getEntryByID(Entry::Entry::ID id)
{
	entries_lock.lock();
	EntriesByID::iterator e = entriesByID.find(id);
	entries_lock.unlock();
	if (e == entriesByID.end()) return NULL;
	return e->second;
}

void Table::encode(tinyxml2::XMLPrinter &xml) const
{
	xml.PushAttribute("someAttribute", "Hello!");
	for (Entries::const_iterator ie = entries.begin(); ie != entries.end(); ie++) {
		xml.OpenElement((*ie)->getType().c_str());
		(*ie)->encode(xml);
		xml.CloseElement();
	}
}

void Table::decode(tinyxml2::XMLElement &xml)
{
	//Iterate through the entries in the table.
	for (tinyxml2::XMLElement *e = xml.FirstChildElement(); e; e = e->NextSiblingElement()) {
		Entry::Entry *entry = Entry::Entry::make(e->Name());
		if (!entry) {
			throw new runtime_error(string("Unable to make database entry of type ") + e->Name());
		}
		entry->decode(*e);
		addEntry(entry);
	}
}

string Table::describe() const
{
	stringstream str;
	str << "[";
	for (Entries::const_iterator ie = entries.begin(); ie != entries.end(); ie++) {
		str << "\n\t" << strutil::indent((*ie)->describe());
	}
	str << "\n]";
	return str.str();
}