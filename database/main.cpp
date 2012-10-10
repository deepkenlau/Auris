/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <gc_cpp.h>
#include "Server.h"


int main(int argc, char *argv[])
{
	GC_INIT();
	Database::Server s;
	s.run(argc, argv);
	return 0;
}