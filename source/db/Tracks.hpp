/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include <iostream>
#include <string>
#include <set>

namespace auris {
namespace db {

using std::istream;
using std::ostream;
using std::set;
using std::string;

class Tracks
{
public:
	set<string> bases;
	set<string> tracks;
	string change_date;

	void read(istream &is)
	{
		std::cerr << "reading tracks file not yet implemented\n";
	}

	void write(ostream &os)
	{
		if (!bases.empty()) {
			os << "Bases: ";
			for (set<string>::const_iterator it = bases.begin(); it != bases.end(); it++) {
				if (it != bases.begin())
					os << ", ";
				os << *it;
			}
			os << "\n";
		}
		os << "Date: " << change_date << "\n";
		os << "\n";
		for (set<string>::const_iterator it = tracks.begin(); it != tracks.end(); it++) {
			os << *it << "\n";
		}
	}
};

} // namespace db
} // namespace auris