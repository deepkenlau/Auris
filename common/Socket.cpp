/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <string>
#include <stdexcept>
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
	int one = 1;
	setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
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
	UnixSocket *sock = new UnixSocket;
	struct hostent *server;
	sock->port = port;
	sock->remoteAddress = hostname;
	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock->fd < 0)
		throw new GenericError("Unable to create socket.", new IOError());
	server = gethostbyname(hostname.c_str());
	if(server == NULL)
		throw new GenericError("Host " + hostname + " not found.");
	bzero((char*)&sock->addr,sizeof(sock->addr));
	sock->addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,
		(char*)&sock->addr.sin_addr.s_addr,
		server->h_length);
	sock->addr.sin_port = htons(port);
	if(connect(sock->fd, (struct sockaddr*)&sock->addr, sizeof(sock->addr)) < 0)
		throw new GenericError("Unable to connect to " + hostname + ".", new IOError());
	sock->open = true;
	return sock;
}

Socket* UnixSocket::accept()
{
	UnixSocket *newsock = new UnixSocket();
	socklen_t len = sizeof(newsock->addr);
	newsock->fd = ::accept(fd, (struct sockaddr *) &newsock->addr, &len);
	if (newsock->fd < 0)
		throw new GenericError("Unable to accept new connection.", new IOError());
	newsock->port = ntohs(newsock->addr.sin_port);
	char ip[INET_ADDRSTRLEN];
	newsock->remoteAddress = inet_ntop(AF_INET, &newsock->addr.sin_addr, ip, INET_ADDRSTRLEN);
	newsock->open = true;
	return newsock;
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
	ssize_t result = recv(fd, buffer, length, MSG_DONTWAIT);
	if (result == EAGAIN || result == EWOULDBLOCK || result == 0) {
		open = false;
		return 0;
	} else if (result < 0) {
		throw new GenericError("Unable to read data from socket.", new IOError());
	}
	return result;
}

int UnixSocket::write(const char *buffer, unsigned int length)
{
	int result = ::write(fd, buffer, length);
	if (result < 0) {
		throw new GenericError("Unable to write data to socket.", new IOError());
	}
	return result;
}

void UnixSocket::close()
{
	::close(fd);
	open = false;
}