/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include "Connection.h"
#include <common/Mutex.h>

using player::Connection;

namespace player
{
	class Session : public gc
	{
		const int id;
		Mutex commandLock;
		bool active;
		bool playing;
		bool paused;
		bool startPlaying;
		std::string host;
	public:
		Session(int id) : id(id) {}
		void start();
		void run();
		void play(std::string url);
		void stop();
		void pause();
		void resume();
		int getId() const;
	};
}