#pragma once
#include <set>
#include "library.h"


class StorageSubsystem {
private:
    std::set<StorageLibrary *> libraries;
    
public:
    void start();
};
