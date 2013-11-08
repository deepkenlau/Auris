/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include "Track.hpp"
#include <list>

namespace auris {
namespace db {

class Database;

class Version
{
public:
	typedef list<Track> TrackList;

	Database& db;
	const string hash;

	TrackList& getTrackList();

protected:
	TrackList trackList;
};

} // namespace db
} // namespace auris