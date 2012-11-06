/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include <ostream>
#include <sstream>
#include "Error.h"


namespace coding
{
	class Decoder : public gc
	{
	public:
		class Array;
		class Object;

		static Decoder* make(const std::string &mimeType);
		static Decoder* make(const std::string &input, const std::string &mimeType);
		static Decoder* make(std::istream &input, const std::string &mimeType);

		virtual void load(const std::string &input) = 0;

		virtual bool getRootArray(Array *&v) = 0;
		virtual bool getRootObject(Object *&v) = 0;

		Array* getRootArray() {
			Array *v;
			if (!getRootArray(v)) throw new Error("Root array does not exist.");
			return v;
		}
		Object* getRootObject() {
			Object *v;
			if (!getRootObject(v)) throw new Error("Root array does not exist.");
			return v;
		}


		class Object
		{
		public:
			virtual bool getArray(const std::string &key, Array *&v) = 0;
			virtual bool getObject(const std::string &key, Object *&v) = 0;
			virtual bool getValue(const std::string &key, std::string &v) = 0;
			virtual bool getValue(const std::string &key, int &v);
			virtual bool getValue(const std::string &key, double &v);
			virtual bool getValue(const std::string &key, bool &v);

			Array* getArray(const std::string &key) {
				Array *v;
				if (!getArray(key, v)) throw new Error("Array for key '" + key + "' does not exist.");
				return v;
			}
			Object* getObject(const std::string &key) {
				Object *v;
				if (!getObject(key, v)) throw new Error("Object for key '" + key + "' does not exist.");
				return v;
			}
			template <typename T> T getValue(const std::string &key) {
				T v;
				if (!getValue(key, v)) throw new Error("Value for key '" + key + "' does not exist.");
				return v;
			}
		};

		class Array
		{
		public:
			virtual bool getArray(Array *&v) = 0;
			virtual bool getObject(Object *&v) = 0;
			virtual bool getValue(std::string &v) = 0;
			virtual bool getValue(int &v);
			virtual bool getValue(double &v);
			virtual bool getValue(bool &v);

			Array* getArray() {
				Array *v;
				if (!getArray(v)) throw new Error("Array does not exist.");
				return v;
			}
			Object* getObject() {
				Object *v;
				if (!getObject(v)) throw new Error("Object does not exist.");
				return v;
			}
			template <typename T> T getValue() {
				T v;
				if (!getValue(v)) {
					std::stringstream str;
					str << "Value does not exist.";
					throw new Error(str.str());
				}
				return v;
			}
		};
	};
}