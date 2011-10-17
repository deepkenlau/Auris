#pragma once
#include "channel.h"


class HTTPChannel : public Channel {
public:
    HTTPChannel();
    void std::string desc();
    void accepted(int fd);
};
