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

		unsigned int getID() const { return id; }
	};
}