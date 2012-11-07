/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "MimeType.h"

MimeType::MimeTypes MimeType::repository;

void MimeType::initRepository()
{
	if (!repository.empty()) return;

	repository.push_back(MimeType()
		.addName("text/xml")
		.addSuffix("xml"));

	repository.push_back(MimeType()
		.addName("text/yaml")
		.addName("application/x-yaml")
		.addSuffix("yml"));

	repository.push_back(MimeType()
		.addName("application/json")
		.addSuffix("json"));
}