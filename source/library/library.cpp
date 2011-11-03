#include "library.h"
#define logn "Library"
#include "../log.h"
#include <sstream>
#include "../uuid.h"
#include "../filesystem.h"
#include <libconfig.h++>


void Library::initialize()
{
	//Ensure the directory exists.
	ensureDirectoryExists();
	
    //Load the configuration.
    loadConfig();
    
    //Fill in any missing information with some default values.
    bool change = false;
    if (uuid.empty()) {
        uuid = generateUUID();
        change = true;
    }
    if (name.empty()) {
        name = "Unnamed Library";
        change = true;
    }
    
    //If there was any missing information filled in, store the config file.
    if (change)
        storeConfig();
    
    //Dump some information.
    log << "initialized " << uuid << ", \"" << name << "\"" << std::endl;
}

/** If the library directory does not exist, creates it. */
void Library::ensureDirectoryExists()
{
	if (FS::fileExists(directory))
        return;
    
    log << "creating library directory at " << (std::string)directory << std::endl;
    if (!FS::makeDirectory(directory))
        err << "unable to create library directory" << std::endl;
}

/** Returns the path to this library's configuration file. */
Path Library::getConfigPath() const
{
    return (directory + "config");
}

void Library::loadConfig()
{
    //Do nothing if the config file does not exist.
    std::string p = getConfigPath();
    if (!FS::fileExists(p))
        return;
    
    //Read the configuration file.
    libconfig::Config cfg;
    try {
        cfg.readFile(p.c_str());
    } catch (const libconfig::FileIOException & e) {
        err << "unable to read config from " << p << std::endl;
        return;
    } catch (const libconfig::ParseException & e) {
        err << "config parse error " << e.getFile() << ":" << e.getLine()
        << " - " << e.getError() << std::endl;
        return;
    }
    
    //Load the basic library attributes.
    cfg.lookupValue("uuid", uuid);
    cfg.lookupValue("name", name);
}

void Library::storeConfig()
{
    libconfig::Config cfg;
    libconfig::Setting & root = cfg.getRoot();
    
    //Store the basic library attributes.
    root.add("uuid", libconfig::Setting::TypeString) = uuid;
    root.add("name", libconfig::Setting::TypeString) = name;
    
    //Write the config file.
    std::string p = getConfigPath();
    try {
        cfg.writeFile(p.c_str());
    } catch (const libconfig::FileIOException & e) {
        err << "unable to write config to " << p << std::endl;
        return;
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
	//Do basic library initialization. This involves creating the basic
	//library directory structure if required and initializing the
	//configuration files.
	initialize();
	
	//Start the subsystems.
	log << "starting subsystems..." << std::endl;
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
