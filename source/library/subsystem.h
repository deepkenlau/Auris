#pragma once
#include "../subsystem.h"

class Library;


class LibrarySubsystem : public Subsystem {
public:
    Library * const library;
    LibrarySubsystem(Library * l);
    
    //Command processing.
    virtual bool onRawCommand(RawCommand * c) { return false; }
};
