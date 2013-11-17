/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include "Generic.hpp"
#include <iostream>
#include <sstream>

namespace auris {
namespace db {
namespace file {

/**
 * @brief Reads and writes an object's preamble.
 */
class Object : public Generic
{
public:
	std::string type;

	/**
	 * @brief Reads an object's preamble from an input stream.
	 *
	 * This function reads the object's type and leaves the input stream in a
	 * state where it points to the beginning of the object's data.
	 */
	void read(std::istream &is)
	{
		std::stringstream buffer;
		while (is.peek() != 0) {
			int c = is.get();
			if (!is.good())
				throw std::runtime_error("end of stream encountered inside object preamble");
			buffer.put(c);
		}
		is.get(); // skip the 0 byte
		type = buffer.str();
	}

	/**
	 * @brief Write an object's preamble to an output stream.
	 *
	 * This function writes the object's type and leaves the output stream in a
	 * state where it points to the beginning of the object's data, ready to be
	 * filled.
	 */
	void write(std::ostream &os)
	{
		if (type.empty())
			throw std::runtime_error("no type set for object");
		os << type;
		os.put(0);
	}
};

} // namespace file
} // namespace db
} // namespace auris