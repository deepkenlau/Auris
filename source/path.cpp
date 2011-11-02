#include "path.h"
#include <map>
#include <sstream>

#ifdef WIN32
#define SEPARATOR "\\"
#else
#define SEPARATOR "/"
#endif


static std::map<const Path *, std::string> stringCache;

void Path::invalidateCache() const
{
    stringCache.erase(this);
}


/** Initializes the path from the given string representation. */
Path::Path(const std::string & path)
{
    setString(path);
}

/** Initializes the path from the given path components. */
Path::Path(const Components & components)
{
    this->components = components;
}

Path::~Path()
{
    //Clear any existing cache entry.
    invalidateCache();
}



/** Sets the path from the given string. */
void Path::setString(std::string path)
{
    components.clear();
    int offset = 0;
    do {
        offset = path.find_first_of(SEPARATOR);
        components.push_back(std::string(path, 0, offset));
        if (offset != std::string::npos)
            path = std::string(path, offset + 1);
    } while (offset != std::string::npos);
    invalidateCache();
}

/** Returns a string representing this path. */
const std::string & Path::getString() const
{
    //If there's a cached string, return that.
    if (stringCache.count(this))
        return stringCache[this];
    
    //Otherwise assemble the path string.
    std::stringstream s;
    bool first = true;
    for (Components::const_iterator ic = components.begin(); ic != components.end(); ic++) {
        if (!first)
            s << SEPARATOR;
        s << *ic;
        first = false;
    }
    
    //Store it in the cache and return it.
    stringCache[this] = s.str();
    return stringCache[this];
}

/** Automatic cast to an std::string. */
Path::operator std::string () const
{
    return getString();
}



/** Appends the given path to this path. */
void Path::append(const std::string & path)
{
    append(Path(path));
}

/** Appends the given path to this path. */
void Path::append(const Path & path)
{
    for (int i = 0; i < path.components.size(); i++)
        components.push_back(path.components[i]);
}



/** Returns a new path with the given path appended. */
Path Path::operator + (const std::string & path) const
{
    Path p(*this);
    p.append(path);
    return p;
}

/** Returns a new path with the given path appended. */
Path Path::operator + (const Path & path) const
{
    Path p(*this);
    p.append(path);
    return p;
}



/** Convenience wrapper around the append function. */
Path & Path::operator += (const std::string & path)
{
    append(path);
}

/** Convenience wrapper around the append function. */
Path & Path::operator += (const Path & path)
{
    append(path);
}
