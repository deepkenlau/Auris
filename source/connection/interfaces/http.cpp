#include "http.h"
#define logn "HTTP Interface"
#include "../../log.h"
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


ConnectionInterfaceHTTP::ConnectionInterfaceHTTP(int port) : port(port)
{
    listenerfd = 0;
    accepterThread = 0;
}

ConnectionInterfaceHTTP::~ConnectionInterfaceHTTP()
{
}

void ConnectionInterfaceHTTP::start()
{
    log << "starting…" << std::endl;
    
    //Initialize a new listening socket.
    listenerfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerfd < 0) {
        err << "unable to open listener socket" << std::endl;
        listenerfd = 0;
        return;
    }
    
    //Create and clear the server socket address structure.
    struct sockaddr_in a;
    memset(&a, 0 ,sizeof(a));
    
    //Fill the server address with some information.
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(port);
    
    //Bind the socket to the created server address.
    if (bind(listenerfd, (struct sockaddr *)&a, sizeof(a)) < 0) {
        err << "unable to bind socket, " << strerror(errno) << std::endl;
        close(listenerfd);
        listenerfd = 0;
        return;
    }
    
    //Start listening for connections.
    listen(listenerfd, 10);
    
    //Start the accepter thread.
    log << "starting accepter thread..." << std::endl;
    int r = pthread_create(&accepterThread, NULL, &ConnectionInterfaceHTTP::accepterThreadMain, this);
    if (r) {
        err << "pthread_create() returned " << r << std::endl;
        return;
    }
    
    //We're done.
    log << "waiting for connections on port " << port << std::endl;
}

void * ConnectionInterfaceHTTP::accepterThreadMain(void * interface)
{
    log << "buhuhuh" << std::endl;
    
    //We're done.
    pthread_exit(NULL);
}
