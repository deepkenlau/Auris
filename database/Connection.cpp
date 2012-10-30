/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include "../common/Socket.h"
#include "../common/Thread.h"
#include <common/HTTP/Request.h>
#include <common/HTTP/Response.h>
#include <common/Error.h>
#include <common/uuid.h>

using database::Connection;
using std::string;
using std::endl;
using std::stringstream;

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
	server->removeConnection(this);
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

	//Disassemble the request path to find out what is being requested and in what format.
	size_t lastSlash = request->path.find_last_of('/');
	size_t lastDot   = request->path.find_last_of('.');
	string path, suffix;
	if (lastDot != string::npos && (lastSlash == string::npos || lastSlash < lastDot)) {
		path   = request->path.substr(0, lastDot);
		suffix = request->path.substr(lastDot+1);
	} else {
		path = request->path;
		suffix = "";
	}
	clog << "requested " << path << " (as " << suffix << ")" << endl;

	//Process the request.
	if (path == "/songs") {
		stringstream s;
		const library::Library::Songs &e = server->library->getSongs();
		for (library::Library::Songs::const_iterator it = e.begin(); it != e.end(); it++) {
			s << (*it)->getID() << " " << (*it)->getMetadata()->describe() << "\n";
		}

		HTTP::Response r;
		r.content = s.str();
		r.finalize();
		write(r);
		close();
	}
	else if (path == "/add") {
		library::Song *song = server->library->addMedia(Blob(request->content.c_str(), request->content.length()));
		if (!song) {
			throw new GenericError("Library didn't return a song upon addMedia().");
		}

		HTTP::Response r;
		r.content = song->getID();
		r.finalize();
		write(r);
		close();
	}
	else if (path.find("/download/") == 0) {
		string id = path.substr(10); //skip the /download/ part
		library::Song *song = server->library->getSong(id);
		Blob blob = song->loadMainFormat();

		HTTP::Response r;
		r.content.assign((const char*)blob.buffer, blob.length);
		r.finalize();
		write(r);
		close();
	}
	else {
		clog << "unable to serve requested object " << path << endl;
		HTTP::Response r;
		r.statusCode = 404;
		r.statusText = string("Requested object ") + path + " not found.";
		r.finalize();
		write(r);
		close();
	}
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