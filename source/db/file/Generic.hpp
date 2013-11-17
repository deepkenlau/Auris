/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include <iostream>
#include <sstream>

namespace auris {
namespace db {
namespace file {

class Generic
{
public:
	virtual void read(std::istream &is) = 0;
	virtual void write(std::ostream &os) = 0;
};

} // namespace file
} // namespace db
} // namespace auris