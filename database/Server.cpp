/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include "../common/Socket.h"
#include <iostream>
#include <stdexcept>
extern "C" {
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
}

void Server::run(int argc, char *argv[])
{
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