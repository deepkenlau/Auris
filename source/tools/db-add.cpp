/* Copyright (c) 2013 Fabian Schuiki */
#include <common/sha1.hpp>
#include <common/uuid.hpp>
#include <db/Index.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
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
};

class ThisTool : public DatabaseTool
{
public:
	vector<string> opt_files;
	bool opt_import_duplicates;

	ThisTool(int argc, char **argv): DatabaseTool(argc, argv) {}
	const char* usage_string() { return "file1 ..."; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("duplicates,d", "import duplicate files");
		options.add(parameters);

		hidden_options.add_options()
			("file", po::value< vector<string> >(&opt_files), "file to add");
		positional_options.add("file", -1);
	}

	int main()
	{
		opt_import_duplicates = vm.count("duplicates");
		int duplicates_skipped = 0; // increased for every duplicate that was skipped

		// Create the index directory if required.
		fs::path stage_dir = fs::path(opt_repository)/"stage";
		if (!fs::exists(stage_dir)) {
			fs::create_directory(stage_dir);
		} else if (!fs::is_directory(stage_dir)) {
			throw std::runtime_error("Index at " + stage_dir.native() + " is not a directory.");
		}

		// Read the index if it already exists.
		auris::db::Index index;
		fs::path index_path = stage_dir / "index";
		if (fs::exists(index_path)) {
			std::ifstream f(index_path.c_str());
			index.read(f);
		}

		// Process each input file individually.
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

			fs::path staged_file_path = stage_dir / file_hash;
			fs::path repo_file_path = fs::path(opt_repository)/"objects"/file_hash;

			if (!opt_import_duplicates && fs::exists(repo_file_path)) {
				duplicates_skipped++;
				cerr << "skipping " << path.native() << " (duplicate)\n";
				continue;
			}
			if (!opt_import_duplicates && fs::exists(staged_file_path)) {
				duplicates_skipped++;
				cerr << "skipping " << path.native() << " (already in index)\n";
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
			meta << "Added: " << date << "\n";
			meta << "\n" << file_hash << " -- " << path.filename().native() << "\n";
			string metadata = meta.str();

			// Calculate the hash of the metadata to be added.
			string meta_hash = auris::sha1().from_string(metadata).hex();

			// Add the file and metadata to the index.
			fs::path staged_meta_path = stage_dir / meta_hash;

			fs::copy(path, staged_file_path);
			std::ofstream fmeta(staged_meta_path.c_str());
			fmeta << metadata;
			fmeta.close();

			index.additions.insert(file_hash);
			index.additions.insert(meta_hash);

			// Print a line showing what was added.
			cout << meta_hash.substr(0,8) << " " << uuid << " " << path.native() << "\n";
		}

		// Write the index.
		std::ofstream f(index_path.c_str());
		index.write(f);
		f.close();

		// Print a helpful message in case a duplicate was skipped.
		if (duplicates_skipped) {
			cerr << "# Some files were not imported since they already exist in the library or were already added to the index. Add the --duplicates option to force them to be added.\n";
		}

		return 0;
	}
};

int main(int argc, char *argv[])
{
	ThisTool t(argc, argv);
	return t.run();
}