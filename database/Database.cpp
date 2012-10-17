/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Database.h"

Database::Database(std::string path)
{
	this->path = path;
}

void Database::store()
{
	FILE * f = fopen(path.c_str(), "w");
	tinyxml2::XMLPrinter xml(f);
	xml.OpenElement("metadata");
	xml.OpenElement("songs");
	songTable.encode(xml);
	xml.CloseElement();
	xml.OpenElement("artists");
	artistTable.encode(xml);
	xml.CloseElement();
	xml.CloseElement();
	fclose(f);
}

void Database::load()
{
	tinyxml2::XMLDocument xml();
	if(xml.LoadFile(path.c_str()) != NULL) throw new std::runtime_error("Error
	XML-file";
	tinyxml2::XMLElement *e = xml.RootElemenet();
	if (e == NULL)
		throw new std::runtime_error("Could not read root element");
	tinyxml2::XMLElement * songs = e->FirstChildSibling();
	if (songs == NULL) throw new std::runtime_error("Error on reading song table");
	songTable.decode(*songs);

	tinyxml2::XMLElement * artists = e->NextChildSibling();
	if (artists == NULL) throw new std::runtime_error("Error on reading artist table");
	artistTable.decode(*artists);
}