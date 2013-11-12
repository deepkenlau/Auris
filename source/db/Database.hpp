/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include "Track.hpp"
#include <boost/filesystem/path.hpp>
#include <list>

namespace auris {
namespace db {

using std::list;

class Database
{
public:
	typedef boost::filesystem::path Path;
	typedef list<Track> TrackList;

	const Path path;

	Database(const Path& p);
	~Database();

	void add();
	void remove();

	TrackList& getTrackList();

protected:
	TrackList trackList;
};

} // namespace db
} // namespace auris