/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <iostream>
#include <sstream>
#include <common/HTTP/Response.h>
#include <common/HTTP/Request.h>
#include <common/HTTP/Connection.h>


class Socket;

namespace database
{
	class Server;

	class Connection : public HTTP::Connection
	{
		Server *const server;

	public:
		Connection(Socket *socket, Server *server);

		void run();
		void received(HTTP::Request *request);
	};
}