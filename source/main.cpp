#include <iostream>
#include "server.h"


int main(int argc, char ** argv)
{
    std::cout << "In Sono Veritas." << std::endl;
    Server s;
    s.run();
    return 0;
}
