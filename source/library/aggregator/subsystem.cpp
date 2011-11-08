#include "subsystem.h"
#define logn "Aggregator subsystem"
#include "../../log.h"


AggregatorSubsystem::AggregatorSubsystem(Library * l) : LibrarySubsystem(l)
{
}

void AggregatorSubsystem::start()
{
    log << "starting…" << std::endl;
}
