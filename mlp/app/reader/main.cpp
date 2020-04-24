#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>

#include "argparser.h"
#include "solution.h"

namespace arg = argparser;
namespace fs = std::filesystem;

const char help[] = R"doc(
MLP Reader application
======================

Reads solution files and streams information
in the comma-separated values format.
)doc";

template<class Arg, class... Args>
void print_csv_line(Arg arg, Args... args)
{
	std::cout << arg << ",";
	print_csv_line(args...);
}

template<class Arg>
void print_csv_line(Arg arg)
{
	std::cout << arg << std::endl;
}

void print_csv_line()
{
	std::cout << std::endl;
}

struct options_t
{
	std::string sfolder;

	void display_solution_info(Solution const& solution)
	{
		auto name = solution.GetInstance()->GetName();
		auto gap_opt = solution.GetCostGap();
		if (gap_opt) {
			print_csv_line(name, *gap_opt);
		} else {
			print_csv_line(name, "?");
		}
	}
};

int main(int argc, char** argv)
{
	options_t options;

	arg::build_parser(argc, argv, options, help)
		
		.bind("sfolder", &options_t::sfolder,
			arg::doc("Solution folder path"));

	if (!options.sfolder.empty()) {
		print_csv_line("Instance", "Gap");
		auto sdirpath = std::string(DATAPATH) + "/" + options.sfolder;
		for (const auto& entry : fs::directory_iterator(sdirpath)) {
			auto path = entry.path();
			if (path.extension() != ".sol")
				continue; // Accept only tsp instances
			Solution solution;
			std::ifstream fs(path.string());
			if (!(fs >> solution)) {
				print_csv_line(path.filename(), "?");
				continue; // Do not stop iterating
			}
			options.display_solution_info(solution);
		}
	}

	return 0;
}