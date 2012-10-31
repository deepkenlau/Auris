/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <gc_cpp.h>
#include <string>
#include <ostream>
#include <sstream>


namespace coding
{
	class Encoder : public gc
	{
	public:
		static Encoder* makeForSuffix(const std::string &suffix, std::ostream *output = NULL);
		Encoder();

		enum CollectionType {
			kUndecided = 0,
			kArray,
			kObject
		};

		virtual void add(const char *value, const std::string &key = "");
		virtual void add(int value, const std::string &key = "");
		virtual void add(float value, const std::string &key = "");
		virtual void add(bool value, const std::string &key = "");
		virtual void add(const std::string &value, const std::string &key = "") = 0;
		virtual void push(enum CollectionType type, const std::string &key = "") = 0;
		virtual void pop(enum CollectionType type) = 0;

		void pushArray(const std::string &key = "") { push(kArray, key); }
		void pushObject(const std::string &key = "") { push(kObject, key); }

		void popArray() { pop(kArray); }
		void popObject() { pop(kObject); }

		virtual void finalize() {};

		std::string getOutputString();

	protected:
		std::ostream *output;
		std::ostringstream *strstream;

		void setOutput(std::ostream *output);
	};
}