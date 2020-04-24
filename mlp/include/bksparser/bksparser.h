#pragma once

#include <string>
#include <map>
#include <memory>
#include <optional>

using Cost = long long;

class BKSParser
{
public:
	static std::shared_ptr<BKSParser> getInstance();
	std::optional<Cost> getInstanceBKS(std::string name);
private:
	BKSParser();
	static std::shared_ptr<BKSParser> instance;
	static std::map<std::string, Cost> bks_map;
};