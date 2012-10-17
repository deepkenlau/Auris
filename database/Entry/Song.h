/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Entry.h"
#include "Field.h"


namespace Database
{
	namespace Entry
	{
		class Song : public Entry
		{
		public:
			StringField title;
			StringField artist;
			IntegerField length;
			StringField album;
			StringField publisher;
			StringField releaseDate;
			StringField dateAdded;
			StringField comments;
			StringField genre;

			Song() : Entry()
			{
				fields["title"]       = &title;
				fields["artist"]      = &artist;
				fields["length"]      = &length;
				fields["album"]       = &album;
				fields["publisher"]   = &publisher;
				fields["releaseDate"] = &releaseDate;
				fields["dateAdded"]   = &dateAdded;
				fields["comments"]    = &comments;
				fields["genre"]       = &genre;
			}
		};
	}
}