/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Request.h"
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "../Error.h"

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
	unsigned int cons = 0;

	//Parse the first line which consists of the request type.
	size_t firstCRLF = str.find("\r\n");
	if (firstCRLF == string::npos) return NULL;
	string firstLine = str.substr(0, firstCRLF);
	cons = firstCRLF + 2;

	size_t methodPos = firstLine.find(" ");
	if (methodPos == string::npos) return NULL;
	string method = firstLine.substr(0, methodPos);
	Type type;
	if (method == "GET") type = kGET;
	else if (method == "POST") type = kPOST;
	else if (method == "DELETE") type = kDELETE;
	else if (method == "PUT") type = kPUT;
	else {
		throw new GenericError(string("Received HTTP request with unknown method '") + method + "'.");
	}

	size_t pathPos = firstLine.find(" ", methodPos + 1);
	if (pathPos == string::npos) return NULL;
	string path = firstLine.substr(methodPos + 1, pathPos - methodPos - 1);

	//Parse the header.
	unsigned int hsConsumed = 0;
	HeaderSet *hs = HeaderSet::fromString(str.substr(firstCRLF + 2), &hsConsumed);
	cons += hsConsumed;
	if (!hs) return NULL;

	//Read the content.
	int expectedLength = 0;
	if (hs->has("Content-Length")) {
		expectedLength = atoi(hs->get("Content-Length").c_str());
		if (expectedLength > str.size() - cons) {
			return NULL;
		}
	}

	//Create the request object.
	Request *req = new Request;
	req->type = type;
	req->path = path;
	req->headers = *hs;
	req->content = str.substr(cons, expectedLength);

	cons += expectedLength;
	if (consumed) *consumed = cons;

	return req;
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