/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "YAMLEncoder.h"
#include "Error.h"

using coding::YAMLEncoder;


YAMLEncoder::YAMLEncoder() : Encoder()
{
}


void YAMLEncoder::add(const std::string &value, const std::string &key)
{
	insertKey(key);
	*output << " " << value << "\n";
}


void YAMLEncoder::push(enum CollectionType type, const std::string &key)
{
	insertKey(key);
	*output << "\n";
	collections.push(type);
}

void YAMLEncoder::pop(enum CollectionType type)
{
	if (collections.size() <= 1) {
		throw new Error("Trying to pop root collection which is not allowed.");
	} else if (collections.top() != type) {
		throw new Error(type == kObject ? 
			"Trying to pop an object with an array or no collection pushed." :
			"Trying to pop an array with an object or no collection pushed.");
	}
	collections.pop();
}


void YAMLEncoder::finalize()
{
	if (collections.size() > 1) {
		throw new Error("Not all collections have been popped.");
	}
}


void YAMLEncoder::insertKey(const std::string &key)
{
	enum CollectionType t = (key.empty() ? kArray : kObject);

	//Insert the initial collection if required.
	if (collections.size() == 0) {
		collections.push(t);
	}

	//Make sure the collection type matches.
	if (collections.top() != t) {
		throw new Error(t == kObject
			? "Trying to add a value/collection with a key to an array."
			: "Trying to add a value/collection to an object without a key."
		);
	}

	//Insert the key.
	*output << std::string((collections.size()-1)*4, ' ');
	if (key.empty()) {
		*output << "-";
	} else {
		*output << key << ":";
	}
}