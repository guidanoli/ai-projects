#pragma once

#include <memory>
#include <cstddef>

#include "splot.h"
#include "population.h"

class PopulationPlotter : public SolutionPlotter
{
public:
	PopulationPlotter (std::shared_ptr<Population> population_ptr);
	std::shared_ptr<Population> GetPopulation () const;
	std::size_t GetNumberOfSolutions () const;
	std::size_t GetCurrentSolutionIndex () const;
	void SetSolution (std::size_t index);
private:
	std::shared_ptr<Population> population_ptr;
	std::size_t current;
};