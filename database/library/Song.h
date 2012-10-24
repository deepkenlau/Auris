/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include <gc_cpp.h>

namespace database
{
	namespace library
	{
		class Library;
		class Song : public gc
		{
		protected:
			Library * const library;
			std::string id;
		public:
			Library * getLibrary() const {return library;}
			std::string getId() const {return id;}
		};
	}
}