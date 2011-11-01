#include "subsystem.h"
#define logn "Metadata subsystem"
#include "../../log.h"


MetadataSubsystem::MetadataSubsystem(Library * l) : LibrarySubsystem(l)
{
}

void MetadataSubsystem::start()
{
    log << "starting…" << std::endl;
}
