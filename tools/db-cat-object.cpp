/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <auris/aux/mapfile.hpp>
#include <auris/Structure.hpp>
#include <auris/file/Object.hpp>

#include <string>
#include <fstream>
#include <sstream>

namespace auris {
namespace tools {

using namespace auris::aux;
using std::string;

class db_hash_cat : public Generic
{
public:
	string opt_write;
	string opt_object;

	db_hash_cat(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return "<object>"; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("type,t", "show type of object")
			("write,w", po::value<string>(&opt_write), "write object to file instead of stdout");
		options.add(parameters);

		hidden_options.add_options()
			("object", po::value<string>(&opt_object), "name of object to show");
		positional_options.add("object", -1);
	}

	int main()
	{
		bool opt_type = vm.count("type");
		if (opt_object.empty()) {
			cerr << "no object specified\n";
			return 1;
		}

		std::ifstream fobj(dbs.object(dbs.resolve_name(opt_object)).path.c_str());
		if (!fobj.good()) {
			cerr << "cannot open object " << fobj << " for reading\n";
			return 3;
		}

		auris::db::file::Object object;
		object.read(fobj);

		if (opt_type) {
			cout << object.type << '\n';
			return 0;
		}

		if (opt_write.empty()) {
			std::copy(
				std::istreambuf_iterator<char>(fobj),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(std::cout));
		} else {
			mapfile::write(opt_write.c_str(), fobj);
		}
		return 0;
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_hash_cat t(argc, argv);
	return t.run();
}