#pragma once
#include "blob.h"
#include <string>

std::string md5(Blob * data);
std::string md5(const char * data, unsigned int length);
