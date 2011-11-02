#include "library.h"
#include "subsystem.h"


LibrarySubsystem::LibrarySubsystem(Library * l)
: library(l), Subsystem(l->server)
{
}
