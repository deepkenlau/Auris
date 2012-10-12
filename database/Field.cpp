/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Field.h"
#include <stdexcept.h>

using namespace Database;
using std::runtime_error;

Field::operator std::string() const
{
	throw new runtime_error("Cannot access field as String.");
}

Field::operator int() const
{
	throw new runtime_error("Cannot access field as Integer.");
}

Field::operator double() const
{
	throw new runtime_error("Cannot access field as Double.");
}

Field::operator Entry *() const
{
	throw new runtime_error("Cannot access field as Reference.");
}

std::string Field::operator =(std::string v)
{
	throw new runtime_error("Cannot access field as String.");
}

int Field::operator =(int v)
{
	throw new runtime_error("Cannot access field as Integer.");
}

double Field::operator =(double v)
{
	throw new runtime_error("Cannot access field as Double.");
}

Entry* Field::operator =(Entry *v)
{
	throw new runtime_error("Cannot access field as Reference.");
}

std::string ConcreteField<std::string>::describe() const
{
	return v;
}

std::string ConcreteField<int>::describe() const
{
	std::stringstream s;
	s << v;
	return s.str();
}

std::string ConcreteField<double>::describe() const
{
	std::stringstream s;
	s << v;
	return s.str();
}

std::string ConcreteField<Entry *>::describe() const
{
	if (v == NULL) return "NULL";
	return v->getID();
}