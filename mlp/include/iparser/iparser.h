#pragma once
#include <fstream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "defines.h"
#include "instance.h"

using SharedInstance = std::shared_ptr<Instance>;
using SharedInstanceParser = std::shared_ptr<InstanceParser>;
using VarMapValueType = std::variant<std::string, int, double>;
using MapEntry = std::pair<std::string, std::string>;

class IDistanceMatrixBuilder;

class InstanceParser
{
public:
	static SharedInstanceParser Open(std::string const& filename);
	std::optional<SharedInstance> Parse();
	~InstanceParser();
private:
	InstanceParser(std::string const& filename);

	bool ParseSpecification(SharedInstance& instance, MapEntry entry);
	bool ParseData(SharedInstance& instance, std::string key);

	bool ParseDisplayData(SharedInstance& instance);
	bool ParseEdgeWeights(SharedInstance& instance);

	template<typename T>
	std::optional<T> GetEntryValue(std::string key) {
		auto entry = entry_map.find(key);
		if (entry == entry_map.end())
			return std::nullopt;
		try {
			return std::get<T>(entry->second);
		} catch (std::bad_variant_access&) {
			return std::nullopt;
		}
	}
private:
	std::ifstream fs;
	std::string filename;
	std::map<std::string, VarMapValueType> entry_map;
};