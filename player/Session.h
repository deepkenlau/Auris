/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>

namespace player
{
	class Session : public gc
	{
		const int id;
	public:
		Session(int id) : id(id) {}
		void play(std::string host, std::string uuid);
		void stop();
		int getId() const;
	};
}