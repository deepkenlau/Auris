/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <sstream>
#include "../common/Mutex.h"
#include "database/Database.h"
#include "media/Store.h"
#include "library/Library.h"


namespace database
{
	class Connection;
	class Playlist;
	class ShufflePlaylist;

	class Server : public gc
	{
		friend class Connection;
		
	public:
		void run(int argc, char *argv[]);

		Server();

		library::Library *library;

		void reportQuality(std::string from, std::string to, double quality);
		void loadQualities();
		void storeQualities();

		struct Quality { double quality; int count; Quality() { quality = 0; count = 0; }};
		Quality getQuality(const std::string &from, const std::string &to) { qualities_mutex.lock(); Quality q = qualities[from][to]; qualities_mutex.unlock(); return q; };

	private:
		Mutex connections_mutex;
		typedef std::set<Connection*, std::less<Connection*>, gc_allocator<Connection*> > Connections;
		Connections connections;
		void addConnection(Connection* c);

		typedef std::set<Playlist*, std::less<Playlist*>, gc_allocator<Playlist*> > Playlists;
		Mutex playlists_mutex;
		int playlistID;
		Playlists shufflePlaylists;

		typedef std::map<std::string, Quality> Qualities;
		typedef std::map<std::string, Qualities> QualityMap;
		Mutex qualities_mutex;
		QualityMap qualities;

	protected:
		void removeConnection(Connection* c);

		ShufflePlaylist* getShufflePlaylist(unsigned int id = 0);
	};
}