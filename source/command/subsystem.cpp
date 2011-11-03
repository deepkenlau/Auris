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
    
    //Try to find a library this command relates to.
    Library * library = NULL;
    for (std::set<Library *>::iterator i = server->libraries.begin();
    	 i != server->libraries.end(); i++) {
    	if ((*i)->getUUID() == c->arguments[0]) {
    		library = *i;
            log << "- directed at library " << library->getName() << std::endl;
    		break;
    	}
    }
    
    //Try to relay the raw command to the library's subsystems.
    if (library && library->onRawCommand(c)) return;
    
    //Try to relay the raw command to the subsystems.
    if (server->connection.onRawCommand(c)) return;
    if (server->command.onRawCommand(c)) return;
    
    //TODO: Convert the command according to the suffix to YAML, JSON, etc..
    //... insert code here ...
}
