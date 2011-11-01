#pragma once
#include <pthread.h>
#include <set>
#include "interface.h"


class ConnectionInterfaceHTTP : public ConnectionInterface {
    class Handler;
    friend class Handler;
    
private:
    const int port;
    int listenerfd;
    pthread_t accepterThread;

    //Connection handlers.
    std::set<Handler *> handlers;
    pthread_mutex_t handlersMutex;
    
protected:
    void onHandlerDone(Handler * h);

    
public:
    ConnectionInterfaceHTTP(ConnectionSubsystem * c, int port = /*80*/12345);
    ~ConnectionInterfaceHTTP();
    
    void start();
    static void * accepterThreadMain(void * interface);
};
