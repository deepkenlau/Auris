/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include <boost/filesystem.hpp>

namespace auris {
namespace db {

namespace fs = boost::filesystem;

/**
 * @brief Reads and maintains structural information of a database.
 *
 * In simple terms, Structure is a helper class that assists in determining
 * paths for files, and creating and ensuring the directory structure of the
 * database.
 */
class Structure
{
public:
	fs::path dir;
	Structure() {}
	Structure(const fs::path& dir): dir(dir) {}

	struct Object
	{
		const fs::path path;
		const std::string hash;

		explicit Object(const fs::path &path, const std::string& hash): path(path), hash(hash) {}
		Object& prime() { fs::create_directories(path.parent_path()); return *this; }
		bool exists() const { return fs::exists(path); }
	};

	struct Ref
	{
		const fs::path path;
		const std::string name;

		explicit Ref(const fs::path &path, const std::string &name): path(path), name(name) {}
		Ref& prime() { fs::create_directories(path.parent_path()); return *this; }
		bool exists() const { return fs::exists(path); }
	};

	Object object(const std::string& hash) const
	{ return Object(dir/"objects"/hash.substr(0,2)/hash.substr(2), hash); }
	
	Ref ref(const std::string& name) const
	{ return Ref(dir/"refs"/name, name); }

	/**
	 * @brief Tries to resolve the given object name solely based on structure.
	 *
	 * Name expansion, the simplest form of name resolution, can be perform by
	 * only looking at the database structure. If an object with prefix 'in' is
	 * found, 'in' is replaced with the full name of the object.
	 *
	 * If there is an object with the exact same name, true is returned
	 * immediately.
	 *
	 * @return True if an object was found (whose name is now in 'in').
	 */
	bool maybe_resolve_name(std::string &in)
	{
		if (in.length() <= 2)
			return false;
		if (object(in).exists())
			return true;
		
		// Assuming that 'in' is an abbreviated hash, try to find a full one.
		fs::path search_dir = dir/"objects"/in.substr(0,2);
		if (fs::is_directory(search_dir)) {
			fs::directory_iterator it, end;
			for (it = fs::directory_iterator(search_dir); it != end; it++) {
				std::string n = (*it).path().filename().native();
				if (n.size() >= in.size()-2 && n.substr(0, in.size()-2) == in.substr(2)) {
					in.replace(2, std::string::npos, n);
					return true;
				}
			}
		}

		return false;
	}

	/**
	 * @brief Same as maybe_resolve_name(), but throws an exception on failure.
	 */
	std::string resolve_name(const std::string &in)
	{
		std::string s = in;
		if (!maybe_resolve_name(s)) {
			throw std::runtime_error("unknown object name '" + in + "'");
		}
		return s;
	}
};

} // namespace db
} // namespace auris