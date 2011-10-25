#pragma once
#include "interfaces/http.h"


class ConnectionSubsystem {
private:
    ConnectionInterfaceHTTP httpInterface;
    
public:
    void start();
};
