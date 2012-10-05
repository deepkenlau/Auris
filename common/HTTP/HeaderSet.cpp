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
	return str.str();
}

