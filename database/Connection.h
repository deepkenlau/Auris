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

		std::streambuf inputBuffer;
		std::streambuf outputBuffer;
	public:
		Connection(Socket *socket, Server *server);

		void start();
		void run();

		void received();
		void write(const char *data, unsigned int length);
	};
}