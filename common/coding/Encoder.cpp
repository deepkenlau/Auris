/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Encoder.h"
#include "Error.h"
#include "YAMLEncoder.h"
#include "JSONEncoder.h"

using coding::Encoder;
using std::string;
using std::ostringstream;
using std::ostream;


Encoder* Encoder::makeForSuffix(const string &suffix, std::ostream *output)
{
	Encoder *e = NULL;
	if (suffix == "yml") e = new YAMLEncoder();
	if (suffix == "json") e = new JSONEncoder();
	if (e) {
		e->setOutput(output);
	} else {
		throw new Error("No encoder available for suffix '" + suffix + "'.");
	}
	return e;
}

Encoder::Encoder()
{
	output = NULL;
	strstream = NULL;
}


void Encoder::add(const char *value, const std::string &key)
{
	add(string(value), key);
}

void Encoder::add(int value, const std::string &key)
{
	ostringstream s;
	s << value;
	add(s.str(), key);
}

void Encoder::add(float value, const std::string &key)
{
	ostringstream s;
	s << value;
	add(s.str(), key);
}

void Encoder::add(bool value, const std::string &key)
{
	add(string(value ? "true" : "false"), key);
}


void Encoder::setOutput(ostream *output)
{
	if (output) {
		this->output = output;
	} else {
		this->strstream = new ostringstream;
		this->output = this->strstream;
	}
}

string Encoder::getOutputString()
{
	if (!strstream) {
		throw new GenericError("Trying to get output string of encoder that writes to custom stream.");
	}
	return strstream->str();
}