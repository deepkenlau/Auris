/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include "Encoder.h"
#include <stack>


namespace coding
{
	class XMLEncoder : public Encoder
	{
	public:
		XMLEncoder();

		void add(const std::string &value, const std::string &key = "");
		void push(enum CollectionType type, const std::string &key = "");
		void pop(enum CollectionType type);

		void finalize();

	private:
		struct Collection {
			enum CollectionType type;
			std::string name;
			std::string attrs;
			std::string body;
			Collection(enum CollectionType t, std::string n) : type(t), name(n) {}
		};
		std::stack<Collection> collections;

		static std::string escape(std::string s);

		void ensureCollectionType(enum CollectionType type);
		std::string finalizeCollection(Collection &c);
	};
}