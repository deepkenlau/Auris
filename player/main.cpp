/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <gc_cpp.h>
#include <iostream>
#include <common/Error.h>
#include "Player.h"


int main(int argc, char *argv[])
{
	GC_INIT();
	player::Player s;

	std::cout << "el player\n";
	/*
	try {
		s.run(argc, argv);
	} catch (Error *e) {
		std::cerr << "*** " << e->what() << std::endl;
	} catch (std::exception &e) {
		std::cerr << "*** exception: " << e.what() << std::endl;
	} catch (std::exception *e) {
		std::cerr << "*** exception: " << e->what() << std::endl;
	}
	*/
	return 0;
}