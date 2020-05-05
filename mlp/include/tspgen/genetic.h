#pragma once

#include <memory>
#include <functional>

#include "population.h"

struct PopulationStatus
{
	// sli = since last improvement
	std::size_t generations_sli = 0;
	unsigned long long seconds_sli = 0;
	std::shared_ptr<Solution> best_solution;
};

class Genetic
{
public:
	using StoppingCriterion = std::function<bool(PopulationStatus const&)>;
	Genetic(std::shared_ptr<Population> population);
	std::shared_ptr<Solution> explore(StoppingCriterion stopping_criterion);
private:
	std::shared_ptr<Population> p;
};