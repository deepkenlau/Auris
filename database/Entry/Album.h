/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Field.h"
#include "Entry.h"

namespace Database
{
	namespace Entry
	{
		class Album : public Entry
		{
		public:
			StringField title;
			StringField artist;
			IntegerField length;
			StringField releaseDate;
			StringField dateAdded;
			StringField publisher;
			StringField comments;
			StringField genre;

			Album() : Entry()
			{
				fields["title"] = &title;
				fields["artist"] = &artist;
				fields["length"] = &length;
				fields["releaseDate"] = &releaseDate;
				fields["dateAdded"] = &dateAdded;
				fields["publisher"] = &publisher;
				fields["comments"] = &comments;
				fields["genre"] = &genre;
			}
		};
	}
}