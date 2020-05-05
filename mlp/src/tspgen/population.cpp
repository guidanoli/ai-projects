#include "population.h"

#include <random>

Population::Population(
	std::shared_ptr<Instance> instance_ptr,
	std::size_t minsize,
	std::size_t maxsize,
	std::size_t window,
	unsigned int seed) :

	instance_ptr(instance_ptr),
	minsize(minsize),
	maxsize(maxsize),
	rng(seed)
{
	for (std::size_t i = 0; i < minsize; ++i)
		push_back(std::make_shared<Solution>(instance_ptr, window, rng));
}