/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>
#include <string>
#include <gc_cpp.h>

namespace database
{
	namespace library
	{
		class Library;
		class Song : public gc
		{
		public:
			Library * getLibrary() const { return library; }
			std::string getID() const { return id; }

			Song(Library *library, std::string id);

			database::database::Song* getMetadata() const;
			std::set<std::string> getFormats() const;

		private:
			Library * const library;
			std::string id;
		};
	}
}