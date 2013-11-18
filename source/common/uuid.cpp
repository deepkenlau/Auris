/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "uuid.hpp"

using auris::uuid;

// Windows
#ifdef WIN32
#include <Rpc.h>
std::string uuid::generate()
{
	UUID uuid;
	UuidCreate (&uuid);
	
	unsigned char* str;
	UuidToStringA(&uuid, &str);
	
	std::string s((char *)str);

	RpcStringFreeA(&str);
	return s;
}

// useful operating systems
#else
#include <uuid/uuid.h>
std::string uuid::generate()
{
	uuid_t uuid;
	uuid_generate_random(uuid);
	char s[37];
	uuid_unparse(uuid, s);
	for (char *p = s; *p; p++)
		*p = tolower(*p);
	return s;
}
#endif