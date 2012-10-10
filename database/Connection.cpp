/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <iostream>
#include "Connection.h"
#include "Server.h"
#include "../common/Socket.h"
#include "../common/Thread.h"

using namespace Database;
using std::endl;

#define clog std::cout << "[connection " << socket->getRemoteAddress() << "] "


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
	return NULL;
}

/** Starts the thread that handles this connection. */
void Connection::start()
{
	Thread::make(connectionThread, this);
}

void Connection::run()
{
	clog << "connected" << endl;

	socket->write("Hello\n", 6);
	sleep(3);
	socket->write("Bitch... ", 9);
	sleep(1);
	socket->write("please!\n", 8);

	//Since we're done, remove the connection from the server.
	socket->close();
	clog << "closed" << endl;
	server->removeConnection(this);
}