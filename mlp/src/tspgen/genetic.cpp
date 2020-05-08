#include "genetic.h"

#include <chrono>
#include <iostream>

Genetic::Genetic(std::shared_ptr<Population> population) :
	p(population)
{}

PopulationStatus Genetic::explore(StoppingCriterion stopping_criterion)
{
	PopulationStatus status;

	auto gen_sli = p->GetGenerationCount();
	auto best_cost = p->GetSolutionCost(p->GetBestSolution());
	auto start = std::chrono::steady_clock::now();
	auto time_sli = start;

	status.seconds = 0;
	status.generations = 0;
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

			time_sli = std::chrono::steady_clock::now();
		}

		status.best_solution = p->GetBestSolution();
		status.generations = p->GetGenerationCount();
		status.generations_sli = status.generations - gen_sli;
		auto now = std::chrono::steady_clock::now();
		status.seconds_sli = std::chrono::duration_cast<std::chrono::seconds>
			(now - time_sli).count();
		status.seconds = std::chrono::duration_cast<std::chrono::seconds>
			(now - start).count();
	}

	return status;
}
