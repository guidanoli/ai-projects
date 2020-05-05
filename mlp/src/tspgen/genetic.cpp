#include "genetic.h"

#include <chrono>
#include <iostream>

Genetic::Genetic(std::shared_ptr<Population> population) :
	p(population)
{}

std::shared_ptr<Solution> Genetic::explore(StoppingCriterion stopping_criterion)
{
	PopulationStatus status;

	auto gen_sli = p->GetGenerationCount();
	auto best_cost = p->GetSolutionCost(p->GetBestSolution());
	auto start = std::chrono::steady_clock::now();

	status.generations_sli = 0;
	status.seconds_sli = 0;
	status.best_solution = p->GetBestSolution();

	while (!stopping_criterion(status)) {

		p->DoNextGeneration();

		auto const& best_solution = p->GetBestSolution();
		auto curr_best_cost = p->GetSolutionCost(best_solution);
		if (curr_best_cost < best_cost) {
			best_cost = curr_best_cost;
			gen_sli = p->GetGenerationCount();

			if (p->GetVerbosity()) {
				std::cout << "Gen " << gen_sli;
				auto gap_opt = best_solution->GetCostGap();
				if (gap_opt)
					std::cout << " - Gap " << *gap_opt * 100 << "%";
				std::cout << std::endl;
			}

			start = std::chrono::steady_clock::now();
		}

		status.best_solution = p->GetBestSolution();
		status.generations_sli = p->GetGenerationCount() - gen_sli;
		status.seconds_sli = std::chrono::duration_cast<std::chrono::seconds>
			(std::chrono::steady_clock::now() - start).count();
	}

	return status.best_solution;
}
