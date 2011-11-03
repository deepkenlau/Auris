#include <fstream>
#include <cerrno>
#include <cstring>
#include "subsystem.h"
#define logn "Storage subsystem"
#include "../../log.h"
#include "../../command/rawcommand.h"
#include "../library.h"
#include "../../md5.h"
#include "../../uuid.h"
#include "../../filesystem.h"
#include <libconfig.h++>


/** Loads the index of the originals from disk. */
void StorageSubsystem::loadIndex(Index & index, const Path & path)
{
    index.clear();
    
    //Check for the existence of the index file.
    std::string p = path;
    if (!FS::fileExists(p))
        return;
    
    //Open the file.
    std::ifstream f(p.c_str());
    while (!f.eof()) {
        IndexEntry e;
        f >> e.uuid;     if (e.uuid.empty()) continue;
        f >> e.suffix;   if (e.suffix.empty()) continue;
        f >> e.checksum; if (e.checksum.empty()) continue;
        index.insert(e);
    }
    f.close();
}

/** Stores the index of the originals to disk. */
void StorageSubsystem::storeIndex(Index & index, const Path & path)
{
    //Open the file for writing.
    std::string p = path;
    std::ofstream f(p.c_str());
    
    //Add the files.
    for (Index::iterator i = originals.begin(); i != originals.end(); i++) {
        const IndexEntry & e = *i;
        f << e.uuid << " ";
        f << e.suffix << " ";
        f << e.checksum << "\n";
    }
    f.close();
}

/** Returns the path to the directory containing the original audio files. */
Path StorageSubsystem::getOriginalsDir() const
{
    return (library->directory + "originals");
}

/** Returns the path to the originals index file. */
Path StorageSubsystem::getOriginalsIndexPath() const
{
    return (getOriginalsDir() + "index");
}


StorageSubsystem::StorageSubsystem(Library * l) : LibrarySubsystem(l)
{
}

void StorageSubsystem::start()
{
    log << "starting…" << std::endl;
    
    //Load the indices.
    log << "loading originals index…" << std::endl;
    loadIndex(originals, getOriginalsIndexPath());
    log << "- " << originals.size() << " loaded" << std::endl;
}

bool StorageSubsystem::onRawCommand(RawCommand * c)
{
    //Ignore empty commands.
    if (c->arguments.empty())
        return false;
    
    //Process upload commands.
    if (c->arguments[1] == "upload") {
        
        //Extract the suffix.
        std::string & name = c->arguments[2];
        int colon = name.find_last_of('.');
        std::string suffix(name, colon + 1);
        
        //Add the file.
        if (addFile(c->data, suffix)) {
            c->response.setSuccessful(true);
            c->response.finish();
        } else {
            c->response.setSuccessful(false);
            c->response << "unable to upload file\r\n";
            c->response.finish();
        }
        
        return true;
    }
    
    //Debug.
    if (c->arguments[1] == "hello") {
    	c->response << "world!";
    	c->response.finish();
        return true;
    }
}

/** Creates a new file in the music library with the given file content, if it
 * doesn't already exist. The suffix is used to identify the file format. */
bool StorageSubsystem::addFile(Blob * content, std::string suffix)
{
    //Calculate the MD5 checksum of the blob.
    std::string checksum = md5(content);
    log << "adding " << suffix << " file (" << content->getLength() << " bytes), md5 = " << checksum << std::endl;
    
    //Check whether there already is a file with the same checksum.
    for (Index::iterator i = originals.begin(); i != originals.end(); i++) {
        if ((*i).checksum == checksum) {
            //TODO: further compare the file to rule out the possibility that
            //two different files generated the same MD5 checksum.
            log << "- file with checksum " << checksum << " already exists, namely " << (*i).uuid << std::endl;
            return false;
        }
    }
    
    //Calculate a new UUID for this file.
    std::string uuid = generateUUID();
    
    //Assemble the file name.
    std::stringstream filename;
    filename << uuid;
    filename << ".";
    filename << suffix;
    
    //Make sure the originals directory exists.
    Path path = getOriginalsDir();
    if (!FS::fileExists(path))
        if (!FS::makeDirectory(path))
            return false;
    
    //Assemble the path and store the file.
    path += filename.str();
    log << "- destination file is " << (std::string)path << std::endl;
    std::ofstream f(((std::string)path).c_str());
    if (!f.write(content->getData(), content->getLength())) {
        err << "unable to write file " << (std::string)path << ", " << strerror(errno) << std::endl;
        return false;
    }
    f.close();
    
    //Add the file to the originals index.
    IndexEntry i;
    i.uuid = uuid;
    i.suffix = suffix;
    i.checksum = checksum;
    originals.insert(i);
    
    //Rewrite the index to disk.
    storeIndex(originals, getOriginalsIndexPath());
    
    return true;
}
