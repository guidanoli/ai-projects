#pragma once
#include "instance.h"
#include <fstream>
#include <list>
#include <memory>
class Solution : public std::list<std::size_t>
{
public:
	Solution() = default;
	Solution (std::shared_ptr<Instance> instance_ptr);
	std::shared_ptr<Instance> GetInstance ();
	int GetCost ();

	// (de)serialization functions
	// check for std::(i|o)fstream::good.
	friend std::ifstream& operator>> (std::ifstream& ifs, Solution& s);
	friend std::ofstream& operator<< (std::ofstream& ofs, Solution& s);
private:
	std::shared_ptr<Instance> instance_ptr;
};
