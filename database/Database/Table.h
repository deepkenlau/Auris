/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>

#include "Entity.h"


namespace database
{
	namespace database
	{
		class Database;
		class Table : public gc
		{
		public:
			Table(Database* db);

			Database* getDatabase() const { return database; }

		protected:
			Database* const database;
		};
	}
}