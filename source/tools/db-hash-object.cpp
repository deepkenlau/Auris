/* Copyright (c) 2013 Fabian Schuiki */
#include "Generic.hpp"
#include <common/sha1.hpp>
#include <aux/mapfile.hpp>
#include <string>

namespace auris {
namespace tools {

using std::string;

class db_hash_object : public Generic
{
public:
	string opt_file;
	string opt_type;

	db_hash_object(int argc, char **argv): Generic(argc, argv) { opt_type = "blob"; }
	const char* usage_string() const { return "<file>"; }

	void build_options()
	{
		po::options_description parameters("Parameters");
		parameters.add_options()
			("stdin", "read from standard input")
			("write,w", "write object to database")
			("type,t", po::value<string>(&opt_type), "file type annotated in object");
		options.add(parameters);

		hidden_options.add_options()
			("file", po::value<string>(&opt_file), "file to hash");
		positional_options.add("file", -1);
	}

	int main()
	{
		bool opt_stdin = vm.count("stdin");
		bool opt_write = vm.count("write");
		if (opt_file.empty() && !opt_stdin) {
			cerr << "no file specified\n";
			return 1;
		}

		std::stringstream buffer;
		buffer << opt_type;
		buffer.put(0);

		if (opt_stdin) {
			std::copy(
				std::istreambuf_iterator<char>(std::cin),
				std::istreambuf_iterator<char>(),
				std::ostreambuf_iterator<char>(buffer));
		} else {
			aux::mapfile::read(opt_file.c_str(), buffer);
		}

		string hash = sha1().from_stream(buffer).hex();

		if (opt_write) {
			fs::path pgrp = repo/"objects"/hash.substr(0,2);
			fs::path pobj = pgrp/hash.substr(2);
			ensure_dir(pgrp);
			buffer.clear();
			buffer.seekg(0);
			aux::mapfile::write(pobj.c_str(), buffer);
		}

		cout << hash << '\n';
		return 0;
	}

private:
	void ensure_dir(fs::path dir)
	{
		if (!fs::exists(dir)) {
			fs::create_directory(dir);
		} else if (!fs::is_directory(dir)) {
			throw std::runtime_error(dir.native() + " is not a directory");
		}
	}
};

} // namespace tools
} // namespace auris

int main(int argc, char *argv[])
{
	auris::tools::db_hash_object t(argc, argv);
	return t.run();
}