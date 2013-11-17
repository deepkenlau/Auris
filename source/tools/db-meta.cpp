/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <common/sha1.hpp>
#include <common/Date.hpp>
#include <aux/mapfile.hpp>
#include <db/file/Object.hpp>
#include <db/file/Index.hpp>
#include <db/file/Track.hpp>

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
		string track_ref = dbs.resolve_name(opt_track);

		std::ifstream track_ifs(dbs.object(track_ref).path.c_str());
		if (!track_ifs.good())
			throw std::runtime_error("cannot open track " + track_ref);

		db::file::Object track_object;
		track_object.read(track_ifs);
		if (track_object.type != "track")
			throw std::runtime_error("object " + nice_hash(track_ref) + " is not a track");

		auris::db::file::Track track;
		track.read(track_ifs);
		track_ifs.close();

		// Display mode, in case no fields are to be set or deleted.
		if (opt_set_fields.empty() && opt_delete_fields.empty())
		{
			cerr << "# track " << nice_hash(track_ref) << " (" << track.formats.size() << " formats)\n";
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
			for (vector<string>::iterator it = opt_delete_fields.begin(); it != opt_delete_fields.end(); it++) {
				track.md.erase(*it);
			}
			for (vector<string>::iterator it = opt_set_fields.begin(); it != opt_set_fields.end(); it++) {
				size_t sep = (*it).find('=');
				if (sep == string::npos)
					throw std::runtime_error("malformatted field '" + *it + "'");
				track.md[(*it).substr(0,sep)] = (*it).substr(sep+1);
			}

			db::file::Object track_object;
			track_object.type = "track";

			std::stringstream track_buffer;
			track_object.write(track_buffer);
			track.write(track_buffer);
			string track_hash = auris::sha1().from_stream(track_buffer).hex();
			if (track_hash == track_ref)
				return 0; // equal hashes means no changes, so no need to update the index
			track_buffer.clear();
			track_buffer.seekg(0);

			std::string index_ref;
			auris::db::file::Index index;
			if (mapfile::maybe_read(dbs.ref("index").path.c_str(), index_ref)) {
				std::ifstream f(dbs.object(index_ref).path.c_str());
				if (!f.good())
					throw std::runtime_error("index does not exist");

				db::file::Object index_object;
				index_object.read(f);
				if (index_object.type != "index")
					throw std::runtime_error("object " + nice_hash(index_ref) + " is not an index");

				index.read(f);
				index.base = index_ref;
			}
			index.date = auris::Date().str();

			set<string>::iterator it = index.tracks.find(track_ref);
			if (it == index.tracks.end()) {
				if (index_ref.empty()) {
					cerr << "no index in place";
				} else {
					cerr << "index " << nice_hash(index_ref) << " does not contain track " << opt_track << '\n';
				}
				return 1;
			}
			index.tracks.erase(it);
			index.tracks.insert(track_hash);

			db::file::Object index_object;
			index_object.type = "index";

			std::stringstream index_buffer;
			index_object.write(index_buffer);
			index.write(index_buffer);
			string index_str = index_buffer.str();
			string index_hash = auris::sha1().from_stream(index_buffer).hex();
			index_buffer.clear();
			index_buffer.seekg(0);

			mapfile::write(dbs.object(track_hash).prime().path.c_str(), track_buffer);
			mapfile::write(dbs.object(index_hash).prime().path.c_str(), index_buffer);
			mapfile::write(dbs.ref("index").prime().path.c_str(), index_hash);

			cout << "track: " << track_hash << '\n';
			cout << "index: " << index_hash << '\n';
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