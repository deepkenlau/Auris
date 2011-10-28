#pragma once
#include "../subsystem.h"


class StorageSubsystem : public Subsystem {
public:
    StorageSubsystem(Server * s);
    void start();
    
    bool onRawCommand(RawCommand * c);
};
