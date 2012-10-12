/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once

namespace Database
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
		virtual operator =(std::string v);
		virtual operator =(int v);
		virtual operator =(double v);
		virtual operator =(Entry * v);
		virtual std::string describe() const;
	};

	template <typename T> class ConcreteField : public Field
	{
	protected:
		T value;
	public:
		virtual operator T() const {return value;}
		virtual operator =(T v) {value = v;}
	};

	class StringField : public ConcreteField<std::string>
	{
	public:
		enum Type getType() {return kString;}
		std::string describe() const;
	};
	class IntegerField : public ConcreteField<int>
	{
	public:
		enum Type getType() {return kInteger;}
		std::string describe() const;
	};
	class FloatField : public ConcreteField<double>
	{
	public:
		enum Type getType() {return kFloat;}
		std::string describe() const;
	};
	class CounterField : public ConcreteField<int>
	{
	public:
		enum Type getType() {return kCounter;}
		std::string describe() const;
	};
	class ReferenceField : public ConcreteField<Entry *>
	{
	public:
		enum Type getType() {return kReference;}
		std::string describe() const;
	};
}