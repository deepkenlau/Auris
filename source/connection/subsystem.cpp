#include "subsystem.h"
#define logn "Connection subsystem"
#include "../log.h"
#include "../server.h"


ConnectionSubsystem::ConnectionSubsystem(Server * s)
: Subsystem(s), httpInterface(this)
{
}

void ConnectionSubsystem::start()
{
    log << "starting…" << std::endl;
    httpInterface.start();
}

/** This function is to be called everytime one of the connection interfaces re-
 * ceives a command. The ConnectionSubsystem will then cope with relaying the
 * command to the command subsystem which will perform further processing. */
void ConnectionSubsystem::onReceivedCommand(RawCommand * c)
{
    //Simply relay the command to the command subsystem.
    server->command.processRawCommand(c);
}
