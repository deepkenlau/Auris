/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>

class Blob : public gc
{
public:
	int length;
	const void *buffer;
	Blob() { this->length = 0; this->buffer = NULL; }
	Blob(const void * buffer, int length)
	{
		this->length = length;
		this->buffer = buffer;
	}
};