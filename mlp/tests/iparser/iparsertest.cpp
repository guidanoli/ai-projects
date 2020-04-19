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
		assert(instance_ptr->GetSize() > 0);
		assert(instance_ptr->GetSourceFilePath() == instance_path);

		// Test creating solution
		auto solution = Solution(instance_ptr);
		auto n = instance_ptr->GetSize();
		assert(solution.size() == n - 1);
		std::vector<bool> node_set(n - 1, false);
		for (auto node : solution) {
			assert(node > 0 && node < n);
			assert(!node_set[node - 1]);
			node_set[node - 1] = true;
		}

		// Test cost and gap
		Cost scost = solution.GetCost();
		assert(scost > 0);
		auto gap_opt = solution.GetCostGap();
		if (gap_opt) {
			auto gap = *gap_opt;
			assert(gap <= (CostGap) 0);
			std::cout << " (" << gap * 100 << "% gap)\n";
		}

		// Interact with solution
		if (interact) {
			bool again = false;
			do {
				std::cout << "Options:\n"
					<< "\t0 - end\n"
					<< "\t1 - shift\n"
					<< ">>> ";

				int opt;
				std::cin >> opt;

				if (opt != 0) {
					int i, j;
					bool must_improve;
					dump(solution);

					std::cout << "n = " << solution.size() << std::endl;
					std::cout << "i = ";
					std::cin >> i;
					std::cout << "j = ";
					std::cin >> j;
					std::cout << "must improve = ";
					std::cin >> must_improve;
					bool applied;

					switch (opt) {
					case 1:
						applied = solution.Shift(i, j, must_improve);
						break;
					default:
						std::cerr << "Invalid option\n";
						return;
					}

					std::cout << (applied ? "" : "not ") << "applied\n";
					dump(solution);

					std::cout << "do again? ";
					std::cin >> again;
				}
			} while (again);
		}
	}

	void dump(Solution const& solution)
	{
		std::cout << "[ ";
		for (auto const& node : solution)
			std::cout << node << " ";
		std::cout << "]\n";
		std::cout << "Cost = " << solution.GetCost() << std::endl;
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

		.abort_on("help");

	auto bks_instance = BKSParser::getInstance();

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