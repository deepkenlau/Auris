/* Copyright © 2013 Fabian Schuiki */
#include "Database.hpp"

using auris::db::Database;


Database::Database(const Path& p): path(p)
{
}

Database::~Database()
{
}

Database::TrackList& Database::getTrackList()
{
	
	return trackList;
}