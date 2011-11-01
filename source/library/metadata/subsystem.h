#pragma once
#include "../subsystem.h"


class MetadataSubsystem : public LibrarySubsystem {
public:
    MetadataSubsystem(Library * l);
    void start();
};
