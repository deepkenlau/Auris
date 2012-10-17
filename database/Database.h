/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

namespace Database
{
	class Database
	{
		std::string path;
	public:
		Database(std::string path);
		Table songTable;
		Table artistTable;
		void store() const;
		void load();
	};
}