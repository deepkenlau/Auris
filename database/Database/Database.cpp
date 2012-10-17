#include "Database.h"
#include <common/sha1_utils.h>
#include <common/FileSystem.h>
#include <fstream>
#include <string>
#include <stdexcept>

using std::string;
using std::ofstream;
using std::runtime_error;
using database::database::Database;


Database::Database(const Path &path)
{
	this->path = path;
}

void Database::load()
{

}

void Database::commit()
{

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
		throw runtime_error(string("unable to persist object ") + sum);
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