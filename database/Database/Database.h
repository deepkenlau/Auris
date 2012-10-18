/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include <common/Path.h>

#include "Commit.h"


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

			std::string persistObject(const std::string &object) const;

			ConcreteTable<Song>& getSongs() { return head.songs; }

		protected:
			Path path;
			Commit head;

			Path getObjectsDirectory() const;
			Path getObjectPath(const std::string &checksum) const;

			Path getMetadataHeadPath() const;
		};
	}
}