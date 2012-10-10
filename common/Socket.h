/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>

class Socket
{
public:
	static Socket* makeListening(int port);
	static Socket* makeConnected(std::string hostname, int port);
	
	virtual Socket* accept() = 0;
	virtual bool poll(unsigned int timeout) = 0;
	virtual int read(char *buffer, unsigned int length) = 0;
	virtual int write(const char *buffer, unsigned int length) = 0;
};