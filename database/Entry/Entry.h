/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include <map>
#include <gc_cpp.h>
#include <gc_allocator.h>
#include "../../common/tinyxml2.h"


namespace Database
{
	namespace Entry
	{
		class Field;
		class Entry : public gc
		{
		public:
			typedef std::string ID;

			ID getID() const;
			void setID(ID id);
			
			std::string describe() const;
			void encode(tinyxml2::XMLPrinter &xml) const;
			void decode(tinyxml2::XMLElement &xml);

			static Entry* make(const std::string &type);
			virtual std::string getType() = 0;

		protected:
			typedef std::map<std::string, Field*, std::less<std::string>, gc_allocator<Field*> > Fields;

			ID id;
			Fields fields;
		};
	}
}

#define DATABASE_ENTRY_FIELD(name) fields[#name] = &name