/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <iostream>
#include <sstream>
#include "../common/Mutex.h"

class Socket;

namespace Database
{
	class Server;

	class Connection : public gc
	{
		Socket *socket;
		Server *server;

		std::stringbuf inputBuffer;
		std::stringbuf outputBuffer;
		Mutex outputBuffer_lock;
	public:
		Connection(Socket *socket, Server *server);

		void start();
		void run();

		void received();
		void write(const char *data, unsigned int length);

		const std::string& getClientName();
	};
}