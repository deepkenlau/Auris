/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>
#include <string>
#include <common/Blob.h>
#include <gc_cpp.h>
#include <gc_allocator.h>


namespace database
{
	namespace library
	{
		class Library : public gc
		{
			database::Database database;
			database::media::Store store;
			typedef std::set<Song *, std::less<Song*>, gc_allocator<Song *> > Songs;
			Songs songs;
		public:
			Song * getSong(std::string id);
			Song * addMedia(Blob blob);
			void loadSongs();
		};
	}
}