/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include <map>
#include <gc_cpp.h>
#include <gc_allocator.h>


namespace Database
{
	class Field;
	class Entry : public gc
	{
	public:
		typedef std::string ID;

		ID getID() const;
		void setID(ID id);

	private:
		typedef std::map<std::string, Field*, gc_allocator<Field*> > Fields;

		ID id;
		Fields fields;
	};
}