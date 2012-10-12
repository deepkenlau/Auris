#include <cassert>
#include <iostream>
#include <gc_cpp.h>
#include "../database/Table.h"

#define assert_equal(a,b) if (a != b) { std::cerr << #a << ": expected " << b << ", got " << a << std::endl; return 1; }

int main(int argc, char *argv[])
{
	GC_INIT();

	Database::Table table;
	Database::Entry *e = new Database::Entry;
	(*e)["title"]  = "Fire Hive";
	(*e)["artist"] = "Knive Party";
	(*e)["rating"] = 0.98;
	(*e)["count"]  = 687;

	return 0;
}