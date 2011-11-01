#include "subsystem.h"
#define logn "Storage subsystem"
#include "../log.h"
#include "../command/rawcommand.h"


StorageSubsystem::StorageSubsystem(Library * l) : Library::Subsystem(l)
{
}

void StorageSubsystem::start()
{
    log << "starting…" << std::endl;
    
    //DEBUG: Initialize a music library under ~/Music/Auris since the main part
    //of the program is developped under Mac OS X.
    /*struct passwd * pw = getpwuid(getuid());
    const char * homedir = pw->pw_dir;
    std::string path(homedir);
    path += "/Music/Auris";
    StorageLibrary * l = new StorageLibrary(path);
    libraries.insert(l);*/
}

bool StorageSubsystem::onRawCommand(RawCommand * c)
{
    //Ignore empty commands.
    if (c->arguments.empty())
        return false;
    
    //We process upload commands.
    /*if (c->arguments[0] == "upload") {
        
        //Extract the suffix.
        std::string & name = c->arguments[1];
        int colon = name.find_last_of('.');
        std::string suffix(name, colon + 1);
        
        //Add the uploaded file to each library object.
        for (std::set<StorageLibrary *>::iterator il = libraries.begin();
             il != libraries.end(); il++)
            (*il)->addFile(suffix, c->data);
    }*/
    
    //Debug.
    if (c->arguments[0] == "hello") {
    	c->response << "world!";
    	c->response.finish();
    }
}
