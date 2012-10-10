/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>

class Socket : public gc
{
	int port;
	std::string remoteAddress;

public:
	static Socket* makeListening(int port);
	static Socket* makeConnected(std::string hostname, int port);
	
	virtual Socket* accept() = 0;
	virtual bool poll(unsigned int timeout_ms) = 0;
	virtual int read(char *buffer, unsigned int length) = 0;
	virtual int write(const char *buffer, unsigned int length) = 0;
	virtual void close() = 0;
	virtual bool isOpen() = 0;

	int getPort() const { return port; }
	const std::string& getRemoteAddress() const { return remoteAddress; }
};