#pragma once
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "instance.h"

using ValueType = std::variant<std::string, int, float>;

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
	bool ParseSpecification(Instance i, std::string key, std::string value);
	bool ParseData(Instance i, std::string key);
private:
	std::ifstream fs;
	std::map<std::string, ValueType> entry_map;
};