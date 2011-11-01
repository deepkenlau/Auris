#include "library.h"
#define logn "Library"
#include "log.h"
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include "uuid.h"


void Library::ensureDirectoryExists()
{
    //Check if the directory exists, returning immediately if that is the case.
    struct stat st;
    if (stat(directory.c_str(), &st) == 0)
        return;
    
    //Otherwise create the directory.
    log << "creating library directory at " << directory << std::endl;
   if (mkdir(directory.c_str(), 0777) != 0)
       err << "unable to create library directory at " << directory << ", "
       << strerror(errno) << std::endl;
}

Library::Library(const Path & directory) : directory(directory)
{
    //Make sure that the directory for the library exists.
    ensureDirectoryExists();
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

void Library::addFile(std::string suffix, Blob * data)
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
}
