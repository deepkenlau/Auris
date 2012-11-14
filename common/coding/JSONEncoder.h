/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Encoder.h"
#include <stack>


namespace coding
{
	class JSONEncoder : public Encoder
	{
	public:
		JSONEncoder();

		void add(int value, const std::string &key = "");
		void add(float value, const std::string &key = "");
		void add(bool value, const std::string &key = "");
		void add(const std::string &value, const std::string &key = "");
		void push(enum CollectionType type, const std::string &key = "");
		void pop(enum CollectionType type);

		void finalize();

	private:
		struct Collection {
			enum CollectionType type;
			bool empty;
			Collection(enum CollectionType t) { type = t; empty = true; }
		};
		std::stack<Collection> collections;
		void insertKey(const std::string &key);
	};
}