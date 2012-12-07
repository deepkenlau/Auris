/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include "Connection.h"
#include <common/Mutex.h>
#include <list>


namespace player
{
	class Output : public gc
	{
		const int id;
		std::list<std::string> songList;
		Mutex lock;

	public:
		Output(int id) : id(id) {}
		void start();
		void run();
		void source(HTTP::Request *request);
		void play(HTTP::Request *request);
		void pause();
		void resume();
		void stop();
		void skip();
		void previous();
		void metadata();
		int getId() const;
	};
}