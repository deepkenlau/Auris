#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "../blob.h"


class RawCommand {
public:
    std::vector<std::string> arguments;
    Blob * data;
    
    RawCommand() { data = NULL; }
    ~RawCommand() { if (data) delete (data); data = NULL; }
    
    const std::string desc() const {
        std::stringstream s;
        s << "{ ";
        for (int i = 0; i < arguments.size(); i++)
            s << arguments[i] << " ";
        s << "}";
        return s.str();
    }
};
