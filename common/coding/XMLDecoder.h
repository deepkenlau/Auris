/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include <ostream>
#include <sstream>
#include "Decoder.h"
#include "../tinyxml2.h"


namespace coding
{
	class XMLDecoder : public Decoder
	{
		tinyxml2::XMLDocument doc;
	public:
		void load(const std::string &input);

		bool getRootArray(Array *&v);
		bool getRootObject(Object *&v);

		class Object : public Decoder::Object
		{
			tinyxml2::XMLElement *element;
		public:
			Object(tinyxml2::XMLElement *e) : element(e) {}
			bool getArray(const std::string &key, Decoder::Array *&v);
			bool getObject(const std::string &key, Decoder::Object *&v);
			bool getValue(const std::string &key, std::string &v);
		};

		class Array : public Decoder::Array
		{
			tinyxml2::XMLElement *element;
			tinyxml2::XMLElement *current;
		public:
			Array(tinyxml2::XMLElement *e) : element(e) { current = element->FirstChildElement(); }
			bool getArray(Decoder::Array *&v);
			bool getObject(Decoder::Object *&v);
			bool getValue(std::string &v);
		};

	private:
		tinyxml2::XMLElement* getRootElement();
	};
}