/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include <map>
#include <string>

namespace auris {
namespace db {

using std::string;
using std::map;

class Database;

class Track
{
public:
	Database& db;

	typedef map<string,string> StringMap;
	StringMap meta;
	StringMap media;
};

} // namespace db
} // namespace auris