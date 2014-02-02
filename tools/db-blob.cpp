/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <auris/aux/sha1.hpp>
#include <auris/aux/Date.hpp>
#include <auris/aux/mapfile.hpp>
#include <auris/file/Object.hpp>
#include <auris/file/Index.hpp>
#include <auris/file/Track.hpp>
#include <auris/Object.hpp>

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
			("delete,d", "delete the blob from the track");
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
				print_blob(cout, *it);
				cout << '\n';
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
			fs::path path(opt_add);
			if (!fs::exists(path)) {
				cerr << path << " does not exist\n";
				return 1;
			}

			std::stringstream file_buffer;
			mapfile::read(path.c_str(), file_buffer);
			string file_hash = sha1().from_stream(file_buffer).hex();
			file_buffer.clear();
			file_buffer.seekg(0);

			for (set<db::file::Track::Blob>::const_iterator it = track.blobs.begin(); it != track.blobs.end(); it++) {
				if ((*it).blob_ref == file_hash) {
					cerr << "track already contains " << file_hash << '\n';
					return 1;
				}
			}

			db::file::Track::Blob blob(file_hash, "", path.filename().native());
			if (vm.count("format")) blob.format = opt_format;
			if (vm.count("name")) blob.orig_name = opt_name;
			track.blobs.insert(blob);

			std::ofstream f(dbs.object(file_hash).prime().path.c_str());
			db::file::Object file_object;
			file_object.type = "blob";
			file_object.write(f);
			std::copy(
				std::istreambuf_iterator<char>(file_buffer),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(f));

			track_modified = true;
			cerr << "# + "; print_blob(cerr, blob); cerr << '\n';
			cout << "blob " << file_hash << '\n';
		}
		if (!track_modified && (vm.count("format") || vm.count("name"))) {
			if (blob == track.blobs.end()) {
				cerr << "no blob specified to be modified\n";
				return 1;
			}
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
			index.date = Date().raw();

			map<string,string>::iterator it = index.tracks.find(track.id);
			if (it == index.tracks.end() || it->second != track.hash_in) {
				cerr << "track " << nice_hash(track.id) << " is not in the index\n";
				return 1;
			}
			it->second = track.hash_out;

			track.write();
			index.write();

			cout << "track " << track.hash_out << '\n';
			cout << "index " << index.hash_out << '\n';
		}

		return 0;
	}

private:
	void print_blob(std::ostream& o, const db::file::Track::Blob& b)
	{
		o << nice_hash(b.blob_ref);
		if (!b.format.empty())
			o << ' ' << b.format;
		if (!b.orig_name.empty())
			o << " -- " << b.orig_name;
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_blob t(argc, argv);
	return t.run();
}