/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Store.h"
#include <common/sha1_utils.h>
#include <stdexcept>
#include <fstream>
#include <common/FileSystem.h>
#include <dirent.h>

//debug
#include <iostream.h>

using database::media::Store;
using std::runtime_error;
using namespace sha1;

Store::Store(const Path &path)
{
	this->path = path;
}

void Store::persist(const Blob &data, std::string name, std::string format)
{
	std::string checksum = sha1::calc(format);
	Path formatsdir = path.down("formats");
	FileSystem::makeDirectory(formatsdir);
	Path tmp = formatsdir.down(checksum);
	std::ofstream f(tmp, std::fstream::trunc);
	if (f.fail())
		throw runtime_error("Error on opening format file.");
	f.write(format.c_str(), format.length());
	if (f.bad())
		throw runtime_error("Error on writing format file.");
	f.close();
	if(f.fail())
		throw runtime_error("Error on closing format file.");

	Path objectsdir = path.down("objects");
	FileSystem::makeDirectory(objectsdir);
	tmp = objectsdir.down(checksum);
	FileSystem::makeDirectory(tmp);
	tmp = tmp.down(name);
	std::ofstream fData(tmp, std::fstream::trunc);
	if (fData.fail())
		throw runtime_error("Error on opening data file.");
	fData.write((const char *)data.buffer, data.length);
	if (fData.bad())
		throw runtime_error("Error on writing data file.");
	fData.close();
	if(fData.fail())
		throw runtime_error("Error on closing data file.");
}

Blob Store::load(std::string name, std::string format)
{
	std::string checksum = sha1::calc(format);
	Path tmp = path.down("objects").down(checksum).down(name);
	std::ifstream f(tmp, std::fstream::in);
	if (f.fail())
		throw runtime_error("Error on opening data file.");

	f.seekg(0, std::ios_base::end);
	if (f.fail() || f.bad())
		throw runtime_error("Error on seeking data file");
	int size = f.tellg();

	f.seekg(0, std::ios_base::beg);
	if (f.fail() || f.bad())
		throw runtime_error("Error on seeking data file");

	Blob blob(new char[size], size);
	f.read((char*)blob.buffer, size);

	if (f.bad())
		throw runtime_error("Error on reading data file.");
	f.close();
	if(f.fail())
		throw runtime_error("Error on closing data file.");
	return blob;
}

void Store::setMainFormat(std::string name, std::string format)
{
	Path tmp = path.down("heads");
	FileSystem::makeDirectory(tmp);
	tmp = tmp.down(name);
	std::ofstream f(tmp, std::fstream::trunc);
	if (f.fail())
		throw runtime_error("Error on opening format file.");
	f.write(format.c_str(), format.length());
	if (f.bad())
		throw runtime_error("Error on writing format file.");
	f.close();
	if(f.fail())
		throw runtime_error("Error on closing format file.");
}

std::string Store::getMainFormat(std::string name)
{
	Path tmp = path.down("heads").down(name);
	std::ifstream f(tmp, std::fstream::in);
	if (f.fail())
		throw runtime_error("Error on opening main format file.");

	f.seekg(0, std::ios_base::end);
	if (f.fail() || f.bad())
		throw runtime_error("Error on seeking main format file");
	int size = f.tellg();

	f.seekg(0, std::ios_base::beg);
	if (f.fail() || f.bad())
		throw runtime_error("Error on seeking main format file");

	char * str = new char[size+1];
	f.read(str, size);
	str[size] = 0;
	if (f.bad())
		throw runtime_error("Error on reading main format file.");
	f.close();
	if(f.fail())
		throw runtime_error("Error on closing main format file.");
	return str;
}

std::set<std::string> Store::getFormats()
{
	std::set<std::string> filenames;
	Path tmp = path.down("formats");
	DIR dp;
	struct dirent *dirp;
    if((dp  = opendir(tmp)) == NULL)
    	throw runtime_error("Error on opening formats directory");
	while ((dirp = readdir(dp)) != NULL) {
		filenames.insert(std::string(dirp->d_name));
 	}
 	for(std::set<std::string>::iterator it = filenames; it != filenames.end; it++)
 	{
//debug
std::cout << it << endl;
 	}
 	return NULL;
}