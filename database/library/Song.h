/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>
#include <string>
#include <gc_cpp.h>
#include <common/Blob.h>


namespace database
{
	namespace database
	{
		class Song;
	}
	
	namespace library
	{
		class Library;
		class Song : public gc
		{
		public:
			Library * getLibrary() const { return library; }
			std::string getID() const { return id; }

			Song(Library *library, std::string id);

			database::Song* getMetadata() const;
			std::set<std::string> getFormats() const;

			Blob loadMainFormat() const;

		private:
			Library * const library;
			std::string id;
		};
	}
}