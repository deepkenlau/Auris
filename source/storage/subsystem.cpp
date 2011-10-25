#include "subsystem.h"
#define logn "Storage subsystem"
#include "../log.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


void StorageSubsystem::start()
{
    log << "starting…" << std::endl;
    
    //DEBUG: Initialize a music library under ~/Music/Auris since the main part
    //of the program is developped under Mac OS X.
    struct passwd * pw = getpwuid(getuid());
    const char * homedir = pw->pw_dir;
    std::string path(homedir);
    path += "/Music/Auris";
    StorageLibrary * l = new StorageLibrary(path);
    libraries.insert(l);
}
