/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <string>
#include "Socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static Socket* Socket::makeListening(int port)
{
	Socket *socket = new Socket;
	socket->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket->fd < 0)
		throw new std::exception("Error on creating socket.");
	socket->addr.sin_family = AF_INET;
	socket->addr.sin_port = htons(port);
	socket->addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(socket->fd,(struct sockaddr*)&socket->addr,sizeof(socket->addr)) < 0)
		throw new std::exception("Error on binding.");
	listen(socket->fd, 5);
	return socket;
}

static Socket* Socket::makeConnected(std::sting hostname, int port)
{
	Socket *socket = new Socket;
	struct hostent *server;
	socket->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket-fd < 0)
		throw new std::exception("Error on creating socket.");
	server = gethostbyname(hostname);
	if(server == NULL)
		throw new std::exception("Server not found");
	bzero((char*)&socket->addr,sizeof(socket->addr));
	socket->addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,
		(char*)&socket->addr.sin_addr.s_addr,
		server->h_length);
	socket->addr.sin_port = htons(port);
	if(connect(socket->fd,
		(struct sockaddr*)&socket->addr,
		sizeof(socket->addr)) < 0)
		throw new std::exception("Error on connecting");
	return socket;
}