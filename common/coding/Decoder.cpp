/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Decoder.h"
#include "XMLDecoder.h"

using std::string;
using std::istream;
using coding::Decoder;


Decoder* Decoder::make(const string &mimeType)
{
	if (mimeType == "text/xml") return new XMLDecoder();
	throw new Error("No decoder exists for MIME type " + mimeType + ".");
}

Decoder* Decoder::make(const string &input, const string &mimeType)
{
	Decoder *d = make(mimeType);
	d->load(input);
	return d;
}

Decoder* Decoder::make(istream &input, const string &mimeType)
{
	if (!input.good()) {
		throw new Error("Input stream passed to decoder factory is not good().");
	}
	Decoder *d = make(mimeType);
	d->load(string((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>()));
	return d;
}


bool Decoder::Object::getValue(const string &key, int &v)
{
	string sv;
	if (!getValue(key, sv)) return false;
	v = atoi(sv.c_str());
	return true;
}
bool Decoder::Object::getValue(const string &key, double &v)
{
	string sv;
	if (!getValue(key, sv)) return false;
	v = atof(sv.c_str());
	return true;
}
bool Decoder::Object::getValue(const string &key, bool &v)
{
	string sv;
	if (!getValue(key, sv)) return false;
	int i = atoi(sv.c_str());
	v = (sv == "true" || i);
	return true;
}


bool Decoder::Array::getValue(int &v)
{
	string sv;
	if (!getValue(sv)) return false;
	v = atoi(sv.c_str());
	return true;
}
bool Decoder::Array::getValue(double &v)
{
	string sv;
	if (!getValue(sv)) return false;
	v = atof(sv.c_str());
	return true;
}
bool Decoder::Array::getValue(bool &v)
{
	string sv;
	if (!getValue(sv)) return false;
	int i = atoi(sv.c_str());
	v = (sv == "true" || i);
	return true;
}