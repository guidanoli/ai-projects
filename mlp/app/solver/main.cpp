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
'isfile' parameter.)";

struct options_t
{
	std::string ifile;
	std::string sfile;
	// std::string heuristic;
	// unsigned int seed = 0;
	// float gap_threshhold = 0;
	bool does_save = false;
	bool new_solution = false;
	bool verbose = true;
	// float ils_perturbation_factor = 0;
	std::string savefile;

	bool save(Solution const& solution) {
		if (!does_save || savefile.empty()) return false;
		std::ofstream ofs(savefile, std::ios::out);
		return (bool) (ofs << solution);
	}

	void print_gap(Solution const& solution) {
		auto gap_opt = solution.GetCostGap();
		if (gap_opt && verbose)
			std::cout << "Gap = " << *gap_opt * 100 << "%\n";
	}
};

int main(int argc, char** argv)
{
	options_t options;

	arg::build_parser(argc, argv, options, help)

		.bind("ifile", &options_t::ifile,
			arg::doc("TSP instance file path"),
			arg::def("brazil58.tsp"))

		.bind("sfile", &options_t::sfile,
			arg::doc("TSP solution file path"))

		.bind("save", &options_t::does_save,
			arg::doc("Save every new solution"),
			arg::def(false))

		.bind("savefile", &options_t::savefile,
			arg::doc("Output filepath for new "
				     "solutions (*.sol for instances)"))

		.bind("verbose", &options_t::verbose,
			arg::doc("Verbose output"),
			arg::def(false))
		
		//.bind("heuristic", &options_t::heuristic,
		//	arg::doc("Solving heuristic"),
		//	arg::def("ils"))

		//.bind("seed", &options_t::seed,
		//	arg::doc("Random seed"),
		//	arg::def(2020))

		//.bind("gap", &options_t::gap_threshhold,
		//	arg::doc("Gap threshold for stopping"),
		//	arg::def(-0.5f))

		//.bind("ils_perturbation", &options_t::ils_perturbation_factor,
		//	arg::doc("Pertubation factor of ILS"),
		//	arg::def(0.25f))

		.abort_on("help");

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

		if (options.verbose)
			std::cout << "Parsing instance " << options.ifile << "... ";

		auto instance_opt = iparser->Parse();

		if (!instance_opt)
			return 1;

		if (options.verbose)
			std::cout << "OK\n";

		auto instance = *instance_opt;
		solution = Solution(instance);
	} else {
		if (options.verbose)
			std::cout << "Parsing solution " << options.sfile << "... ";

		std::string sfilepath = std::string(DATAPATH) + "/" + options.sfile;
		std::ifstream ifs(sfilepath);

		if (ifs >> solution) {
			if (options.verbose)
				std::cout << "OK\n";
		} else {
			return 1;
		}
	}

	options.print_gap(solution);

	if (options.does_save)
		std::string savepath = std::string(DATAPATH) + "/" + options.savefile;

	//auto stopping_criterion = [options] (Solution const& solution) {
	//	auto gap_opt = solution.GetCostGap();
	//	if (!gap_opt) return true;
	//	return (*gap_opt) >= options.gap_threshhold;
	//};

	//if (options.heuristic == "ils") {
	//	IteratedLocalSearch ils(options.seed);
	//	
	//	if (options.verbose)
	//		std::cout << "Starting ILS...\n";

	//	auto end_solution = ils.explore(solution,
	//	                                options.ils_perturbation_factor,
	//	                                stopping_criterion);

	//	if (options.verbose) {
	//		std::cout << "End of ILS...\n";
	//		options.print_gap(end_solution);
	//	}

	//	if (options.does_save) {
	//		std::cout << "Saving solution in "
	//		          << options.savefile << std::endl;
	//		if (!options.save(end_solution))
	//			std::cerr << "It was not possible to save solution.\n";
	//	}
	//} else {
	//	std::cerr << "Unknwon heuristic named '" << options.heuristic << "'.\n";
	//}

	return 0;
}
