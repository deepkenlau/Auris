/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Field.h"
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include "Entity.h"

using namespace database::database;
using std::runtime_error;
using std::string;


Field::operator string() const
{
	throw runtime_error("Cannot access field as String.");
}

Field::operator int() const
{
	throw runtime_error("Cannot access field as Integer.");
}

Field::operator double() const
{
	throw runtime_error("Cannot access field as Double.");
}

string Field::operator =(string v)
{
	throw runtime_error("Cannot access field as String.");
}

int Field::operator =(int v)
{
	throw runtime_error("Cannot access field as Integer.");
}

double Field::operator =(double v)
{
	throw runtime_error("Cannot access field as Double.");
}

namespace database
{
	namespace database
	{
		template<> enum Field::Type StringField::getType() const { return kString; }
		template<> void StringField::decode(const std::string &str)	{ value = str; }
		template<> bool StringField::isEmpty() const { return value.empty(); }

		template<> enum Field::Type IntegerField::getType() const { return kString; }
		template<> void IntegerField::decode(const std::string &str) { value = atoi(str.c_str()); }
	}
}