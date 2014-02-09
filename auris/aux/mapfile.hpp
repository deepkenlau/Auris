/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include "trim.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <stdexcept>

namespace auris {
namespace aux {

/**
 * @brief Functions for reading and writing files as a whole.
 */
class mapfile
{
public:
	static bool maybe_read(const char* path, std::ostream &os)
	{
		std::ifstream is(path);
		if (!is.good())
			return false;
		std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(os));
		return true;
	}

	static bool maybe_read(const char* path, std::string &str)
	{
		std::ifstream is(path);
		if (!is.good())
			return false;
		str.assign(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>());
		trim(str, "\n\t\r ");
		return true;
	}

	static void read(const char* path, std::ostream &os)
	{
		if (!maybe_read(path, os))
			throw std::runtime_error("unable to open file for reading");
	}

	static void read(const char* path, std::string &str)
	{
		if (!maybe_read(path, str))
			throw std::runtime_error("unable to open file for reading");
	}


	static bool maybe_write(const char* path, std::istream &is)
	{
		std::ofstream os(path);
		if (!os.good())
			return false;
		std::copy(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(os));
		return true;
	}

	static bool maybe_write(const char* path, const std::string &str)
	{
		std::stringstream is(*(str.end()-1) == '\n' ? str : str + '\n');
		return maybe_write(path, is);
	}

	static void write(const char* path, std::istream &is)
	{
		if (!maybe_write(path, is))
			throw std::runtime_error("unable to open file for writing");
	}

	static void write(const char* path, const std::string &str)
	{
		if (!maybe_write(path, str))
			throw std::runtime_error("unable to open file for writing");
	}
};

} // namespace aux
} // namespace auris