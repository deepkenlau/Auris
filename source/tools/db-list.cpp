/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <aux/mapfile.hpp>
#include <db/file/Object.hpp>
#include <db/file/Index.hpp>

#include <string>
#include <set>

namespace auris {
namespace tools {

using namespace auris::aux;
using std::string;
using std::set;

/**
 * @brief Tool that adds files to the database.
 */
class db_list : public Generic
{
public:
	db_list(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return ""; }

	int main()
	{
		string index_ref;
		if (!mapfile::maybe_read(dbs.ref("index").path.c_str(), index_ref))
			return 0;

		std::ifstream index_ifs(dbs.object(index_ref).path.c_str());
		if (!index_ifs.good())
			throw std::runtime_error("index does not exist");
		db::file::Index index;
		index.read(index_ifs);

		cerr << "# index " << nice_hash(index_ref) << ", " << index.tracks.size() << " tracks\n";
		cerr << "# changed on " << index.date << '\n';
		for (set<string>::const_iterator it = index.tracks.begin(); it != index.tracks.end(); it++) {
			cout << nice_hash(*it) << '\n';
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