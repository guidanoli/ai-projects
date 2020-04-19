#include "argparser.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>

using namespace argparser;

using test_input = std::vector<std::string>;
using test_output = std::vector<std::pair<std::string, std::string>>;

std::vector<std::pair<test_input, test_output>> testcases = {
	// Test case #0
	{
		// Input
		{ "program", "-abc", "--k=v", "--key=value", "--f", "--flag" },

		// Output
		{
			{ "abc",  "" },
			{ "a",    "1" },
			{ "b",    "1" },
			{ "c",    "1" },
			{ "d",    "" },
			{ "k",    "v" },
			{ "key",  "value" },
			{ "j",    "" },
			{ "flag", "1" },
			{ "f",    "1" },
			{ "v",    "" },
		}
	},
	// Test case #1
	{
		// Input
		{ "program", "-h", "--h", "--h=0" },

		// Output
		{
			{ "h", "1" },
		}
	},
	// Test case #2
	{
		// Input
		{ "program", "-h", "--h=0", "--h" },

		// Output
		{
			{ "h", "1" },
		}
	},
	// Test case #3
	{
		// Input
		{ "program", "--h", "-h", "--h=0" },

		// Output
		{
			{ "h", "1" },
		}
	},
	// Test case #4
	{
		// Input
		{ "program", "--h=0", "-h", "--h" },

		// Output
		{
			{ "h", "0" },
		}
	},
	// Test case #5
	{
		// Input
		{ "program", "--h", "--h=0", "-h" },

		// Output
		{
			{ "h", "1" },
		}
	},
	// Test case #6
	{
		// Input
		{ "program", "--h=0", "--h", "-h" },

		// Output
		{
			{ "h", "0" },
		}
	},
	// Test case #7
	{
		// Input
		{ "program", "--answer=42", "--pi=3.141592", "--e=2.71828182845904",
		  "--bool", "--l=-1", "--ul=1", "--ll=-1", "--ull=1", "--key=val" },

		// Output
		{
			{ "answer", "42" },
			{ "bool",   "1" },
			{ "pi",     "3.141592" },
			{ "e",      "2.71828182845904" },
			{ "l",      "-1" },
			{ "ul",     "1" },
			{ "ll",     "-1" },
			{ "ull",    "1" },
			{ "key",    "val" },
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
	char** get() { return argv; }
	~argv_t() { destroy_argv(argv, n); }
private:
	argv_t(std::vector<std::string> const& strings)
		: argv(get_argv(strings)), n(strings.size()) {}
	std::size_t n;
	char** argv;
};

struct A
{
	std::string a, b, c; // cf
	std::string f, flag; // mcf
	std::string k, key;  // vf
	std::string z = "x"; // defaults
};

struct B
{
	int i = 0;
	bool b = 0;
	long l = 0;
	double d = 0;
	float f = 0;
	long long ll = 0;
	unsigned long ul = 0;
	unsigned long long ull = 0;
	std::string s;
};

int main(int argc, char** argv)
{
	if (argc == 1) {
		// Testing mode

		// arglist
		for (auto& [input, output] : testcases) {
			int argn = (int) input.size();
			auto args_ptr = argv_t::init(input);
			auto list = arglist(argn, args_ptr->get());
			for (auto entry : output) {
				auto [key, expected] = entry;
				auto actual = list[key];
				assert(actual == expected);
			}
		}

		// argparser
		{
			// Test case #0
			auto& [input, output] = testcases[0];
			int argn = (int) input.size();
			auto args_ptr = argv_t::init(input);

			A obj;
			build_parser(argn, args_ptr->get(), obj)
				.bind("a",    &A::a)
				.bind("b",    &A::b)
				.bind("c",    &A::c)
				.bind("f",    &A::f)
				.bind("flag", &A::flag)
				.bind("k",    &A::k)
				.bind("key",  &A::key);

			assert(obj.a == "1");
			assert(obj.b == "1");
			assert(obj.c == "1");
			assert(obj.f == "1");
			assert(obj.flag == "1");
			assert(obj.k == "v");
			assert(obj.key == "value");
			assert(obj.z == "x");
		}
		{
			// Test case #7
			auto& [input, output] = testcases[7];
			int argn = (int) input.size();
			auto args_ptr = argv_t::init(input);

			B obj;
			build_parser(argn, args_ptr->get(), obj)
				.bind("pi",     &B::f)
				.bind("e",      &B::d)
				.bind("answer", &B::i)
				.bind("bool",   &B::b)
				.bind("l",      &B::l)
				.bind("ul",     &B::ul)
				.bind("ll",     &B::ll)
				.bind("ull",    &B::ull)
				.bind("key",    &B::s);

			assert(abs(obj.f - 3.141592f) < 1E-7);
			assert(abs(obj.d - 2.71828182845904) < 1E-16);
			assert(obj.i == 42);
			assert(obj.b == true);
			assert(obj.l == -1);
			assert(obj.ul == 1);
			assert(obj.ll == -1);
			assert(obj.ull == 1);
			assert(obj.s == "val");
		}

	} else {
		// Interactive mode
		try {
			B object;
			
			auto args =
				build_parser(argc, argv, object)
				.bind("f",   &B::f,   def(3.141592f))
				.bind("d",   &B::d,   def(2.71828182845904))
				.bind("i",   &B::i,   def(42))
				.bind("b",   &B::b,   def(true))
				.bind("l",   &B::l,   def(-1))
				.bind("ul",  &B::ul,  def(1))
				.bind("ll",  &B::ll,  def(-1))
				.bind("ull", &B::ull, def(1))
				.bind("s",   &B::s,   def("string"));

			std::cout.precision(7);
			std::cout
				<< "float = " << object.f << std::endl;
			std::cout.precision(16);
			std::cout
				<< "double = " << object.d << std::endl
				<< "int = " << object.i << std::endl
				<< "boolean = " << object.b << std::endl
				<< "long = " << object.l << std::endl
				<< "unsigned long = " << object.ul << std::endl
				<< "long long = " << object.ll << std::endl
				<< "unsigned long long = " << object.ull << std::endl
				<< "string = " << object.s << std::endl;

		} catch (std::runtime_error& e) {
			std::cerr << e.what() << std::endl;
			return 1;
		}
	}
	return 0;
}