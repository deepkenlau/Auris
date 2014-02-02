/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include "Object.hpp" // for preamble
#include <iostream>
#include <sstream>

namespace auris {
namespace db {
namespace file {

template <typename N> void write_stream(std::ostream &os, const N &name, std::istream &value)
{
	os << name << ' ';
	while (value.good()) {
		int c = value.get();
		if (!value.good())
			break;
		os.put(c);
		if (c == '\n')
			os.put(' ');
	}
	os.put('\n');
}

template <typename N, typename V> void write_value(std::ostream &os, const N &name, V &value)
{
	std::stringstream buffer;
	buffer << value;
	write_stream<N>(os, name, buffer);
}

inline bool read_stream(std::istream &is, std::ostream &name, std::ostream &value)
{
	if (!is.good() || is.peek() == '\n')
		return false;

	// Read into the name buffer, looking for the " " separator.
	while (is.good()) {
		int c = is.get();
		if (c < 0)
			throw std::runtime_error("line read is not a valid field (unexpected end of stream)");
		if (c == ' ')
			break;
		if (c == '\n')
			throw std::runtime_error("line read is not a valid field (unexpected end of line)");
		name.put(c);
	}

	// Read into the value buffer.
	while (is.good()) {
		int c = is.get();
		if (c < 0) break;
		if (c == '\n') {
			if (is.peek() == ' ') {
				is.get();
			} else {
				break;
			}
		}
		value.put(c);
	}

	return true;
}

template <typename N, typename V> bool read_value(std::istream &is, N &name, V &value)
{
	std::stringstream nb, vb;
	if (!read_stream(is, nb, vb))
		return false;
	nb >> name;
	vb >> value;
	return true;
}

inline bool read_value(std::istream &is, std::string &name, std::string &value)
{
	std::stringstream nb, vb;
	if (!read_stream(is, nb, vb))
		return false;
	name = nb.str();
	value = vb.str();
	return true;
}

inline bool read_stream(std::istream &is, std::string &name, std::ostream &value)
{
	std::stringstream nb;
	if (!read_stream(is, nb, value))
		return false;
	name = nb.str();
	return true;
}



inline void write_preamble(std::ostream &os, const std::string& type)
{
	if (!os.good())
		throw std::runtime_error("cannot write " + type + " file, output stream invalid");

	Object o;
	o.type = type;
	o.write(os);

	if (!os.good())
		throw std::runtime_error("output stream for " + type + " file becomes invalid after writing preamble");
}

inline void read_preamble(std::istream &is, const std::string& required_type)
{
	if (!is.good())
		throw std::runtime_error("cannot read " + required_type + " file, input stream invalid");

	Object o;
	o.read(is);

	if (o.type != required_type)
		throw std::runtime_error("object is not a " + required_type);
	if (!is.good())
		throw std::runtime_error("input stream for " + required_type + " file becomes invalid after preamble");
}

} // namespace file
} // namespace db
} // namespace auris