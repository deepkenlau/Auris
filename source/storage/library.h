#pragma once
#include <string>
#include "../blob.h"


class StorageLibrary {
private:
    const std::string directory;
    
    void ensureDirectoryExists();
    
public:
    StorageLibrary(std::string directory);
    ~StorageLibrary();
    
    void addFile(std::string suffix, Blob * data);
};
