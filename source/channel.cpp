#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "channel.h"


Channel::Channel(int port) : port(port)
{
    //Initialize a new listening socket.
    listenerfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerfd < 0) {
        std::cerr << desc() << ": *** unable to open listener socket" << std::endl;
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
        std::cerr << desc() << ": *** unableto bind socket, " << strerror(errno) << std::endl;
        close(listenerfd);
        listenerfd = 0;
        return;
    }
}

Channel::~Channel()
{
    //Close the listening socket. We should never get here actually.
    close(listenerfd);
}

std::string Channel::desc()
{
    return "generic channel";
}

void Channel::run()
{
    //Start listening for connections.
    listen(listenerfd, 10);
    std::cout << desc() << ": waiting for connections on port " << port << std::endl;
    
    //Create a subprocess that handles new connections.
    int pid = fork();
    if (pid == 0) {
        
        //Enter the main wait loop.
        while (true) {
            
            //Prepare the client address which will be filled with information
            //on the client that just connected.
            struct sockaddr_in a;
            socklen_t al = sizeof(a);
            
            //Accept the next new connection.
            int clientfd = accept(listenerfd, (struct sockaddr *)&a, &al);
            if (clientfd < 0) {
                std::cerr << desc() << ": *** unable to accept connection, " << strerror(errno) << std::endl;
            } else {
                
                //Fork the process to handle the individual connection.
                int pid = fork();
                if (pid == 0) {
                    accepted(clientfd);
                    //exit(0);
                }
                
                //The parent keeps listening for connections.
                else {
                    close(clientfd);
                }
            }
        }
    }
    
    //The parent closes the listening socket and returns.
    else {
        close(listenerfd);
    }
}

void Channel::accepted(int fd)
{
    //Close the connection.
    close(fd);
}
