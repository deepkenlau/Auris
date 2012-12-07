/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include "Connection.h"
#include <common/Mutex.h>
#include <map>


namespace player
{
	class Output : public gc
	{
		const int id;
		int list_id;
		bool playing_list;
		bool playing_song;
		bool paused;
		//bool source_is_set = false;

		//temporary
		int currentPosition;
		int currentSong;
		//\temporary
		std::map<int, std::string> songList;
		Mutex lock;

	public:
		Output(int id) : id(id) {start();}
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