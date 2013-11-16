/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <db/file/Track.hpp>

#include <fstream>
#include <string>
#include <map>
#include <set>

namespace auris {
namespace tools {

using std::string;
using std::map;
using std::set;

/**
 * @brief Tool that displays and modifies tracks.
 */
class db_meta : public Generic
{
public:
	fs::path repo;
	string opt_track;

	db_meta(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return "<track>"; }

	void build_options()
	{
		hidden_options.add_options()
			("track", po::value<string>(&opt_track), "track whose metadata is of interest");
		positional_options.add("track", -1);
	}

	int main()
	{
		repo = opt_repository;

		if (opt_track.empty()) {
			cerr << "no track specified\n";
			return 2;
		}
		string track_ref = resolve_refish(opt_track);

		std::ifstream track_ifs((repo/"objects"/track_ref).c_str());
		if (!track_ifs.good())
			throw std::runtime_error("cannot open track " + track_ref);
		auris::db::file::Track track;
		track.read(track_ifs);

		cout << "# track " << nice_hash(track_ref) << " (" << track.formats.size() << " formats)\n";
		cout << "Id: " << track.id << '\n';
		for (map<string,string>::const_iterator it = track.md.begin(); it != track.md.end(); it++) {
			cout << it->first << ": " << it->second << '\n';
		}
		cout << '\n';
		for (set<auris::db::file::Track::Format>::const_iterator it = track.formats.begin(); it != track.formats.end(); it++) {
			cout << nice_hash((*it).blob_ref);
			if (!(*it).format.empty())
				cout << "  <" << (*it).format << '>';
			cout << "  " << (*it).orig_name << '\n';
		}

		return 0;
	}

private:
	string resolve_refish(const string& in)
	{
		if (in.empty())
			throw std::runtime_error("input reference empty");
		if (in[0] != '.') {
			if (fs::exists(repo/"objects"/in))
				return in;
			if (fs::exists(repo/"objects")) {
				fs::directory_iterator it, end;
				for (it = fs::directory_iterator(repo/"objects"); it != end; it++) {
					fs::path p = (*it).path();
					string n = p.filename().native();
					if (n.size() >= in.size() && n.substr(0, in.size()) == in)
						return n;
				}
			}
		}
		throw std::runtime_error("unknown reference " + in);
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_meta t(argc, argv);
	return t.run();
}