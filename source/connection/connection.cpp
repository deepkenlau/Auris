#include "connection.h"
#define logn "Connection subsystem"
#include "../log.h"


void Connection::start()
{
    log << "starting…" << std::endl;
    httpInterface.start();
}
