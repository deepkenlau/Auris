/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "HeaderSet.h"
#include <iostream>
#include <sstream>
using namespace HTTP;

void HeaderSet::add(const std::string &field, const std::string &value)
{
	fields[field] = value;
}

void HeaderSet::remove(const std::string &field)
{
	fields.erase(field);
}

bool HeaderSet::has(const std::string &field) const
{
	return fields.count(field);
}

std::string HeaderSet::get(const std::string &field) const
{
	Fields::const_iterator f = fields.find(field);
	if (f == fields.end()) return "";
	return f->second;
}

HeaderSet* HeaderSet::fromString(const std::string &str, unsigned int *consumed)
{
	size_t pos = str.find("\r\n\r\n");
	if(pos == std::string::npos) return NULL;
	std::string substr = str.substr(0, pos+4);
	size_t oldpos = 0;
	size_t colonpos = 0;
	while(true)
	{
		pos = substr.find("\r\n", oldpos);
		if(pos == std::string::npos) break;
		colonpos = substr.find(": ",oldpos);
		if (colonpos != std::string::npos) {
			std::string field = substr.substr(oldpos,colonpos-oldpos);
			std::string value = substr.substr(colonpos+2,pos-colonpos-2);
			std::cout << "parsing " << field << ": \"" << value << "\"\n";
			//fields[substr.substr(oldpos,colonpos)] = substr.substr(colonpos+2,pos);
		}
		oldpos = pos+2;
	}
}

std::string HeaderSet::toString() const
{
	std::stringstream str;
	for(Fields::const_iterator it = fields.begin(); it!=fields.end(); it++)
	{
		str << it->first;
		str << ": ";
		str << it->second;
		str << "\r\n";
	}
	str << "\r\n";
	return str.str();
}

