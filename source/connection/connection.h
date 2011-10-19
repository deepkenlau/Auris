#pragma once
#include "interfaces/http.h"


class Connection {
private:
    ConnectionInterfaceHTTP httpInterface;
    
public:
    void start();
}