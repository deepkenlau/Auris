/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include <gc_cpp.h>


class Error : public gc
{
public:
	Error(Error *underlying = NULL);
	virtual std::string describe() = 0;
	virtual std::string prefix() = 0;
	std::string describeAll();
	std::string what();
protected:
	Error* const underlying;
};

class GenericError : public Error
{
public:
	GenericError(std::string message, Error *underlying = NULL);
	virtual std::string describe();
	virtual std::string prefix() { return "Error: "; }
protected:
	std::string message;
};

class IOError : public Error
{
public:
	IOError(std::string path = "");
	virtual std::string describe();
	virtual std::string prefix() { return "IO error: "; }
protected:
	std::string path;
	std::string error;
};