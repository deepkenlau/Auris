#pragma once

class RawCommand;
class Server;


class Subsystem {
public:
    Server * const server;
    Subsystem(Server * s) : server(s) {}
    
    //Command processing.
    virtual bool onRawCommand(RawCommand * c) { return false; }
};
