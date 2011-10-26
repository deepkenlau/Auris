#include "server.h"
#define logn "Server"
#include "log.h"


Server::Server() : connection(this), command(this), storage(this)
{
    //Initialize the list of subsystems.
    subsystems.insert(&connection);
    subsystems.insert(&command);
    subsystems.insert(&storage);
}


void Server::run()
{
    //Start the subsystems.
    log << "starting subsystems…" << std::endl;
    connection.start();
    command.start();
    storage.start();
    
    //Enter the main wait loop.
    log << "running" << std::endl;
    while (1) sleep(1);
}
