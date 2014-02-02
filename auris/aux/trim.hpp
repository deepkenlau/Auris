/* Copyright (c) 2013 Fabian Schuiki */
#pragma once
#include <string>

namespace auris {
namespace aux {

template <typename C> std::string& ltrim(std::string &s, const C& cs)
{
	size_t p = s.find_first_not_of(cs);
	if (p == std::string::npos) {
		s.clear();
		return s;
	}
	s.erase(0, p);
	return s;
}

template <typename C> std::string& rtrim(std::string &s, const C& cs)
{
	size_t p = s.find_last_not_of(cs);
	if (p == std::string::npos) {
		s.clear();
		return s;
	}
	s.erase(p+1); // p points to the last character not in cs, so we start chopping of at p+1
	return s;
}

template <typename C> std::string& trim(std::string &s, const C& cs)
{
	ltrim(s, cs);
	rtrim(s, cs);
	return s;
}

template <typename Cl, typename Cr> std::string& trim(std::string &s, const Cl& csl, const Cr& csr)
{
	ltrim(s, csl);
	rtrim(s, csr);
	return s;
}

} // namespace aux
} // namespace trim