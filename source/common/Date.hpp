/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include <ctime>

namespace auris {

class Date
{
public:
	struct tm now;

	Date() { from(time(0)); }
	Date(time_t t) { from(t); }
	Date(struct tm t) : now(t) {}

	Date& from(time_t t) { now = *localtime(&t); return *this; }
	Date& from(struct tm t) { now = t; return *this; }

	size_t str(char *dst, size_t len)
	{
		return strftime(dst, len, "%Y-%m-%d %H:%M:%S %z", &now);
	}

	std::string str()
	{
		char buffer[80];
		str(buffer, sizeof(buffer));
		return buffer;
	}
};

} // namespace auris