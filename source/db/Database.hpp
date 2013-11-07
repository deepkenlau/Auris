/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include <boost/filesystem/path.hpp>

namespace auris {
namespace db {

class Database
{
public:
	typedef boost::filesystem::path Path;
	const Path path;

	Database(const Path& p);
	~Database();

	void add();
	void remove();

	
};

} // namespace db
} // namespace auris