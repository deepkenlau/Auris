/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include "Generic.hpp"
#include <set>
#include <string>

namespace auris {
namespace db {
namespace file {

using std::string;
using std::set;

/**
 * @brief Index file listing the tracks of a database.
 */
class Index : public Generic
{
public:
	string base;
	string date;
	set<string> tracks;

	/**
	 * @brief Reads the index from an input stream.
	 */
	void read(std::istream &is)
	{
		read_preamble(is, "INDEX");

		base.clear();
		date.clear();
		tracks.clear();

		// Read the fields. The read_value function will read fields until a
		// newline character or the end of the stream has been encountered.
		// The loop assigns the values to the appropriate members and throws
		// an exception for unknown fields.
		string name, value;
		while (read_value(is, name, value)) {
			if (name == "Date") {
				date = value;
				continue;
			}
			if (name == "Base") {
				base = value;
				continue;
			}
			throw std::runtime_error("unknown field \"" + value + "\"");
		}

		// Read the lines containing track hashes. The while loop consumes all
		// characters left in the input stream. The characters are written to
		// a buffer which is added as a track listing as soon as a '\n' is
		// encountered. The additional 'empty' flag makes sure no empty lines
		// are added (e.g. the newline separating fields and this list).
		std::stringstream buffer;
		bool empty = true;
		while (is.good()) {
			int c = is.get();
			if (c == '\n') {
				if (!empty)
					tracks.insert(buffer.str());
				buffer.str("");
			} else {
				empty = false;
				buffer.put(c);
			}
		}
	}

	/**
	 * @brief Writes the database index to an output stream.
	 */
	void write(std::ostream &os)
	{
		if (date.empty()) throw std::runtime_error("index: missing date field");

		os << "INDEX\n";	
		if (!base.empty()) write_value(os, "Base", base);
		write_value(os, "Date", date);
		os.put('\n');
		for (set<string>::const_iterator it = tracks.begin(); it != tracks.end(); it++) {
			os << *it << '\n';
		}
	}
};

} // namespace file
} // namespace db
} // namespace auris