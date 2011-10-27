#pragma once
#include <string>
#include <vector>


class Path {
public:
    typedef std::vector<std::string> Components;
    
private:
    Components components;
    void invalidateCache() const;
    
public:
    Path(const std::string & path);
    Path(const Components & components);
    ~Path();
    
    void setString(std::string path);
    const std::string & getString() const;
    
    operator std::string () const;
};
