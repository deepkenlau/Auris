#include <cassert>
#include <iostream>
#include "../common/HTTP/HeaderSet.h"

#define assert_equal(a,b) if (a != b) std::cerr << #a << ": expected " << b << ", got " << a << std::endl; return 1;

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
	assert_equal(hso, "Content-Type: text/plain\r\nContent-Length: 50823\r\n");

	return 0;
}