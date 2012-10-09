/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <iostream>

using std::cerr;
using std::endl;


int main(int argc, char *argv[])
{
	int port = 8080;

	//Setup the listening socket for the control connections.
	Socket* listener = Socket::makeListening(port);
	if (!listener) {
		cerr << "unable to create listening socket" << endl;
		return 1;
	}

	//Enter the main loop that waits for new connections.
	while (Socket* newConnection = listener->accept()) {
		
	}

	return 0;
}