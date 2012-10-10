/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <string>
#include <stdexcept>
#include "Socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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
	return NULL;
}

bool UnixSocket::poll(unsigned int timeout)
{
	return false;
}

int UnixSocket::read(char *buffer, unsigned int length)
{
	return 0;
}

int UnixSocket::write(const char *buffer, unsigned int length)
{
	return 0;
}

void UnixSocket::close()
{
}

bool UnixSocket::isOpen()
{
	return false;
}