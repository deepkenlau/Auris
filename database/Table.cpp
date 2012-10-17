/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Table.h"
using namespace Database;


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
		xml.OpenElement("entry");
		(*ie)->encode(xml);
		xml.CloseElement();
	}
}

void Table::decode(tinyxml2::XMLElement &xml)
{
	//Iterate through the entries in the table.
	for (tinyxml2::XMLElement *e = xml->FirstChildElement("entry"); e; e = e->NextSiblingElement("entry")) {
		
	}
}