/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include "file/Generic.hpp"
#include <common/sha1.hpp>
#include <aux/mapfile.hpp>
#include <string>
#include <sstream>

namespace auris {
namespace db {

/**
 * @brief Writes a Generic file to a buffer and calculates the hash.
 */
class ObjectWriter
{
public:
	file::Generic &object;
	std::stringstream &buffer;
	std::string hash;

	explicit ObjectWriter(file::Generic &object): object(object), buffer(internal_buffer) {}
	explicit ObjectWriter(file::Generic &object, std::stringstream &buffer): object(object), buffer(buffer) {}

	ObjectWriter& fill_buffer()
	{
		if (!hash.empty())
			return *this;

		object.write(buffer);
		hash = sha1().from_stream(buffer).hex();
		
		buffer.clear();
		buffer.seekg(0);
	}

	ObjectWriter& write(const char* path)
	{
		fill_buffer(); // fill the buffer if not already done
		aux::mapfile::write(path, buffer);
	}

	ObjectWriter& write(const fs::path &path) { return write(path.c_str()); }
	ObjectWriter& write(Structure::Object dbso) { return write(dbso.prime().path.c_str()); }
	ObjectWriter& write(const Structure &dbs) { fill_buffer(); return write(dbs.object(hash)); }

private:
	std::stringstream internal_buffer; // in case the constructor does not provide a buffer
};

} // namespace db
} // namespace auris