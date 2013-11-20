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
 * @brief Tool that displays and modifies a track's blobs.
 */
class db_blob : public Generic
{
public:
	string opt_track;
	string opt_blob;
	string opt_format;
	string opt_name;
	string opt_add;
	string opt_delete;

	db_blob(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return "<track> [<blob>]"; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("list,l", "list the track's blobs")
			("format,f", po::value<string>(&opt_format), "change the format string of the blob")
			("name,n", po::value<string>(&opt_name), "change the original name hint of the blob")
			("add,a", po::value<string>(&opt_add), "add the given file to the track")
			("delete,d", po::value<string>(&opt_delete), "delete the given blob from the track");
		options.add(parameters);

		hidden_options.add_options()
			("track", po::value<string>(&opt_track), "track of interest")
			("blob", po::value<string>(&opt_blob), "blob of interest");
		positional_options.add("track", 1);
		positional_options.add("blob", 1);
	}

	int main()
	{
		bool opt_list = vm.count("list");

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
		string resolved_track;
		map<string,string>::const_iterator it = index.tracks.lower_bound(opt_track);
		if (it != index.tracks.end()
			&& it->first.size() >= opt_track.size()
			&& it->first.substr(0, opt_track.size()) == opt_track) {
			resolved_track = it->second;
		} else {
			resolved_track = dbs.resolve_name(opt_track);
		}

		// Read the track specified by the user.
		db::ObjectBuffer<db::file::Track> track(dbs);
		track.read(resolved_track);

		// Resolve the blob if it has been set.
		set<db::file::Track::Blob>::iterator blob = track.blobs.end();
		if (!opt_blob.empty()) {
			for (set<db::file::Track::Blob>::iterator it = track.blobs.begin(); it != track.blobs.end(); it++) {
				if ((*it).blob_ref.size() >= opt_blob.size()
					&& (*it).blob_ref.substr(0, opt_blob.size()) == opt_blob) {
					blob = it;
					break;
				}
			}
			if (blob == track.blobs.end()) {
				cerr << "track " << opt_track << " has no blob " << opt_blob << '\n';
				return 1;
			}
		}

		if (opt_list) {
			for (set<db::file::Track::Blob>::const_iterator it = track.blobs.begin(); it != track.blobs.end(); it++) {
				cout << nice_hash((*it).blob_ref);
				if (!(*it).format.empty())
					cout << ' ' << (*it).format;
				if (!(*it).orig_name.empty())
					cout << " -- " << (*it).orig_name << '\n';
			}
			return 0;
		}

		bool track_modified = false;
		if (vm.count("delete")) {
			if (blob == track.blobs.end()) {
				cerr << "no blob specified to be deleted\n";
				return 1;
			}
			track.blobs.erase(blob);
			blob = track.blobs.end();
			track_modified = true;
		}
		if (vm.count("add")) {
			cout << "would now add blob " << opt_add << " (format " << opt_format << ", orig_name " << opt_name << ")\n";
			track_modified = true;
		}
		if (!track_modified) {
			db::file::Track::Blob b = *blob;
			if (vm.count("format")) {
				b.format = opt_format;
				track_modified = true;
			}
			if (vm.count("name")) {
				b.orig_name = opt_name;
				track_modified = true; 
			}
			if (track_modified) {
				track.blobs.erase(blob);
				track.blobs.insert(b);
				blob = track.blobs.end();
			}
		}

		if (track_modified) {
			track.fill_buffer();
			if (track.hash_in == track.hash_out)
				return 0;

			index.parent = index.hash_in;
			index.date = Date().str();

			map<string,string>::iterator it = index.tracks.find(track.id);
			if (it == index.tracks.end() || it->second != track.hash_in) {
				cerr << "track " << nice_hash(track.id) << " is not in the index\n";
				return 1;
			}
			it->second = track.hash_out;

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
	auris::tools::db_blob t(argc, argv);
	return t.run();
}