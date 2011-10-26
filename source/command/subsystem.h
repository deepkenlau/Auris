#pragma once
#include "rawcommand.h"
#include "../subsystem.h"


class CommandSubsystem : public Subsystem {
public:
    CommandSubsystem(Server * s);
    void start();
    
    void processRawCommand(RawCommand * c);
};
