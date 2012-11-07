/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Session.h"
#include "Player.h"
#include "../common/Socket.h"
#include <iostream>
#include <stdexcept>
extern "C" {
	#include <libavformat/avformat.h>
}

using player::Player;
using player::Session;
using std::cerr;
using std::endl;
using std::runtime_error;


Player::Player()
{
	nextSessionId = 0;
}

Session * Player::makeSession()
{
	sessions_mutex.lock();
	Session * session = new Session(nextSessionId++);
	sessions.insert(session);
	sessions_mutex.unlock();
	return session;
}

Session * Player::getSession(int sid)
{
//debug
std::cout << "Looking for session " << sid << std::endl;
	sessions_mutex.lock();
	for(Sessions::iterator i = sessions.begin(); i != sessions.end(); i++)
	{
		std::cout << "Looking at session " << (*i)->getId() << endl;
		if ((*i)->getId() == sid)
		{
			sessions_mutex.unlock();
			return *i;
		}
	}
	sessions_mutex.unlock();
	return NULL;
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