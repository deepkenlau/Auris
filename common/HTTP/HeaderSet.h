/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <map>
#include <string>

namespace HTTP
{
	class HeaderSet
	{
		typedef std::map<std::string, std::string> Fields;
		Fields fields;
	public:
		void add(const std::string &field, const std::string &value);
		void remove(const std::string &field);
		bool has(const std::string &field) const;
		std::string get(const std::string &field) const;
		static HeaderSet* fromString(const std::string &str, unsigned int *consumed = NULL);
		std::string toString() const;

	};
}