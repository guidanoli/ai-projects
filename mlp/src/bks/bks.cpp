#include "bks.h"

#include <stdexcept>
#include <regex>
#include <fstream>

std::map<std::string, int> BKS::bks_map;
std::shared_ptr<BKS> BKS::instance = std::shared_ptr<BKS>(new BKS());

BKS::BKS()
{
	std::string bksfile = std::string(DATAPATH) + "/bks.txt";
	std::ifstream fs;
	fs.open(bksfile);
	if (!fs.is_open())
		throw std::runtime_error("BKS file not found");
	std::string line;
	int line_cnt = 1;
	while (std::getline(fs, line)) {
		const std::regex rgx("([^ \t]+)[ \t]*:[ \t]*(\\d+)");
		std::smatch match;
		if (std::regex_match(line, match, rgx) &&
			match.size() > 2) {
			std::string name = match[1];
			std::string bks_str = match[2];
			int bks = stoi(bks_str);
			bks_map.insert(std::make_pair(name, bks));
		} else {
			throw std::runtime_error("Ill-formed line " + line_cnt);
		}
		++line_cnt;
	}
}

std::shared_ptr<BKS> BKS::getInstance()
{
	return instance;
}

std::optional<int> BKS::getInstanceBKS(std::string name)
{
	auto entry = bks_map.find(name);
	if (entry == bks_map.end())
		return std::nullopt;
	else
		return entry->second;
}