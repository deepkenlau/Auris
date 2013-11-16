/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"

#include <common/sha1.hpp>
#include <common/uuid.hpp>
#include <common/Date.hpp>
#include <db/file/Index.hpp>
#include <db/file/Track.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <set>

namespace auris {
namespace tools {

using std::string;
using std::vector;
using std::set;
using std::stringstream;

/**
 * @brief Tool that adds files to the database.
 */
class db_add : public Generic
{
public:
	vector<string> opt_files;

	db_add(int argc, char **argv): Generic(argc, argv) {}
	const char* usage_string() { return "file1 ..."; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("duplicates,d", "import duplicate files")
			("list,l", "list tracks in database");
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
		bool opt_list = vm.count("list");
		int duplicates_skipped = 0; // increased for every duplicate that was skipped

		// Useful paths
		fs::path dir = fs::path(opt_repository);
		ensure_dir(dir);
		ensure_dir(dir/"objects");
		ensure_dir(dir/"refs");

		// Read the tracks ref if it exists and read the tracks list it points
		// to. Otherwise create a new empty tracks list.
		auris::db::file::Index tracks;
		string tracks_ref;
		fs::path tracks_ref_path = dir/"refs"/"tracks";
		if (fs::exists(tracks_ref_path)) {
			std::ifstream ftr(tracks_ref_path.c_str());
			ftr >> tracks_ref;

			std::ifstream ft((dir/"objects"/tracks_ref).c_str());
			tracks.read(ft);
		}

		// List all tracks and abort if so requested.
		if (opt_list) {
			cerr << "# index " << nice_hash(tracks_ref) << " (" << tracks.date << ")\n";
			for (set<string>::const_iterator it = tracks.tracks.begin(); it != tracks.tracks.end(); it++) {
				cout << nice_hash(*it) << "\n";
			}
			return 0;
		}

		// Process each input file individually.
		bool tracks_modified = false;
		for (int i = 0; i < opt_files.size(); i++)
		{
			fs::path path(opt_files[i]);
			if (!fs::exists(path)) {
				cerr << path << " does not exist" << endl;
				continue;
			}

			// Calculate the hash of the file to be added, skipping the file if
			// it already exists in the library.
			string file_hash = auris::sha1().from_file(path.c_str()).hex();
			fs::path db_file_path = dir/"objects"/file_hash;

			if (!opt_import_duplicates && fs::exists(db_file_path)) {
				duplicates_skipped++;
				cerr << "skipping " << path.native() << " (duplicate)\n";
				continue;
			}

			// Build the initial metadata entry.
			auris::db::file::Track meta;
			meta.id = auris::uuid::generate();
			meta.md["Title"] = path.filename().native();
			meta.md["Added"] = auris::Date().str();
			meta.formats.insert(auris::db::file::Track::Format(file_hash, "", path.filename().native()));
			stringstream meta_buffer;
			meta.write(meta_buffer);
			string metadata = meta_buffer.str();

			// Calculate the hash of the metadata to be added and write the
			// data to tisk.
			string meta_hash = auris::sha1().from_string(metadata).hex();
			tracks.tracks.insert(meta_hash);
			tracks_modified = true;
			fs::path db_meta_path = dir/"objects"/meta_hash;

			if (!fs::exists(db_file_path)) fs::copy(path, db_file_path);
			std::ofstream f(db_meta_path.c_str());
			f << metadata;
			f.close();

			// Print a line showing what was added.
			cout << nice_hash(meta_hash) << " " << meta.id << " " << path.native() << "\n";
		}

		// Write the new tracks file.
		if (tracks_modified) {
			tracks.date = auris::Date().str();
			tracks.base = tracks_ref;
			stringstream tracks_stream;
			tracks.write(tracks_stream);
			string tracks_string = tracks_stream.str();
			string tracks_hash = auris::sha1().from_string(tracks_string).hex();

			if (tracks_hash != tracks_ref) {
				std::ofstream ftracks((dir/"objects"/tracks_hash).c_str());
				ftracks << tracks_string;
				ftracks.close();

				std::ofstream ftr(tracks_ref_path.c_str());
				ftr << tracks_hash << "\n";
				ftr.close();

				if (!tracks_ref.empty()) {
					fs::remove(dir/"objects"/tracks_ref);
				}
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