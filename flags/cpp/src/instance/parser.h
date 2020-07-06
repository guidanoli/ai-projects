#pragma once

#include <fstream> // std::ifstream
#include <vector> // std::vector

#include "instance.h" // Instance

namespace flags
{

	std::ifstream& operator>>(std::ifstream &fs, std::vector<Instance>& instances);

}