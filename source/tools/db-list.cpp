/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <db/file/Index.hpp>
#include <db/Object.hpp>

#include <string>
#include <map>

namespace auris {
namespace tools {

using namespace auris::aux;
using std::string;
using std::map;

/**
 * @brief Tool that adds files to the database.
 */
class db_list : public Generic
{
public:
	db_list(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return ""; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("verbose,v", "show additional information");
		options.add(parameters);
	}

	int main()
	{
		bool opt_verbose = vm.count("verbose");

		db::ObjectBuffer<db::file::Index> index(dbs);
		index.maybe_ref("index");

		if (opt_verbose) {
			cout << "# index " << nice_hash(index.hash_in) << ", " << index.tracks.size() << " tracks\n";
			cout << "# changed on " << index.date << '\n';
		}
		for (map<string, string>::const_iterator it = index.tracks.begin(); it != index.tracks.end(); it++) {
			cout << nice_hash(it->first) << ' ' << nice_hash(it->second) << '\n';
		}

		return 0;
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_list t(argc, argv);
	return t.run();
}