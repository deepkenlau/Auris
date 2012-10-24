/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif
}
#include "uuid.h"

using std::string;


string uuid::generate()
{
#ifdef WIN32
    UUID uuid;
    UuidCreate (&uuid);
    
    unsigned char * str;
    UuidToStringA(&uuid, &str);
    
    std::string s((char *)str);
    
    RpcStringFreeA(&str);
#else
    uuid_t uuid;
    uuid_generate_random(uuid);
    char s[37];
    uuid_unparse(uuid, s);
#endif
    return s;
}
