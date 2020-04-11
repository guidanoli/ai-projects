#include "argparser.h"

#include <regex>

using namespace argparser;

// Grammar

const auto regex = std::regex(
"^-([^-]+)$|"         // eg: -v
"^--([^=]+)=(.+)$|"   // eg: --verbosity=2
"^--([^=]+)$"         // eg: --verbose
);

std::optional<std::shared_ptr<arglist>>
	arglist::init(int argc, char const* const* argv)
{
	auto ptr = std::shared_ptr<arglist>(new arglist());
	for (int i = 1; i < argc; ++i)
		if (!ptr->match_argument(argv[i]))
			return std::nullopt;
	return ptr;
}

bool arglist::match_argument(std::string arg)
{
	std::smatch match;
	if (!std::regex_match(arg, match, regex))
		return false;
	std::string c = match[1],
	            k = match[2],
	            v = match[3],
	            f = match[4];
	if (!c.empty()) {
		for (std::size_t i = 0; i < c.size(); ++i) {
			auto ch = c.substr(i, 1);
			map.insert(std::make_pair(ch, true));
		}
	} else if (!k.empty()) {
		map.insert(std::make_pair(k, v));
	} else if (!f.empty()) {
		map.insert(std::make_pair(f, true));
	} else {
		return false;
	}
	return true;
}