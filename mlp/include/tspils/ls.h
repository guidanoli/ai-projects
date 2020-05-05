#pragma once

#include <cstddef>
#include <random>

#include "solution.h"

class LocalSearch
{
public:
	LocalSearch(std::default_random_engine& rng);
	LocalSearch(unsigned int seed);
	int findLocalMinimum(Solution& solution);
	void perturbSolution(Solution& solution, std::size_t pertubationSize);
private:
	std::default_random_engine rng;
};