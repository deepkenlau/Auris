#include "subsystem.h"
#define logn "Metadata subsystem"
#include "../log.h"


MetadataSubsystem::MetadataSubsystem(Server * s) : Subsystem(s)
{
}

void MetadataSubsystem::start()
{
    log << "starting…" << std::endl;
}
