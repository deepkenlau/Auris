/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>

#include "Table.h"


namespace database
{
	namespace database
	{
		class Database : public gc
		{
		public:
			Database(const std::string &path);

			const std::string& getPath() const { return path; }

			void load();
			void commit();

			/*Table<Song> songs;
			Table<Album> albums;
			Table<Artist> artists;*/

			std::string persistObject(const std::string &object) const;

		protected:
			std::string path;
		};
	}
}