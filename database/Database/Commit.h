/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>

#include "Table.h"
#include "Song.h"


namespace database
{
	namespace database
	{
		class Database;
		class Commit : public gc
		{
		public:
			Commit(Database* db, const std::string &hash = "");

			Database* getDatabase() const { return database; }
			const std::string& getHash() const { return hash; }
			Commit* getBase() { return base; }

			std::string persist();
			void load(const std::string &data);

			ConcreteTable<Song> songs;

		protected:
			Database* const database;
			std::string hash;
			Commit* base;
		};
	}
}