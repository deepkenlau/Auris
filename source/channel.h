#pragma once


class Channel {
private:
    const int port;
    int listenerfd;
    
public:
    Channel(int port);
    ~Channel();
    
    virtual std::string desc();
    
    //Stats the channel and waits for connections.
    void run();
    
    //Callback function called when the server accepted a connection.
    virtual void accepted(int fd);
};
