#include <iostream>
#include <string>

#include "iparser.h"
#include "argparser.h"

namespace arg = argparser;

const char help[] = R"(
MLP Solver application
======================

--ifile
	TSP instance filename.
	default: "brazil58.tsp"
)";

struct options_t
{
	std::string ifile;
};

static options_t options;

int main(int argc, char** argv)
{
	arg::parse(argc, argv, options, help)
		.bind("ifile", &options_t::ifile, arg::def("brazil58.tsp"));

	std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
	auto iparser = InstanceParser::Open(ifilepath);

	std::cout << "Parsing instance " << options.ifile << "... ";
	auto instance_opt = iparser->Parse();

	if (!instance_opt)
		return 1;

	std::cout << "OK\n";
	auto instance = *instance_opt;
	// solve...

	return 0;
}