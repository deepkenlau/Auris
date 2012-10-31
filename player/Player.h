/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#define __STDC_CONSTANT_MACROS
#include <set>
#include <gc_cpp.h>
#include <gc_allocator.h>
#include <sstream>
#include "../common/Mutex.h"


namespace player
{
	class Connection;
	class Player : public gc
	{
		friend class Connection;
		
	public:
		void run(int argc, char *argv[]);

		Player();

	private:
		Mutex connections_mutex;
		typedef std::set<Connection*, std::less<Connection*>, gc_allocator<Connection*> > Connections;
		Connections connections;
		void addConnection(Connection* c);

	protected:
		void removeConnection(Connection* c);
	};
}