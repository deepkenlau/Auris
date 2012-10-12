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
	};
	class IntegerField : public ConcreteField<int>
	{
	public:
		enum Type getType() {return kInteger;}
	};
	class FloatField : public ConcreteField<double>
	{
	public:
		enum Type getType() {return kFloat;}
	};
	class CounterField : public ConcreteField<int>
	{
	public:
		enum Type getType() {return kCounter;}
	};
	class ReferenceField : public ConcreteField<Entry *>
	{
	public:
		enum Type getType() {return kReference;}
	};
}