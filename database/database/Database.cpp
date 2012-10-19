/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Database.h"
#include "Error.h"
#include <common/sha1_utils.h>
#include <common/FileSystem.h>
#include <common/Error.h>
#include <fstream>
#include <string>
#include <stdexcept>
#include <iostream>

using std::string;
using std::ifstream;
using std::ofstream;
using std::runtime_error;
using std::ios;
using database::database::Database;


Database::Database(const Path &path) : head(this)
{
	this->path = path;
}

void Database::load()
{
	//Try to read the metadata head.
	Path p = getMetadataHeadPath();
	ifstream fin(p);
	if (!fin.good()) return; //don't throw an exception here!

	//Interpret the contents of the file as commit checksum.
	string sha;
	fin >> sha;
	fin.close();

	//Replace the old commit with a new one with the given SHA.
	if (head.getHash() == sha) return;
	std::cout << "Database: loading " << sha << std::endl;
	head.load(sha);
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


/** Stores the given data in the database's object directory and returns the
 * name of the generated object. */
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

std::string Database::loadObject(const std::string &hash) const
{
	//Get the path to the object file.
	Path p = getObjectPath(hash);

	//Try to read the object file.
	ifstream fin(p);
	if (!fin.good()) {
		throw new Error(string("Unable to load object ") + hash + ".", new IOError(p));
	}
	string data;
	fin.seekg(0, ios::end);
	data.resize(fin.tellg());
	fin.seekg(0, ios::beg);
	fin.read(&data[0], data.size());
	fin.close();

	return data;
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