/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include "../common/Socket.h"
#include "../common/Thread.h"

using namespace Database;
using std::endl;

#define clog std::cout << "[connection " << getClientName() << "] "


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
	const int CHARS_PER_PERIOD = 1024;
	char buffer[CHARS_PER_PERIOD];
	int chars_read;
	while(true)
	{
		if(socket->poll(10))
		{
			do
			{
				chars_read = socket->read(buffer, CHARS_PER_PERIOD);
				inputBuffer.sputn(buffer, chars_read);
			} while (chars_read == CHARS_PER_PERIOD);
			this->received();
		}
		while(true)
		{
			outputBuffer_lock.lock();
			chars_read = outputBuffer.sgetn(buffer, CHARS_PER_PERIOD);
			outputBuffer_lock.unlock();
			if (chars_read == 0) break;
			socket->write(buffer, chars_read);
		}
	}
	//Since we're done, remove the connection from the server.
	socket->close();
	clog << "closed" << endl;
	server->removeConnection(this);
}

/** Called whenever new data arrives in the input buffer. Called on the Connection's
 * thread, which makes it unnecessary to have a lock for the input buffer. */
void Connection::received()
{
	clog << inputBuffer.in_avail() << " bytes in the input buffer" << endl;
	server->debugChat(this, inputBuffer);
}

/** Stores the given data in the output buffer to be sent. Thread-safe. */
void Connection::write(const char *data, unsigned int length)
{
	outputBuffer_lock.lock();
	outputBuffer.sputn(data, length);
	outputBuffer_lock.unlock();
}

/** Returns a string to identify the connected client. This might be the client's
 * IP address, the client's name if logged in, or the like. */
const std::string& Connection::getClientName()
{
	return socket->getRemoteAddress();

}