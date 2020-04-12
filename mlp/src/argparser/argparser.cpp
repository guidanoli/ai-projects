#include "argparser.h"

#include <regex>

using namespace argparser;

// Grammar

const auto regex = std::regex(
"^-([^-]+)$|"         // eg: -v
"^--([^=]+)=(.+)$|"   // eg: --verbosity=2
"^--([^=]+)$"         // eg: --verbose
);

arglist::arglist(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
		if (!match_argument(argv[i]))
			throw std::runtime_error("invalid argument " +
			                         std::string(argv[i]));
}

const std::string arglist::operator[](const std::string key) const {
	auto const& entry = map.find(key);
	if (entry == map.end())
		return std::string("");
	return entry->second;
}

bool arglist::match_argument(std::string arg)
{
	std::smatch match;
	if (!std::regex_match(arg, match, regex))
		return false;
	std::string c = match[1], // cf
	            k = match[2], // key of vf
	            v = match[3], // value of vf
	            f = match[4]; // mcf
	if (!c.empty()) {
		for (std::size_t i = 0; i < c.size(); ++i) {
			auto ch = c.substr(i, 1);
			map.insert(std::make_pair(ch, "1"));
		}
	} else if (!k.empty()) {
		map.insert(std::make_pair(k, v));
	} else if (!f.empty()) {
		map.insert(std::make_pair(f, "1"));
	} else {
		return false;
	}
	return true;
}