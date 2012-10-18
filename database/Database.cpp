/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <stdexcept>
#include "Database.h"

using std::runtime_error;


Database::Database::Database(std::string path)
{
	this->path = path;
}

void Database::Database::store() const
{
	FILE * f = fopen(path.c_str(), "w");
	tinyxml2::XMLPrinter xml(f);
	xml.OpenElement("metadata");
	xml.OpenElement("songs");
	songTable.encode(xml);
	xml.CloseElement();
	xml.OpenElement("albums");
	albumTable.encode(xml);
	xml.CloseElement();
	xml.CloseElement();
	fclose(f);
}

void Database::Database::load()
{
	tinyxml2::XMLDocument xml;
	if(xml.LoadFile(path.c_str()) != 0) throw new runtime_error("Error XML-file");
	tinyxml2::XMLElement *e = xml.RootElement();
	if (e == NULL)
		throw new runtime_error("Could not read root element");
	tinyxml2::XMLElement * songs = e->FirstChildElement("songs");
	if (songs == NULL) throw new runtime_error("Error on reading song table");
	songTable.decode(*songs);

	tinyxml2::XMLElement * albums = e->FirstChildElement("albums");
	if (albums == NULL) throw new runtime_error("Error on reading album table");
	albumTable.decode(*albums);
}