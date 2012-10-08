#include <cassert>
#include <iostream>
#include "../common/HTTP/HeaderSet.h"
#include "../common/HTTP/Request.h"

#define assert_equal(a,b) if (a != b) { std::cerr << #a << ": expected " << b << ", got " << a << std::endl; return 1; }

int main(int argc, char *argv[])
{
	HTTP::HeaderSet hs;
	hs.add("Content-Type", "text/plain");
	assert(hs.has("Content-Type"));
	hs.add("Content-Length", "50823");
	hs.add("Rubbish", "123");
	hs.remove("Rubbish");
	assert(!hs.has("Rubbish"));

	std::string hso = hs.toString();
	assert_equal(hso, "Content-Length: 50823\r\nContent-Type: text/plain\r\n\r\n");

	HTTP::HeaderSet *hsp = HTTP::HeaderSet::fromString(hso);
	assert(hsp && "parsed header must be valid");
	std::string hspo = hsp->toString();
	assert_equal(hspo, hso);

	//Test the HTTP::Request object.
	HTTP::Request req;
	req.type = HTTP::Request::kPOST;
	req.path = "/meta.json";
	req.headers.add("Content-Type", "application/json");
	req.headers.add("User-Agent", "auris-db");
	req.content = "Hello World";

	std::string reqo = req.toString();
	assert_equal(reqo, "POST /meta.json HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: auris-db\r\n\r\nHello World\r\n");

	HTTP::Request *reqp = HTTP::Request::fromString(reqo);
	assert(reqp && "parsed request must be valid");
	std::string reqpo = reqp->toString();
	assert_equal(reqpo, reqo);

	return 0;
}