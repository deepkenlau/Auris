#pragma once
#include "interfaces/http.h"
#include "../subsystem.h"

class RawCommand;
class Server;


class ConnectionSubsystem : public Subsystem {
private:
    ConnectionInterfaceHTTP httpInterface;
    
public:
    ConnectionSubsystem(Server * s);
    void start();
    
    void onReceivedCommand(RawCommand * c);
};
