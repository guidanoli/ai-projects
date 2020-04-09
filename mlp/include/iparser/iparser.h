#pragma once
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "instance.h"

using MapValueType = std::variant<std::string, int, float>;
using MapEntry = std::pair<std::string, std::string>;

class InstanceParser
{
public:
	template<typename... Args>
	static std::shared_ptr<InstanceParser> Open(Args... args)
	{
		return std::shared_ptr<InstanceParser>(new InstanceParser(args...));
	}
	std::optional<Instance> Parse();
	~InstanceParser();
private:
	InstanceParser(std::string const& filename);
	bool ParseSpecification(Instance instance, MapEntry entry);
	bool ParseData(Instance instance, std::string key);
private:
	std::ifstream fs;
	std::map<std::string, MapValueType> entry_map;
};