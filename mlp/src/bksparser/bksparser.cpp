#include "bksparser.h"

#include <stdexcept>
#include <regex>
#include <fstream>

std::map<std::string, Cost> BKSParser::bks_map;
std::shared_ptr<BKSParser> BKSParser::instance = std::shared_ptr<BKSParser>(new BKSParser());

BKSParser::BKSParser()
{
	std::string bksfile = std::string(DATAPATH) + "/bks.txt";
	std::ifstream fs;
	fs.open(bksfile);
	if (!fs.is_open())
		throw std::runtime_error("BKS file not found");
	std::string line;
	int line_cnt = 1;
	while (std::getline(fs, line)) {
		const std::regex rgx("([^ \t]+)[ \t]+(\\d+)");
		std::smatch match;
		if (std::regex_match(line, match, rgx) &&
			match.size() > 2) {
			std::string name = match[1];
			std::string bks_str = match[2];
			Cost bks = stoull(bks_str); // Hard-coded (string to ull)
			bks_map.insert(std::make_pair(name, bks));
		} else {
			throw std::runtime_error("Ill-formed line " + line_cnt);
		}
		++line_cnt;
	}
}

std::shared_ptr<BKSParser> BKSParser::getInstance()
{
	return instance;
}

std::optional<Cost> BKSParser::getInstanceBKS(std::string name)
{
	auto entry = bks_map.find(name);
	if (entry == bks_map.end())
		return std::nullopt;
	else
		return entry->second;
}