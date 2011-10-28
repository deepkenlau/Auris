#pragma once
#include "connection/subsystem.h"
#include "command/subsystem.h"
#include "storage/subsystem.h"
#include "metadata/subsystem.h"
#include "library.h"
#include <set>


class Server {
    friend class ConnectionSubsystem;
    friend class CommandSubsystem;
    friend class StorageSubsystem;
    friend class MetadataSubsystem;
    
protected:
    //Subsystems
    std::set<Subsystem *> subsystems;
    ConnectionSubsystem connection;
    CommandSubsystem command;
    StorageSubsystem storage;
    MetadataSubsystem metadata;
    
    //Libraries
    std::set<Library *> libraries;
    
public:
    Server();
    void run();
};
