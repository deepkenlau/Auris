/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>

namespace database
{
	class Server;

	class Playlist : public gc
	{
	protected:
		Server *const server;
		unsigned int id;

	public:
		Playlist(Server *server, unsigned int id) : server(server), id(id) {}

		typedef struct { int start; int length; } Range;
		const static Range kNilRange;

		unsigned int getID() const { return id; }
	};
}