/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "HeaderSet.h"
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
	if(pos == str::string::npos) return NULL;
	std::string substr = str.substr(0, pos+4);
	int oldpos = 0;
	pos == substr.find("\r\n");
	do
	{
		pos = substr.find
	} while(pos != std::string::npos)
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

