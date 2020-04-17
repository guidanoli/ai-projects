#pragma once

#include <string>
#include <map>
#include <memory>
#include <optional>

class BKS
{
public:
	static std::shared_ptr<BKS> getInstance();
	std::optional<int> getInstanceBKS(std::string name);
private:
	BKS();
	static std::shared_ptr<BKS> instance;
	static std::map<std::string, int> bks_map;
};