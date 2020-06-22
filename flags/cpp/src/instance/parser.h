#pragma once

#include <fstream>
#include <vector>

#include "instance.h"

namespace flags
{

	std::ifstream& operator>>(std::ifstream &fs, std::vector<Instance>& instances);

}