#pragma once
#include "../subsystem.h"
#include <set>
#include <string>
#include "../../blob.h"
#include "../../path.h"


class StorageSubsystem : public LibrarySubsystem {
private:
    struct IndexEntry {
        std::string uuid;
        std::string suffix;
        std::string checksum;
        bool operator == (const IndexEntry & i) const { return uuid == i.uuid; }
        bool operator < (const IndexEntry & i) const { return uuid < i.uuid; }
    };
    typedef std::set<IndexEntry> Index;
    
    void loadIndex(Index & index, const Path & path);
    void storeIndex(Index & index, const Path & path);
    
    Index originals;
    
    Path getOriginalsDir() const;
    Path getOriginalsIndexPath() const;
    
public:
    StorageSubsystem(Library * l);
    void start();
    
    bool onRawCommand(RawCommand * c);
    
    bool addFile(Blob * content, std::string suffix);
};
