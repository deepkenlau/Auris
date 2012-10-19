/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <string>
#include <sstream>


namespace database
{
	namespace database
	{
		class Entry;
		class Field
		{
		public:
			virtual void encode(std::stringstream &str) const = 0;
			virtual void decode(const std::string &str) = 0;

			enum Type { kString, kInteger, kFloat, kCounter };
			virtual enum Type getType() const = 0;

			virtual operator std::string() const;
			virtual operator int() const;
			virtual operator double() const;

			virtual std::string operator =(std::string v);
			virtual int operator =(int v);
			virtual double operator =(double v);

			virtual std::string describe() const { std::stringstream str; encode(str); return str.str(); }
			virtual bool isEmpty() const = 0;
			virtual void clear() = 0;
		};

		template <typename T> class ConcreteField : public Field
		{
		public:
			T operator =(T v) { value = v; return v; }
			operator T() const { return value; }

			enum Type getType() const;
			void encode(std::stringstream &str) const { str << value; }
			void decode(const std::string &str);
			bool isEmpty() const { return !value; }
			void clear() { value = 0; }

		protected:
			T value;
		};

		typedef ConcreteField<std::string> StringField;
		typedef ConcreteField<int> IntegerField;

		template<> bool StringField::isEmpty() const;
		template<> void StringField::clear();
		template<> void IntegerField::decode(const std::string &str);
	}
}