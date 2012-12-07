/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include "../common/Socket.h"
#include "ShufflePlaylist.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
extern "C" {
	#define __STDC_CONSTANT_MACROS
	#include <libavformat/avformat.h>
}

using database::Server;
using std::cerr;
using std::endl;
using std::runtime_error;
using std::ofstream;
using std::ifstream;


Server::Server()
{
	Path p("~/Music/Auris");
	library = new library::Library(p);
	playlistID = 1;
}

void Server::run(int argc, char *argv[])
{
	avcodec_register_all();
	av_register_all();

	//Load the database.
	library->load();
	loadQualities();

	//Setup the listening socket for the control connections.
	Socket* listener = Socket::makeListening(8080);
	if (!listener)
		throw runtime_error("unable to create listening socket");

	//Enter the main loop that waits for new connections.
	while (true) {
		Socket* newSocket = listener->accept();
		Connection* c = new Connection(newSocket, this);
		addConnection(c);
		c->start();
	}
}

/** Adds a connection to the server's list of connections. Thread-safe. */
void Server::addConnection(Connection* c)
{
	connections_mutex.lock();
	connections.insert(c);
	connections_mutex.unlock();
}

/** Removes a connection to the server's list of connections. Thread-safe. */
void Server::removeConnection(Connection* c)
{
	connections_mutex.lock();
	connections.erase(c);
	connections_mutex.unlock();
}

/** Returns the shuffle playlist with the given ID, or a new one if 0 is passed. */
database::ShufflePlaylist* Server::getShufflePlaylist(unsigned int id)
{
	ShufflePlaylist *p = NULL;

	//Try to find the playlist with the given ID.
	if (id) {
		for (Playlists::iterator ip = shufflePlaylists.begin(); ip != shufflePlaylists.end(); ip++) {
			if ((*ip)->getID() == id) {
				p = (ShufflePlaylist*)*ip;
				break;
			}
		}
	}

	//If no playlist was found, create a new one.
	if (!p) {
		playlists_mutex.lock();
		p = new ShufflePlaylist(this, playlistID++);
		shufflePlaylists.insert(p);
		playlists_mutex.unlock();
	}

	return p;
}

void Server::reportQuality(std::string from, std::string to, double quality)
{
	qualities_mutex.lock();
	qualities[from][to].quality += quality;
	qualities[from][to].count++;
	qualities_mutex.unlock();
	storeQualities();
}

void Server::loadQualities()
{
	std::cout << "loading qualities" << std::endl;
	qualities_mutex.lock();
	ifstream f(library->getPath().down("qualities"));
	while (!f.eof()) {
		std::string from, to;
		double quality;
		int count;
		f >> from >> to >> quality >> count;
		if (from.empty() || to.empty()) break;
		qualities[from][to].quality = quality;
		qualities[from][to].count = count;
	}
	f.close();
	qualities_mutex.unlock();
}

void Server::storeQualities()
{
	qualities_mutex.lock();
	ofstream f(library->getPath().down("qualities"));
	for (QualityMap::iterator ia = qualities.begin(); ia != qualities.end(); ia++) {
		for (Qualities::iterator ib = ia->second.begin(); ib != ia->second.end(); ib++) {
			f << ia->first << " " << ib->first << " " << ib->second.quality << " " << ib->second.count << "\n";
		}
	}
	f.close();
	qualities_mutex.unlock();
}