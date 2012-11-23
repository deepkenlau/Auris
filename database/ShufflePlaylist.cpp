/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "ShufflePlaylist.h"
#include "library/Library.h"
#include "Server.h"

using database::ShufflePlaylist;


ShufflePlaylist::ShufflePlaylist(Server *server, unsigned int id) : Playlist(server, id)
{

}

void ShufflePlaylist::generate(coding::Encoder *encoder)
{
	lock.lock();
	for (int i = offset; i < songs.size(); i++) {
		encoder->add(songs[i]->getID());
	}
	offset = songs.size();
	lock.unlock();
}

void ShufflePlaylist::extend()
{
	lock.lock();
	library::Library::Songs songs = server->library->getSongs();
	int max = songs.size();
	int i = rand() % max;
	
	lock.unlock();
}