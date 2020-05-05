#pragma once

#include <cstddef>
#include <vector>
#include <random>
#include <memory>

#include "solution.h"

class Population : public std::vector<std::shared_ptr<Solution>>
{
public:
	Population(std::shared_ptr<Instance> instance_ptr,
		std::size_t minsize, std::size_t maxsize,
		std::size_t window, unsigned int seed);
private:
	std::shared_ptr<Instance> instance_ptr;
	std::size_t minsize, maxsize;
	std::default_random_engine rng;
};