#include "filesystem.h"

#include <sys/types.h>
#include <sys/stat.h>


bool FileSystem::fileExists(const std::string & path)
{
    struct stat st;
    return (stat(path.c_str(), &st) == 0);
}
