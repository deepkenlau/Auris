/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

#if defined(AURIS_CONSOLE_ENTITY)
	#if defined(AURIS_CONSOLE_IDENTITY)
		#define AURIS_CONSOLE_PREFIX "[" << AURIS_CONSOLE_ENTITY << " " << AURIS_CONSOLE_IDENTITY << "] "
	#else
		#define AURIS_CONSOLE_PREFIX "[" << AURIS_CONSOLE_ENTITY << "] "
	#endif
#else
	#define AURIS_CONSOLE_PREFIX ""
#endif

#define clog std::cout << "\033[1;36m" << AURIS_CONSOLE_PREFIX << "\033[0m\033[37m" << __FUNCTION__ << "." << __LINE__ << ":\033[0m "
#define cerror std::cerr << "\033[1;31m" << AURIS_CONSOLE_PREFIX << "\033[0m\033[37m" << __FUNCTION__ << "." << __LINE__ << ":\033[0m \033[1;31m***\033[0m "