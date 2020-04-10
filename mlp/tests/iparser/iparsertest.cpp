#include <filesystem>
#include <iostream>

#include "iparser.h"

namespace fs = std::filesystem;

int main(void)
{
	for (const auto& entry : fs::directory_iterator(DATAPATH)) {
		auto path = entry.path();
		auto iparser = InstanceParser::Open(path.string());
		std::cout << "Parsing " << path.filename() << "... ";
		auto instance = iparser->Parse();
		std::cout << (instance ? "OK" : "ERROR") << std::endl;
		assert(instance);
	}
}