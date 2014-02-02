/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include <ctime>

namespace auris {
namespace aux {

class Date
{
public:
	time_t timestamp;

	Date() { from(time(0)); }
	Date(time_t t) { from(t); }

	Date& from(time_t t) { timestamp = t; return *this; }

	size_t str(char *dst, size_t len)
	{
		struct tm now = *localtime(&timestamp);
		return strftime(dst, len, "%Y-%m-%d %H:%M:%S %z", &now);
	}

	std::string str()
	{
		char buffer[80];
		str(buffer, sizeof(buffer));
		return buffer;
	}

	size_t raw(char *dst, size_t len)
	{
		return snprintf(dst, len, "%li", timestamp);
	}

	std::string raw()
	{
		char buffer[16];
		raw(buffer, sizeof(buffer));
		return buffer;
	}
};

} // namespace aux
} // namespace auris