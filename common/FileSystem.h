/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

class Path;

namespace FileSystem
{
	bool fileExists(const Path &p);
	void makeDirectory(const Path &dir);
};