/* Copyright (c) 2013 Fabian Schuiki */
#include <common/sha1.hpp>
#include <common/uuid.hpp>
#include <common/Date.hpp>
#include <db/file/Index.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <set>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::set;
using std::stringstream;

class DatabaseTool
{
public:
	int argc;
	char **argv;
	po::variables_map vm;

	string opt_repository;

	DatabaseTool(int argc, char **argv): argc(argc), argv(argv)
	{
		opt_repository = ".";
	}

	void print_usage()
	{
		cout << "usage: " << *argv << ' ' << usage_string() << endl;
	}

	int run()
	{
		try {
			// Configure the general command line options.
			po::options_description general("General options");
			general.add_options()
				("help,h", "show help message")
				("repository,r", po::value<string>(&opt_repository), "path of repository to operate on");
			options.add(general);
			build_options();

			// Build the set of options which are used for parsing.
			po::options_description all_options;
			all_options.add(options).add(hidden_options);

			// Parse the command line options.
			po::store(po::command_line_parser(argc, argv)
				.options(all_options)
				.positional(positional_options)
				.run(), vm);
			po::notify(vm);

			// Show the help if required.
			if (vm.count("help")) {
				print_usage();
				cout << options << endl;
				return 1;
			}

			// Call the subclass' main function.
			return main();

		} catch (std::exception &e) {
			cerr << e.what() << endl;
			return 1;
		}
		return 0;
	}

	virtual const char * usage_string() = 0;
	virtual void build_options() {};
	virtual int main() = 0;

protected:
	po::options_description options;
	po::options_description hidden_options;
	po::positional_options_description positional_options;

	string nice_hash(const string& in)
	{
		return in.substr(0, 7);
	}
};

class ThisTool : public DatabaseTool
{
public:
	vector<string> opt_files;

	ThisTool(int argc, char **argv): DatabaseTool(argc, argv) {}
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
			time_t now = time(0);
			struct tm now_tm = *localtime(&now);
			char date[80];
			strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S %z", &now_tm);

			string uuid = auris::uuid::generate();
			stringstream meta;
			meta << "ID: " << uuid << "\n";
			meta << "Title: " << path.filename().native() << "\n";
			meta << "Added: " << auris::Date().str() << "\n";
			meta << "\n" << file_hash << " -- " << path.filename().native() << "\n";
			string metadata = meta.str();

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
			cout << meta_hash.substr(0,8) << " " << uuid << " " << path.native() << "\n";
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

int main(int argc, char *argv[])
{
	ThisTool t(argc, argv);
	return t.run();
}