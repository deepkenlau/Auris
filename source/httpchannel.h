#pragma once
#include "channel.h"


class HTTPChannel : public Channel {
public:
    HTTPChannel();
    std::string desc();
    void accepted(int fd);
};
