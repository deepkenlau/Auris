/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <auris/aux/mapfile.hpp>
#include <auris/file/Index.hpp>
#include <auris/Object.hpp>

#include <string>
#include <set>

namespace auris {
namespace tools {

using namespace auris::aux;
using std::string;
using std::set;

class db_rev_list : public Generic
{
public:
	db_rev_list(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return ""; }

	int main()
	{
		// Read refs/index manually.
		string initial_hash;
		if (!mapfile::maybe_read(dbs.ref("index").path.c_str(), initial_hash)) {
			cerr << "no index found\n";
			return 1;
		}

		// Walk over the index revisions.
		string hash = initial_hash;
		while (!hash.empty())
		{
			db::ObjectBuffer<db::file::Index> index(dbs);
			index.read(hash);
			cout << hash << '\n';
			hash = index.parent;
		}

		return 0;
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_rev_list t(argc, argv);
	return t.run();
}