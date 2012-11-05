#include <cassert>
#include <iostream>
#include <gc_cpp.h>
#include <stdexcept>
#include <common/Error.h>
#include <common/coding/Decoder.h>

using std::runtime_error;
using coding::Decoder;
using coding::Node;


int main(int argc, char *argv[])
{
	GC_INIT();
	try {
		std::ifstream fin("~/Desktop/output.xml");
		Decoder *d = Decoder::make(fin, "text/xml");

		Node &n = d->getArray("names");
		Node &s = d->getObject("structure");

		

		fin.close();
	}
	catch (std::runtime_error &e) {
		std::cerr << "runtime error: " << e.what() << std::endl;
	}
	catch (Error *e) {
		std::cerr << "*** " << e->what() << std::endl;
	}

	return 0;
}
