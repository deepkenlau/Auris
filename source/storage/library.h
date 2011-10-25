#pragma once
#include <string>


class StorageLibrary {
private:
    const std::string directory;
    
    void ensureDirectoryExists();
    
public:
    StorageLibrary(std::string directory);
    ~StorageLibrary();
};
