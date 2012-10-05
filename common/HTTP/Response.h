/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "HeaderSet.h"
#include <string>

namespace HTTP{
	class Response
	{
	public:
		HeaderSet headers;
		std::string content;
		Response();
		static Response* fromString(const std::string &str, unsigned int *consumed = NULL);
		std::string toString() const;
	};
}