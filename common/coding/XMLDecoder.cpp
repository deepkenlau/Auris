/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "XMLDecoder.h"
#include <common/tinyxml2.h>
#include <iostream>

using coding::XMLDecoder;
using std::string;


void XMLDecoder::load(const string &input)
{
    doc.Parse(input.c_str());
    if (doc.Error()) {
    	string msg("Unable to parse XML.");
    	if (const char *s = doc.GetErrorStr1()) { msg += " "; msg += s; msg += "."; }
    	if (const char *s = doc.GetErrorStr2()) { msg += " "; msg += s; msg += "."; }
    	throw new Error(msg);
    }
}


tinyxml2::XMLElement* XMLDecoder::getRootElement()
{
	tinyxml2::XMLElement *e = doc.RootElement();
	if (!e) {
		throw new Error("XML does not have a root element.");
	}
	return e;
}


bool XMLDecoder::getRootArray(Decoder::Array *&v)
{
	tinyxml2::XMLElement *e = getRootElement();
	v = new Array(e);
	return true;
}

bool XMLDecoder::getRootObject(Decoder::Object *&v)
{
	tinyxml2::XMLElement *e = getRootElement();
	v = new Object(e);
	return true;
}


bool XMLDecoder::Object::getArray(const std::string &key, Decoder::Array *&v)
{
	tinyxml2::XMLElement *e = element->FirstChildElement(key.c_str());
	if (!e) return false;
	v = new Array(e);
	return true;
}
bool XMLDecoder::Object::getObject(const std::string &key, Decoder::Object *&v)
{
	tinyxml2::XMLElement *e = element->FirstChildElement(key.c_str());
	if (!e) return false;
	v = new Object(e);
	return true;
}
bool XMLDecoder::Object::getValue(const std::string &key, std::string &v)
{
	const char *s = element->Attribute(key.c_str());
	if (!s) return false;
	v = s;
	return true;
}


XMLDecoder::Array::Array(tinyxml2::XMLElement *e)
{
	element = e;
	current = e->FirstChildElement();
}

bool XMLDecoder::Array::getArray(Decoder::Array *&v)
{
	if (!current) return false;
	v = new Array(current);
	current = current->NextSiblingElement();
	return true;
}
bool XMLDecoder::Array::getObject(Decoder::Object *&v)
{
	if (!current) return false;
	v = new Object(current);
	current = current->NextSiblingElement();
	return true;
}
bool XMLDecoder::Array::getValue(std::string &v)
{
	if (!current) return false;
	v = current->GetText();
	current = current->NextSiblingElement();
	return true;
}