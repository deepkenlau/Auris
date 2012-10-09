/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Server.h"


int main(int argc, char *argv[])
{
	Database::Server s;
	s.run(argc, argv);
	return 0;
}