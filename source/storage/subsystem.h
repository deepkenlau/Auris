#pragma once
#include <set>
#include "library.h"
#include "../subsystem.h"


class StorageSubsystem : public Subsystem {
private:
    std::set<StorageLibrary *> libraries;
    
public:
    StorageSubsystem(Server * s);
    void start();
    
    bool onRawCommand(RawCommand * c);
};
