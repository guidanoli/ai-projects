#include <filesystem>
#include <iostream>

#include "bks.h"
#include "iparser.h"
#include "solution.h"

namespace fs = std::filesystem;

int main(void)
{
	auto bks_instance = BKS::getInstance();

	for (const auto& entry : fs::directory_iterator(DATAPATH)) {
		auto path = entry.path();
		if (path.extension() != ".tsp")
			continue; // Accept only tsp instances
		auto iparser = InstanceParser::Open(path.string());
		std::cout << "Parsing " << path.filename() << "... ";
		auto instance_ptr_opt = iparser->Parse();
		std::cout << (instance_ptr_opt ? "OK" : "ERROR");

		// Test parsed instance
		assert(instance_ptr_opt);
		auto instance_ptr = *instance_ptr_opt;
		assert(instance_ptr);
		assert(instance_ptr->GetSize() > 0);
		assert(instance_ptr->GetSourceFilePath() == path);

		// Test creating solution
		auto solution = Solution(instance_ptr);
		auto n = instance_ptr->GetSize();
		assert(solution.size() == n);
		std::vector<bool> node_set(n, false);
		for (auto node : solution) {
			assert(!node_set[node]);
			node_set[node] = true;
		}
		int scost = solution.GetCost();
		assert(scost > 0);

		// Test getting BKS
		std::string instance_name = path.stem().string();
		auto bks_opt = bks_instance->getInstanceBKS(instance_name);
		assert(bks_opt);
		auto bks = *bks_opt;
		assert(solution.GetCost() >= bks);
		float ratio = 1 - ((float) scost / (float) bks);
		std::cout << " (" << ratio * 100 << "% gap)\n";
	}
}