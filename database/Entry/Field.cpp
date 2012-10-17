/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Field.h"
#include <stdexcept>
#include <sstream>
#include "Entry.h"

using namespace Database::Entry;
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

std::string StringField::describe() const
{
	return value;
}

std::string IntegerField::describe() const
{
	std::stringstream s;
	s << value;
	return s.str();
}

std::string FloatField::describe() const
{
	std::stringstream s;
	s << value;
	return s.str();
}

std::string ReferenceField::describe() const
{
	if (value == NULL) return "NULL";
	return value->getID();
}