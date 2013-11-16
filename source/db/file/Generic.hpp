/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include <iostream>
#include <sstream>

namespace auris {
namespace db {
namespace file {

class Generic
{
public:
	virtual void read(std::istream &is) = 0;
	virtual void write(std::ostream &os) = 0;

protected:
	template <typename N, typename V> void write_value(std::ostream &os, const N &name, V &value)
	{
		std::stringstream buffer;
		buffer << value;
		write_stream<N>(os, name, buffer);
	}

	template <typename N> void write_stream(std::ostream &os, const N &name, std::istream &value)
	{
		os << name << ": ";
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

	bool read_value(std::istream &is, std::string &name, std::string &value)
	{
		std::stringstream nb, vb;
		if (!read_stream(is, nb, vb))
			return false;
		name = nb.str();
		value = vb.str();
		return true;
	}

	bool read_stream(std::istream &is, std::string &name, std::ostream &value)
	{
		std::stringstream nb;
		if (!read_stream(is, nb, value))
			return false;
		name = nb.str();
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

	bool read_stream(std::istream &is, std::ostream &name, std::ostream &value)
	{
		if (!is.good() || is.peek() == '\n')
			return false;

		// Read into the name buffer, looking for the ":" separator.
		while (is.good()) {
			int c = is.get();
			if (c == ':') {
				if (is.peek() == ' ')
					is.get();
				break;
			}
			if (c == '\n') {
				throw std::runtime_error("line read is not a valid field (missing ':')");
			}
			name.put(c);
		}

		// Read into the value buffer.
		while (is.good()) {
			int c = is.get();
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

	void read_preamble(std::istream &is, const char *str)
	{
		const char *ptr = str;
		while (is.good()) {
			int c = is.get();
			if (*ptr == 0 && c == '\n')
				return;
			if (c != *ptr)
				break;
			ptr++;
		}
		std::stringstream s;
		s << "preamble of file not \"" << str << "\" as expected";
		throw std::runtime_error(s.str());
	}
};

} // namespace file
} // namespace db
} // namespace auris