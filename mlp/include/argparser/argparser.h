#pragma once

#include <optional>
#include <map>
#include <memory>
#include <string>
#include <variant>

namespace argparser
{
	using argval = std::variant<std::string, bool>;

	class arglist
	{
	public:
		static std::optional<std::shared_ptr<arglist>>
			init(int argc, char const* const* argv);

		std::optional<argval> get(const std::string key) const {
			auto entry = map.find(key);
			if (entry == map.end())
				return std::nullopt;
			return entry->second;
		}
	private:
		arglist() = default;
		bool match_argument(std::string arg);
	private:
		std::map<std::string, argval> map;
	};
}