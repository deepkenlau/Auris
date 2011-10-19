#pragma once
#include <pthread.h>
#include "interface.h"


class ConnectionInterfaceHTTP : public ConnectionInterface {
private:
    const int port;
    int listenerfd;
    pthread_t accepterThread;
    
public:
    ConnectionInterfaceHTTP(int port = /*80*/12345);
    ~ConnectionInterfaceHTTP();
    
    void start();
    static void * accepterThreadMain(void * interface);
};
