/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace auris {
namespace db {

using std::istream;
using std::ostream;
using std::string;
using std::set;
using std::vector;

/**
 * @brief Reads and writes the index file.
 *
 * The index file lists the pending additions, patches and deletions from the
 * database.
 */
class Index
{
public:
	set<string> additions;
	set<string> deletions;
	set<string> patches;

	Index() {}
	Index(istream& is) { read(is); }

	void read(istream& is)
	{
		vector<char> line;
		while (is.good())
		{
			// Read until the end of the line is found.
			line.clear();
			int c;
			while (is.good() && (c = is.get()) != '\n')
				line.push_back((char)c);

			// Skip empty lines or comment lines.
			if (line.empty() || line[0] == '#')
				continue;

			// Parse the line, moving its contents into one of the sets.
			size_t sol = 1;
			for (sol = 1; sol < line.size() && (line[sol] == ' ' || line[sol] == '\t'); sol++);
			string param(&line[sol], line.size()-sol);

			switch (line[0]) {
				case '+': additions.insert(param); break;
				case '-': deletions.insert(param); break;
				case 'p': patches.insert(param); break;
				default: std::cerr << "ignoring garbage line " << string(&line[0], line.size()) << " in index\n";
			}
		}
	}

	void write(ostream& os) const
	{
		set<string>::const_iterator it;
		for (it = additions.begin(); it != additions.end(); it++)
			os << "+ " << *it << '\n';
		for (it = deletions.begin(); it != deletions.end(); it++)
			os << "- " << *it << '\n';
		for (it = patches.begin(); it != patches.end(); it++)
			os << "p " << *it << '\n';
	}
};

} // namespace db
} // namespace auris