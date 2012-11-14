/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "JSONEncoder.h"
#include "Error.h"

using coding::JSONEncoder;


JSONEncoder::JSONEncoder() : Encoder()
{
}


void JSONEncoder::add(int value, const std::string &key)
{
	insertKey(key);
	*output << value;
}

void JSONEncoder::add(float value, const std::string &key)
{
	insertKey(key);
	*output << value;
}

void JSONEncoder::add(bool value, const std::string &key)
{
	insertKey(key);
	*output << (value ? "true" : "false");
}

void JSONEncoder::add(const std::string &value, const std::string &key)
{
	insertKey(key);
	*output << "\"" << value << "\"";
}


void JSONEncoder::push(enum CollectionType type, const std::string &key)
{
	insertKey(key);
	*output << (type == kArray ? "[" : "{");
	collections.push(Collection(type));
}

void JSONEncoder::pop(enum CollectionType type)
{
	if (collections.size() <= 1) {
		throw new Error("Trying to pop root collection which is not allowed.");
	} else if (collections.top().type != type) {
		throw new Error(type == kObject ? 
			"Trying to pop an object with an array or no collection pushed." :
			"Trying to pop an array with an object or no collection pushed.");
	}
	*output << (collections.top().type == kArray ? "]" : "}");
	collections.pop();
}


void JSONEncoder::finalize()
{
	if (collections.size() > 1) {
		throw new Error("Not all collections have been popped.");
	}
	if (collections.size() == 1) {
		*output << (collections.top().type == kArray ? "]" : "}");
	}
}


void JSONEncoder::insertKey(const std::string &key)
{
	enum CollectionType t = (key.empty() ? kArray : kObject);

	//Insert the initial collection if required.
	if (collections.size() == 0) {
		*output << (t == kArray ? "[" : "{");
		collections.push(Collection(t));
	}

	//Make sure the collection type matches.
	if (collections.top().type != t) {
		throw new Error(t == kObject
			? "Trying to add a value/collection with a key to an array."
			: "Trying to add a value/collection to an object without a key."
		);
	}

	//Insert the key.
	if (!collections.top().empty) {
		*output << ", ";
	} else {
		collections.top().empty = false;
	}
	if (!key.empty()) {
		*output << "\"" << key << "\": ";
	}
}