#include "parser.h"

#include <filesystem>

namespace fs = std::filesystem;

using namespace flags;

int main(int argc, char** argv)
{
	auto datafilepath = fs::path(DATAPATH) / "flag.data";
	auto datafilestream = std::ifstream(datafilepath);
	std::vector<Instance> instance_vector;
	datafilestream >> instance_vector;
	return 0;
}