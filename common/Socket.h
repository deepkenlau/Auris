/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

class Socket
{
	
public:
	static Socket* makeListening(int port);
	static Socket* makeConnected(std::string hostname, int port);
	Socket* accept();
	bool poll(int timeout);
	int read(char *buffer, unsigned int length);
	int write(const char *source, unsigned int length);
};