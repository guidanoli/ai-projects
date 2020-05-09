#include <filesystem>
#include <iostream>

#include "argparser.h"
#include "bksparser.h"
#include "iparser.h"
#include "solution.h"

namespace fs = std::filesystem;
namespace arg = argparser;

const char help[] = R"doc(
Instance parser tester
======================)doc";

struct options_t
{
	bool interact = false;
	std::string ifile;

	void open_instance(std::string const& instance_path) {
		auto iparser = InstanceParser::Open(instance_path);
		std::cout << "Parsing " << instance_path << "... ";
		auto instance_ptr_opt = iparser->Parse();
		std::cout << (instance_ptr_opt ? "OK" : "ERROR");

		// Test parsed instance
		assert(instance_ptr_opt);
		auto instance_ptr = *instance_ptr_opt;
		assert(instance_ptr);
		assert(instance_ptr->IsValid());
		assert(instance_ptr->GetSourceFilePath() == instance_path);

		// Test creating solution
		auto solution = Solution(instance_ptr);
		assert(solution.IsValid());

		// Test cost and gap
		Cost scost = solution.GetCost();
		assert(scost > 0);
		auto gap_opt = solution.GetCostGap();
		if (gap_opt) {
			auto gap = *gap_opt;
			assert(gap <= 0);
			std::cout << " (" << gap * 100 << "% gap)\n";
		}

		// Interact with solution
		if (interact) {
			while(true) {
				std::cout << "Options:\n"
					<< "\t0 - end\n"
					<< "\t1 - shift\n"
					<< "\t2 - swap\n"
					<< "\t3 - 2-opt\n"
					<< "\t4 - shift2\n"
					<< ">>> ";

				int opt;
				std::cin >> opt;

				if (opt == 0)
					break;

				int p, q, r = 0;
				bool must_improve;
				dump(solution);
				
				std::cout << "p = ";
				std::cin >> p;
				std::cout << "q = ";
				std::cin >> q;
				if (opt == 4) {
					std::cout << "r = ";
					std::cin >> r;
				}
				std::cout << "Must improve? ";
				std::cin >> must_improve;
				std::size_t lb, ub;
				int temp;
				std::size_t* lb_ptr = nullptr, * ub_ptr = nullptr;
				std::cout << "Lower bound (or -1)? ";
				std::cin >> temp;
				if (temp >= 0) {
					lb = temp;
					lb_ptr = &lb;
				}
				std::cout << "Upper bound (or -1)? ";
				std::cin >> temp;
				if (temp >= 0) {
					ub = temp;
					ub_ptr = &ub;
				}

				bool applied;

				switch (opt) {
				case 1:
					applied = solution.Shift(p, q, must_improve, lb_ptr, ub_ptr);
					break;
				case 2:
					applied = solution.Swap(p, q, must_improve, lb_ptr, ub_ptr);
					break;
				case 3:
					applied = solution.Opt2(p, q, must_improve, lb_ptr, ub_ptr);
					break;
				case 4:
					applied = solution.Shift2(p, q, r, must_improve, lb_ptr, ub_ptr);
					break;
				default:
					std::cerr << "Invalid option\n";
					return;
				}

				std::cout << ">>> " << (applied ? "" : "not ") << "applied\n\n";
				if (lb_ptr)
					std::cout << "New lower bound = " << *lb_ptr << std::endl;
				if (ub_ptr)
					std::cout << "New upper bound = " << *ub_ptr << std::endl;
				if (applied)
					dump(solution);
			}
		}
	}

	void dump(Solution const& solution)
	{
		std::cout << "--------------------\n";
		std::cout << "Order = ";
		solution.Print();
		auto n = solution.size();
		std::cout << "Size = " << n << std::endl;
		std::cout << "Cost = " << solution.GetCost() << std::endl;
		std::cout << "Latencies = ";
		solution.PrintLatencyMap();
		std::cout << "--------------------\n\n";
	}
};

int main(int argc, char **argv)
{
	options_t options;

	arg::build_parser(argc, argv, options, help)

		.bind("i", &options_t::interact,
			arg::doc("Enable interactiveness"),
			arg::def(false))

		.bind("ifile", &options_t::ifile,
			arg::doc("Input file"))

		.build();

	auto bks_instance = BKSParser::GetInstance();

	if (options.ifile.empty()) {
		for (const auto& entry : fs::directory_iterator(DATAPATH)) {
			auto path = entry.path();
			if (path.extension() != ".tsp")
				continue; // Accept only tsp instances
			options.open_instance(path.string());
		}
	} else {
		std::string instance_path = std::string(DATAPATH) + "/" + options.ifile;
		options.open_instance(instance_path);
	}
	return 0;
}