#include "subsystem.h"
#define logn "Storage subsystem"
#include "../../log.h"
#include "../../command/rawcommand.h"
#include "../library.h"


/** Returns the path to the directory containing the original audio files. */
Path StorageSubsystem::getOriginalsDir() const
{
    return (library->directory + "originals");
}


StorageSubsystem::StorageSubsystem(Library * l) : LibrarySubsystem(l)
{
}

void StorageSubsystem::start()
{
    log << "starting…" << std::endl;
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

/** Creates a new file in the music library with the given file content, if it
 * doesn't already exist. */
void StorageSubsystem::addFile(Blob * content)
{
    
}
