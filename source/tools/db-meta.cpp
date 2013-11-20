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
	bool opt_only_blobs;
	vector<string> opt_fields_vector, opt_set_fields, opt_delete_fields;
	set<string> opt_fields;

	db_meta(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return "<track>"; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("only-metadata,m", "display only metadata")
			("only-blobs,b", "display only blobs")
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
		opt_only_blobs = vm.count("only-blobs");
		opt_fields.insert(opt_fields_vector.begin(), opt_fields_vector.end());

		if (opt_track.empty()) {
			cerr << "no track specified\n";
			return 2;
		}

		// Load the index as it is used by the name resolver and the rest of
		// the tool in case of a track modification.
		db::ObjectBuffer<db::file::Index> index(dbs);
		index.maybe_ref("index"); // maybe_* since refs/index might not exist

		// Resolve the given track name. First attempt to find a track id that
		// has the requested name as a prefix. If none is found, try the
		// regular structure-based resolution that looks for object hashes that
		// match the input.
		string resolved_name;
		map<string,string>::const_iterator it = index.tracks.lower_bound(opt_track);
		if (it != index.tracks.end()
			&& it->first.size() >= opt_track.size()
			&& it->first.substr(0, opt_track.size()) == opt_track) {
			resolved_name = it->second;
		} else {
			resolved_name = dbs.resolve_name(opt_track);
		}

		// Read the track specified by the user.
		db::ObjectBuffer<db::file::Track> track(dbs);
		track.read(resolved_name);

		// Display mode, in case no fields are to be set or deleted.
		if (opt_set_fields.empty() && opt_delete_fields.empty())
		{
			const int fw = 10;
			cerr << "# track " << nice_hash(track.hash_in) << " (" << track.blobs.size() << " blobs)\n";
			if (!opt_only_blobs) {
				if (opt_fields.empty() || opt_fields.count("id")) {
					cout.width(fw);
					cout << std::left << "id:" << ' ' << track.id << '\n';
				}
				for (map<string,string>::const_iterator it = track.md.begin(); it != track.md.end(); it++) {
					if (!opt_fields.empty() && !opt_fields.count(it->first))
						continue; // if field filter is set, skip fields it does not name
					cout.width(fw);
					cout << std::left << (it->first + ":") << ' ' << it->second << '\n';
				}
			}

			if (!opt_only_metadata && !opt_only_blobs)
				cout << '\n';

			if (!opt_only_metadata) {
				for (set<auris::db::file::Track::Blob>::const_iterator it = track.blobs.begin(); it != track.blobs.end(); it++) {
					cout << nice_hash((*it).blob_ref);
					if (!(*it).format.empty())
						cout << " <" << (*it).format << '>';
					cout << " " << (*it).orig_name << '\n';
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

			// Adjust the index such that a new revision is created, pointing
			// back to its current state on disk.
			index.parent = index.hash_in;
			index.date = Date().str();

			map<string, string>::iterator it = index.tracks.find(track.id);
			if (it == index.tracks.end() || it->second != track.hash_in) {
				cerr << "track " + track.id + " " + nice_hash(track.hash_in) + " is not in the index\n";
				return 1;
			}
			it->second = track.hash_out;

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