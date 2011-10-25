#include "subsystem.h"
#define logn "Connection subsystem"
#include "../log.h"


void ConnectionSubsystem::start()
{
    log << "starting…" << std::endl;
    httpInterface.start();
}
