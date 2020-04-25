#include "ils.h"

#include <algorithm>
#include <chrono>

#include "ls.h"

Solution IteratedLocalSearch::explore (Solution const& initial_solution,
	                                    double pertubation,
	                                    StoppingCriterion stopping_criterion)
{
	LocalSearch ls(seed);
	Solution solution(initial_solution);

	std::size_t n = solution.GetInstance()->GetSize();
	std::size_t pertubationSize = (std::size_t) (n * pertubation);
	pertubationSize = std::clamp(pertubationSize, (std::size_t) 1, n);

	ls.findLocalMinimum(solution);
	Solution bestSolution(solution);
	auto bestCost = bestSolution.GetCost();
	auto currCost = bestCost;

	auto beggining = std::chrono::steady_clock::now();

	IterationStatus status { bestSolution };

	while (!stopping_criterion(status)) {

		ls.perturbSolution(solution, pertubationSize);
		ls.findLocalMinimum(solution);
		currCost = solution.GetCost();
		if (bestCost > currCost) {
			bestSolution = Solution(solution);
			bestCost = currCost;
			beggining = std::chrono::steady_clock::now();
			status.duration = 0;
			status.iteration_id = 0;
			status.solution = bestSolution;
		}

		++status.iteration_id;
		status.duration = std::chrono::duration_cast<std::chrono::seconds>
			(std::chrono::steady_clock::now() - beggining).count();

	}
	
	return bestSolution;
}
