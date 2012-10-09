/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Server.h"
#include <iostream>

using namespace Database;
using std::cerr;
using std::endl;


void Server::run(int argc, char *argv[])
{
	//Setup the listening socket for the control connections.
	Socket* listener = Socket::makeListening(port);
	if (!listener) {
		cerr << "unable to create listening socket" << endl;
		return;
	}

	//Enter the main loop that waits for new connections.
	while (Socket* newSocket = listener->accept()) {
		Connection* c = Connection::make(newSocket, this);
		addConnection(c);
	}

	//Clean up.
	delete listener;
}

/** Adds a connection to the server's list of connections. Thread-safe.
 * By calling this function the ownership of the connection moves to the server,
 * which will cause the connection to be deleted when it is removed from the
 * list. */
void Server::addConnection(Connection* c)
{
	connections_mutex.lock();
	connections.insert(c);
	connections_mutex.unlock();
}

/** Removes a connection to the server's list of connections. Thread-safe.
 * Also deletes the connection, do your clean up before calling this function. */
void Server::removeConnection(Connection* c)
{
	connections_mutex.lock();
	connections.erase(c);
	connections_mutex.unlock();
}