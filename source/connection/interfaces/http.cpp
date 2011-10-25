#include "http.h"
#define logn "HTTP Interface"
#include "../../log.h"
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>
#include "../../command/rawcommand.h"


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
    
    //Prepare some space for the parsed HTTP request.
	std::string request;
    std::string path;
	std::map<std::string,std::string> headers;
    
    //Read buffer.
    char rawBuffer[1024];
    int leftoverStart = 0, leftoverEnd = 0;
	std::stringstream lineBuffer;
    
    //Read state machine.
	bool keepReading = true;
	bool firstLine = true;
	bool skipNextChar = false;
	
    //As long as we're still inside the header, keep reading.
	while (keepReading) {
        
        //Fetch the next portions of the buffer and iterate over it.
		int nRead = read(h->fd, rawBuffer, 1024);
		for (int i = 0; i < nRead; i++) {
            
            //If we've arrived at a CRLF, interpret the line.
			if (rawBuffer[i] == '\r' && rawBuffer[i+1] == '\n') {
                
                //Skip the next character which will be the \n.
				skipNextChar = true;
                
                //Fetch whatever is in the line buffer and empty it for the next
                //line.
				std::string line = lineBuffer.str();
				lineBuffer.str("");
                
                //If we've read an empty line, this indicates that the header of
                //the HTTP request is over.
				if (line.empty()) {
                    keepReading = false;
                    
                    //Mark which portion of the raw buffer already contains bi-
                    //nary data.
                    leftoverStart = (i + 2);
                    leftoverEnd = nRead;
                }
                
                //Otherwise we have to parse the line.
				else {
                    
                    //If this is the first line, we have to proceed differently
                    //than with the rest of the header lines.
                    if (firstLine) {
                        firstLine = false;
                        
                        //Separate the request type (GET, PUT, etc.) from the
                        //requested URL.
                        int space1 = line.find_first_of(' ');
                        int space2 = line.find_last_of(' ');
                        request = std::string(line, 0, space1);
                        path = std::string(line, space1 + 1, space2 - space1 - 1);
                    }
                    
                    //Otherwise this is a header line which we have to split
                    //into key and value portions.
                    else {
						int colon = line.find_first_of(':');
						std::string key(line, 0, colon);
						std::string value(line, colon + 2);
                        headers[key] = value;
					}
				}
			}
            
            //Otherwise move the next character into the line buffer if not told
            //to ignore it.
			else if (skipNextChar)
                skipNextChar = false;
			else
				lineBuffer << rawBuffer[i];
		}
	}
    
    //Dump some information on what kind of request we have received.
    log << request << " request for " << path << std::endl;
    for (std::map<std::string, std::string>::iterator it = headers.begin();
         it != headers.end(); it++)
        log << "   " << it->first << ": " << it->second << std::endl;
    log << "- [" << leftoverStart << ", " << leftoverEnd << "] already contains binary data" << std::endl;
    
    //If this is either a POST or PUT command, extract the content length so we
    //know how much binary data we ought to expect.
    unsigned long contentLength = 0;
    if (request == "POST" || request == "PUT")
        contentLength = atol(headers["Content-Length"].c_str());
    log << "- expecting " << contentLength << " bytes of data" << std::endl;
    
    //Receive the entire file.
    unsigned char * content = NULL;
    if (contentLength > 0) {
        
        //Allocate enough memory for the file.
        content = new unsigned char [contentLength];
        unsigned long i = 0;
        
        //Copy the existing buffer.
        i = (leftoverEnd - leftoverStart);
        memcpy(content, rawBuffer + leftoverStart, i);
        
        //Receive the entire file.
        while (i < contentLength) {
            int nRead = read(h->fd, content + i, 1024);
            i += nRead;
        }
        log << "- done" << std::endl;
    }
    
    //Dump the object temporarily.
    std::stringstream name;
    name << "/tmp/";
    name << "aurisd_reception_";
    name << time(NULL);
    name << ".mp3";
    const char * p = name.str().c_str();
    FILE * df = fopen(p, "w");
    fwrite(content, 1, contentLength, df);
    fclose(df);
    log << "- received file written to " << p << std::endl;
    
    //Wrap the reception up in a command object. The path the HTTP request is
    //considering is split apart at the forward slashes to generate a list of
    //arguments.
    //TODO: urldecode the arguments
    RawCommand * c = new RawCommand;
    std::string s(path, (path[0] == '/' ? 1 : 0));
    while (true) {
        int fwds = s.find_first_of('/');
        if (fwds == std::string::npos)
            break;
        c->arguments.push_back(std::string(s, 0, fwds));
        s = std::string(s, fwds + 1);
    }
    c->arguments.push_back(s);
    c->data = content;
    log << "= " << c->desc() << std::endl;
    delete c;
    
    //We're through, close the connection.
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
