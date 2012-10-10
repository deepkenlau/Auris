/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>

class Socket;

namespace Database
{
	class Server;

	class Connection : public gc
	{
		Socket *socket;
		Server *server;
	public:
		Connection(Socket *socket, Server *server);

		void start();
		void run();
	};
}