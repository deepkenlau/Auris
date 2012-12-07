/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>
#include <string>
#include <common/Blob.h>
#include <common/Path.h>
#include <common/Mutex.h>
#include <gc_cpp.h>
#include <gc_allocator.h>
#include "Song.h"
#include "../database/Database.h"
#include "../media/Store.h"


namespace database
{
	namespace library
	{
		class Library : public gc
		{
		public:
			typedef std::set<Song*, std::less<Song*>, gc_allocator<Song*> > Songs;

			Library(Path p);
			void load();

			database::Database& getDatabase() { return database; }
			media::Store& getStore() { return store; }

			Song* addMedia(const Blob &blob);

			Song* getSong(std::string id);
			Songs getSongs();

			const Path& getPath() const { return path; }

		private:
			database::Database database;
			media::Store store;
			const Path path;

			Songs songs;
			Mutex songs_lock;
		};
	}
}