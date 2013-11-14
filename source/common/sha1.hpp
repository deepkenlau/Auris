/* Copyright © 2013 Fabian Schuiki */
#pragma once
#include <boost/uuid/sha1.hpp>
#include <istream>
#include <fstream>
#include <string>

namespace auris {

class sha1
{
public:
	sha1() {}
	sha1(const sha1& s): sha(s.sha) {}
	sha1(const char* buffer, size_t len) { from_buffer(buffer, len); }
	sha1(const std::string& str) { from_string(str); }
	sha1(std::istream& is) { from_stream(is); }

	sha1& from_buffer(const char* buffer, size_t len)
	{
	    sha.process_bytes(buffer, len);
		return *this;
	}

	sha1& from_string(const std::string& str)
	{
		sha.process_bytes(str.c_str(), str.length());
		return *this;
	}

	sha1& from_stream(std::istream& is)
	{
		if (!is.good()) throw std::runtime_error("input stream not good");
		char buffer[1024];
		while (is.good()) {
			is.read(buffer, sizeof(buffer));
			sha.process_bytes(buffer, is.gcount());
		}
		return *this;
	}

	sha1& from_file(const char* path)
	{
		std::ifstream f(path);
		return from_stream(f);
	}

	size_t raw(char *dst)
	{
		char *dst_start = dst;
		unsigned int hash[5];
		sha.get_digest(hash);
		for (int i = 0; i < 5; i++) {
			for (int n = 24; n >= 0; n -= 8) {
				*dst = (hash[i] >> n) & 0xFF;
				dst++;
			}
		}
		return dst - dst_start;
	}

	size_t hex(char *dst, bool trailing_zero = true)
	{
		char *dst_start = dst;
		static const char *lookup = "0123456789abcdef";
		unsigned int hash[5];
		sha.get_digest(hash);
		for (int i = 0; i < 5; i++) {
			for (int n = 28; n >= 0; n -= 4) {
				*dst = lookup[(hash[i] >> n) & 0xF];
				dst++;
			}
		}
		if (trailing_zero) {
			*dst = 0;
			dst++;
		}
		return dst - dst_start;
	}

	std::string hex()
	{
		std::string op;
		op.resize(40);
		hex(&op[0], false);
		return op;
	}

private:
	boost::uuids::detail::sha1 sha;
};

} // namespace auris