/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include "sha1.h"

namespace sha1
{
	std::string calc(const std::string &in)
	{
		unsigned char hash[20];
		char hexstring[41];
		sha1::calc(in.c_str(), in.length(), hash); // 10 is the length of the string
		sha1::toHexString(hash, hexstring);
		return hexstring;
	}
}