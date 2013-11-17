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
 * @brief Reads a Generic file from disk.
 */
class ObjectReader
{
public:
	file::Generic &object;
	std::string hash;

	explicit ObjectReader(file::Generic &object): object(object) {}

	bool maybe_read(const char* path, const std::string& hash = "")
	{
		this->hash = hash;
		std::ifstream f(path);

		if (!f.good()) {
			return false;
		} else {
			object.read(f);
			return true;
		}
	}

	bool maybe_read(const fs::path &path, const std::string& hash = "") { return maybe_read(path.c_str(), hash); }
	bool maybe_read(Structure::Object dbso) { return maybe_read(dbso.path.c_str(), dbso.hash); }

	ObjectReader& read(const char* path, const std::string& hash = "")
	{
		if (!maybe_read(path, hash)) {
			throw std::runtime_error(std::string("object does not exist at ") + path);
		}
		return *this;
	}

	ObjectReader& read(const fs::path &path, const std::string& hash = "") { return read(path.c_str(), hash); }
	ObjectReader& read(Structure::Object dbso) { return read(dbso.path.c_str(), dbso.hash); }
};

/**
 * @brief Writes a Generic file to a buffer, calculates the hash and writes the
 * file to disk.
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

		return *this;
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