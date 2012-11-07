/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Player.h"
#include "Session.h"
#include "../common/Socket.h"
#include "../common/Thread.h"
#include <common/HTTP/Request.h>
#include <common/HTTP/Response.h>
#include <common/Error.h>
#include <common/uuid.h>
#include <typeinfo>
#include <string>
#include <cstdlib>

using player::Connection;
using player::Session;
using std::string;
using std::endl;
using std::stringstream;

#define clog std::cout << "[connection " << getClientName() << "] "



/** Creates a new connection object that will handle communication on the given
 * socket for the given server object. */
Connection::Connection(Socket *socket, Player* player)
{
	this->socket = socket;
	this->player = player;
}

/** Main function of the connection thread. Simply calls the connection's run
 * function. */
static void* connectionThread(void *param)
{
	Connection *c = (Connection*)param;
	try {
		c->run();
	} catch (Error *e) {
		std::cerr << "*** uncaught connection exception: " << e->what() << std::endl;
	} catch (std::exception &e) {
		std::cerr << "*** uncaught connection exception " << typeid(e).name() << ": " << e.what() << std::endl;
	}
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
	try {
		//Enter the main loop which runs as long as the socket is connected.
		const int BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE];
		while (socket->isOpen()) {
			int num_read;

			//Wait for incoming data for 10ms.
			int bytesRead = 0;
			while (socket->poll(10) && socket->isOpen()) {
				num_read = socket->read(buffer, BUFFER_SIZE);
				inputBuffer.append(buffer, num_read);
				bytesRead += num_read;
			}

			//Call the reception handler if we've read any data.
			if (bytesRead) {
				this->received();
			}


			//Write data to the socket as long as there's data left in the output buffer.
			int write_offset = 0;
			do {
				outputBuffer_lock.lock();
				num_read = std::min<int>(BUFFER_SIZE, outputBuffer.length() - write_offset);
				outputBuffer.copy(buffer, num_read, write_offset);
				outputBuffer_lock.unlock();

				if (num_read > 0) {
					socket->write(buffer, num_read);
					write_offset += num_read;
				}
			} while (num_read > 0);

			if (write_offset > 0) {
				outputBuffer_lock.lock();
				outputBuffer.assign(outputBuffer, write_offset, outputBuffer.length() - write_offset);
				int length = outputBuffer.length();
				outputBuffer_lock.unlock();
				if (length == 0 && closeAfterWrite) {
					socket->close();
				}
			}
		}
	} catch (Error *e) {
		clog << "*** " << e->what() << endl;
		socket->write("500 Internal Server Error\r\n", 27);
	}

	//We're done, close the connection and remove it from the server.
	socket->close();
	clog << "closed" << endl;
	player->removeConnection(this);
}

/** Called whenever new data arrives in the input buffer. Called on the Connection's
 * thread, which makes it unnecessary to have a lock for the input buffer. */
void Connection::received()
{
	//Try to parse the HTTP request that was received.
	unsigned int consumed = 0;
	HTTP::Request *request = HTTP::Request::fromString(inputBuffer, &consumed);
	if (consumed > 0) {
		inputBuffer.assign(inputBuffer, consumed, inputBuffer.length() - consumed);
	}
	if (!request) return;
	clog << "processing request" << endl;

	clog << request->path << endl;

	if (request->path.at(0) != '/')
		throw new GenericError("Invalid Command.");
	int i,j;
	for (i = 1; i < request->path.length()
		&& request->path.at(i) != '/'; i++){}

	std::string sid = request->path.substr(1,i-1);
	std::cout << "pathString = " << request->path << endl; 	
	std::cout << "sidString = " << sid << endl;

	for (j = ++i; j < request->path.length()
		&& request->path.at(j) != '/'; j++){}

	std::string command;
	std::string host;

	if (j == request->path.length())
		command = request->path.substr(i);
	else
	{
		command = request->path.substr(i, j-i);
		host = request->path.substr(j+1);
	}

	Session * session;
	int  sidInt;
	if (sid == "new")
	{
		session = player->makeSession();
		sidInt = session->getId();
		session->start();

	} else if (sid.length() >= 4 && sid.substr(0, 3) == "sid")
	{
		Session * s = player->getSession(sidInt = atoi(sid.substr(4).c_str()));
		if (s == NULL)
			throw new GenericError("Session not found.");
	} else
	{
		throw new GenericError("Invalid command. '" + command + "'");
	}

	if (command == "play")
	{
		session->play(host);
	} else if (command == "stop")
	{
		player->getSession(atoi(sid.substr(3).c_str()))->stop();

	} else if (command == "pause")
	{
		player->getSession(atoi(sid.substr(3).c_str()))->pause();
	} else if (command == "resume")
	{
		player->getSession(atoi(sid.substr(3).c_str()))->resume();
	} else
		throw new GenericError("Invalid command.");


	HTTP::Response r;
	std::stringstream strstream;
	strstream << sidInt;
	r.content = strstream.str();
	r.finalize();
	write(r);
	close();
	/*if(request->path.at(0) != '/')
	{
		clog << "invalid request" << endl;
		HTTP::Response r;
		r.statusCode = 404;
		r.statusText = string("Requested object ") + request->path + " not found.";
		r.finalize();
		write(r);
		close();
		return;
	}
	int i,j;
	for(i = 1; i < request->path.length() &&
		request->path.at(i) != '/'; i++)
	{}
	if (i == request->path.length())
	{
		clog << "invalid request" << endl;
		HTTP::Response r;
		r.statusCode = 400;
		r.statusText = string("Requested object ") + request->path + " not found.";
		r.finalize();
		write(r);
		close();
		return;		
	}
	std::string command = request->path.substr(0,i);
	clog << command << endl;
	if (command == "/play")
	{
		clog << "Creating new session.\n";

		//create a new session
		Session * session = player->makeSession();
		clog << "Created new session.\n";
		session->play(request->path, this);
	}
	else if (command == "/stop")
	{

	}
	else {
		clog << "unable to serve requested object " << request->path << endl;
		HTTP::Response r;
		r.statusCode = 404;
		r.statusText = string("Requested object ") + request->path + " not found.";
		r.finalize();
		write(r);
		close();
	}*/
}

/** Stores the given data in the output buffer to be sent. Thread-safe. */
void Connection::write(const char *data, unsigned int length)
{
	outputBuffer_lock.lock();
	outputBuffer.append(data, length);
	outputBuffer_lock.unlock();
}

void Connection::write(HTTP::Response &r)
{
	write(r.toString());
}
void Connection::write(HTTP::Request &r)
{
	write(r.toString());
}
void Connection::write(const std::string &s)
{
	write(s.c_str(), s.length());
}

/** Marks the connection as to be closed as soon as the last byte of the output
 * buffer is sent. */
void Connection::close()
{
	closeAfterWrite = true;
}

/** Returns a string to identify the connected client. This might be the client's
 * IP address, the client's name if logged in, or the like. */
const std::string& Connection::getClientName()
{
	return socket->getRemoteAddress();

}