/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include <vector>


class MimeType
{
public:
	typedef std::vector<std::string> Strings;

	static MimeType makeWithName(std::string name)
	{
		initRepository();
		for (MimeTypes::iterator im = repository.begin(); im != repository.end(); im++) {
			for (Strings::iterator in = (*im).names.begin(); in != (*im).names.end(); in++) {
				if (*in == name) return *im;
			}
		}
		return MimeType().addName(name);
	}

	static MimeType makeWithSuffix(std::string suffix)
	{
		initRepository();
		for (MimeTypes::iterator im = repository.begin(); im != repository.end(); im++) {
			for (Strings::iterator is = (*im).suffices.begin(); is != (*im).suffices.end(); is++) {
				if (*is == suffix) return *im;
			}
		}
		MimeType t = MimeType().addName("application/octet");
		if (!suffix.empty()) t.addSuffix(suffix);
		return t;
	}

	const std::string getName() { return names.empty() ? "application/octet" : names[0]; }
	const std::string getSuffix() { return suffices.empty() ? "" : suffices[0]; }

	const Strings& getNames() const { return names; }
	const Strings& getSuffices() const { return suffices; }

protected:
	typedef std::vector<MimeType> MimeTypes;
	static MimeTypes repository;
	static void initRepository();

	Strings names;
	Strings suffices;

	MimeType& addName(const std::string &n) { names.push_back(n); return *this; }
	MimeType& addSuffix(const std::string &s) { suffices.push_back(s); return *this; }
};