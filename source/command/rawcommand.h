#pragma once
#include <string>
#include <vector>


class RawCommand {
public:
    std::vector<std::string> arguments;
    unsigned char * data;
    
    RawCommand() { data = NULL; }
    ~RawCommand() { if (data) delete (data); data = NULL; }
    
    const std::string desc() const {
        std::stringstream s;
        s << "command { ";
        for (int i = 0; i < arguments.size(); i++)
            s << arguments[i] << " ";
        s << "}";
        return s.str();
    }
};
