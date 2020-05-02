#include "ils.h"

#include <algorithm>
#include <chrono>

#include "ls.h"

std::size_t getPertubationSize(double perturbation, std::size_t n) {
	std::size_t perturbationSize = (std::size_t) (n * perturbation);
	return std::clamp(perturbationSize, (std::size_t) 1, n);
}

Solution IteratedLocalSearch::explore (Solution const& initial_solution,
                                       double perturbation,
                                       unsigned long long ils_decay_factor,
                                       StoppingCriterion stopping_criterion)
{
	LocalSearch ls(seed);
	Solution solution(initial_solution);

	double initial_perturbation = perturbation;
	std::size_t n = solution.GetInstance()->GetSize();
	std::size_t perturbationSize = getPertubationSize(perturbation, n);

	ls.findLocalMinimum(solution);
	Solution bestSolution(solution);
	auto bestCost = bestSolution.GetCost();
	auto currCost = bestCost;

	auto const t_start = std::chrono::steady_clock::now();
	auto t_last_improvement = t_start;

	IterationStatus status { bestSolution };

	status.perturbationSize = perturbationSize;

	while (!stopping_criterion(status)) {

		ls.perturbSolution(solution, perturbationSize);
		ls.findLocalMinimum(solution);
		currCost = solution.GetCost();

		auto const t_now = std::chrono::steady_clock::now();

		if (bestCost > currCost) {
			t_last_improvement = t_now;
			bestSolution = Solution(solution);
			bestCost = currCost;
			status.t_last_improvement = 0;
			status.iteration_id = 0;
			status.solution = bestSolution;
		}

		++status.iteration_id;
		status.t_last_improvement = 
			std::chrono::duration_cast<std::chrono::seconds>
			(t_now - t_last_improvement).count();

		unsigned long long t_total =
			std::chrono::duration_cast<std::chrono::seconds>
			(t_now - t_start).count();

		if (ils_decay_factor != 0) {
			perturbation = initial_perturbation
				* exp2(- (double) t_total / (double) ils_decay_factor);
			perturbationSize = getPertubationSize(perturbation, n);
			status.perturbationSize = perturbationSize;
		}
	}
	
	return bestSolution;
}
