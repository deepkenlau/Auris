#pragma once
#include "connection/subsystem.h"
#include "command/subsystem.h"
#include "storage/subsystem.h"


class Server {
private:
    //Subsystems
    ConnectionSubsystem connection;
    CommandSubsystem command;
    StorageSubsystem storage;
    
public:
    void run();
};
