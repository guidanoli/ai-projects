#include <filesystem>
#include <iostream>

#include "iparser.h"

namespace fs = std::filesystem;

int main(void)
{
	for (const auto& entry : fs::directory_iterator(DATAPATH)) {
		auto iparser = InstanceParser::Open(entry.path().string());
	}
}