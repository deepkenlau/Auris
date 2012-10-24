/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Song.h"

using database::library::Song;
using std::set;
using std::string;


Song::Song(Library *l, std::string id) : library(l)
{
	this->id = id;
}

database::database::Song* Song::getMetadata() const
{
	database::Song *md = library->getMetadataDatabase()->getSongs()->getEntity(id);
	if (!md) {
		throw new Error(string("No metadata available for song ") + id + ".");
	}
	return md;
}

set<string> Song::getFormats() const
{
	return library->getMediaStore()->getFormats(id);
}