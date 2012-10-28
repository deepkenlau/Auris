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

			virtual Entity* newEntity() = 0;

			typedef std::set<Entity*, std::less<Entity*>, gc_allocator<Entity*> > Entities;
			const Entities& getEntities() const;
			Entity* getEntity(std::string id) const;

			std::string persist() const;
			void load(const std::string &hash);

			std::string describe() const;

		protected:
			Commit* const commit;

			virtual Entity* makeEntity() = 0;

			void addEntity(Entity *e);
			void removeEntity(Entity *e);

			Entities entities;
		};

		template <typename T>
		class ConcreteTable : public Table
		{
		public:
			ConcreteTable(Commit* c) : Table(c) {}
			T* newEntity() { T* e = new T(this); addEntity(e); return e; }

		protected:
			Entity* makeEntity() { return new T(this); }
		};
	}
}