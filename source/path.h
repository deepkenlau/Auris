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
    
    void append(const std::string & path);
    void append(const Path & path);
    
    Path operator + (const std::string & path) const;
    Path operator + (const Path & path) const;
    
    Path & operator += (const std::string & path);
    Path & operator += (const Path & path);
};
