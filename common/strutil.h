/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <map>
#include <string>
#include <iostream>

class strutil
{
public:
	static std::string indent(const std::string &s)
	{
		return s;
	}

	typedef std::map<std::string, std::string> Dictionary;
	static Dictionary parseDictionary(const std::string &s)
	{
		Dictionary dict;
		size_t lineStart = 0, lineEnd = lineStart;
		do {
			lineStart = lineEnd+1;
			lineEnd = s.find('\n', lineStart);
			if (lineEnd == std::string::npos)
				lineEnd = s.length();

			size_t colon = s.find(": ", lineStart);
			if (colon > lineEnd) continue;
			dict[s.substr(lineStart, colon-lineStart)] = s.substr(colon+2, lineEnd-colon-2);
		} while (lineEnd < s.length());
		return dict;
	}
};