/* Copyright (c) 2013 Fabian Schuiki */
#include <common/sha1.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <string>
#include <vector>

namespace po = boost::program_options;
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

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
		cout << "running db-add on repo " << opt_repository << endl;
		for (int i = 0; i < opt_files.size(); i++) {
			char hex[41];
			cout << "- adding " << opt_files[i];
			auris::sha1().from_file(opt_files[i].c_str()).hex(hex);
			cout << " " << hex << endl;
		}

		return 0;
	}
};

int main(int argc, char *argv[])
{
	ThisTool t(argc, argv);
	return t.run();
}