/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <iostream>
#include <sstream>
#include <common/Mutex.h>
#include <common/HTTP/Response.h>
#include <common/HTTP/Request.h>

class Socket;

namespace player
{
	class Player;

	class Connection : public gc
	{
		Socket *socket;
		Player *player;

		std::string inputBuffer;
		std::string outputBuffer;
		Mutex outputBuffer_lock;
		bool closeAfterWrite;

	public:
		Connection(Socket *socket, Player *player);
		static std::string ** parseRequest(std::string request);
		
		void start();
		void run();

		void received();
		void write(const char *data, unsigned int length);
		void close();

		void write(HTTP::Response &r);
		void write(HTTP::Request &r);
		void write(const std::string &s);

		const std::string& getClientName();
	};
}