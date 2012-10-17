#include "FileSystem.h"
#include "Path.h"
#include <sys/stat.h>
#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;


/** Returns whether the file or directory at the given path exists. */
bool FileSystem::fileExists(const Path &p)
{
	struct stat st;
	return stat(p, &st) == 0;
}

/** Creates a directory at the given path, creating intermediate directories
 * if required. Does nothing if the directory already exists. Throws an error
 * if the directory could not be created. */
void FileSystem::makeDirectory(const Path &dir)
{
	if (fileExists(dir))
		return;
	
	Path parent = dir.up();
	if (!fileExists(parent))
		makeDirectory(parent);
	
	if (mkdir(dir, 0777) != 0) {
		throw runtime_error(string("unable to make directory ") + dir.str());
	}
}