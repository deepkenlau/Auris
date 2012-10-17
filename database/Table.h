/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <map>
#include <set>
#include "../common/tinyxml2.h"
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

		void encode(tinyxml2::XMLPrinter &xml) const;
		void decode(tinyxml2::XMLElement &xml);

		std::string describe() const;

	private:
		typedef std::map<Entry::Entry::ID, Entry::Entry*> EntriesByID;

		Mutex entries_lock;
		Entries entries;
		EntriesByID entriesByID;
	};
}