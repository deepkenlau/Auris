/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "../Error.h"


namespace coding
{
	class Error : public GenericError
	{
	public:
		Error(std::string message, Error *underlying = NULL) : GenericError(message, underlying) {}
		virtual std::string prefix() { return "Coding Error: "; }
	};
}