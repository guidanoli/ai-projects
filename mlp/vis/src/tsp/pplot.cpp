#include "pplot.h"

PopulationPlotter::PopulationPlotter(std::shared_ptr<Population> population_ptr) :
	SolutionPlotter((*population_ptr)[0]),
	population_ptr(population_ptr),
	current(0)
{}

std::shared_ptr<Population> PopulationPlotter::GetPopulation() const
{
	return population_ptr;
}

std::size_t PopulationPlotter::GetNumberOfSolutions() const
{
	return population_ptr->size();
}

std::size_t PopulationPlotter::GetCurrentSolutionIndex() const
{
	return current;
}

void PopulationPlotter::SetSolution(std::size_t index)
{
	current = index;
	solution_ptr = (*population_ptr)[index];
}


