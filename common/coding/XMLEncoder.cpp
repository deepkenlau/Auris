/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "XMLEncoder.h"
#include "Error.h"
#include <iostream>
#include <common/strutil.h>

using coding::XMLEncoder;


XMLEncoder::XMLEncoder() : Encoder()
{
}


void XMLEncoder::add(const std::string &value, const std::string &key)
{
	if (key.empty()) {
		ensureCollectionType(kArray);
		collections.top().body +=  "<string>" + value + "</string>\n";
	} else {
		ensureCollectionType(kObject);
		collections.top().attrs += " " + key + "=\"" + value + "\"";
	}
}


void XMLEncoder::push(enum CollectionType type, const std::string &key)
{
	std::cout << "pushing collection '" << key << "'\n";
	if (key.empty()) {
		ensureCollectionType(kArray);
		collections.push(Collection(type, (type == kObject ? "object" : "array")));
	} else {
		ensureCollectionType(kObject);
		collections.push(Collection(type, key));
	}
}

void XMLEncoder::pop(enum CollectionType type)
{
	if (collections.size() <= 1) {
		throw new Error("Trying to pop root collection which is not allowed.");
	} else if (collections.top().type != type) {
		throw new Error(type == kObject ? 
			"Trying to pop an object with an array or no collection pushed." :
			"Trying to pop an array with an object or no collection pushed.");
	}
	Collection c = collections.top();
	std::cout << "popping collection '" << c.name << "'\n";
	collections.pop();
	collections.top().body += finalizeCollection(c);
	std::cout << "body of " << collections.top().name << " modified to " << collections.top().body << "\n";
}


void XMLEncoder::finalize()
{
	if (collections.size() > 1) {
		throw new Error("Not all collections have been popped.");
	}
	if (collections.size() == 1) {
		//*output << (collections.top().type == kArray ? "]" : "}");
		*output << finalizeCollection(collections.top());
	}
}

void XMLEncoder::ensureCollectionType(enum CollectionType type)
{
	if (collections.size() > 0) {
		if (collections.top().type != type) {
			throw new Error(type == kObject
				? "Trying to add a value/collection with a key to an array."
				: "Trying to add a value/collection to an object without a key."
			);
		}
	} else {
		*output << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
		collections.push(Collection(type, "root"));
	}
}

std::string XMLEncoder::finalizeCollection(Collection &c)
{
	std::stringstream op;
	op << "<" << c.name << c.attrs;
	if (c.body.empty()) {
		op << " />\n";
	} else {
		op << ">\n";
		op << "\t" << strutil::indent(c.body);
		op << "</" << c.name << ">\n";
	}
	return op.str();
}