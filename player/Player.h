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
	class Output;
	class Player : public gc
	{
		friend class Connection;
		friend class Sessions;
	public:
		void run(int argc, char *argv[]);

		Player();
		//Output * makeOutput();
		/*
		assume following output ids:
		1: speakers
		2: headphone1
		3: headphone2
		*/
		Output * getOutput(int oid);
		/* if the Output object with id oid has not
		been created, it will be created
		*/
	private:
		Mutex outputs_mutex;
		Mutex connections_mutex;

		typedef std::set<Output*, std::less<Output*>, gc_allocator<Output*> > Outputs;
		Outputs outputs;

		typedef std::set<Connection*, std::less<Connection*>, gc_allocator<Connection*> > Connections;
		Connections connections;
		void addConnection(Connection* c);

	protected:
		void removeConnection(Connection* c);
		void removeOutput(Output* o);
	};
}