/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>

#include "../common/Mutex.h"


namespace Database
{
	class Connection;

	class Server
	{
	public:
		void run(int argc, char *argv[]);

	private:
		Mutex connections_mutex;
		std::set<Connection*> connections;
		void addConnection(Connection* c);
		void removeConnection(Connection* c);
	};
}