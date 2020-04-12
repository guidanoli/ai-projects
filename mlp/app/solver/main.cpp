#include <iostream>
#include <string>

#include "iparser.h"
#include "argparser.h"

namespace arg = argparser;

const char help[] = R"(
MLP Solver application
======================

--ifile
	MLP instance filename.
	default: "brazil58.tsp"
)";

struct mlp
{
	std::string ifile;
};

static mlp args;

int main(int argc, char** argv)
{
	arg::parse(argc, argv, args, help)
		.bind("ifile", &mlp::ifile, arg::def("brazil58.tsp"));

	std::string ifilepath = std::string(DATAPATH) + "/" + args.ifile;
	auto iparser = InstanceParser::Open(ifilepath);

	std::cout << "Parsing instance " << args.ifile << "... ";
	auto instance_opt = iparser->Parse();

	if (!instance_opt)
		return 1;

	std::cout << "OK\n";
	auto instance = *instance_opt;
	// solve...

	return 0;
}