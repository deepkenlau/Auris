/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>

#define FIELD_ASSIGNMENT_OPERATOR(T) \
virtual operator T() const { return value; } \
virtual T operator =(T v) { value = v; return v; }

namespace Database
{
	namespace Entry
	{
		class Entry;
		class Field
		{
		public:
			virtual void serialize(tinyxml2::XMLPrinter &xml) const = 0;
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
		};

		template <typename T> class ConcreteField : public Field
		{
		public:
			void serialize(tinyxml2::XMLPrinter &xml) const
			{
				xml.PushText(value);
			}
		protected:
			T value;
		};

		class StringField : public ConcreteField<std::string>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(std::string)

			enum Type getType() const {return kString;}
			virtual std::string describe() const;
		};
		class IntegerField : public ConcreteField<int>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(int)

			enum Type getType() const {return kInteger;}
			std::string describe() const;
		};
		class FloatField : public ConcreteField<double>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(double)

			enum Type getType() const {return kFloat;}
			std::string describe() const;
		};
		class CounterField : public ConcreteField<int>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(int)

			enum Type getType() const {return kCounter;}
			std::string describe() const;
		};
		class ReferenceField : public ConcreteField<Entry*>
		{
		public:
			FIELD_ASSIGNMENT_OPERATOR(Entry*)

			enum Type getType() const {return kReference;}
			std::string describe() const;
		};
	}
}