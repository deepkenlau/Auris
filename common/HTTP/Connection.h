/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "../Connection.h"


namespace HTTP
{
	class Request;
	class Response;

	class Connection : public ::Connection
	{
	public:
		Connection(Socket *socket);

		virtual void received(std::string &input);
		void write(const Response &r);

		/** Called on the connection's thread whenever a complete HTTP request
		 * has arrived. */
		virtual void received(Request *request) = 0;
	};
}