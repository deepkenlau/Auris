#include "filesystem.h"

#include <sys/types.h>
#include <sys/stat.h>


/** Returns whether the file at the given location exists. */
bool FS::fileExists(const std::string & path)
{
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}

/** Tries to create the directory at the given location. */
bool FS::makeDirectory(const std::string & path)
{
    int result = mkdir(path.c_str(), 0777);
    return (result == 0);
}
