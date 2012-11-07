#include <cassert>
#include <iostream>
#include <gc_cpp.h>
#include <stdexcept>
#include <common/Error.h>
#include <common/coding/Decoder.h>
#include <fstream>

using std::runtime_error;
using coding::Decoder;


int main(int argc, char *argv[])
{
	GC_INIT();
	try {
		std::ifstream fin("/Users/fabian/Desktop/output.xml");

		Decoder *d = Decoder::make(fin, "text/xml");
		Decoder::Object *root = d->getRootObject();

		Decoder::Array *names = root->getArray("names");
		Decoder::Object *structure = root->getObject("structure");

		std::cout << "name: " << structure->getValue<std::string>("name") << std::endl;
		std::cout << "test: " << structure->getValue<std::string>("test") << std::endl;
		std::cout << "someBool: " << structure->getValue<bool>("someBool") << std::endl;
		std::cout << "aFloat: " << structure->getValue<double>("aFloat") << std::endl;

		std::string s;
		while (names->getValue(s)) {
			std::cout << "- name '" << s << "'" << std::endl;
		}

		fin.close();
	}
	catch (std::runtime_error &e) {
		std::cerr << "runtime error: " << e.what() << std::endl;
	}
	catch (std::exception &e) {
		std::cerr << "exception: " << e.what() << std::endl;
	}
	catch (Error *e) {
		std::cerr << "*** " << e->what() << std::endl;
	}

	return 0;
}
