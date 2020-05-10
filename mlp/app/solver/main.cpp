#include <filesystem>
#include <iostream>
#include <string>
#include <chrono>

#include "ils.h"
#include "genetic.h"
#include "population.h"

#include "csv.h"

#include "iparser.h"
#include "argparser.h"
#include "solution.h"

namespace arg = argparser;
namespace fs = std::filesystem;

const char help[] = R"(
MLP Solver application
======================
Solves an instance or a solution.
Heuristics:
- ils: Iterated Local Search
- gen: Genetic Algorithm
)";

struct options_t
{
	std::string ifile;
	std::string ifolder;
	std::string sfile;
	std::string heuristic;
	unsigned long long max_iterations_sli = 0;
	unsigned long long max_seconds_sli = 0;
	
	unsigned int seed = 0;
	std::size_t gammak = 0;
	float gap_threshhold = 0;
	bool does_save = false;
	bool verbose = true;
	bool validate = false;

	unsigned long long ils_decay_factor = 0;
	float ils_perturbation_factor = 0;

	std::size_t gen_minsize = 0;
	std::size_t gen_maxsize = 0;
	std::size_t gen_window = 0;
	std::size_t gen_mating_pool_size = 0;
	std::size_t gen_max_generations_sli = 0;
	std::size_t gen_max_generations = 0;
	double gen_mut_pmin = 0.0, gen_mut_pmax = 0.0, gen_mut = 0.0;
	unsigned long long gen_max_seconds = 0;

	std::string csvpath;
	std::string savefolder;
	std::string savefilename;

	char csvDecimalSeparator = 0;
	std::unique_ptr<csv::writer> csvWriter;

	bool stop_ils(IterationStatus const& status) const {
		if (validate &&
			!status.solution->IsValid()) {
			std::cout << "Solution isn't valid.\n";
			return true;
		}
		if (max_iterations_sli &&
			status.iteration_id > max_iterations_sli) {
			std::cout << "Exceeded maximum iteration count s.l.i. of "
				<< max_iterations_sli << std::endl;
			return true;
		}
		if (max_seconds_sli &&
			status.t_last_improvement > max_seconds_sli) {
			std::cout << "Exceeded maximum elapsed time s.l.i. of "
				<< max_seconds_sli << " s\n";
			return true;
		}
		auto gap_opt = status.solution->GetCostGap();
		if (gap_opt && *gap_opt >= gap_threshhold) {
			std::cout << "Exceeded gap threshhold of "
				<< gap_threshhold * 100 << "%\n";
			return true;
		}
		if (status.perturbationSize == 1) {
			std::cout << "Exceeded perturbation size limit of 1\n";
			return true;
		}
		if (verbose) {
			std::cout << "Phi = " << status.perturbationSize << " ";
			print_gap(*(status.solution));
		}
		return false;
	}

	bool stop_gen(PopulationStatus const& status) const {
		if (validate &&
			!status.best_solution->IsValid()) {
			std::cout << "Solution isn't valid.\n";
			return true;
		}
		if (gen_max_generations_sli &&
			status.generations_sli > gen_max_generations_sli) {
			std::cout << "Exceeded maximum generation s.l.i. count of "
				<< gen_max_generations_sli << std::endl;
			return true;
		}
		if (max_seconds_sli &&
			status.seconds_sli > max_seconds_sli) {
			std::cout << "Exceeded maximum elapsed time s.l.i. of "
				<< max_seconds_sli << " s\n";
			return true;
		}
		if (gen_max_generations &&
			status.generations > gen_max_generations) {
			std::cout << "Exceeded maximum generation count of "
				<< gen_max_generations << std::endl;
			return true;
		}
		if (gen_max_seconds &&
			status.seconds > gen_max_seconds) {
			std::cout << "Exceeded maximum elapsed time of "
				<< max_seconds_sli << " s\n";
			return true;
		}
		auto gap_opt = status.best_solution->GetCostGap();
		if (gap_opt && *gap_opt >= gap_threshhold) {
			std::cout << "Exceeded gap threshhold of "
				<< gap_threshhold * 100 << "%\n";
			return true;
		}
		return false;
	}

	void print_ils_status(IterationStatus const& status) {
		print_gap(*(status.solution));
		std::cout << "Total time = " << status.t << " s\n";
		write_csv_line(status.solution->GetInstance()->GetName(),
			status.solution->GetCostGap(),
			status.t);
	}

	void print_gen_status(PopulationStatus const& status) {
		print_gap(*(status.best_solution));
		std::cout << "Total time = " << status.seconds << " s\n";
		write_csv_line(status.best_solution->GetInstance()->GetName(),
			status.best_solution->GetCostGap(),
			status.seconds);
	}

	bool solve(Solution &solution) {
		if (heuristic == "ils") {
			IteratedLocalSearch ils(seed);
			std::cout << "Starting ILS...\n";
			auto status = ils.explore(solution,
				ils_perturbation_factor,
				ils_decay_factor,
				[this](IterationStatus const& status) {
				return stop_ils(status);
			});
			std::cout << "End of ILS...\n";
			print_ils_status(status);
			if (does_save) {
				std::cout << "Saving solution in "
					<< savefolder << "/" << savefilename << std::endl;
				if (!save(*(status.solution)))
					std::cerr << "It was not possible to save solution.\n";
			}
		} else if (heuristic == "gen") {
			auto instance_ptr = solution.GetInstance();
			auto pop = std::make_shared<Population>(instance_ptr,
				gen_minsize, gen_maxsize, gen_window, seed);
			pop->SetVerbosity(verbose);
			pop->SetMatingPoolSize(gen_mating_pool_size);
			pop->SetMutationChance(gen_mut);
			pop->SetMutationMin(gen_mut_pmin);
			pop->SetMutationMax(gen_mut_pmax);
			auto gen = Genetic(pop);
			std::cout << "Starting GEN...\n";
			auto status = gen.explore(
				[this] (PopulationStatus const& status) {
				return stop_gen(status);
			});
			std::cout << "End of GEN...\n";
			print_gen_status(status);
			if (does_save) {
				std::cout << "Saving solution in "
					<< savefolder << "/" << savefilename << std::endl;
				if (!save(*(status.best_solution)))
					std::cerr << "It was not possible to save solution.\n";
			}
		} else {
			std::cerr << "Unknwon heuristic named '" << heuristic << "'.\n";
			return false;
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

	void write_csv_ils_info() {
		if (!csvWriter) return;
		*csvWriter << "Heuristic" << "Iterated Local Search" << csv::nl
			<< "Iterations SLI (0 = undefined)" << max_iterations_sli << csv::nl
			<< "Time SLI (0 = undefined)" << max_seconds_sli << csv::nl
			<< "Decay Factor" << ils_decay_factor << csv::nl
			<< "Perturbation Factor (%)" << ils_perturbation_factor << csv::nl;
	}

	void write_csv_gen_info() {
		if (!csvWriter) return;
		*csvWriter << "Heuristic" << "Genetic" << csv::nl
			<< "Generations MAX (0 = undefined)" << gen_max_generations << csv::nl
			<< "Generations SLI (0 = undefined)" << gen_max_generations_sli << csv::nl
			<< "Time MAX (0 = undefined)" << gen_max_seconds << csv::nl
			<< "Time SLI (0 = undefined)" << max_seconds_sli << csv::nl
			<< "Mating Pool Size" << gen_mating_pool_size << csv::nl
			<< "Neighbourhood Window" << gen_window << csv::nl
			<< "Population MIN" << gen_minsize << csv::nl
			<< "Population MAX" << gen_maxsize << csv::nl
			<< "Mutation chance (%)" << gen_mut << csv::nl
			<< "Mutation Perturbation Factor MIN (%)" << gen_mut_pmin << csv::nl
			<< "Mutation Perturbation Factor MAX (%)" << gen_mut_pmax << csv::nl;
	}

	void write_csv_header() {
		if (!csvWriter) return;
		*csvWriter << "Seed" << seed << csv::nl
			<< "Gamma K (0 = using default, 50)" << gammak << csv::nl
			<< "Gap Threshhold" << gap_threshhold << csv::nl
			<< "Instance" << "Gap (%)" << "Time (s)" << csv::nl;
	}

	void write_csv_line(std::string instanceName,
		std::optional<double> gap_opt,
		unsigned long long time) {
		if (!csvWriter) return;
		*csvWriter << instanceName;
		if (gap_opt)
			*csvWriter << *gap_opt;
		else
			*csvWriter << csv::nc;
		*csvWriter << time << csv::nl;
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
			arg::doc("Solving heuristic. Available: ils, gen"),
			arg::def("ils"))

		.bind("seed", &options_t::seed,
			arg::doc("Random seed"),
			arg::def(2020))

		.bind("gap", &options_t::gap_threshhold,
			arg::doc("Gap threshold for stopping"))

		.bind("ils-perturbation", &options_t::ils_perturbation_factor,
			arg::doc("Pertubation factor of ILS"),
			arg::def(0.25f))

		.bind("max-iterations", &options_t::max_iterations_sli,
			arg::doc("Maximum number of iterations/generations since last improved"),
			arg::def(1000))

		.bind("max-seconds", &options_t::max_seconds_sli,
			arg::doc("Maximum time elapsed (in seconds) since last improved"))

		.bind("decay", &options_t::ils_decay_factor,
			arg::doc("Decay factor. After this many iterations, the "
			         "perturbation size decreases by ~63%."),
			arg::def(32))

		.bind("gamma-k", &options_t::gammak,
			arg::doc("Gamma set size"))

		.bind("validate", &options_t::validate,
			arg::doc("Check if solution is valid every iteration"))

		.bind("gen-window", &options_t::gen_window,
			arg::doc("Window of neighbours in random greedy construction "
			         "heuristic used in the genetic algorithm"),
			arg::def(2))

		.bind("gen-mating-pool-size", &options_t::gen_mating_pool_size,
			arg::doc("Genetic algorithm mating pool size"),
			arg::def(30))

		.bind("gen-min-size", &options_t::gen_minsize,
			arg::doc("Genetic algorithm minimum population size"),
			arg::def(10))

		.bind("gen-max-size", &options_t::gen_maxsize,
			arg::doc("Genetic algorithm maximum population size"),
			arg::def(30))

		.bind("gen-max-generations-sli", &options_t::gen_max_generations_sli,
			arg::doc("Genetic algorithm maximum number of generations since "
			         "last improved"),
			arg::def(1000))

		.bind("gen-mut-pmin", &options_t::gen_mut_pmin,
			arg::doc("Genetic algorithm minimum perturbation magnitude"),
			arg::def(0.0))

		.bind("gen-mut-pmax", &options_t::gen_mut_pmax,
			arg::doc("Genetic algorithm maximum perturbation magnitude"),
			arg::def(0.05))

		.bind("gen-mut", &options_t::gen_mut,
			arg::doc("Genetic algorithm probability of mutation"),
			arg::def(0.01))

		.bind("gen-max-generations", &options_t::gen_max_generations,
			arg::doc("Genetic algorithm maximum number of generations"))

		.bind("gen-max-seconds", &options_t::gen_max_seconds,
			arg::doc("Genetic algorithm maximum elapsed time"))

		.bind("csv-path", &options_t::csvpath,
			arg::doc("Path to CSV file with results"))

		.bind("csv-decimal-separator", &options_t::csvDecimalSeparator,
			arg::doc("Decimal separator in CSV files"),
			arg::def(','))

		.build();
	
	if (!options.csvpath.empty()) {
		options.csvWriter = std::make_unique<csv::writer>(
			std::string(DATAPATH) + "/" + options.csvpath + "/" +
			std::to_string(options.seed) + ".csv");
		options.csvWriter->setDecimalSep(options.csvDecimalSeparator);
		if (options.heuristic == "ils") {
			options.write_csv_ils_info();
		} else if (options.heuristic == "gen") {
			options.write_csv_gen_info();
		}
		options.write_csv_header();
	}

	if (!options.ifile.empty()) {
		std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
		auto iparser = InstanceParser::Open(ifilepath);
		std::cout << "Parsing instance " << options.ifile << "... ";
		auto instance_opt = iparser->Parse();
		std::cout << (instance_opt ? "OK" : "ERROR") << std::endl;
		if (!instance_opt)
			return 1;
		auto instance = *instance_opt;
		if (options.validate && !instance->IsValid())
			return 1;
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
			if (options.validate && !instance_ptr->IsValid())
				return 1;
			if (options.gammak)
				instance_ptr->SetK(options.gammak);
			Solution solution(instance_ptr);
			auto instance_filename = fs::path(instance_path).filename();
			options.savefilename = instance_filename.string() + ".sol";
			options.solve(solution);
			std::cout << std::endl;
		}
	}
	return 0;
}
