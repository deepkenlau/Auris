/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <common/Error.h>


namespace database
{
	namespace library
	{
		class Error : public GenericError
		{
		public:
			Error(std::string message, ::Error *underlying = NULL) : GenericError(message, underlying) {}
			virtual std::string prefix() { return "Library error: "; }
		};
	}
}