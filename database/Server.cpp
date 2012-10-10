/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include "../common/Socket.h"
#include <iostream>

using namespace Database;
using std::cerr;
using std::endl;


void Server::run(int argc, char *argv[])
{
	//Setup the listening socket for the control connections.
	Socket* listener = Socket::makeListening(8080);
	if (!listener) {
		cerr << "unable to create listening socket" << endl;
		return;
	}

	//Enter the main loop that waits for new connections.
	while (Socket* newSocket = listener->accept()) {
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