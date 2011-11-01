#pragma once
#include "connection/subsystem.h"
#include "command/subsystem.h"
#include "library/library.h"
#include <set>


class Server {
    friend class ConnectionSubsystem;
    friend class CommandSubsystem;
    
protected:
    //Subsystems
    ConnectionSubsystem connection;
    CommandSubsystem command;
    
    //Libraries
    std::set<Library *> libraries;
    
public:
    Server();
    void run();
};
