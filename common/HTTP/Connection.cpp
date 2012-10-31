/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Request.h"
#include "Response.h"
#include "Error.h"
#include "../Error.h"

using std::string;
using std::endl;


HTTP::Connection::Connection(Socket *socket) : ::Connection(socket)
{
}

void HTTP::Connection::received(std::string &input)
{
	//Try to parse the HTTP request that was received.
	unsigned int consumed = 0;
	Request *request = Request::fromString(input, &consumed);
	if (consumed > 0) {
		input.assign(input, consumed, input.length() - consumed);
	}
	if (!request) return;

	//Hand the received request to the handler function. Make sure to catch possible exceptions.
	try {
		received(request);
	}
	catch (Error *e) {
		err() << e->what() << endl;

		//Return an appropriate representation error.
		Response r;
		r.statusCode = e->getStatus();
		r.statusText = e->getMessage();
		r.finalize();
		write(r);
		close();
	}
	catch (::Error *e) {
		err() << e->what() << endl;

		//Return an internal server error.
		Response r;
		r.statusCode = 500;
		r.statusText = string("Internal Server Error");
		r.content = e->what() + "\n";
		r.finalize();
		write(r);
		close();
	}
}

/** Sends the given response. The caller is responsible for finalizing the
 * Response and making sure it is valid. */
void HTTP::Connection::write(const Response &r)
{
	::Connection::write(r.toString());
}