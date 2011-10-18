#include <iostream>
#include "httpchannel.h"


int main(int argc, char ** argv)
{
    std::cout << "In Sono Veritas." << std::endl;
    
    //Initialize the listening HTTP listening socket.
    HTTPChannel http;
    http.run();
    while (true) sleep(1);
    return 0;
}
