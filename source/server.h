#pragma once
#include "connection/subsystem.h"
#include "command/subsystem.h"
#include "storage/subsystem.h"


class Server {
    friend class ConnectionSubsystem;
    friend class CommandSubsystem;
    friend class StorageSubsystem;
    
protected:
    //Subsystems
    std::set<Subsystem *> subsystems;
    ConnectionSubsystem connection;
    CommandSubsystem command;
    StorageSubsystem storage;
    
public:
    Server();
    
    void run();
};
