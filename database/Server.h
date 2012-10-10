/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <set>
#include <gc_cpp.h>
#include <gc_allocator.h>
#include "../common/Mutex.h"


namespace Database
{
	class Connection;

	class Server : public gc
	{
	public:
		void run(int argc, char *argv[]);

	private:
		Mutex connections_mutex;
		std::set<Connection*, std::less<Connection*>, gc_allocator<Connection*> > connections;
		void addConnection(Connection* c);
		void removeConnection(Connection* c);
	};
}