/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include "../../common/tinyxml2.h"

#define FIELD_ASSIGNMENT_OPERATOR(T) \
virtual operator T() const { return value; } \
virtual T operator =(T v) { value = v; return v; } \

namespace Database
{
	namespace Entry
	{
		class Entry;
		class Field
		{
		public:
			virtual void encode(tinyxml2::XMLPrinter &xml) const = 0;
			enum Type{kString, kInteger, kFloat, kCounter, kReference};
			virtual enum Type getType() const = 0;
			virtual operator std::string() const;
			virtual operator int() const;
			virtual operator double() const;
			virtual operator Entry *() const;
			virtual std::string operator =(std::string v);
			virtual int operator =(int v);
			virtual double operator =(double v);
			virtual Entry* operator =(Entry *v);
			virtual std::string describe() const = 0;
			virtual bool isEmpty() const = 0;
		};

		template <typename T> class ConcreteField : public Field
		{
		protected:
			T value;
		};

		class StringField : public ConcreteField<std::string>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(std::string)
			virtual bool isEmpty() const {return value.empty();}
			enum Type getType() const {return kString;}
			virtual std::string describe() const;
			virtual void encode(tinyxml2::XMLPrinter &xml) const { xml.PushText(value.c_str()); }
		};
		class IntegerField : public ConcreteField<int>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(int)
			virtual bool isEmpty() const {return !value;}
			enum Type getType() const {return kInteger;}
			std::string describe() const;
			virtual void encode(tinyxml2::XMLPrinter &xml) const { xml.PushText(value); }
		};
		class FloatField : public ConcreteField<double>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(double)
			virtual bool isEmpty() const {return !value;}
			enum Type getType() const {return kFloat;}
			std::string describe() const;
			virtual void encode(tinyxml2::XMLPrinter &xml) const { xml.PushText(value); }
		};
		class CounterField : public ConcreteField<int>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(int)
			virtual bool isEmpty() const {return !value;}
			enum Type getType() const {return kCounter;}
			std::string describe() const;
			virtual void encode(tinyxml2::XMLPrinter &xml) const { xml.PushText(value); }
		};
		class ReferenceField : public ConcreteField<Entry*>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(Entry*)
			virtual bool isEmpty() const {return !value;}
			enum Type getType() const {return kReference;}
			std::string describe() const;
			virtual void encode(tinyxml2::XMLPrinter &xml) const;
		};
	}
}