/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <map>
#include <string>
#include <common/Mutex.h>
#include <common/coding/Encoder.h>
#include <common/coding/Decoder.h>


namespace database
{
	namespace database
	{
		class Database;
		class Table;
		class Field;
		class Entity : public gc
		{
		public:
			Entity(Table* t);

			Table* getTable() const { return table; }
			Database* getDatabase() const;

			void setID(const std::string &id);
			std::string getID() const;

			virtual std::string persist() const;
			void load(const std::string &hash);

			std::string describe() const;

			void encode(coding::Encoder *encoder) const;
			void decode(coding::Decoder::Object *object);

		protected:
			Table* const table;

			typedef std::map<std::string, Field*, std::less<std::string>, gc_allocator<Field*> > Fields;

			Mutex lock;
			std::string id;
			Fields fields;
		};
	}
}