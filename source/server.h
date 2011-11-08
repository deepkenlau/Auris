#pragma once
#include "connection/subsystem.h"
#include "command/subsystem.h"
#include "library/library.h"

#include <set>
#include <pthread.h>


class Server {
    friend class ConnectionSubsystem;
    friend class CommandSubsystem;
    
public:
    typedef std::set<Library *> LibrarySet;
    
protected:
    //Subsystems
    ConnectionSubsystem connection;
    CommandSubsystem command;
    
    //Libraries
    LibrarySet libraries;
    pthread_mutex_t libraries_mutex;
    
public:
    Server();
    ~Server();
    
    void run();
    
    void addLibrary(Library * l);
    void removeLibrary(Library * l);
    const LibrarySet getLibraries();
};
