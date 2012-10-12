/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Table.h"
using namespace Database;


void Table::addEntry(Entry *e)
{
	entries_lock.lock();
	entries.insert(e);
	entriesByID[e->getID()] = e;
	entries_lock.unlock();
}

void Table::removeEntry(Entry *e)
{
	entries_lock.lock();
	entries.erase(e);
	entriesByID.erase(e->getID());
	entries_lock.unlock();
}

/** Returns the entry with the given ID, or null if none is found. */
Entry* Table::getEntryByID(Entry::ID id)
{
	entries_lock.lock();
	EntriesByID::iterator e = entriesByID.find(id);
	entries_lock.unlock();
	if (e == entriesByID.end()) return NULL;
	return e->second;
}