/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <string>
#include <stdexcept>
#include <iostream>
#include "Socket.h"
#include "Error.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <cstring>
#include <cerrno>

using std::runtime_error;
using std::string;
using std::endl;

#define clog std::cout << "[socket " << getRemoteAddress() << "] "

#ifdef __APPLE__
	#define SEND_RECV_OPTIONS 0
#else
	#define SEND_RECV_OPTIONS MSG_NOSIGNAL
#endif


/** Unix implementation of a socket. */
class UnixSocket : public Socket
{
	friend class Socket;
protected:
	int fd;
	struct sockaddr_in addr;
	bool open;

public:
	Socket* accept();
	bool poll(unsigned int timeout);
	int read(char *buffer, unsigned int length);
	int write(const char *buffer, unsigned int length);
	void close();
	bool isOpen() { return open; }
};


/** Creates a socket waiting for connections on the specified port. */
Socket* Socket::makeListening(int port)
{
	UnixSocket *sock = new UnixSocket;
	sock->port = port;
	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock->fd < 0)
		throw new GenericError("Unable to create socket.", new IOError());

#ifdef __APPLE__
	int one = 1;
	setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
#endif

	sock->addr.sin_family = AF_INET;
	sock->addr.sin_port = htons(port);
	sock->addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock->fd, (struct sockaddr*)&sock->addr, sizeof(sock->addr)) < 0)
		throw new GenericError("Unable to bind socket.", new IOError());
	listen(sock->fd, 5);
	sock->open = true;
	return sock;
}

/** Returns a socket connected to the the given host on the specified port. */
Socket* Socket::makeConnected(string hostname, int port)
{
	//Resolve the hostname.
	struct hostent *server = gethostbyname(hostname.c_str());
	if(server == NULL)
		throw new GenericError("Host " + hostname + " not found.");

	//Create the socket.
	UnixSocket *sock = new UnixSocket;
	sock->port = port;
	sock->remoteAddress = hostname;
	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock->fd < 0) {
		throw new GenericError("Unable to create socket.", new IOError());
	}
	bzero((char*)&sock->addr,sizeof(sock->addr));
	sock->addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,
		(char*)&sock->addr.sin_addr.s_addr,
		server->h_length);
	sock->addr.sin_port = htons(port);

	//Prevent the socket from generating a SIGPIPE.
#ifdef __APPLE__
	int one = 1;
	setsockopt(sock->fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&one, sizeof(int));
#endif

	//Connect to the host.
	if(connect(sock->fd, (struct sockaddr*)&sock->addr, sizeof(sock->addr)) < 0) {
		::close(sock->fd);
		throw new GenericError("Unable to connect to " + hostname + ".", new IOError());
	}
	sock->open = true;

	return sock;
}

Socket* UnixSocket::accept()
{
	//Accept new connection
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int newfd = ::accept(fd, (struct sockaddr *) &addr, &len);
	if (newfd < 0) {
		throw new GenericError("Unable to accept new connection.", new IOError());
	}

	//Prevent the socket from generating a SIGPIPE.
#ifdef __APPLE__
	int one = 1;
	setsockopt(newfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&one, sizeof(int));
#endif

	//Create the new socket object.
	char ip[INET_ADDRSTRLEN];
	UnixSocket *s = new UnixSocket();
	s->fd = newfd;
	s->addr = addr;
	s->port = ntohs(addr.sin_port);
	s->remoteAddress = inet_ntop(AF_INET, &addr.sin_addr, ip, INET_ADDRSTRLEN);
	s->open = true;
	return s;
}

bool UnixSocket::poll(unsigned int timeout_ms)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	struct timeval zeit = {0, timeout_ms*1000};
	int s = select(fd + 1, &set, NULL, NULL, &zeit);
	if(s == -1)
		throw new GenericError("Unable to poll socket.", new IOError());
	return s;
}

int UnixSocket::read(char *buffer, unsigned int length)
{
	ssize_t result = recv(fd, buffer, length, SEND_RECV_OPTIONS);
	if (result == 0) {
		open = false;
		return 0;
	} else if (result < 0) {
		if (errno == EPIPE) {
			open = false;
			return 0;
		} else {
			clog << "*** socket recv returned " << result << endl;
			throw new GenericError("Unable to read data from socket.", new IOError());
		}
	}
	return result;
}

int UnixSocket::write(const char *buffer, unsigned int length)
{
	int result = ::send(fd, buffer, length, SEND_RECV_OPTIONS);
	if (result < 0) {
		if (errno == EPIPE) {
			open = false;
			return 0;
		} else {
			clog << "*** socket write returned " << result << endl;
			throw new GenericError("Unable to write data to socket.", new IOError());
		}
	}
	return result;
}

void UnixSocket::close()
{
	::close(fd);
	open = false;
}
