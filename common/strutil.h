/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <map>
#include <string>
#include <iostream>

class strutil
{
public:
	static std::string indent(std::string s)
	{
		size_t offset = 0;
		while ((offset = s.find('\n', offset)) != std::string::npos) {
			s.insert(offset+1, "\t");
			offset++;
		}
		return s;
	}

	typedef std::map<std::string, std::string> Dictionary;
	static Dictionary parseDictionary(const std::string &s)
	{
		Dictionary dict;
		size_t lineStart = 0, lineEnd = lineStart;
		do {
			lineEnd = s.find('\n', lineStart);
			if (lineEnd == std::string::npos)
				lineEnd = s.length();

			size_t colon = s.find(": ", lineStart);
			if (colon <= lineEnd) {
				dict[s.substr(lineStart, colon-lineStart)] = s.substr(colon+2, lineEnd-colon-2);
			}
			lineStart = lineEnd+1;
		} while (lineEnd < s.length());
		return dict;
	}
};