/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <common/sha1.hpp>
#include <common/Date.hpp>
#include <aux/mapfile.hpp>
#include <db/file/Object.hpp>
#include <db/file/Index.hpp>
#include <db/file/Track.hpp>
#include <db/Object.hpp>

#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>

namespace auris {
namespace tools {

using namespace auris::aux;
using std::string;
using std::map;
using std::set;
using std::vector;

/**
 * @brief Tool that displays and modifies tracks.
 */
class db_meta : public Generic
{
public:
	string opt_track;
	bool opt_only_metadata;
	bool opt_only_formats;
	vector<string> opt_fields_vector, opt_set_fields, opt_delete_fields;
	set<string> opt_fields;

	db_meta(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return "<track>"; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("only-metadata,m", "display only metadata")
			("only-formats,f", "display only formats")
			("field,n", po::value< vector<string> >(&opt_fields_vector), "filter metadata (multiple allowed)")
			("set-field,s", po::value< vector<string> >(&opt_set_fields), "change metadata (<field>=<value>)")
			("delete-field,d", po::value< vector<string> >(&opt_delete_fields), "delete metadata field");
		options.add(parameters);

		hidden_options.add_options()
			("track", po::value<string>(&opt_track), "track whose metadata is of interest");
		positional_options.add("track", -1);
	}

	int main()
	{
		opt_only_metadata = vm.count("only-metadata");
		opt_only_formats = vm.count("only-formats");
		opt_fields.insert(opt_fields_vector.begin(), opt_fields_vector.end());

		if (opt_track.empty()) {
			cerr << "no track specified\n";
			return 2;
		}

		// Read the track specified by the user.
		db::ObjectBuffer<db::file::Track> track(dbs);
		track.read(dbs.resolve_name(opt_track));

		// Display mode, in case no fields are to be set or deleted.
		if (opt_set_fields.empty() && opt_delete_fields.empty())
		{
			cerr << "# track " << nice_hash(track.hash_in) << " (" << track.formats.size() << " formats)\n";
			if (!opt_only_formats) {
				if (opt_fields.empty() || opt_fields.count("Id")) {
					cout << "Id: " << track.id << '\n';
				}
				for (map<string,string>::const_iterator it = track.md.begin(); it != track.md.end(); it++) {
					if (!opt_fields.empty() && !opt_fields.count(it->first))
						continue; // if field filter is set, skip fields it does not name
					cout << it->first << ": " << it->second << '\n';
				}
			}

			if (!opt_only_metadata && !opt_only_formats)
				cout << '\n';

			if (!opt_only_metadata) {
				for (set<auris::db::file::Track::Format>::const_iterator it = track.formats.begin(); it != track.formats.end(); it++) {
					cout << nice_hash((*it).blob_ref);
					if (!(*it).format.empty())
						cout << "  <" << (*it).format << '>';
					cout << "  " << (*it).orig_name << '\n';
				}
			}
		}

		// Modification mode, in case fields need to be changed or deleted.
		else
		{
			// Apply the modifications. Delete fields first, then set the
			// field values received as command line options.
			for (vector<string>::iterator it = opt_delete_fields.begin(); it != opt_delete_fields.end(); it++) {
				track.md.erase(*it);
			}
			for (vector<string>::iterator it = opt_set_fields.begin(); it != opt_set_fields.end(); it++) {
				size_t sep = (*it).find('=');
				if (sep == string::npos)
					throw std::runtime_error("malformatted field '" + *it + "'");
				track.md[(*it).substr(0,sep)] = (*it).substr(sep+1);
			}

			// Call fill_buffer() right away, since the modification of the
			// index that follows further down requires the track's new hash.
			track.fill_buffer();
			if (track.hash_in == track.hash_out)
				return 0;

			// Read the current index, pointed to by refs/index. The
			// ObjectBuffer will ensure that the ref is being updated whenever
			// a new version of the index is written to disk.
			db::ObjectBuffer<db::file::Index> index(dbs);
			index.maybe_ref("index"); // maybe_* since refs/index might not exist
			index.base = index.hash_in;
			index.date = Date().str();

			set<string>::iterator it = index.tracks.find(track.hash_in);
			if (it == index.tracks.end()) {
				cerr << "track " + nice_hash(track.hash_in) + " is not in the index\n";
				return 1;
			}
			index.tracks.erase(it);
			index.tracks.insert(track.hash_out);

			// Write the track and index to disk. index.write() will also cause
			// refs/index to be updated due to the maybe_ref() call at the top.
			track.write();
			index.write();

			cout << "track: " << track.hash_out << '\n';
			cout << "index: " << index.hash_out << '\n';
		}

		return 0;
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_meta t(argc, argv);
	return t.run();
}