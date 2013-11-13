/* Copyright (c) 2013 Fabian Schuiki */
#include <common/sha1.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <sstream>

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
			visible_options.add(general);
			build_options();

			// Parse the command line options.
			po::store(po::command_line_parser(argc, argv)
				.options(options)
				.positional(positional_options)
				.run(), vm);
			po::notify(vm);

			// Show the help if required.
			if (vm.count("help")) {
				print_usage();
				cout << visible_options << endl;
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
	po::options_description visible_options;
	po::positional_options_description positional_options;
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
			("file", po::value< vector<string> >(&opt_files), "file to add");

		options.add(parameters);
		positional_options.add("file", -1);
	}

	int main()
	{
		// Create the index directory if required.
		fs::path index_path(opt_repository);
		index_path /= "index";
		if (!fs::exists(index_path)) {
			fs::create_directory(index_path);
		} else if (!fs::is_directory(index_path)) {
			throw std::runtime_error("Index at " + index_path.native() + " is not a directory.");
		}

		// Process each input file individually.
		for (int i = 0; i < opt_files.size(); i++) {
			char file_hash[41];
			auris::sha1().from_file(opt_files[i].c_str()).hex(file_hash);
			cout << "+ " << opt_files[i] << " " << file_hash << endl;

			fs::path path(opt_files[i]);
			stringstream meta;
			meta << "Title: " << path.filename().native() << "\n";
			meta << "Extension: " << path.extension().native() << "\n";
			meta << "Added: " << "some_random_date" << "\n";
			meta << "\n" << file_hash << "\n";
			string metadata = meta.str();

			char meta_hash[41];
			auris::sha1().from_string(metadata).hex(meta_hash);

			string file_name("+");
			file_name += file_hash;
			fs::path index_file_path = index_path / file_name;
			fs::path index_meta_path = index_path / meta_hash;

			cout << "file goes to " << index_file_path << endl;
			cout << "meta goes to " << index_meta_path << endl;
		}

		return 0;
	}
};

int main(int argc, char *argv[])
{
	ThisTool t(argc, argv);
	return t.run();
}