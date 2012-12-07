/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Playlist.h"
#include <map>
#include <common/Mutex.h>
#include <common/coding/Encoder.h>
#include "library/Song.h"


namespace database
{
	class ShufflePlaylist : public Playlist
	{
	public:
		typedef std::map<int, std::string> Songs;

		ShufflePlaylist(Server *server, unsigned int id);
		void encode(coding::Encoder *encoder, Range range = kNilRange);
		void prepareRange(Range range);

		Songs getSongs() { lock.lock(); Songs s = songs; lock.unlock(); return s; }

	protected:
		Mutex lock;
		Songs songs;

		int min_id;
		int max_id;
	};
}