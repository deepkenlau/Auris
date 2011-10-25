#include "library.h"
#define logn "Library"
#include "../log.h"
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>


void StorageLibrary::ensureDirectoryExists()
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

StorageLibrary::StorageLibrary(std::string d) : directory(d)
{
    //Make sure that the directory for the library exists.
    ensureDirectoryExists();
}

StorageLibrary::~StorageLibrary()
{
}
