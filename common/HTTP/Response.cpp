/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Response.h"
#include <string>

static Response* Response::fromString(const std::string &str, unsigned int *consumed = NULL)
{
	Response* response = new Response;
	unsigned int consumed = 0;
	HeaderSet *headerSet = HeaderSet::fromString(str, &consumed);
	if (headerSet == NULL) return NULL;
	response->headers =* headerSet;
	response->content = str.substr(str, consumed);
	return response;
}

std::string Response::toString() const
{
	std::string str = headers.toString();
	str += content;
	return str;
}
