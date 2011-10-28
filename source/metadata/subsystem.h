#pragma once
#include "../subsystem.h"


class MetadataSubsystem : public Subsystem {
public:
    MetadataSubsystem(Server * s);
    void start();
};
