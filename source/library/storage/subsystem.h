#pragma once
#include "../subsystem.h"


class StorageSubsystem : public LibrarySubsystem {
public:
    StorageSubsystem(Library * l);
    void start();
    
    bool onRawCommand(RawCommand * c);
};
