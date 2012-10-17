/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include <common/Path.h>

#include "Table.h"


namespace database
{
	namespace database
	{
		class Database : public gc
		{
		public:
			Database(const Path &path);

			const Path& getPath() const { return path; }

			void load();
			void commit();

			/*Table<Song> songs;
			Table<Album> albums;
			Table<Artist> artists;*/

			std::string persistObject(const std::string &object) const;

		protected:
			Path path;

			Path getObjectsDirectory() const;
			Path getObjectPath(const std::string &checksum) const;
		};
	}
}