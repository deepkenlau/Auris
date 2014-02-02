/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <common/sha1.hpp>
#include <common/uuid.hpp>
#include <common/Date.hpp>
#include <db/file/Object.hpp>
#include <db/file/Index.hpp>
#include <db/file/Track.hpp>
#include <aux/mapfile.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <map>

namespace auris {
namespace tools {

using namespace auris::aux;
using std::string;
using std::vector;
using std::set;
using std::map;
using std::stringstream;

/**
 * @brief Tool that adds files to the database.
 */
class db_add : public Generic
{
public:
	vector<string> opt_files;
	vector<string> opt_remove;

	db_add(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() const { return "file1 ..."; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("duplicates,d", "import duplicate files")
			("remove", po::value< vector<string> >(&opt_remove), "remove track");
		options.add(parameters);

		hidden_options.add_options()
			("file", po::value< vector<string> >(&opt_files), "file to add");
		positional_options.add("file", -1);
	}

	void ensure_dir(fs::path dir)
	{
		if (!fs::exists(dir)) {
			fs::create_directory(dir);
		} else if (!fs::is_directory(dir)) {
			throw std::runtime_error(dir.native() + " is not a directory");
		}
	}

	int main()
	{
		bool opt_import_duplicates = vm.count("duplicates");
		int duplicates_skipped = 0; // increased for every duplicate that was skipped
		bool index_modified = false; // set to true if the index was modified

		// Read the index. If there is no current index (i.e. the repository is
		// empty), simply do nothing and keep an empty index object.
		db::file::Index index;
		string orig_index_ref;
		if (mapfile::maybe_read(dbs.ref("index").path.c_str(), orig_index_ref)) {
			std::ifstream f(dbs.object(orig_index_ref).path.c_str());
			if (!f.good())
				throw std::runtime_error("index does not exist");
			index.read(f);
		}

		// Process each input file individually.
		for (int i = 0; i < opt_files.size(); i++)
		{
			fs::path path(opt_files[i]);
			if (!fs::exists(path)) {
				cerr << path << " does not exist\n";
				continue;
			}

			// Read the entire input file into a buffer. Calculate the file
			// hash and rewind the buffer to the start of the file, since the
			// hashing moves the cursor position to the file's end.
			stringstream file_buffer;
			mapfile::read(path.c_str(), file_buffer);
			string file_hash = auris::sha1().from_stream(file_buffer).hex();
			file_buffer.clear();
			file_buffer.seekg(0);

			if (!opt_import_duplicates && dbs.object(file_hash).exists()) {
				duplicates_skipped++;
				cerr << "skipping " << path.native() << " (duplicate)\n";
				continue;
			}

			// Build the initial metadata entry.
			auris::db::file::Track track;
			track.id = sha1().from_string(auris::uuid::generate()).hex();
			track.md["title"] = path.filename().native();
			track.md["added"] = Date().raw();
			track.blobs.insert(auris::db::file::Track::Blob(file_hash, "", path.filename().native()));

			stringstream track_buffer;
			track.write(track_buffer);
			string track_hash = auris::sha1().from_stream(track_buffer).hex();
			track_buffer.clear();
			track_buffer.seekg(0);

			// Modify the index, write the file to disk.
			index.tracks[track.id] = track_hash;
			index_modified = true;

			if (!dbs.object(file_hash).exists())
			{
				std::ofstream f(dbs.object(file_hash).prime().path.c_str());

				db::file::Object file_object;
				file_object.type = "blob";
				file_object.write(f);

				std::copy(
					std::istreambuf_iterator<char>(file_buffer),
					std::istreambuf_iterator<char>(),
					std::ostreambuf_iterator<char>(f));
			}
			mapfile::write(dbs.object(track_hash).prime().path.c_str(), track_buffer);

			// Print a line showing what was added.
			cout << track.id << ' ' << nice_hash(track_hash) << '\n';
		}

		// Remove tracks.
		for (int i = 0; i < opt_remove.size(); i++)
		{
			string n = opt_remove[i];
			if (n.size() < 3) {
				cerr << "name '" << n << "' is too short\n";
				continue;
			}

			map<string,string>::iterator it = index.tracks.lower_bound(n);
			if (it != index.tracks.end()
				&& it->first.size() >= n.size()
				&& it->first.substr(0, n.size()) == n) {
				index.tracks.erase(it);
				index_modified = true;
			} else {
				for (it = index.tracks.begin(); it != index.tracks.end(); it++) {
					if (it->second.size() >= n.size()
						&& it->second.substr(0, n.size()) == n) {
						index.tracks.erase(it);
						index_modified = true;
					}
				}
			}
		}

		// In case the index was modified, we need to write it to disk again.
		if (index_modified) {
			index.date = Date().raw();
			index.parent = orig_index_ref;

			stringstream index_buffer;
			index.write(index_buffer);
			string index_hash = auris::sha1().from_stream(index_buffer).hex();
			index_buffer.clear();
			index_buffer.seekg(0);

			if (index_hash != orig_index_ref) {
				mapfile::write(dbs.object(index_hash).prime().path.c_str(), index_buffer);
				mapfile::write(dbs.ref("index").prime().path.c_str(), index_hash);
			}
		}

		// Print a helpful message in case a duplicate was skipped.
		if (duplicates_skipped) {
			cerr <<
			"# Some files were not imported since they already exist in the library or were\n"
			"# already added to the index. Add the --duplicates option to force them to be\n"
			"# added.\n";
		}

		return 0;
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_add t(argc, argv);
	return t.run();
}