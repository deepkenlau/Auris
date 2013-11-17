/* Copyright (c) 2013 Fabian Schuiki */
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <db/Structure.hpp>
#include <iostream>
#include <string>

namespace auris {
namespace tools {

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using std::cout;
using std::cerr;

/**
 * @brief Common structure of command line utilities.
 */
class Generic
{
public:
	int argc;
	char **argv;
	po::variables_map vm;

	std::string opt_repository;

	Generic(int argc, char **argv): argc(argc), argv(argv)
	{
		opt_repository = ".";
	}

	/**
	 * @brief Prints the usage line of the tool.
	 *
	 * The output is of the form "usage: <argv[0]> <usage_string>", where
	 * argv[0] corresponds to the command that was used to launch the tool and
	 * usage_string is the result of the usage_string() method which has to be
	 * overridden by subclasses.
	 */
	void print_usage() const
	{
		cout << "usage: " << *argv << ' ' << usage_string() << '\n';
	}

	/**
	 * @brief Parses the command line options and runs the tool.
	 *
	 * This function first initializes the command line option description with
	 * general commands, then calls the subclass' build_options() function to
	 * add the actual tool's options to the description.
	 *
	 * Then the command line options are parsed, taking into account the
	 * positional arguments specified. The general options are processed (e.g.
	 * a request for --help) and the subclass' main() function is finally
	 * called.
	 *
	 * Note that this function catches exceptions, prints them to the standard
	 * error and returns the error code 1.
	 *
	 * @return The return value of the subclass' main() function.
	 */
	int run()
	{
		try {
			// Configure the general command line options.
			po::options_description general("General options");
			general.add_options()
				("help,h", "show help message")
				("repository,r", po::value<std::string>(&opt_repository), "path of repository to operate on");
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
				cout << options << '\n';
				return 1;
			}

			// Call the subclass' main function.
			repo = opt_repository;
			dbs = db::Structure(repo);
			return main();

		} catch (std::exception &e) {
			cerr << e.what() << '\n';
			return 1;
		}
		return 0;
	}

	/**
	 * @brief Overridden by subclasses to provide the usage string.
	 *
	 * If the --help option is passed on the command line or the tool calls the
	 * print_usage() function, the string "usage: <argv[0]> <usage_string>" is
	 * printed to the console, where argv[0] is the command that was used to
	 * launch the tool and usage_string is the result of the usage_string()
	 * function.
	 */
	virtual const char * usage_string() const = 0;

	/**
	 * @brief Overridden by subclasses to provide their command line options.
	 *
	 * This function gives subclasses the opportunity to declare their own
	 * command line options. This should be done by manipulating either the
	 * 'options' or 'hidden_options' member variable, with the former appearing
	 * on the tool's help page and the latter being hidden from it.
	 *
	 * For positional options, the subclass may manipulate the
	 * 'positional_options' member. Refer to the boost::program_options manual
	 * for more information.
	 */
	virtual void build_options() {};

	/**
	 * @brief Overridden by subclasses to implement their actual functionality.
	 *
	 * This function is called within run(), and subclasses should implement
	 * all their functionality herein. Exceptions that are thrown inside main()
	 * are printed to standard error and will cause the program to abort with
	 * exit code 1.
	 *
	 * @return The return value of the tool. run() simply forwards this value.
	 */
	virtual int main() = 0;

protected:
	po::options_description options;
	po::options_description hidden_options;
	po::positional_options_description positional_options;

	fs::path repo;
	db::Structure dbs;

	std::string nice_hash(const std::string& in)
	{
		return in.substr(0, 7);
	}
};

} // namespace tools
} // namespace auris