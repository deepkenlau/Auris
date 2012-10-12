/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

namespace Database
{
	class Entry
	{
		std::map<std::string, Field *> fields;
		std::string id;
	};
}