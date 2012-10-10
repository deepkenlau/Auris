/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <iostream>
#include "Connection.h"
#include "Server.h"
#include "../common/Thread.h"
using namespace Database;

#define clog cout << "[connection " << socket->getRemoteAddress() << "] "


/** Creates a new connection object that will handle communication on the given
 * socket for the given server object. */
Connection::Connection(Socket *socket, Server *server)
{
	this->socket = socket;
	this->server = server;
}

/** Main function of the connection thread. Simply calls the connection's run
 * function. */
static void* connectionThread(void *param)
{
	Connection *c = (Connection*)param;
	c->run();
}

/** Starts the thread that handles this connection. */
void Connection::start()
{
	Thread::make(connectionThread, this);
}

void Connection::run()
{
	clog << "connected" << endl;

	//Since we're done, remove the connection from the server.
	clog << "closed" << endl;
	server->removeConnection(this);
}