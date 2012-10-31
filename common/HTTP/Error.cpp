/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Error.h"
#include "Request.h"
#include <sstream>

using std::string;
using std::stringstream;


HTTP::Error::Error(int status, const std::string message, Request *request, ::Error *underlying)
: ::Error(underlying)
{
	this->status  = status;
	this->message = message;
	this->request = request;
}

string HTTP::Error::describe()
{
	stringstream s;
	s << status << " " << message;
	if (request) {
		s << " Underlying request:\n" << request->toString() << ".";
	}
	return s.str();
}