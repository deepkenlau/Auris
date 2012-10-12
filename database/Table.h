/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

namespace Database
{
	class Entry;
	class Table
	{
		std::set<Entry *> entries;
	};
}