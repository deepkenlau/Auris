#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "../blob.h"
#include "response.h"


class RawCommand {
public:
	Response response;
    std::vector<std::string> arguments;
    Blob * data;
    
    RawCommand(ResponseHandler * rh = NULL) : response(rh) { data = NULL; }
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
