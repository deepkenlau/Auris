/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <map>
#include <string>


namespace database
{
	namespace database
	{
		class Table;
		class Entity : public gc
		{
		public:
			Entity(Table* t);

			Table* getTable() const { return table; }

			typedef std::map<std::string, std::string> Fields;
			Fields fields;

			std::string persist() const;

		protected:
			Table* const table;
		};
	}
}