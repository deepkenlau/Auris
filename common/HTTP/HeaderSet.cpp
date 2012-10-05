/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "HeaderSet.h"

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
	return fields.count[field];
}

std::string get(const std::string &field) const
{
	return fields[field];
}

static HeaderSet* HeaderSet::fromString(const std::string &str, unsigned int *consumed = NULL);
{
	size_t pos = str.find("\r\n\r\n");
	if(pos == str::string::npos) return NULL;
	std::string substr = str.substr(0, pos+4);
	size_t oldpos = 0;
	size_t colonpos = 0;
	do
	{
		pos = substr.find("\r\n", oldpos);
		colonpos = substr.find(": ",oldpos);
		if(colonpos != std::sting::npos)
			fields[substr.substr(oldpos,colonpos)] = substr.substr(colonpos+2,pos);
		oldpos = pos+2;
	} while(pos != std::string::npos)
}

std::string HeaderSet::toString() const
{
	std::stringstream str;
	for(Fields::const_iterator it = fields.cbegin(); it!=fields.cend(); it++)
	{
		str << it->first;
		str << ": ";
		str << it->second;
		str << "\r\n";
	}
	str << "\r\n";
	return str.str();
}

