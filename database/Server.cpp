/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include "../common/Socket.h"
#include "ShufflePlaylist.h"
#include <iostream>
#include <stdexcept>
extern "C" {
	#define __STDC_CONSTANT_MACROS
	#include <libavformat/avformat.h>
}

using database::Server;
using std::cerr;
using std::endl;
using std::runtime_error;


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