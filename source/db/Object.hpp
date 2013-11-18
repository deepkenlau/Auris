/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include "file/Generic.hpp"
#include "Structure.hpp"
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
		return *this;
	}

	ObjectWriter& write(const fs::path &path) { return write(path.c_str()); }
	ObjectWriter& write(Structure::Object dbso) { return write(dbso.prime().path.c_str()); }
	ObjectWriter& write(const Structure &dbs) { fill_buffer(); return write(dbs.object(hash)); }

private:
	std::stringstream internal_buffer; // in case the constructor does not provide a buffer
};

/**
 * @brief Reads and writes Generic file T.
 */
template <class T> class ObjectBuffer : public T
{
public:
	typedef ObjectBuffer<T> self;
	const Structure &dbs;
	std::string maintain_ref;
	std::string hash_in, hash_out;

	ObjectBuffer(const Structure &dbs):
		dbs(dbs), reader(*this), writer(*this) {}
	ObjectBuffer(const Structure &dbs, std::stringstream &write_buffer):
		dbs(dbs), reader(*this), writer(*this, write_buffer) {}


	bool maybe_read(const std::string &hash)
	{
		assert_unread();
		hash_in = hash;
		return reader.maybe_read(dbs.object(hash));
	}

	self& read(const std::string &hash)
	{
		assert_unread();
		hash_in = hash;
		reader.read(dbs.object(hash));
		return *this;
	}

	bool maybe_ref(const std::string &ref)
	{
		maintain_ref = ref;
		std::string hash;
		if (!aux::mapfile::maybe_read(dbs.ref(ref).path.c_str(), hash))
			return false;
		read(hash);
		return true;
	}

	self& ref(const std::string &ref)
	{
		assert_unread();
		maintain_ref = ref;
		std::string hash;
		aux::mapfile::read(dbs.ref(ref).path.c_str(), hash);
		read(hash);
		return *this;
	}

	self& fill_buffer()
	{
		writer.fill_buffer();
		hash_out = writer.hash;
		return *this;
	}

	self& write()
	{
		assert_unwritten();
		fill_buffer(); // won't do anything if already done so
		if (hash_in == hash_out)
			return *this; // equal hashes means no changes, so no need to write anything
		writer.write(dbs);
		if (!maintain_ref.empty())
			aux::mapfile::write(dbs.ref(maintain_ref).prime().path.c_str(), writer.hash);
		return *this;
	}

private:
	ObjectReader reader;
	ObjectWriter writer;

	void assert_unread()
	{
		if (!hash_in.empty()) {
			throw std::runtime_error("object has already been read from " + hash_in);
		}
	}

	void assert_unwritten()
	{
		if (!writer.buffer.good()) {
			throw std::runtime_error("object has already been written to " + hash_out);
		}
	}
};

} // namespace db
} // namespace auris
