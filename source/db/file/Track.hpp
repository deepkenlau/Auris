/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include "Generic.hpp"
#include "readwrite.hpp"
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>

namespace auris {
namespace db {
namespace file {

using std::string;
using std::set;
using std::map;

/**
 * @brief Track file associating metadata with blobs.
 */
class Track : public Generic
{
public:
	/**
	 * @brief Triple of blob reference, format and name.
	 *
	 * This structure is used to capture the different blobs listed within a
	 * track file.
	 */
	class Blob
	{
	public:
		string blob_ref;
		string format;
		string orig_name;

		Blob(const string &br, const string &fmt, const string &on):
			blob_ref(br), format(fmt), orig_name(on) {}

		bool operator< (const Blob &f) const { return blob_ref < f.blob_ref; }
	};

	string id;
	map<string,string> md; // metadata
	set<Blob> blobs;

	/**
	 * @brief Reads the track from an input stream.
	 */
	void read(std::istream &is)
	{
		read_preamble(is, "track");
		
		id.clear();
		md.clear();
		blobs.clear();

		// Read the fields. The read_value function will read fields until a
		// newline has been encountered. The field 'Id' is stored separately in
		// a dedicated member, all other fields are interpreted as metadata and
		// are stored in the 'md' member.
		string name, value;
		while (read_value(is, name, value)) {
			if (name == "id") {
				id = value;
			} else {
				md[name] = value;
			}
		}

		// Reads the blobs. Separated by a newline, the track file lists its
		// different blobs. First, the blob reference (hash) is read until a
		// space (' ') is encountered (which is then skipped). Second, the
		// format field describing the encoding of the data is read until the
		// double hyphen separator ('--') is encountered. Third, the rest of
		// the line is read into the original name field which represents the
		// name of the file when it was added (as a hint). Finally, the format
		// is wrapped up and added to the 'blobs' member set.
		while (is.good()) {
			if (is.peek() == '\n') {
				is.get();
				continue;
			}
			std::stringstream br_buffer, fmt_buffer, on_buffer;

			// Read the blob reference field, delimited by a space (' ').
			while (is.peek() != ' ') {
				int c = is.get();
				if (c == '\n')
					throw std::runtime_error("track: input contains malformatted line, within blob reference, after '" + br_buffer.str() + "'");
				if (!is.good())
					throw std::runtime_error("track: unexpected end of file, within blob reference of format line, after '" + br_buffer.str() + "'");
				br_buffer.put(c);
			}
			is.get(); // skip whitespace

			// Read the format field, delimited by a double hyphen and a space ('-- ').
			int ctmp[3] = {0,0,0};
			while (ctmp[1] != '-' || ctmp[2] != '-' || is.peek() != ' ') {
				int c = is.get();
				int bc = ctmp[0]; // character that will move into buffer
				ctmp[0] = ctmp[1];
				ctmp[1] = ctmp[2];
				ctmp[2] = c;
				if (c == '\n')
					throw std::runtime_error("track: input contains malformatted line, within format, after '" + fmt_buffer.str() + "'");
				if (!is.good())
					throw std::runtime_error("track: unexpected end of file, within format field of format line, after '" + fmt_buffer.str() + "'");
				if (bc != 0)
					fmt_buffer.put(bc);
			}
			is.get(); // skip whitespace

			// Read the rest of the line which is the original format field.
			while (is.peek() != '\n' && is.good()) {
				int c = is.get();
				on_buffer.put(c);
			}
			is.get(); // skip newline
			is.peek(); // checks for EOF such that the loop terminates properly

			blobs.insert(Blob(br_buffer.str(), fmt_buffer.str(), on_buffer.str()));
		}
	}

	/**
	 * @brief Writes the track to an output stream.
	 */
	void write(std::ostream &os)
	{
		if (id.empty())	throw std::runtime_error("track: missing 'id' field");

		write_preamble(os, "track");
		write_value(os, "id", id);
		for (map<string,string>::const_iterator it = md.begin(); it != md.end(); it++) {
			write_value(os, it->first, it->second);
		}
		os.put('\n');
		for (set<Blob>::const_iterator it = blobs.begin(); it != blobs.end(); it++) {
			os << (*it).blob_ref;
			if (!(*it).format.empty())
				os << ' ' << (*it).format;
			os << " -- ";
			std::replace_copy((*it).orig_name.begin(), (*it).orig_name.end(), std::ostream_iterator<char>(os), '\n', ' ');
			os.put('\n');
		}
	}
};

} // namespace file
} // namespace db
} // namespace auris