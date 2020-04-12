#pragma once

#include <cstdlib>
#include <map>
#include <optional>
#include <string>
#include <sstream>

namespace argparser
{
	class arglist
	{
	public:
		arglist(int argc, char** argv);
		const std::string operator[](const std::string key) const;
	private:
		bool match_argument(std::string name);
	private:
		std::map<std::string, std::string> map;
	};

	template<class Class>
	struct def
	{
	public:
		explicit def(Class const& value) : value(value) {}
		Class const& operator*() const { return value; }
	private:
		Class const& value;
	};

	template<class Class>
	class parser
	{
	public:
		parser(arglist& list, Class& handle) :
			list(list), handle(handle) {}

		template<
			class _Class,
			class _Type,
			class... _Extra
		> parser& bind(const std::string name, _Type _Class::*field,
			_Extra... extras) {
			std::string argval = list[name];
			pre_extras(name, field, extras...);
			if (!argval.empty())
				std::istringstream(argval) >> handle.*field;
			return *this;
		}
	private:
		template<
			class _Class,
			class _Type
		> void pre_extras(const std::string name, _Type _Class::* field) {}

		template<
			class _Class,
			class _Type,
			class _Def,
			class... _Extra
		> void pre_extras(const std::string name, _Type _Class::* field,
			def<_Def>& value, _Extra... extras) {
			handle.*field = *value;
			pre_extras(name, field, extras...);
		}
	private:
		arglist list;
		Class& handle;
	};

	template<class Class>
	parser<Class> parse(int argc, char** argv, Class& obj,
		const char* help = nullptr) {
		try {
			return parser<Class>(arglist(argc, argv), obj);
		} catch (std::runtime_error& e) {
			if (help != nullptr) {
				std::cout << e.what()
				          << std::endl
				          << help << std::endl;
				exit(EXIT_FAILURE);
			} else {
				throw; // Can't help!
			}
		}
	}
}