/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include <ostream>
#include <sstream>


namespace coding
{
	class Decoder : public gc
	{
	public:
		static Decoder* makeForInput(std::istream *input, const std::string &type);

		class Array;
		class Object;

		class Object
		{
		public:
			virtual bool getArray(const std::string &key, Array &v) = 0;
			virtual bool getObject(const std::string &key, Object &v) = 0;
			virtual bool getValue(const std::string &key, std::string &v) = 0;
			virtual bool getValue(const std::string &key, int &v);
			virtual bool getValue(const std::string &key, double &v);
			virtual bool getValue(const std::string &key, bool &v);

			virtual Array getArray(const std::string &key);
			virtual Object getObject(const std::string &key);
			virtual std::string getValue(const std::string &key);
			virtual int getValue(const std::string &key);
			virtual double getValue(const std::string &key);
			virtual bool getValue(const std::string &key);
		};

		class Array
		{
		public:
			virtual bool getArray(int index, Array &v) = 0;
			virtual bool getObject(int index, Object &v) = 0;
			virtual bool getValue(int index, std::string &v) = 0;
			virtual bool getValue(int index, int &v);
			virtual bool getValue(int index, double &v);
			virtual bool getValue(int index, bool &v);

			virtual Array getArray(int index);
			virtual Object getObject(int index);
			virtual std::string getValue(int index);
			virtual int getValue(int index);
			virtual double getValue(int index);
			virtual double getValue(int index);
		};
	};
}