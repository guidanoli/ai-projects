#include "pplot.h"

PopulationPlotter::PopulationPlotter(std::shared_ptr<Population> p) :
	SolutionPlotter((*p)[0]),
	p(p),
	current(0)
{}

std::shared_ptr<Population> PopulationPlotter::GetPopulation() const
{
	return p;
}

std::size_t PopulationPlotter::GetNumberOfSolutions() const
{
	return p->size();
}

std::size_t PopulationPlotter::GetCurrentSolutionIndex() const
{
	return current;
}

void PopulationPlotter::SetSolution(std::size_t index)
{
	current = index;
	solution_ptr = (*p)[index];
}


