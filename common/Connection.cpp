/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Socket.h"
#include "Thread.h"
#include "Error.h"
#include <typeinfo>

using std::string;
using std::stringstream;
using std::endl;


/** Creates a new connection object that will handle communication on the given
 * socket. */
Connection::Connection(Socket *socket)
{
	this->socket = socket;
}

/** Main function of the connection thread. Simply calls the connection's run
 * function. */
static void* connectionThread(void *param)
{
	Connection *c = (Connection*)param;
	try {
		c->run();
	} catch (Error *e) {
		c->err() << "uncaught connection " << e->what() << endl;
	} catch (std::exception &e) {
		c->err() << "uncaught connection " << typeid(e).name() << ": " << e.what() << endl;
	}
	return NULL;
}

/** Starts the thread that handles this connection. */
void Connection::start()
{
	Thread::make(connectionThread, this);
}

/** The connection's main function that handles communication. Always called on
 * the connection's own thread, so make sure proper synchronization measures
 * are in place. The function returns after the connection has closed. Override
 * this method if you need to keep track of connection closings. */
void Connection::run()
{
	log() << "connected" << endl;

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
				this->received(inputBuffer);
			}


			//Write data to the socket as long as there's data left in the output buffer.
			int writeOffset = 0;
			do {
				outputBuffer_lock.lock();
				num_read = std::min<int>(BUFFER_SIZE, outputBuffer.length() - writeOffset);
				outputBuffer.copy(buffer, num_read, writeOffset);
				outputBuffer_lock.unlock();

				if (num_read > 0) {
					socket->write(buffer, num_read);
					writeOffset += num_read;
				}
			} while (num_read > 0);

			int inputLength = 0;
			if (writeOffset > 0) {
				outputBuffer_lock.lock();
				outputBuffer.assign(outputBuffer, writeOffset, outputBuffer.length() - writeOffset);
				inputLength = outputBuffer.length();
				outputBuffer_lock.unlock();
			} else {
				outputBuffer_lock.lock();
				inputLength = outputBuffer.length();
				outputBuffer_lock.unlock();
			}
			if (inputLength == 0 && closeAfterWrite) {
				socket->close();
			}
		}
	} catch (Error *e) {
		err() << "aborting connection due to " << e->what() << endl;
	}

	//We're done, close the connection and remove it from the server.
	socket->close();
	log() << "closed" << endl;
}

/** Stores the given data in the output buffer to be sent. Thread-safe. */
void Connection::write(const char *data, unsigned int length)
{
	outputBuffer_lock.lock();
	outputBuffer.append(data, length);
	outputBuffer_lock.unlock();
}

/** Stores the given string in the output buffer to be sent. Thread-safe. */
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
const std::string& Connection::getClientName() const
{
	return socket->getRemoteAddress();
}

/** Returns an std::ostream intended for regular logging, prefixed with the connection info. */
std::ostream& Connection::log() const
{
	return (std::cout << "[" << describe() << "] ");
}

/** Returns an std::ostream intended for error logging, prefixed with the connection info. */
std::ostream& Connection::err() const
{
	return (std::cerr << "*** [" << describe() << "] ");
}

/** Returns a string describing the connection. Used during logging to mark
 * lines produced by the connection. */
std::string Connection::describe() const
{
	return string("connection ") + getClientName();
}