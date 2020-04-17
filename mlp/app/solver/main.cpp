#include <iostream>
#include <string>

#include "iparser.h"
#include "argparser.h"
#include "solution.h"

namespace arg = argparser;

const char help[] = R"(
MLP Solver application
======================

By default, the application opens the instance file and begins with the trivial
initial solution. But you can also load an already existing solution with the
'isfile' parameter.

--ifile=<path>
	TSP instance file path.
	default: "brazil58.tsp"

--sfile=<path>
	TSP solution file path.
	default: not defined

--save
	Save solution or not
	default: false

--savefile=<path>
	Path for saving solution
	default: <ifile>.sol | <sfile>
)";

struct options_t
{
	std::string ifile;
	std::string sfile;
	bool does_save = false;
	bool new_solution = false;
	std::string savefile;
};

static options_t options;

int main(int argc, char** argv)
{
	arg::parse(argc, argv, options, help)
		.bind("ifile", &options_t::ifile, arg::def("brazil58.tsp"))
		.bind("sfile", &options_t::sfile, arg::def(""))
		.bind("save", &options_t::does_save, arg::def(false))
		.bind("savefile", &options_t::savefile, arg::def(""));

	options.new_solution = options.sfile.empty();

	if (options.savefile.empty()) {
		if (options.new_solution)
			options.savefile = options.ifile + ".sol";
		else
			options.savefile = options.sfile;
	}

	Solution solution;

	if (options.new_solution) {
		std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
		auto iparser = InstanceParser::Open(ifilepath);

		std::cout << "Parsing instance " << options.ifile << "... ";
		auto instance_opt = iparser->Parse();

		if (!instance_opt)
			return 1;

		std::cout << "OK\n";
		auto instance = *instance_opt;
		solution = Solution(instance);
	} else {
		std::cout << "Parsing solution " << options.sfile << "... ";

		std::string sfilepath = std::string(DATAPATH) + "/" + options.sfile;
		std::ifstream ifs(sfilepath);

		if (ifs >> solution)
			std::cout << "OK\n";
		else
			return 1;
	}

	auto cost = solution.GetCost();
	std::cout << "Initial cost: " << cost << std::endl;

	if (options.does_save) {
		std::string savepath = std::string(DATAPATH) + "/" + options.savefile;
		std::ofstream ofs(savepath, std::ios::out);

		std::cout << "Saving solution in " << options.savefile << "... ";
		ofs << solution;
		bool good = ofs.good();
		ofs.close();

		if (good)
			std::cout << "OK\n";
		else
			return 1;
	}

	return 0;
}