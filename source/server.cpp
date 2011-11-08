#include "server.h"
#define logn "Server"
#include "log.h"
#include <sys/types.h>
#include <pwd.h>


Server::Server() :
connection(this),
command(this)
{
    pthread_mutex_init(&libraries_mutex, NULL);
}

Server::~Server()
{
    pthread_mutex_destroy(&libraries_mutex);
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
    //libraries.insert(l);
    addLibrary(l);
    l->start();
    
    //Enter the main wait loop.
    log << "running" << std::endl;
    while (1) sleep(1);
}

void Server::addLibrary(Library * l)
{
    pthread_mutex_lock(&libraries_mutex);
    libraries.insert(l);
    pthread_mutex_unlock(&libraries_mutex);
}

void Server::removeLibrary(Library * l)
{
    pthread_mutex_lock(&libraries_mutex);
    removeLibrary(l);
    pthread_mutex_unlock(&libraries_mutex);
}

const Server::LibrarySet Server::getLibraries()
{
    pthread_mutex_lock(&libraries_mutex);
    LibrarySet l = libraries;
    pthread_mutex_unlock(&libraries_mutex);
    return l;
}
