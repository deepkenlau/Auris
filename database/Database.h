/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include "Table.h"

namespace Database
{
	class Database
	{
		std::string path;
	public:
		Database(std::string path);
		Table songTable;
		Table albumTable;
		void store() const;
		void load();
	};
}