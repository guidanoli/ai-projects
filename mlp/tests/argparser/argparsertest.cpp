#include "argparser.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>

using namespace argparser;

using test_input = std::vector<std::string>;
using argument_value = std::optional<argval>;
using argument_entry = std::pair<std::string, argument_value>;
using test_output = std::vector<argument_entry>;

std::vector<std::pair<test_input, test_output>> testcases = {
	// Test case #1
	{
		// Input
		{ "program", "-abc", "--k=v", "--key=value", "--f", "--flag" },

		// Output
		{
			{ "abc", std::nullopt },
			{ "a", true },
			{ "b", true },
			{ "c", true },
			{ "d", std::nullopt },
			{ "k", std::string("v") },
			{ "key", std::string("value") },
			{ "j", std::nullopt },
			{ "flag", true },
			{ "f", true },
			{ "v", std::nullopt },
		}
	},
	// Test case #2
	{
		// Input
		{ "program", "-h", "--h", "--h=TRUE" },

		// Output
		{
			{ "h", true },
		}
	},
	// Test case #3
	{
		// Input
		{ "program", "-h", "--h=TRUE", "--h" },

		// Output
		{
			{ "h", true },
		}
	},
	// Test case #4
	{
		// Input
		{ "program", "--h", "-h", "--h=TRUE" },

		// Output
		{
			{ "h", true },
		}
	},
	// Test case #5
	{
		// Input
		{ "program", "--h=TRUE", "-h", "--h" },

		// Output
		{
			{ "h", std::string("TRUE") },
		}
	},
	// Test case #6
	{
		// Input
		{ "program", "--h", "--h=TRUE", "-h" },

		// Output
		{
			{ "h", true },
		}
	},
	// Test case #7
	{
		// Input
		{ "program", "--h=TRUE", "--h", "-h" },

		// Output
		{
			{ "h", std::string("TRUE") },
		}
	},
};

char** get_argv(std::vector<std::string> const& strings)
{
	auto argn = strings.size();
	char** args = new char* [argn];
	for (std::size_t i = 0; i < argn; ++i) {
		auto strsz = strings[i].size();
		args[i] = new char[strsz + 1];
		for (std::size_t j = 0; j < strsz; ++j)
			args[i][j] = strings[i][j];
		args[i][strsz] = '\0';
	}
	return args;
}

void destroy_argv(char** argv, std::size_t n)
{
	if (!argv)
		return;
	for (std::size_t i = 0; i < n; ++i)
		delete[] argv[i];
	delete[] argv;
}

class argv_t
{
public:
	static std::shared_ptr<argv_t> init(
		std::vector<std::string> const& strings) {
		return std::shared_ptr<argv_t>(new argv_t(strings));
	}
	const char* const* get() { return argv; }
	~argv_t() { destroy_argv(argv, n); }
private:
	argv_t(std::vector<std::string> const& strings)
		: argv(get_argv(strings)), n(strings.size()) {}
	std::size_t n;
	char** argv;
};

int main(int argc, char** argv)
{
	if (argc == 1) {
		// Testing mode
		for (auto& [input, output] : testcases) {
			int argn = (int) input.size();
			auto args_ptr = argv_t::init(input);
			auto arglist_opt = arglist::init(argn, args_ptr->get());
			assert(arglist_opt);
			auto arglist = *arglist_opt;
			for (auto entry : output) {
				auto [key, expected] = entry;
				auto actual = arglist->get(key);
				assert(actual == expected);
			}
		}
	} else {
		// Interactive mode
		auto arglist_opt = arglist::init(argc, argv);
		if (!arglist_opt) {
			std::cerr << "Invalid arguments!" << std::endl;
			return 1;
		}
		auto arglist = *arglist_opt;
		auto show_opt = arglist->get("show");
		if (!show_opt) {
			std::cout << "Hint: add a flag --show=<arg> to show an "
			          << "argument <arg> value.\n";
			return 0;
		}
		auto show = *show_opt;
		try {
			auto showstr = std::get<std::string>(show);
			auto arg_opt = arglist->get(showstr);
			if (!arg_opt) {
				std::cout << "Argument " << showstr << " is not set\n";
			}  else {
				try {
					auto argstr = std::get<std::string>(*arg_opt);
					std::cout << "Argument " << showstr << " is (std::string) "
						<< argstr << ".\n";
				} catch (std::bad_variant_access&) {
					try {
						auto argbool = std::get<bool>(*arg_opt);
						std::cout << "Argument " << showstr << " is (bool) "
							<< (argbool ? "true" : "false") << ".\n";
					} catch (std::bad_variant_access&) {
						std::cerr << "Something went wrong...\n";
						assert(false);
						return 1;
					}
				}
			}
		} catch (std::bad_variant_access&) {
			std::cerr << "Hint: the flag --show must be valored.\n";
			return 1;
		}
	}
	return 0;
}