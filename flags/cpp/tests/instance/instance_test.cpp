#include "parser.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;
using namespace flags;

int main(int argc, char** argv)
{
	auto path = fs::path(DATAPATH) / "flag.data";
	auto fs = ifstream(path);
	vector<Instance> instances;
	fs >> instances;
	for (const auto& instance : instances) {
		instance.pretty();
		cout << "\n============================\n";
	}
	return 0;
}