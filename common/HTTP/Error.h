/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "../Error.h"
#include <string>

namespace HTTP
{
	class Request;
	class Error : public ::Error
	{
	public:
		Error(int status, const std::string message, Request *request = NULL, ::Error *underlying = NULL);
		virtual std::string describe();
		virtual std::string prefix() { return "HTTP Error: "; }
	protected:
		int status;
		std::string message;
		Request *request;
	};

	#define HTTP_ERROR_CLASS(_name, _status, _prefix) \
	class _name : public Error \
	{ \
	public: \
		_name(const std::string message, Request *request, ::Error *underlying = NULL) \
		: Error(_status, message, request, underlying) {} \
		virtual std::string prefix() { return "HTTP " #_status " " _prefix ": "; } \
	}

	HTTP_ERROR_CLASS(BadRequestError, 400, "Bad Request");
	HTTP_ERROR_CLASS(NotFoundError, 404, "Not Found");

	/*class BadRequestError : public Error
	{
	public:
		BadRequestError(const std::string message, Request *request, ::Error *underlying = NULL)
		: Error(400, message, request, underlying);
		virtual std::string prefix() { return "HTTP Bad Request: "; }
	};

	class NotFoundError : public Error
	{
	public:
		NotFoundError(const std::string message, Request *request, ::Error *underlying = NULL)
		: Error(404, message, request, underlying);
		virtual std::string prefix() { return "HTTP Not Found: "; }
	}*/
}