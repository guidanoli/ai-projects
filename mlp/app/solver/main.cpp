#include <filesystem>
#include <iostream>
#include <string>

#include "ils.h"
#include "iparser.h"
#include "argparser.h"
#include "solution.h"

namespace arg = argparser;
namespace fs = std::filesystem;

const char help[] = R"(
MLP Solver application
======================
By default, the application opens the instance file and begins with the trivial
initial solution. But you can also load an already existing solution with the
'isfile' parameter.)";

struct options_t
{
	std::string ifile;
	std::string ifolder;
	std::string sfile;
	std::string heuristic;
	unsigned long long max_iterations = 0;
	unsigned long long max_seconds = 0;
	unsigned int seed = 0;
	std::size_t gammak = 0;
	float gap_threshhold = 0;
	bool does_save = false;
	bool verbose = true;
	bool validate = false;
	float ils_perturbation_factor = 0;
	std::string savefolder;
	std::string savefilename;

	bool stop_ils(IterationStatus const& status) const {
		if (validate &&
			!status.solution.IsValid()) {
			std::cout << "Solution isn't valid.\n";
			return true;
		}
		if (max_iterations &&
			status.iteration_id > max_iterations) {
			std::cout << "Exceeded maximum iteration count of "
				<< max_iterations << std::endl;
			return true;
		}
		if (max_seconds &&
			status.duration > max_seconds) {
			std::cout << "Exceeded maximum elapsed time of "
				<< max_seconds << " seconds\n";
			return true;
		}
		auto gap_opt = status.solution.GetCostGap();
		if (gap_opt && *gap_opt >= gap_threshhold) {
			std::cout << "Exceeded gap threshhold of "
				<< gap_threshhold * 100 << "%\n";
			return true;
		}
		if (verbose)
			print_gap(status.solution);
		return false;
	}

	bool solve(Solution &solution) const {
		if (heuristic == "ils") {
			IteratedLocalSearch ils(seed);
			std::cout << "Starting ILS...\n";
			auto end_solution = ils.explore(solution,
				ils_perturbation_factor,
				[*this] (IterationStatus const& status) {
					return stop_ils(status);
				});
			std::cout << "End of ILS...\n";
			print_gap(end_solution);
			if (does_save) {
				std::cout << "Saving solution in "
					<< savefolder << "/" << savefilename << std::endl;
				if (!save(end_solution))
					std::cerr << "It was not possible to save solution.\n";
			}
		} else {
			std::cerr << "Unknwon heuristic named '" << heuristic << "'.\n";
		}
		return true;
	}

	bool save(Solution const& solution) const {
		if (!does_save || savefolder.empty()) return false;
		auto savepath = fs::path(DATAPATH) / savefolder / savefilename;
		std::ofstream ofs(savepath, std::ios::out);
		return (bool) (ofs << solution);
	}

	void print_gap(Solution const& solution) const {
		auto gap_opt = solution.GetCostGap();
		if (gap_opt) std::cout << "Gap = " << *gap_opt * 100 << "%\n";
	}
};

int main(int argc, char** argv)
{
	options_t options;

	arg::build_parser(argc, argv, options, help)

		.bind("ifile", &options_t::ifile,
			arg::doc("TSP instance file path"))

		.bind("ifolder", &options_t::ifolder,
			arg::doc("TSP instance file folder"))

		.bind("sfile", &options_t::sfile,
			arg::doc("TSP solution file path"))

		.bind("save", &options_t::does_save,
			arg::doc("Save every new solution"),
			arg::def(false))

		.bind("savefolder", &options_t::savefolder,
			arg::doc("Output folder for solutions"))

		.bind("verbose", &options_t::verbose,
			arg::doc("Verbose output"),
			arg::def(false))
		
		.bind("heuristic", &options_t::heuristic,
			arg::doc("Solving heuristic"),
			arg::def("ils"))

		.bind("seed", &options_t::seed,
			arg::doc("Random seed"),
			arg::def(2020))

		.bind("gap", &options_t::gap_threshhold,
			arg::doc("Gap threshold for stopping"))

		.bind("ils-perturbation", &options_t::ils_perturbation_factor,
			arg::doc("Pertubation factor of ILS"),
			arg::def(0.25f))

		.bind("max-iterations", &options_t::max_iterations,
			arg::doc("Maximum number of iterations"),
			arg::def(3000))

		.bind("max-seconds", &options_t::max_seconds,
			arg::doc("Maximum time elapsed (in seconds)"),
			arg::def(60))

		.bind("gamma-k", &options_t::gammak,
			arg::doc("Gamma set size"))

		.bind("validate", &options_t::validate,
			arg::doc("Check if solution is valid every iteration"))

		.build();

	if (!options.ifile.empty()) {
		std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
		auto iparser = InstanceParser::Open(ifilepath);
		std::cout << "Parsing instance " << options.ifile << "... ";
		auto instance_opt = iparser->Parse();
		std::cout << (instance_opt ? "OK" : "ERROR") << std::endl;
		if (!instance_opt)
			return 1;
		auto instance = *instance_opt;
		if (options.gammak)
			instance->SetK(options.gammak);
		Solution solution(instance);
		options.savefilename = options.ifile + ".sol";
		options.solve(solution);
	}

	if (!options.sfile.empty()) {
		std::cout << "Parsing solution " << options.sfile << "... ";
		std::string sfilepath = std::string(DATAPATH) + "/" + options.sfile;
		std::ifstream ifs(sfilepath);
		Solution solution;
		bool success = (bool) (ifs >> solution);
		std::cout << (success ? "OK" : "ERROR") << std::endl;
		if (!success)
			return 1;
		if (options.gammak)
			solution.GetInstance()->SetK(options.gammak);
		options.savefilename = options.sfile;
		options.solve(solution);
	}

	if (!options.ifolder.empty()) {
		auto sdirpath = std::string(DATAPATH) + "/" + options.ifolder;
		for (const auto& entry : fs::directory_iterator(sdirpath)) {
			auto path = entry.path();
			if (path.extension() != ".tsp")
				continue; // Accept only tsp instances
			auto instance_path = path.string();
			auto iparser = InstanceParser::Open(instance_path);
			std::cout << "Parsing instance " << path.filename() << "... ";
			auto instance_ptr_opt = iparser->Parse();
			std::cout << (instance_ptr_opt ? "OK" : "ERROR") << std::endl;
			if (!instance_ptr_opt) continue; // Ignore errors
			auto instance_ptr = *instance_ptr_opt;
			if (options.gammak)
				instance_ptr->SetK(options.gammak);
			Solution solution(instance_ptr);
			options.savefilename = instance_path + ".sol";
			options.solve(solution);
			std::cout << std::endl;
		}
	}
	return 0;
}
