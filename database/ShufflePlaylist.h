/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Playlist.h"
#include <vector>
#include <common/Mutex.h>
#include <common/coding/Encoder.h>
#include "library/Song.h"


namespace database
{
	class ShufflePlaylist : public Playlist
	{
	public:
		typedef std::vector<library::Song*> Songs;

		ShufflePlaylist(Server *server, unsigned int id);
		void generate(coding::Encoder *encoder);

	protected:
		Mutex lock;
		Songs songs;
		int offset;

		void extend();
	};
}