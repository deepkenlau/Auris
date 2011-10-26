#include "subsystem.h"
#define logn "Command subsystem"
#include "../log.h"
#include <set>
#include "../server.h"



CommandSubsystem::CommandSubsystem(Server * s) : Subsystem(s)
{
}

void CommandSubsystem::start()
{
    log << "starting…" << std::endl;
}

/** Processes the given raw command. This involves converting it to a fully
 * fledged command if it contains some sort of YAML, JSON, Confuse or the data
 * the like, and distributing it to the right subsystems. */
void CommandSubsystem::processRawCommand(RawCommand * c)
{
    log << "received raw command " << c->desc() << std::endl;
    
    //Try to relay the raw command to the subsystems.
    for (std::set<Subsystem *>::iterator is = server->subsystems.begin();
         is != server->subsystems.end(); is++)
        if ((*is)->onRawCommand(c))
            return;
    
    //TODO: Convert the command according to the suffix to YAML, JSON, etc..
    //... insert code here ...
}
