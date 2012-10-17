#include "Database.h"
#include <common/sha1_utils.h>
#include <fstream>
#include <string>
#include <stdexcept>

using std::string;
using std::ofstream;
using std::runtime_error;
using database::database::Database;


Database::Database(const string &path)
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

	//Write the object to disk.
	string fp = (path + "/" + sum);
	ofstream fout(fp.c_str());
	if (!fout.good()) {
		throw runtime_error(string("unable to persist object ") + sum);
	}
	fout << object;
	fout.close();

	//Return the checksum so the caller may refer to it.
	return sum;
}