/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Request.h"
#include <string>
#include <sstream>

using namespace HTTP;
using std::string;
using std::stringstream;


Request::Request()
{
	type = kGET;
}

/**
 * Tries to construct an HTTP request object from the given string.
 * @return Returns the object representing the request on success, or NULL if
 * the string could not be parsed. If set, consumed will be filled with
 * the amount of data that was consumed from the string to parse the request.
 */
Request* Request::fromString(const string &str, unsigned int *consumed)
{
	return NULL;
}

string Request::toString() const
{
	stringstream s;
	switch (type) {
		case kGET:    s << "GET "; break;
		case kPOST:   s << "POST "; break;
		case kDELETE: s << "DELETE "; break;
		case kPUT:    s << "PUT "; break;
	}
	s << path;
	s << " HTTP/1.1\r\n";
	s << headers.toString();
	s << content;
	s << "\r\n";
	return s.str();
}