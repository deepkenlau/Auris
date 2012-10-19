/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Entity.h"
#include "Field.h"


namespace database
{
	namespace database
	{
		class Song : public Entity
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

			Song(Table* t) : Entity(t)
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