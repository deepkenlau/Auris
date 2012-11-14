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

		int getStatus() const { return status; }
		const std::string& getMessage() const { return message; }
		Request* getRequest() const { return request; }
	protected:
		int status;
		std::string message;
		Request *request;
	};


	/* Define some HTTP errors that connections may catch and convert into an
	 * appropriate response. */
	#define HTTP_ERROR_CLASS(_name, _status, _prefix) \
	class _name : public Error \
	{ \
	public: \
		_name(const std::string message, Request *request = NULL, ::Error *underlying = NULL) \
		: Error(_status, message, request, underlying) {} \
		virtual std::string prefix() { return "HTTP " _prefix ": "; } \
	};

	HTTP_ERROR_CLASS(BadRequestError, 400, "Bad Request")
	HTTP_ERROR_CLASS(NotFoundError, 404, "Not Found")
	HTTP_ERROR_CLASS(MethodNotAllowedError, 405, "Method Not Allowed")
}