#pragma once
#include <string>
#include "blob.h"


class Library {
private:
    const std::string directory;
    
    void ensureDirectoryExists();
    
public:
    Library(std::string directory);
    ~Library();
    
    void addFile(std::string suffix, Blob * data);
};
