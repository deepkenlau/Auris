#include "library.h"
#define logn "Library"
#include "../log.h"
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include "uuid.h"


void Library::initialize()
{
	//Ensure the directory exists.
	ensureDirectoryExists();
	
	//Generate a UUID for the library.
	//TODO: load this stuff from the library config file!
	uuid = generateUUID();
	name = "Debug Library";
}

void Library::ensureDirectoryExists()
{
	if (!FS::fileExists(directory)) {
    	log << "creating library directory at " << (std::string)directory << std::endl;
		if (!FS::makeDirectory(directory))
			err << "unable to create library directory at " << (std::string)directory << ", "
       		<< strerror(errno) << std::endl;
	}
}

Library::Library(Server * s, const Path & directory)
: server(s), directory(directory), storage(this), metadata(this)
{
}

Library::~Library()
{
}

const std::string & Library::getUUID() const
{
    return uuid;
}

const std::string & Library::getName() const
{
    return name;
}

void Library::start()
{
	log << "starting subsystems..." << std::endl;
	
	//Do basic library initialization. This involves creating the basic
	//library directory structure if required and initializing the
	//configuration files.
	initialize();
	
	//Start the subsystems.
	storage.start();
	metadata.start();
}

bool Library::onRawCommand(RawCommand * c)
{
	if (storage.onRawCommand(c)) return true;
	if (metadata.onRawCommand(c)) return true;
	return false;
}

/*void Library::addFile(std::string suffix, Blob * data)
{
    //Assemble the file name for this file.
    std::stringstream name;
    name << generateUUID();
    name << ".";
    name << suffix;
    
    //Assemble the storage path.
    std::stringstream path;
    path << directory;
    path << "/";
    path << name.str();
    
    //Store the file.
    std::ofstream f(path.str().c_str());
    f.write(data->getData(), data->getLength());
    f.close();
}*/
