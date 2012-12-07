/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Output.h"
#include "Player.h"
#include "../common/Socket.h"
#include <iostream>
#include <stdexcept>
extern "C" {
	#include <libavformat/avformat.h>
}

using player::Player;
using player::Output;
using std::cerr;
using std::endl;
using std::runtime_error;


Player::Player()
{
}

Output * Player::getOutput(int oid)
{
	outputs_mutex.lock();
	for(Outputs::iterator i = outputs.begin(); i != outputs.end(); i++)
	{
		std::cout << "Looking at Output " << (*i)->getId() << endl;
		if ((*i)->getId() == oid)
		{
			outputs_mutex.unlock();
			return *i;
		}
	}
	Output * output = new Output(oid);
	outputs.insert(output);
	outputs_mutex.unlock();
	return output;
}

void Player::run(int argc, char *argv[])
{
	av_register_all();

	//Setup the listening socket for the control connections.
	Socket* listener = Socket::makeListening(8081);
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
void Player::addConnection(Connection* c)
{
	connections_mutex.lock();
	connections.insert(c);
	connections_mutex.unlock();
}

/** Removes a connection to the server's list of connections. Thread-safe. */
void Player::removeConnection(Connection* c)
{
	connections_mutex.lock();
	connections.erase(c);
	connections_mutex.unlock();
}

void Player::removeOutput(Output * o)
{
	outputs_mutex.lock();
	outputs.erase(o);
	outputs_mutex.unlock();
}