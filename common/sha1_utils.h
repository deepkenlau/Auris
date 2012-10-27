/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include "sha1.h"

namespace sha1
{
	std::string calc(const std::string &in);
}