/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Response.h"
#include <sstream>

using HTTP::Response;
using std::string;
using std::stringstream;


Response::Response()
{
	statusCode = 200;
	statusText = "OK";
}

/** Finalizes the response, i.e. adds the Content-Length header if it is not
 * yet set. */
void Response::finalize()
{
	if (!headers.has("Content-Lenth")) {
		stringstream s;
		s << content.length();
		headers.add("Content-Length", s.str());
	}
}

Response* Response::fromString(const string &str, unsigned int *consumed)
{
	unsigned int cons = 0;

	//Parse the first line which should contain status information.
	size_t crlf = str.find("\r\n");
	if (crlf == string::npos) return NULL;

	size_t space0 = str.find(' ');
	size_t space1 = str.find(' ', space0);
	if (space0 == string::npos || space1 == string::npos) return NULL;

	string statusCode = str.substr(space0+1, space1-space0-1);
	string statusText = str.substr(space1+1, crlf);
	cons = crlf+2;

	//Parse the headers.
	unsigned int headersConsumed = 0;
	HeaderSet *hs = HeaderSet::fromString(str.substr(cons), &headersConsumed);
	if (hs == NULL) return NULL;
	cons += headersConsumed;

	//Read the content.
	int expectedLength = 0;
	if (hs->has("Content-Length")) {
		expectedLength = atoi(hs->get("Content-Length").c_str());
		if (expectedLength > str.size() - cons) {
			return NULL;
		}
	}

	//Create the reponse object.
	Response *r = new Response;
	r->statusCode = atoi(statusCode.c_str());
	r->statusText = statusText;
	r->headers = *hs;
	r->content = str.substr(cons, expectedLength);

	cons += expectedLength;
	if (consumed) *consumed = cons;

	return r;
}

std::string Response::toString() const
{
	stringstream s;
	s << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
	s << headers.toString();
	s << content;
	return s.str();
}
