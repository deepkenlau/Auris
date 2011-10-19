#include "server.h"
#define logn "Server"
#include "log.h"


void Server::run()
{
    //Start the subsystems.
    log << "starting subsystems…" << std::endl;
    connection.start();
    
    //Enter the main wait loop.
    log << "running" << std::endl;
    while (1) sleep(1);
}
