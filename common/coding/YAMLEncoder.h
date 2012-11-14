/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Encoder.h"
#include <stack>


namespace coding
{
	class YAMLEncoder : public Encoder
	{
	public:
		YAMLEncoder();

		void add(const std::string &value, const std::string &key = "");
		void push(enum CollectionType type, const std::string &key = "");
		void pop(enum CollectionType type);

		void finalize();

	private:
		std::stack<enum CollectionType> collections;

		void insertKey(const std::string &key);
	};
}