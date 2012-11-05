/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once


namespace coding
{
	class Value : public gc
	{
	public:
		enum Type { kScalar, kArray, kMap };
		virtual enum Type getType() const = 0;
	};

	class Scalar : public Value
	{
		
	public:
		enum Type getType() const { return kScalar; }

		enum DataType { kString, kInteger, kFloat, kBool };
		virtual enum DataType getDataType() const { return dataType; }
	};

	class Array : public Value
	{
	public:
		enum Type getType() const { return kArray; }
	};

	class Map : public Value
	{
	public:
		enum Type getType() const { return kMap; }
	};
}