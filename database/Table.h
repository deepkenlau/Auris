/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <map>
#include <set>
#include "Entry.h"
#include "../common/Mutex.h"

namespace Database
{
	class Table : public gc
	{
	public:
		typedef std::set<Entry*, std::less<Entry*>, gc_allocator<Entry*> > Entries;

		void addEntry(Entry *e);
		void removeEntry(Entry *e);

		Entry* getEntryByID(Entry::ID id);

	private:
		typedef std::map<Entry::ID, Entry*> EntriesByID;

		Mutex entries_lock;
		Entries entries;
		EntriesByID entriesByID;
	};
}