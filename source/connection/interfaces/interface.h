#pragma once

class ConnectionSubsystem;


class ConnectionInterface {
public:
    ConnectionSubsystem * const connection;
    ConnectionInterface(ConnectionSubsystem * c) : connection(c) {}
    virtual void start() {}
};
