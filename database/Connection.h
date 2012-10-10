/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

class Socket;

namespace Database
{
	class Server;

	class Connection
	{
	public:
		static Connection* make(Socket *socket, Server *server);

		void fork();
	};
}