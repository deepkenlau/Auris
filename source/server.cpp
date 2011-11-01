#include "server.h"
#define logn "Server"
#include "log.h"
#include <sys/types.h>
#include <pwd.h>


Server::Server() :
connection(this),
command(this)
{
}


/** Runs the server. This launches all subsystems and processes all incoming
 * commands and data. The function won't return until all subsystems have shut
 * down on their own. */
void Server::run()
{
    //Start the subsystems.
    log << "starting subsystems…" << std::endl;
    connection.start();
    command.start();
    
    //Initialize a debugging library.
    log << "initializing debug library..." << std::endl;
    struct passwd * pw = getpwuid(getuid());
    const char * homedir = pw->pw_dir;
    std::string path(homedir);
    path += "/Music/Auris";
    Library * l = new Library(this, path);
    libraries.insert(l);
    l->start();
    
    //Enter the main wait loop.
    log << "running" << std::endl;
    while (1) sleep(1);
}
