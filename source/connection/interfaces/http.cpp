#include "http.h"
#define logn "HTTP Interface"
#include "../../log.h"
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


class ConnectionInterfaceHTTP::Handler {
private:
    ConnectionInterfaceHTTP * const interface;
    int fd;
    pthread_t thread;
    static void * threadMain(void * handler);
    
public:
    Handler(ConnectionInterfaceHTTP * interface, int socket);
    ~Handler();
    
    void start();
};

ConnectionInterfaceHTTP::Handler::Handler(ConnectionInterfaceHTTP * interface, int socket) : interface(interface)
{
    fd = socket;
}

ConnectionInterfaceHTTP::Handler::~Handler()
{
    //Close the socket if it is still open.
    if (fd)
        close(fd);
}

void ConnectionInterfaceHTTP::Handler::start()
{
    //Spawn the communication thread.
    pthread_create(&thread, NULL, &ConnectionInterfaceHTTP::Handler::threadMain, this);
}

void * ConnectionInterfaceHTTP::Handler::threadMain(void * handler)
{
    Handler * h = (Handler *)handler;
    
    //Read the HTTP header.
    std::stringstream headerBuffer;
    std::map<std::string, std::string> headers;
    char buffer[1024];
    do {
        int i = 1;
        read(h->fd, buffer 1024);
        
        while (buffer[i-1] != '\r' && buffer[i] != '\n')
            headerBuffer << buffer[i++];
    } while (
    
    //Do the communication.
    char buffer[10000];
    read(h->fd, buffer, 10000);
    log << buffer << std::endl;
    close(h->fd);
    h->fd = 0;
    
    //We're done. Inform the interface we're done.
    h->interface->onHandlerDone(h);
    pthread_exit(NULL);
}



/** Called by a Handler if it finishes its tasks. Causes it to be removed from
 * the list of active handlers. */
void ConnectionInterfaceHTTP::onHandlerDone(Handler * h)
{
    pthread_mutex_lock(&handlersMutex);
    handlers.erase(h);
    delete h;
    pthread_mutex_unlock(&handlersMutex);
}

ConnectionInterfaceHTTP::ConnectionInterfaceHTTP(int port) : port(port)
{
    pthread_mutex_init(&handlersMutex, NULL);
    listenerfd = 0;
    accepterThread = 0;
}

ConnectionInterfaceHTTP::~ConnectionInterfaceHTTP()
{
    pthread_mutex_destroy(&handlersMutex);
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
	int reuseAddress = 1;
	setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR, &reuseAddress, sizeof(reuseAddress));
    
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
    ConnectionInterfaceHTTP * i = (ConnectionInterfaceHTTP *)interface;
    
    //Loop indefinitely and accept new socket connections.
    while (true) {
        
        //Prepare the client address.
		struct sockaddr_in clientAddress;
		socklen_t clientLength = sizeof(clientAddress);
		
		//Accept new connections.
		int clientfd = accept(i->listenerfd, (struct sockaddr *)&clientAddress, &clientLength);
		if (clientfd < 0) {
			err << "unable to accept connection, " << strerror(errno) << std::endl;
			continue;
		}
        
        //Create a handler for this connection.
        Handler * h = new Handler(i, clientfd);
        pthread_mutex_lock(&i->handlersMutex);
        i->handlers.insert(h);
        pthread_mutex_unlock(&i->handlersMutex);
        h->start();
    }
    
    //Close the listener socket.
    close(i->listenerfd);
    i->listenerfd = 0;
    
    //We're done.
    pthread_exit(NULL);
}
