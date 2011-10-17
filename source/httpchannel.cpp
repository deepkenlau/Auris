#include "httpchannel.h"


HTTPChannel::HTTPChannel() : Channel(80)
{
}

void std::string HTTPChannel::desc()
{
    return "HTTP channel";
}

void HTTPChannel::accepted(int fd)
{
}
