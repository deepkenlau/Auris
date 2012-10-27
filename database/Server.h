/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <sstream>
#include "../common/Mutex.h"
#include "database/Database.h"
#include "media/Store.h"


namespace database
{
	class Connection;

	class Server : public gc
	{
		friend class Connection;
		
	public:
		void run(int argc, char *argv[]);

		Server();

		database::Database *database;
		media::Store *media;

	private:
		Mutex connections_mutex;
		typedef std::set<Connection*, std::less<Connection*>, gc_allocator<Connection*> > Connections;
		Connections connections;
		void addConnection(Connection* c);

	protected:
		void removeConnection(Connection* c);
	};
}