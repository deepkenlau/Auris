/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include "HeaderSet.h"

namespace HTTP
{
	class Request
	{
	public:
		enum Type {
			kGET,
			kPOST,
			kPUT,
			kDELETE
		};

		enum Type type;
		std::string path;
		HeaderSet headers;
		std::string content;

		Request();

		static Request* fromString(const std::string &str, unsigned int *consumed = NULL);
		std::string toString() const;
	};
}