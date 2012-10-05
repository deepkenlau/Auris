#include <cassert>
#include <iostream>
#include "../common/HTTP/HeaderSet.h"

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

	return 0;
}