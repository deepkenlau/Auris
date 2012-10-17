/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <string>
#include <stdexcept>
#include "Socket.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <cstring>

using std::runtime_error;
using std::string;


/** Unix implementation of a socket. */
class UnixSocket : public Socket
{
	friend class Socket;
protected:
	int fd;
	struct sockaddr_in addr;

public:
	Socket* accept();
	bool poll(unsigned int timeout);
	int read(char *buffer, unsigned int length);
	int write(const char *buffer, unsigned int length);
	void close();
	bool isOpen();
};


/** Creates a socket waiting for connections on the specified port. */
Socket* Socket::makeListening(int port)
{
	UnixSocket *sock = new UnixSocket;
	sock->port = port;
	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock->fd < 0)
		throw new runtime_error("Error on creating socket.");
	int one = 1;
	setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	sock->addr.sin_family = AF_INET;
	sock->addr.sin_port = htons(port);
	sock->addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sock->fd,(struct sockaddr*)&sock->addr,sizeof(sock->addr)) < 0)
		throw new runtime_error("Error on binding.");
	listen(sock->fd, 5);
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
		throw new runtime_error("Error on creating socket.");
	server = gethostbyname(hostname.c_str());
	if(server == NULL)
		throw new runtime_error("Server not found");
	bzero((char*)&sock->addr,sizeof(sock->addr));
	sock->addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,
		(char*)&sock->addr.sin_addr.s_addr,
		server->h_length);
	sock->addr.sin_port = htons(port);
	if(connect(sock->fd, (struct sockaddr*)&sock->addr, sizeof(sock->addr)) < 0)
		throw new runtime_error("Error on connecting");
	return sock;
}

Socket* UnixSocket::accept()
{
	UnixSocket *newsock = new UnixSocket();
	socklen_t len = sizeof(newsock->addr);
	newsock->fd = ::accept(fd, (struct sockaddr *) &newsock->addr, &len);
	if (newsock->fd < 0)
		throw new runtime_error("Error on accepting.");
	newsock->port = ntohs(newsock->addr.sin_port);
	char ip[INET_ADDRSTRLEN];
	newsock->remoteAddress = inet_ntop(AF_INET, &newsock->addr.sin_addr, ip, INET_ADDRSTRLEN);
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
		throw new runtime_error("Error on poll.");
	return s;
}

int UnixSocket::read(char *buffer, unsigned int length)
{
	int bytes_read = ::read(fd, buffer, length);
	if (bytes_read < 0)
		throw new runtime_error("Unable to read bytes from socket.");
	return bytes_read;
}

int UnixSocket::write(const char *buffer, unsigned int length)
{
	int bytes_written = ::write(fd, buffer, length);
	if (bytes_written < 0)
		throw new runtime_error("Unable to write bytes from socket.");
	return bytes_written;
}

void UnixSocket::close()
{
	::close(fd);
}

bool UnixSocket::isOpen()
{
	int val;
	val = ::write(fd, &val, 0);
	if (val < 0) return false;
	return true;
}