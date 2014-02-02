/* Copyright © 2012-2014 Fabian Schuiki, Sandro Sgier */
#include <string>

namespace auris {
namespace aux {

class uuid
{
public:
    static std::string generate();
};

} // namespace aux
} // namespace auris

// Windows
#ifdef WIN32
#include <Rpc.h>
inline std::string auris::aux::uuid::generate()
{
	UUID uuid;
	UuidCreate (&uuid);

	unsigned char* str;
	UuidToStringA(&uuid, &str);

	std::string s((char *)str);

	RpcStringFreeA(&str);
	return s;
}

// actual operating systems
#else
#include <uuid/uuid.h>
inline std::string auris::aux::uuid::generate()
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