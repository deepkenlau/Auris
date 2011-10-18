#include "httpchannel.h"
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>


HTTPChannel::HTTPChannel() : Channel(12345)
{
}

std::string HTTPChannel::desc()
{
    return "HTTP channel";
}

void HTTPChannel::accepted(int fd)
{
    char buffer[10025];
    read(fd, buffer, 10024);
    std::cout << buffer << std::endl;
}
