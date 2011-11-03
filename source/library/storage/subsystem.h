#pragma once
#include "../subsystem.h"
#include <map>
#include <string>
#include "../../blob.h"
#include "../../path.h"


class StorageSubsystem : public LibrarySubsystem {
private:
    std::map<std::string, std::string> originalsChecksums;
    Path getOriginalsDir() const;
    
public:
    StorageSubsystem(Library * l);
    void start();
    
    bool onRawCommand(RawCommand * c);
    
    void addFile(Blob * content);
};
