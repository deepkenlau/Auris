/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <map>
#include <set>
#include "Entry/Entry.h"
#include "../common/Mutex.h"

namespace Database
{
	class Table : public gc
	{
	public:
		typedef std::set<Entry::Entry*, std::less<Entry::Entry*>, gc_allocator<Entry::Entry*> > Entries;

		void addEntry(Entry::Entry *e);
		void removeEntry(Entry::Entry *e);

		Entry::Entry* getEntryByID(Entry::Entry::ID id);

	private:
		typedef std::map<Entry::Entry::ID, Entry::Entry*> EntriesByID;

		Mutex entries_lock;
		Entries entries;
		EntriesByID entriesByID;
	};
}