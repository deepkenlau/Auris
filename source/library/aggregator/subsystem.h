#pragma once
#include "../subsystem.h"


class AggregatorSubsystem : public LibrarySubsystem {
public:
    AggregatorSubsystem(Library * l);
    void start();
};
