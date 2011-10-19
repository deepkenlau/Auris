#pragma once
#include "connection/connection.h"


class Server {
private:
    //Subsystems
    Connection connection;
    
public:
    void run();
};
