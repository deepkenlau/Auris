/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <set>
#include <string>


#include "Entity.h"


namespace database
{
	namespace database
	{
		class Database;
		class Commit;
		class Table : public gc
		{
		public:
			Table(Commit* c);

			Commit* getCommit() const { return commit; }
			Database* getDatabase() const;

			void addEntity(Entity *e);
			void removeEntity(Entity *e);

			std::string persist() const;
			void load(const std::string &hash);

			std::string describe() const;

		protected:
			Commit* const commit;

			typedef std::set<Entity*, std::less<Entity*>, gc_allocator<Entity*> > Entities;
			Entities entities;
			virtual Entity* makeEntity() = 0;
		};

		template <typename T>
		class ConcreteTable : public Table
		{
		public:
			ConcreteTable(Commit* c) : Table(c) {}
			Entity* makeEntity() { return new T(this); }
		};
	}
}