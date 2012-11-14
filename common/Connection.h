/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <iostream>
#include <sstream>
#include "Mutex.h"

class Socket;

class Connection : public gc
{
	Socket *socket;

	std::string inputBuffer;
	std::string outputBuffer;
	Mutex outputBuffer_lock;
	bool closeAfterWrite;

public:
	Connection(Socket *socket);

	void start();
	virtual void run();

	void write(const char *data, unsigned int length);
	void write(const std::string &s);
	void close();

	/** Called on the Connection's thread whenever new data arrives. The
	 * input containing the unprocessed data is passed as an argument. Feel
	 * free to modify the input, e.g. by removing the processed data. */
	virtual void received(std::string &input) = 0;

	const std::string& getClientName() const;

	/* Consider the following being private. Unfortunately the internal
	 * implementation of Connection requires this to be public. */
	std::ostream& log() const;
	std::ostream& err() const;
	virtual std::string describe() const;
};