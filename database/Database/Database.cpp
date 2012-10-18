#include "Database.h"
#include <common/sha1_utils.h>
#include <common/FileSystem.h>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>

using std::string;
using std::ofstream;
using std::runtime_error;
using database::database::Database;


Database::Database(const Path &path) : head(this)
{
	this->path = path;
}

void Database::load()
{
	//Try to read the metadata head.
}

void Database::commit()
{
	//Persist the head commit and keep its checksum for the pointer.
	string sha = head.persist();
	std::cout << "Database: committed " << sha << std::endl;

	//Get the path where to put the head pointer.
	Path p = getMetadataHeadPath();
	FileSystem::makeDirectory(p.up());

	//Store the head.
	ofstream fout(p);
	if (!fout.good()) {
		throw runtime_error("Unable to store metadata head.");
	}
	fout << sha;
	fout.close();
}


string Database::persistObject(const string &object) const
{
	//Calculate the SHA1 checksum of the object.
	string sum = sha1::calc(object);

	//Get the path to the output object file and make sure the directory exists.
	Path p = getObjectPath(sum);
	FileSystem::makeDirectory(p.up());

	//Write the object to disk.
	ofstream fout(p);
	if (!fout.good()) {
		throw runtime_error(string("Unable to persist object ") + sum + ".");
	}
	fout << object;
	fout.close();

	//Return the checksum so the caller may refer to it.
	return sum;
}

Path Database::getObjectsDirectory() const
{
	return path.down("objects");
}

Path Database::getObjectPath(const std::string &checksum) const
{
	return getObjectsDirectory().down(checksum);
}

Path Database::getMetadataHeadPath() const
{
	return path.down("metadata");
}