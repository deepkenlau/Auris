#pragma once
#include <string>
#include "path.h"
#include "blob.h"


class Library {
private:
    std::string uuid;
    std::string name;
    const Path directory;
    
    void ensureDirectoryExists();
    
public:
    Library(const Path & directory);
    ~Library();
    
    const std::string & getUUID() const;
    const std::string & getName() const;
    
    void addFile(std::string suffix, Blob * data);
};
