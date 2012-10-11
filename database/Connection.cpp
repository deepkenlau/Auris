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
/*
	socket->write("Hello\n", 6);
	sleep(3);
	socket->write("Bitch... ", 9);
	sleep(1);
	socket->write("please!\n", 8);
*/
	const CHARS_PER_PERIOD = 1024;
	char buffer[CHARS_PER_PERIOD];
	int chars_read;
	while(true)
	{
		if(socket->poll(10))
		{
			do
			{
				chars_read = read(buffer, CHARS_PER_PERIOD);
				inputBuffer.sputn(buffer, CHARS_PER_PERIOD);
			} while chars_read == CHARS_PER_PERIOD;
			this->received();
		}
		if(outputBuffer.in_avail() > 0)
		{
			do
			{
				chars_read == outputBuffer.sgetn(buffer,CHARS_PER_PERIOD);
				socket->write(buffer, CHARS_PER_PERIOD);
			} while chars_read == CHARS_PER_PERIOD;
		}
	}
	//Since we're done, remove the connection from the server.
	socket->close();
	clog << "closed" << endl;
	server->removeConnection(this);
}