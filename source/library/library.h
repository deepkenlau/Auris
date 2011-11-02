#pragma once
#include <string>
#include "../path.h"
#include "../blob.h"
#include "storage/subsystem.h"
#include "metadata/subsystem.h"

class Server;


class Library {
	friend class StorageSubsystem;
	friend class MetadataSubsystem;
	
private:
    std::string uuid;
    std::string name;
    const Path directory;
    
    void initialize();
    void ensureDirectoryExists();
    
    Path getConfigPath() const;
    
protected:
    //Subsystems.
    StorageSubsystem storage;
    MetadataSubsystem metadata;
    
public:
    Server * const server;
    
    Library(Server * s, const Path & directory);
    ~Library();
    
    const std::string & getUUID() const;
    const std::string & getName() const;
    
    void start();
    bool onRawCommand(RawCommand * c);
};
