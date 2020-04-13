#include <filesystem>
#include <iostream>
#include <vector>

#include "iparser.h"
#include "solution.h"

namespace fs = std::filesystem;

int main(void)
{
	for (const auto& entry : fs::directory_iterator(DATAPATH)) {
		auto path = entry.path();
		if (path.extension() != ".tsp")
			continue; // Accept only tsp instances
		auto iparser = InstanceParser::Open(path.string());
		std::cout << "Parsing " << path.filename() << "... ";
		auto instance_ptr_opt = iparser->Parse();
		std::cout << (instance_ptr_opt ? "OK" : "ERROR") << std::endl;
		assert(instance_ptr_opt);
		auto instance_ptr = *instance_ptr_opt;
		assert(instance_ptr);
		auto solution = Solution(instance_ptr);
		auto n = instance_ptr->GetDistanceMatrix().getm();
		assert(solution.size() == n);
		std::vector<bool> node_set(n, false);
		for (auto node : solution) {
			assert(!node_set[node]);
			node_set[node] = true;
		}
	}
}